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

    glm::quat camRot = glm::quat(-0.031, -0.424f, -0.160, -0.891);
    camRot = glm::normalize(camRot);

    engine.AddComponent(cam, Camera{.nearPlane = 0.001f, .forward = camRot * glm::vec3(0.0f, 0.0f, -1.0f), .isMain = true});
    engine.AddComponent(cam, Transform{ .position = glm::vec3(1.873f, 1.405f, 1.513f), .rotationQuaternion = camRot });

//    Entity target = engine.CreateEntity();
//    engine.AddComponent(target, Transform{ .position = glm::vec3(0.0f, 1.0f, 0.0f), .scale = glm::vec3(0.1f, 0.1f, 1.0f)});
//    engine.AddComponent(target, Model{ .path = "assets/quad.obj", .textured = false });
//    engine.AddComponent(target, ScriptedMotionPath{
//      .controlPoints = { {0.0f, 1.0f, -1.0f}, {-0.7f, 1.6f, -1.1f}, {0.4f, 1.2f, -1.1f}, {0.0f, 1.0f, -1.0f} },
//      .velocityKeys = { {0.0f, 0.0f}, {0.2f, 0.5f}, {0.8f, 0.5f}, {1.0f, 0.0f} },
//      .totalTime = 4.0f
//      });
//
//    Entity target2 = engine.CreateEntity();
//    engine.AddComponent(target2, Transform{ .position = glm::vec3(0.115f, 0.086f, 0.476f), .scale = glm::vec3(0.1f, 0.1f, 1.0f) });
//    engine.AddComponent(target2, Model{ .path = "assets/quad.obj", .textured = false });
//
//    Entity e = engine.CreateEntity();
//    //engine.AddComponent(e, ScriptedMotionPath{ 
//    //  //.controlPoints = {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
//    //  //.controlPoints = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} , {0.0f, 0.0f, 0.0f}},
//    //  .controlPoints = {{-1.1f, 0.0f, -6.7f}, {2.2f, 0.0ff, 0.0f, -0.6f} , {4.9f, 0.0f, 2.9f}, {-1.8f, 0.0f, 6.9f}, {-7.6f, 0.0f, 0.0f}, {-8.8f, 0.0f, -10.8f}, {0.3f, 0.0f, -16.7f}, {-1.5f, 0.0f, -11.6f}, {-1.1f, 0.0f, -6.7f}},
//    //  .velocityKeys = {{0.0f, 0.0f}, {0.1f, 0.01f}, { 0.45f ,1.0f }, {0.55f, 1.0f}, {0.9f, 0.01f}, { 1.0f, 0.0f }},
//    //  //.velocityKeys = {{0.0f, 1.0f}, {1.0f, 1.0f}},
//    //  .totalTime = 30.0f
//    //  });
//    engine.AddComponent(e, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f) });
//    engine.AddComponent(e, Model{ .path = "assets/trotting_cat.glb", .textured = false });
//    //engine.AddComponent(e, SkeletalAnimator{ .path = "assets/trotting_cat.glb", .animationTime = 0.0f, .currentAnimation = 0, .playing = true, .looping = true, .playbackSpeed = 1.3f });
//    engine.AddComponent(e, IKPoser{
//      .manipulators = {
//        IKPoser::Manipulator{
//          .target = target,
//          .bones = {
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 55, .minPitch = glm::radians(-15.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-15.0f), .maxRoll = glm::radians(15.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 54, .minPitch = glm::radians(-10.0f), .maxPitch = glm::radians(25.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(0.0f), .maxRoll = glm::radians(30.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 53, .minPitch = glm::radians(-10.0f), .maxPitch = glm::radians(20.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(5.0f), .maxRoll = glm::radians(35.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 52, .minPitch = glm::radians(-10.0f), .maxPitch = glm::radians(20.0f), .minYaw = glm::radians(-10.0f), .maxYaw = glm::radians(20.0f), .minRoll = glm::radians(5.0f), .maxRoll = glm::radians(35.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 51, .minPitch = glm::radians(-15.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(5.0f), .maxRoll = glm::radians(35.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 50, .minPitch = glm::radians(-15.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-10.0f), .maxRoll = glm::radians(25.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 49, .minPitch = glm::radians(-20.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-10.0f), .maxRoll = glm::radians(25.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 48, .minPitch = glm::radians(-15.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-80.0f), .maxRoll = glm::radians(-50.0f)},
//          },
//          .EEIndex = 55
//        },
//        IKPoser::Manipulator{
//          .target = target2, 
//          .bones = {
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 16, .minPitch = glm::radians(-15.0f), .maxPitch = glm::radians(15.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-15.0f), .maxRoll = glm::radians(15.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 15, .minPitch = glm::radians(-60.0f), .maxPitch = glm::radians(-30.0f), .minYaw = glm::radians(-10.0f), .maxYaw = glm::radians(25.0f), .minRoll = glm::radians(-25.0f), .maxRoll = glm::radians(5.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 14, .minPitch = glm::radians(-90.0f), .maxPitch = glm::radians(-60.0f), .minYaw = glm::radians(-15.0f), .maxYaw = glm::radians(15.0f), .minRoll = glm::radians(-15.0f), .maxRoll = glm::radians(15.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 13, .minPitch = glm::radians(105.0f), .maxPitch = glm::radians(135.0f), .minYaw = glm::radians(-10.0f), .maxYaw = glm::radians(20.0f), .minRoll = glm::radians(-5.0f), .maxRoll = glm::radians(25.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 12, .minPitch = glm::radians(55.0f), .maxPitch = glm::radians(85.0f), .minYaw = glm::radians(-20.0f), .maxYaw = glm::radians(10.0f), .minRoll = glm::radians(0.0f), .maxRoll = glm::radians(30.0f)},
//IKPoser::Manipulator::ManipulatorBone{.boneIndex = 11, .minPitch = glm::radians(65.0f), .maxPitch = glm::radians(95.0f), .minYaw = glm::radians(-75.0f), .maxYaw = glm::radians(-45.0f), .minRoll = glm::radians(-35.0f), .maxRoll = glm::radians(-5.0f)},
//          },
//          .EEIndex = 16
//        }
//      }
//      });

    Entity floor = engine.CreateEntity();
    engine.AddComponent(floor, Model{ .path = "assets/quad.obj", .textured = false });

    engine.AddComponent(floor, Transform{ .position = glm::vec3(-12.0f, -0.01f, -19.0f), .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f), .scale = glm::vec3(22.0f, 30.0f, 0.0f)});

    Entity springStart = engine.CreateEntity();
    Entity springEnd = engine.CreateEntity();

    engine.AddComponent(springStart, Model{ .path = "assets/quad.obj", .textured = false });
    engine.AddComponent(springStart, Transform{ .position = glm::vec3(1.0f, 3.0f, 2.0f), .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 0.0f) });
    engine.AddComponent(springStart, PointMass{ .mass = 0.0f, .velocity = glm::vec3(0.0f), .acceleration = glm::vec3(0.0f) , .isStatic = true});
    
    engine.AddComponent(springEnd, Model{ .path = "assets/quad.obj", .textured = false });
    engine.AddComponent(springEnd, Transform{ .position = glm::vec3(1.0f, 3.0f, -8.0f), .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 0.0f)});
    engine.AddComponent(springEnd, PointMass{ .mass = 1.0f, .velocity = glm::vec3(0.0f), .acceleration = glm::vec3(0.0f), .isStatic = true });

    Entity springMiddle[6] = {};
    for (int i = 0; i < 6; i++)
    {
      springMiddle[i] = engine.CreateEntity();
      engine.AddComponent(springMiddle[i], Model{ .path = "assets/quad.obj", .textured = false });
      engine.AddComponent(springMiddle[i], Transform{ .position = glm::vec3(1.0f, 3.0f, -1.0f * (i + 1)), .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 0.0f) });
      engine.AddComponent(springMiddle[i], PointMass{ .mass = 1.0f, .velocity = glm::vec3(0.0f), .acceleration = glm::vec3(0.0f), .isStatic = false });
    }

    for (int i = 0; i < 6; i++)
    {
      Entity attachA = (i == 0) ? springStart : springMiddle[i - 1];
      //Entity attachB = (i == 5) ? springEnd : springMiddle[i + 1];
      engine.AddComponent(springMiddle[i], Spring{ .attachment = attachA, .springCoeff = 5.0f, .dampingCoeff = 0.5f, .restLength = 1.0f });
      //engine.AddComponent(springMiddle[i], Spring{ .attachment = attachB, .springCoeff = 5.0f, .dampingCoeff = 0.5f, .restLength = 1.0f });
    }

    engine.AddComponent(springEnd, Spring{ .attachment = springMiddle[5], .springCoeff = 5.0f, .dampingCoeff = 0.5f, .restLength = 1.0f });
    //engine.AddComponent(springStart, Spring{ .attachment = springEnd, .springCoeff = 5.0f, .dampingCoeff = 0.5f, .restLength = 1.0f });
    //engine.AddComponent(springEnd, Spring{ .attachment = springStart, .springCoeff = 5.0f, .dampingCoeff = 0.5f, .restLength = 1.0f });
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