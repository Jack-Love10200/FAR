///
/// @file   IK.cpp
/// @brief  Cyclic Coordinate Descent (CCD) Inverse Kinematics System
/// @author Jack Love
/// @date   17.11.2025
///

#include "PCH/PCH.hpp"
#include "IK.hpp"
#include "Resources/RenderResource.hpp"

#include "imgui.h"

namespace FAR
{
  void IK::Init()
  {

  }

  void IK::PreUpdate()
  {
    
  }

  float AngleBetweenVectors(const glm::vec3& a, const glm::vec3& b)
  {
    float dotProduct = glm::dot(glm::normalize(a), glm::normalize(b));
    dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f); // Clamp to avoid numerical issues
    return acosf(dotProduct); // Angle in radians
  }

  void IK::Update()
  {
    std::vector<Entity> ents = Engine::GetInstance()->GetEntities<Model, IKPoser>();

    //solve IK for all ikposer components
    for (Entity e : ents)
    {
      Transform& trans = Engine::GetInstance()->GetComponent<Transform>(e);
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);
      IKPoser& ikp = Engine::GetInstance()->GetComponent<IKPoser>(e);
      SolveIK(model, ikp, VQS(trans.position, trans.rotationQuaternion, trans.scale));
    }
  }

  void IK::SolveIK(Model& model, IKPoser& ikp, VQS pos)
  {
    for (IKPoser::Manipulator& manipulator : ikp.manipulators)
    {
      Transform& targetTrans = Engine::GetInstance()->GetComponent<Transform>(manipulator.target);
      glm::vec3 targetPos = targetTrans.position;
      SolveManipulator(model, manipulator, pos);
    }
  }

  void IK::SolveManipulator(Model& model, IKPoser::Manipulator& manipulator, VQS pos)
  {
    //get target position
    const glm::vec3 targetPos = Engine::GetInstance()->GetComponent<Transform>(manipulator.target).position;

    //copy of nodes to work with, cant apply heirarchy to the one in the model b/c render system needs it flattened
    std::vector<Model::Node> nodesCopy = model.nodes;
    ApplyNodeHeirarchy(nodesCopy, 0, VQS());

    //previos End Effector position for c
    glm::vec3 lastEEPos = nodesCopy[manipulator.EEIndex].transform.v;

    //iterate until we convege (base case bail out after enough in case of ocilation)
    uint8_t iterations = 0;
    while (iterations < 8)
    {
      //for each bone in the manipulator
      for (const IKPoser::Manipulator::ManipulatorBone& joint : manipulator.bones | std::views::reverse)
      //for (auto [boneIndex, minAngle, maxAngle] : manipulator.bones)
      {
        manipulator.currentEEPos = nodesCopy[manipulator.EEIndex].transform.v;

        //get vectors from current joint to current end effector and target
        glm::vec3 Vdk = targetPos - nodesCopy[joint.boneIndex].transform.v;
        glm::vec3 Vck = manipulator.currentEEPos - nodesCopy[joint.boneIndex].transform.v;

        //get the angle between the two vectors, and the axis to rotate around
        float angle = AngleBetweenVectors(Vck, Vdk);
        glm::vec3 Vk = glm::cross(Vck, Vdk);

        //rotate the actual node by angle around vk
        if (glm::length(Vk) > 0.0001f)
        {
          Vk = glm::normalize(Vk);
          glm::quat rotationDelta = glm::angleAxis(angle, Vk);
          glm::quat parentWorldRot = nodesCopy[model.nodes[joint.boneIndex].parent].transform.q;
          glm::quat localDelta = glm::inverse(parentWorldRot) * rotationDelta * parentWorldRot;
          model.nodes[joint.boneIndex].transform.q = glm::normalize(localDelta * model.nodes[joint.boneIndex].transform.q);
        }

        //clamp everything to the set constraints
        glm::vec3 eulerAngles = glm::eulerAngles(model.nodes[joint.boneIndex].transform.q);
        eulerAngles.x = glm::clamp(eulerAngles.x, joint.minPitch, joint.maxPitch);
        eulerAngles.y = glm::clamp(eulerAngles.y, joint.minYaw, joint.maxYaw);
        eulerAngles.z = glm::clamp(eulerAngles.z, joint.minRoll, joint.maxRoll);
        model.nodes[joint.boneIndex].transform.q = glm::quat(eulerAngles);

        //recalculate heirarchy, this is expensive, but the simple solution for now
        nodesCopy = model.nodes;
        ApplyNodeHeirarchy(nodesCopy, 0, pos);
        manipulator.currentEEPos = nodesCopy[manipulator.EEIndex].transform.v;

        //if the distance to the target is close enough, stop
        if (glm::length(targetPos - manipulator.currentEEPos) < 0.001f)
          return;
      }

      iterations++;

      //if no more significant movement, exit
      if (glm::length(manipulator.currentEEPos - lastEEPos) < 0.001f)
        return; 

      lastEEPos = manipulator.currentEEPos;
    }
  }

  void IK::ApplyNodeHeirarchy(std::vector<Model::Node>& nodes, int nodeIndex, const VQS& parentTransform)
  {
    Model::Node& node = nodes[nodeIndex];
    VQS localTransform = node.transform;
    VQS globalTransform = parentTransform * localTransform;
    //node.skinningTransform = globalTransform * node.inverseBindPose;
    //node.transform = model.animation.globalInverseTransform * globalTransform * node.inverseBindPose;

    node.transform = globalTransform;

    // Recurse for children
    for (int childIdx : node.children) 
    {
      ApplyNodeHeirarchy(nodes, childIdx, globalTransform);
    }
  }

  void IK::PostUpdate()
  {

  }

  void IK::Exit()
  {

  }
}