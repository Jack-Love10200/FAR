#include "PCH.hpp"

#include "Sandbox.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"

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
    engine.CreateComponent(jack, Transform{ .position = glm::vec3(0.0f, 0.0f, -3.0f), .rotation = glm::vec3(-90.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

    adi = engine.CreateEntity();
    engine.CreateComponent(adi, Model{ .path = "assets/Adi_Dancing.fbx", .textured = false, });
    engine.CreateComponent(adi, Transform{ .position = glm::vec3(1.0f, 0.0f, -3.0f), .rotation = glm::vec3(-90.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

    okayu = engine.CreateEntity();
    engine.CreateComponent(okayu, Model{ .path = "assets/okayu/okayu.pmx", .textured = true });
    engine.CreateComponent(okayu, Transform{ .position = glm::vec3(-10.0f, 0.0f, -30.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });
  }

  void Sandbox::PreUpdate()
  {
  }

  void Sandbox::Update()
  {
    const float speed = 0.1f;

    InputResource* inputResc = Engine::GetInstance()->GetResource<InputResource>();

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


  }

  void Sandbox::PostUpdate()
  {
  }

  void Sandbox::Exit()
  {
  }

}