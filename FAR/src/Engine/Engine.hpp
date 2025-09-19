#pragma once

#include "PCH.hpp"

typedef unsigned long long Entity;

struct Transform
{
  glm::vec4 position{ 0.0f, 0.0f, 0.0f, 0.0f };
  glm::vec4 rotation{ 0.0f, 0.0f, 0.0f, 0.0f };
  glm::vec4 scale{ 1.0f, 1.0f, 1.0f, 0.0f };
};

struct Model
{

};

struct Camera
{
  float fov{ 90.0f };
  float nearPlane{ 0.1f };
  float farPlane{ 100.0f };
  glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
  glm::vec3 up{ 0.0f, 1.0f, 0.0f };
};

class Engine
{
public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  void Init();
  void Update();
  void Exit();

  void CreateEntity();

  std::vector<Entity> entities;

  std::array<std::optional<Model>, 50>     models;
  std::array<std::optional<Transform>, 50> transforms;
  std::array<std::optional<Camera>, 50>    cameras;
};