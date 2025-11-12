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
    ImGui::Begin("IK Debug");

    for (Entity e : ents)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);
      IKPoser& ikp = Engine::GetInstance()->GetComponent<IKPoser>(e);

      Transform& targetTrans = Engine::GetInstance()->GetComponent<Transform>(ikp.target);
      glm::vec3 targetPos = targetTrans.position;

      //get euler angles of the bones
      for (int i = 0; i < model.nodes.size(); i++)
      {
        glm::vec3 eulerAngles = glm::eulerAngles(model.nodes[i].transform.q);

        if (i >= 48 && i <= 55)
        {
          ImGui::Text("Bone %d: Pitch: %.2f, Yaw: %.2f, Roll: %.2f", i, glm::degrees(eulerAngles.x), glm::degrees(eulerAngles.y), glm::degrees(eulerAngles.z));
          ImGui::Text("Bone %d Quaternion: (%.4f, %.4f, %.4f, %.4f)", i, model.nodes[i].transform.q.w, model.nodes[i].transform.q.x, model.nodes[i].transform.q.y, model.nodes[i].transform.q.z);
        }

      }

      SolveIK(model, ikp, targetPos);

      Engine::GetInstance()->GetResource<RenderResource>()->DrawPoint({ .position = glm::vec4(ikp.currentEEPos, 1.0f), .color = {0.0f, 1.0f, 0.0f, 1.0f} });
    }
    ImGui::End();
  }

  void IK::SolveIK(Model& model, IKPoser& ikp, const glm::vec3& targetPos)
  {
    std::vector<Model::Node> nodesCopy = model.nodes;
    ApplyNodeHeirarchy(nodesCopy, 0, VQS());

    for (int i = 0; i < model.nodes.size(); i++)
    {
      glm::vec3 eulerAngles = glm::eulerAngles(model.nodes[i].transform.q);

      if (i >= 48 && i <= 55)
        ImGui::Text("Bone %d pos: (%.2f, %.2f, %.2f)", i, nodesCopy[i].transform.v.x, nodesCopy[i].transform.v.y, nodesCopy[i].transform.v.z);

    }

    glm::vec3 lastEEPos = nodesCopy[ikp.manipulator.back()].transform.v;

    while (true)
    {
      glm::vec3 lastEEPosSub = ikp.currentEEPos;

      uint8_t iterations = 0;

      for (int& nodeID : ikp.manipulator | std::views::reverse)
      {
        ikp.currentEEPos = nodesCopy[ikp.manipulator.back()].transform.v;


        glm::vec3 Vdk = targetPos - nodesCopy[nodeID].transform.v;
        glm::vec3 Vck = ikp.currentEEPos - nodesCopy[nodeID].transform.v;

        float angle = AngleBetweenVectors(Vck, Vdk);

        glm::vec3 Vk = glm::cross(Vck, Vdk);

        //rotate the actual node by angle around vk
        if (glm::length(Vk) > 0.0001f)
        {
          Vk = glm::normalize(Vk);
          glm::quat rotationDelta = glm::angleAxis(angle, Vk);
          //rotationDelta = glm::slerp(glm::quat(1, 0, 0, 0), rotationDelta, 0.02f);
          //model.nodes[nodeID].transform.q = glm::normalize(rotationDelta * model.nodes[nodeID].transform.q);
          //model.nodes[nodeID].transform.q = glm::normalize(model.nodes[nodeID].transform.q * rotationDelta);

          glm::quat parentWorldRot = nodesCopy[model.nodes[nodeID].parent].transform.q; // or however you store it
          glm::quat localDelta = glm::inverse(parentWorldRot) * rotationDelta * parentWorldRot;
          model.nodes[nodeID].transform.q = normalize(localDelta * model.nodes[nodeID].transform.q);
        }

        //recalculate heirarchy
        nodesCopy = model.nodes;
        ApplyNodeHeirarchy(nodesCopy, 0, VQS());

        ikp.currentEEPos = nodesCopy[ikp.manipulator.back()].transform.v;

        //if the distance to the target is close enough, stop
        if (glm::length(targetPos - ikp.currentEEPos) < 0.001f)
          return;

      }
      iterations++;
      float len = glm::length(ikp.currentEEPos - lastEEPos);

      //std::cout << "EE Movement this iteration: " << len << std::endl;

      if (len < 0.001f)
      {
        std::cout << "IK Iterations this pass: " << (int)iterations << std::endl;
        return; //no more significant movement, exit
      }

      lastEEPos = ikp.currentEEPos;
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
    for (int childIdx : node.children) {
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