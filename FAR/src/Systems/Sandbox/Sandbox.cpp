#include "PCH/PCH.hpp"

#include "Sandbox.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"
#include "Resources/RenderResource.hpp"
#include "Components/SkeletalAnimator.hpp"
#include "Components/ScriptedMotionPath.hpp"
#include "Components/IKPoser.hpp"

namespace FAR
{
  void Sandbox::Init()
  {
    Engine& engine = *Engine::GetInstance();
    cam = engine.CreateEntity();

    glm::quat camRot = glm::quat(-0.209f, 0.523f, -0.306f, -0.767f);
    camRot = glm::normalize(camRot);

    engine.AddComponent(cam, Camera{.nearPlane = 0.001f, .forward = camRot * glm::vec3(0.0f, 0.0f, -1.0f), .isMain = true});
    engine.AddComponent(cam, Transform{ .position = glm::vec3(-10.0f, 7.0f, 0.0f), .rotationQuaternion = camRot });

    Entity target = engine.CreateEntity();
    engine.AddComponent(target, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(0.33f, 0.33f, 1.0f)});
    engine.AddComponent(target, Model{ .path = "assets/quad.obj", .textured = false });

    Entity e = engine.CreateEntity();
    //engine.AddComponent(e, ScriptedMotionPath{ 
    //  //.controlPoints = {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
    //  //.controlPoints = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} , {0.0f, 0.0f, 0.0f}},
    //  .controlPoints = {{-1.1f, 0.0f, -6.7f}, {2.2f, 0.0f, -2.6f}, {6.1f, 0.0f, -0.6f} , {4.9f, 0.0f, 2.9f}, {-1.8f, 0.0f, 6.9f}, {-7.6f, 0.0f, 0.0f}, {-8.8f, 0.0f, -10.8f}, {0.3f, 0.0f, -16.7f}, {-1.5f, 0.0f, -11.6f}, {-1.1f, 0.0f, -6.7f}},
    //  .velocityKeys = {{0.0f, 0.0f}, {0.1f, 0.01f}, { 0.45f ,1.0f }, {0.55f, 1.0f}, {0.9f, 0.01f}, { 1.0f, 0.0f }},
    //  //.velocityKeys = {{0.0f, 1.0f}, {1.0f, 1.0f}},
    //  .totalTime = 30.0f
    //  });
    engine.AddComponent(e, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f) });
    engine.AddComponent(e, Model{ .path = "assets/trotting_cat.glb", .textured = false });
    engine.AddComponent(e, SkeletalAnimator{ .path = "assets/trotting_cat.glb", .animationTime = 0.0f, .currentAnimation = 0, .playing = true, .looping = true, .playbackSpeed = 1.3f });
    engine.AddComponent(e, IKPoser{ .target = target });

    Entity floor = engine.CreateEntity();
    engine.AddComponent(floor, Model{ .path = "assets/quad.obj", .textured = false });
    engine.AddComponent(floor, Transform{ .position = glm::vec3(-12.0f, -0.01f, -19.0f), .rotationQuaternion = glm::quat(0.707f, 0.707f, 0.0f, 0.0f), .scale = glm::vec3(22.0f, 30.0f, 0.0f)});
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