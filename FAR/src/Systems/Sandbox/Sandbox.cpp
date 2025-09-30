#include "PCH.hpp"

#include "Sandbox.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"
#include "Resources/RenderResource.hpp"

namespace FAR
{
  void Sandbox::Init()
  {
    Engine& engine = *Engine::GetInstance();
    cam = engine.CreateEntity();
    engine.CreateComponent(cam, Camera{ .isMain = true });
    engine.CreateComponent(cam, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f) });

    jack = engine.CreateEntity();
    engine.CreateComponent(jack, Model{ .path = "assets/jack_samba.glb", .textured = true });
    engine.CreateComponent(jack, Transform{ .position = glm::vec3(0.0f, 0.0f, -3.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(0.1f, 0.1f, 0.1f) });

    //adi = engine.CreateEntity();
    //engine.CreateComponent(adi, Model{ .path = "assets/Adi_Dancing.fbx", .textured = false, });
    //engine.CreateComponent(adi, Transform{ .position = glm::vec3(1.0f, 0.0f, -3.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

    //okayu = engine.CreateEntity();
    //engine.CreateComponent(okayu, Model{ .path = "assets/okayu/okayu.pmx", .textured = true });
    //engine.CreateComponent(okayu, Transform{ .position = glm::vec3(-10.0f, 0.0f, -30.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) })/*;

    //pika = engine.CreateEntity();
    //engine.CreateComponent(pika, Model{ .path = "assets/pika.fbx", .textured = true });
    //engine.CreateComponent(pika, Transform{ .position = glm::vec3(-20.0f, 0.0f, -5.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });
  }

  void Sandbox::PreUpdate()
  {
  }

  void Sandbox::Update()
  {

    InputResource* inputResc = Engine::GetInstance()->GetResource<InputResource>();

    Engine::GetInstance()->GetResource<RenderResource>()->DrawRay(
      glm::vec4(0.0f, 0.0f, -4.0f, 1.0f),
      glm::vec4(0.0f, 1.0f, -4.0f, 1.0f));


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

      glm::mat4 rot = glm::mat4(1.0f);
      rot = glm::rotate(rot, glm::radians(-inputResc->mouseDelta.x * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
      rot = glm::rotate(rot, glm::radians(-inputResc->mouseDelta.y * 0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
      camera.forward = glm::vec3(rot * glm::vec4(camera.forward, 0.0f));
    }

    //rotate the "jack" object using its quaternion
    if (inputResc->GetMouseButton(KEYCODE::MOUSE_LEFT))
    {
      Transform& jackTransform = Engine::GetInstance()->GetComponent<Transform>(jack);

      float sensitivity = 0.01f; // tweak
      float yaw = inputResc->mouseDelta.x * sensitivity;
      float pitch = inputResc->mouseDelta.y * sensitivity;

      // build incremental quaternion from mouse movement
      glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
      glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));

      // combine them
      glm::quat rotationDelta = qYaw * qPitch;

      // update object’s orientation
      jackTransform.rotationQuaternion = rotationDelta * jackTransform.rotationQuaternion;
      jackTransform.rotationQuaternion = glm::normalize(jackTransform.rotationQuaternion); // keep it clean
    }


  }

  void Sandbox::PostUpdate()
  {
  }

  void Sandbox::Exit()
  {
  }

}