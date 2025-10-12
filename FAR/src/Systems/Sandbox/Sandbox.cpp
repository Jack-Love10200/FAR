#include "PCH/PCH.hpp"

#include "Sandbox.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"
#include "Resources/RenderResource.hpp"
#include "Components/SkeletalAnimator.hpp"

namespace FAR
{
  void Sandbox::Init()
  {
    Engine& engine = *Engine::GetInstance();
    cam = engine.CreateEntity();
    engine.AddComponent(cam, Camera{ .isMain = true });
    engine.AddComponent(cam, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f), .rotationQuaternion = Quat(0.0f, 0.0f, 0.0f, 1.0f)});

    jack = engine.CreateEntity();
    engine.AddComponent(jack, Model{ .path = "assets/jack_samba.glb", .textured = true});
    engine.AddComponent(jack, Transform{ .position = glm::vec3(0.0f, 0.0f, -3.0f), .scale = glm::vec3(0.1f, 0.1f, 0.1f) });
    engine.AddComponent(jack, SkeletalAnimator{ .path = "assets/jack_samba.glb", .currentAnimation = 0 });

    adi = engine.CreateEntity();
    engine.AddComponent(adi, Model{ .path = "assets/Adi_Dancing.fbx", .textured = false});
    engine.AddComponent(adi, Transform{ .position = glm::vec3(1.0f, 0.0f, -3.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });
    engine.AddComponent(adi,  SkeletalAnimator{ .path = "assets/Adi_Dancing.fbx", .currentAnimation = 0 });

    roman = engine.CreateEntity();
    engine.AddComponent(roman, Model{ .path = "assets/CS460Models/fbx/roman_D.fbx", .textured = true });
    engine.AddComponent(roman, Transform{ .position = glm::vec3(0.0f, 0.0f, -100.0f), .scale = glm::vec3(100.0f, 100.0f, 100.0f) });
    engine.AddComponent(roman, SkeletalAnimator{ .path = "assets/CS460Models/fbx/roman_D.fbx", .currentAnimation = 0, .playing = true, .looping = true});

    egyptian = engine.CreateEntity();
    engine.AddComponent(egyptian, Model{ .path = "assets/CS460Models/fbx/egyptian_B.fbx", .textured = false });
    engine.AddComponent(egyptian, Transform{ .position = glm::vec3(-50.0f, 0.0f, -100.0f), .scale = glm::vec3(100.0f, 100.0f, 100.0f) });
    engine.AddComponent(egyptian, SkeletalAnimator{ .path = "assets/CS460Models/fbx/egyptian_B.fbx", .currentAnimation = 0, .looping = true});

    viking = engine.CreateEntity();
    engine.AddComponent(viking, Model{ .path = "assets/CS460Models/fbx/viking_C.fbx", .textured = true });
    engine.AddComponent(viking, Transform{ .position = glm::vec3(50.0f, 0.0f, -100.0f), .scale = glm::vec3(100.0f, 100.0f, 100.0f) });
    engine.AddComponent(viking, SkeletalAnimator{ .path = "assets/CS460Models/fbx/viking_C.fbx", .currentAnimation = 0, .looping = true });

    //okayu = engine.CreateEntity();
    //engine.AddComponent(okayu, Model{ .path = "assets/okayu/okayu.pmx", .textured = true});
    //engine.AddComponent(okayu, Transform{ .position = glm::vec3(-10.0f, 0.0f, -30.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

    //pika = engine.CreateEntity();
    //engine.AddComponent(pika, Model{ .path = "assets/pika.fbx", .textured = false});
    //engine.AddComponent(pika, Transform{ .position = glm::vec3(-20.0f, 0.0f, -5.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });
    //engine.AddComponent(pika, SkeletalAnimator{ .path = "assets/pika.fbx", .currentAnimation = 0, .looping = true });
  }

  void Sandbox::PreUpdate()
  {
  }

  void Sandbox::Update()
  {

    InputResource* inputResc = Engine::GetInstance()->GetResource<InputResource>();

    float speed = 0.00001f;
    
    if (inputResc->GetKey(KEYCODE::SPACE))
      speed = 0.01f;


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

    if (inputResc->GetMouseButton(KEYCODE::MOUSE_RIGHT))
    {
      float sensitivity = 0.01f; // tweak
      float yaw = inputResc->mouseDelta.x * sensitivity * -1.0f;
      float pitch = inputResc->mouseDelta.y * sensitivity;

      if (glm::dot(camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) > 0.99f && pitch < 0.0f)
        pitch = 0.0f;

      if (glm::dot(camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) > 0.99f && pitch > 0.0f)
        pitch = 0.0f;

      // build incremental quaternion from mouse movement
      Quat qYaw = Quat::FromAxisAngle(yaw, glm::vec3(0, 1, 0));
      Quat qPitch = Quat::FromAxisAngle(pitch, glm::vec3(1, 0, 0));

      camTransform.rotationQuaternion = qYaw * camTransform.rotationQuaternion;
      camTransform.rotationQuaternion = camTransform.rotationQuaternion * qPitch;
      camTransform.rotationQuaternion = camTransform.rotationQuaternion.Normalize(); // keep it clean

      camera.forward = camTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    //rotate the "jack" object using its quaternion
    if (inputResc->GetMouseButton(KEYCODE::MOUSE_LEFT))
    {
      Transform& jackTransform = Engine::GetInstance()->GetComponent<Transform>(jack);

      float sensitivity = 0.01f; // tweak
      float yaw = inputResc->mouseDelta.x * sensitivity;
      float pitch = inputResc->mouseDelta.y * sensitivity;

      if (glm::dot(jackTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) > 0.99f && pitch < 0.0f)
        pitch = 0.0f;

      if (glm::dot(jackTransform.rotationQuaternion * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) > 0.99f && pitch > 0.0f)
        pitch = 0.0f;

      Quat qYaw = Quat::FromAxisAngle(yaw, glm::vec3(0, 1, 0));
      Quat qPitch = Quat::FromAxisAngle(pitch, glm::vec3(1, 0, 0));

      jackTransform.rotationQuaternion = qYaw * jackTransform.rotationQuaternion;
      jackTransform.rotationQuaternion = jackTransform.rotationQuaternion * qPitch;

      jackTransform.rotationQuaternion = jackTransform.rotationQuaternion.Normalize(); // keep it clean
    }


  }

  void Sandbox::PostUpdate()
  {
  }

  void Sandbox::Exit()
  {
  }

}