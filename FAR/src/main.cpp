
#include "PCH.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize2.h"

#include "Engine/Engine.hpp"

//main depends on render for now, will resolve silly dependencies later
#include "Engine/Render.hpp"

int main()
{
  Engine engine;
  Engine::SetInstance(&engine);

  engine.Init();

  Entity cam = engine.CreateEntity();

  engine.CreateCamera(cam, Camera{ .isMain = true });
  engine.CreateTransform(cam, Transform{ .position = glm::vec3(0.0f, 0.0f, 0.0f) });

  Entity jack = engine.CreateEntity();
  engine.CreateModel(jack, Model{ .path = "assets/jack_samba.glb", .textured = true });
  engine.CreateTransform(jack, Transform{ .position = glm::vec3(0.0f, 0.0f, -3.0f), .rotation = glm::vec3(-90.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f)});

  Entity jack2 = engine.CreateEntity();
  engine.CreateModel(jack2, Model{ .path = "assets/Adi_Dancing.fbx", .textured = false, });
  engine.CreateTransform(jack2, Transform{ .position = glm::vec3(1.0f, 0.0f, -3.0f), .rotation = glm::vec3(-90.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

  Entity jack3 = engine.CreateEntity();
  engine.CreateModel(jack3, Model{ .path = "assets/okayu/okayu.pmx", .textured = true });
  engine.CreateTransform(jack3, Transform{ .position = glm::vec3(-10.0f, 0.0f, -30.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = glm::vec3(1.0f, 1.0f, 1.0f) });

  while (RenderGetWindowIsOpen())
  {
    engine.PreUpdate();
    engine.Update();
    engine.PostUpdate();
  }

  engine.Exit();

    std::cout << "Hello World!\n";
}
