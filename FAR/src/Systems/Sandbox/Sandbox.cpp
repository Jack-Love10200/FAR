#include "PCH/PCH.hpp"

#include "Sandbox.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"
#include "Resources/RenderResource.hpp"
#include "Components/SkeletalAnimator.hpp"
#include "Components/ScriptedMotionPath.hpp"
#include "Components/IKPoser.hpp"
#include "Components/PointMass.hpp"
#include "Components/Spring.hpp"


namespace FAR
{
  void Sandbox::Init()
  {
    Engine& engine = *Engine::GetInstance();
    cam = engine.CreateEntity();

    glm::quat camRot = glm::quat(0.040 , -0.664, -0.187, -0.732);
    camRot = glm::normalize(camRot);

    engine.AddComponent(cam, Camera{.nearPlane = 0.001f, .forward = camRot * glm::vec3(0.0f, 0.0f, -1.0f), .isMain = true});
    engine.AddComponent(cam, Transform{ .position = glm::vec3(8.134f, 3.459f, 2.435f), .rotationQuaternion = camRot });

    Entity springCube[64];
    Spring springs[64];

    const int cubeDim = 4;
    const float spacing = 1.5f;

    for (int i = 0; i < 64; i++)
    {
      int x = i % cubeDim;
      int y = (i / cubeDim) % cubeDim;
      int z = i / (cubeDim * cubeDim);

      glm::vec3 pos = glm::vec3(
        x * spacing,
        y * spacing,
        z * spacing
      );

      springCube[i] = engine.CreateEntity();
      engine.AddComponent(springCube[i], Model{
          .path = "assets/quad.obj",
          .textured = false
        });
      engine.AddComponent(springCube[i], Transform{
          .position = pos,
          .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f),
          .scale = glm::vec3(0.1f, 0.1f, 0.1f)
        });
      engine.AddComponent(springCube[i], PointMass{
          .mass = 1.0f,
          .velocity = glm::vec3(0.0f),
          .acceleration = glm::vec3(0.0f),
          .isStatic = false
        });
    }

    //pin top 4 corners
    engine.GetComponent<PointMass>(springCube[12]).isStatic = true;
    engine.GetComponent<PointMass>(springCube[15]).isStatic = true;
    engine.GetComponent<PointMass>(springCube[60]).isStatic = true;
    engine.GetComponent<PointMass>(springCube[63]).isStatic = true;

    const float L = 1.5f;
    const float L2 = L * glm::root_two<float>();
    const float L3 = L * glm::root_three<float>();

    for (int i = 0; i < 64; i++)
    {
      int xi = (i / 16) % 4;
      int yi = (i / 4) % 4;
      int zi = i % 4;

      // STRAIGHT NEIGHBORS (orthogonal)
      if (xi < 3) {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 16], .restLength = L });
        springs[i + 16].attachments.push_back({ .attachedEntity = springCube[i], .restLength = L });
      }

      if (yi < 3) {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 4], .restLength = L });
        springs[i + 4].attachments.push_back({ .attachedEntity = springCube[i], .restLength = L });
      }

      if (zi < 3) {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L });
        springs[i + 1].attachments.push_back({ .attachedEntity = springCube[i], .restLength = L });
      }

      // DIAGONAL NEIGHBORS (two-axis)
      if (xi < 3 && yi < 3)
      {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 16 + 4], .restLength = L2 });
        springs[i + 16].attachments.push_back({ .attachedEntity = springCube[i + 4], .restLength = L2 });
      }

      if (xi < 3 && zi < 3)
      {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 16 + 1], .restLength = L2 });
        springs[i + 16].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L2 });
      }

      if (yi < 3 && zi < 3)
      {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 4 + 1], .restLength = L2 });
        springs[i + 4].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L2 });
      }

      // THREE-AXIS DIAGONALS (XYZ)
      if (xi < 3 && yi < 3 && zi < 3)
      {
        springs[i].attachments.push_back({ .attachedEntity = springCube[i + 16 + 4 + 1], .restLength = L3 });

        springs[i + 1].attachments.push_back({ .attachedEntity = springCube[i + 16 + 4], .restLength = L3 });
        springs[i + 4].attachments.push_back({ .attachedEntity = springCube[i + 16 + 1], .restLength = L3 });
        springs[i + 16].attachments.push_back({ .attachedEntity = springCube[i + 4 + 1], .restLength = L3 });

        springs[i + 1].attachments.push_back({ .attachedEntity = springCube[i + 16], .restLength = L3 });
        springs[i + 4].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L3 });
        springs[i + 16].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L3 });
        springs[i + 16 + 4].attachments.push_back({ .attachedEntity = springCube[i + 1], .restLength = L3 });
      }
    }

    for (int i = 0; i < 64; i++)
    {
      engine.AddComponent<Spring>(springCube[i], springs[i]);
    }
  }

  void Sandbox::PreUpdate()
  {
  }

  void Sandbox::Update()
  {
    InputResource* inputResc = Engine::GetInstance()->GetResource<InputResource>();

    float speed = 0.01f;
    if (!inputResc->GetMouseButton(KEYCODE::MOUSE_RIGHT))
      return;

    Transform& camTransform = Engine::GetInstance()->GetComponent<Transform>(cam);
    Camera& camera = Engine::GetInstance()->GetComponent<Camera>(cam);


    if (inputResc->GetKey(KEYCODE::W))
    {
      camTransform.position += camera.forward * speed;
    }

    if (inputResc->GetKey(KEYCODE::S))
    {

      camTransform.position -= camera.forward * speed;
    }

    if (inputResc->GetKey(KEYCODE::A))
    {
      glm::vec3 right = glm::cross(camera.forward, camera.up);
      camTransform.position -= right * speed;
    }

    if (inputResc->GetKey(KEYCODE::D))
    {
      glm::vec3 right = glm::cross(camera.forward, camera.up);
      camTransform.position += right * speed;
    }


    float sensitivity = 0.01f; // tweak
    float yaw = inputResc->mouseDelta.x * sensitivity * -1.0f;
    float pitch = inputResc->mouseDelta.y * sensitivity;

    if (glm::dot(camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) > 0.99f && pitch < 0.0f)
      pitch = 0.0f;

    if (glm::dot(camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) > 0.99f && pitch > 0.0f)
      pitch = 0.0f;

    // build incremental quaternion from mouse movement

    //glm::quat qYaw = glm::quat::FromAxisAngle(yaw, glm::vec3(0, 1, 0));
    //Quat qPitch = Quat::FromAxisAngle(pitch, glm::vec3(1, 0, 0));

    glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));

    camTransform.rotationQuaternion = qYaw * camTransform.rotationQuaternion;
    camTransform.rotationQuaternion = camTransform.rotationQuaternion * qPitch;
    //camTransform.rotationQuaternion = camTransform.rotationQuaternion.Normalize(); // keep it clean
    camTransform.rotationQuaternion = glm::normalize(camTransform.rotationQuaternion);

    camera.forward = camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f);

  }

  void Sandbox::PostUpdate()
  {
  }

  void Sandbox::Exit()
  {
  }

}