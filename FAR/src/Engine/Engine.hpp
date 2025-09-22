#pragma once

#include "PCH.hpp"

//temp
#include "GL/glew.h"


typedef unsigned long long Entity;

struct Transform
{
  glm::vec3 position{ 0.0f, 0.0f, 0.0f};
  glm::vec3 rotation{ 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{ 1.0f, 1.0f, 1.0f};
  glm::mat4 modelMatrix{ 1.0f };
};

struct Model
{
  std::string path{""};
  GLuint texArray;

  //gpu vao and corresponding index count
  std::vector<std::pair<GLuint, unsigned int>> VAOs;

  //map vao to textures for that mesh
  std::map<GLuint, std::vector<GLuint>> textures;

  int indexCount{ 0 };
  bool textured = false;
  glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

struct Camera
{
  float fov{ 90.0f };
  float nearPlane{ 1.0f };
  float farPlane{ 1000.0f };
  glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
  glm::vec3 up{ 0.0f, 1.0f, 0.0f };

  bool isMain{ false };
};

class Engine
{
public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  Engine()
  {
    return;
  }

  static Engine* eng;
  static void SetInstance(Engine* instance)
  {
    eng = instance;
  }

  static Engine* GetInstance()
  {
    return eng;
  }


  void Init();
  void PreUpdate();
  void Update();
  void PostUpdate();
  void Exit();

  Entity CreateEntity();
  void CreateModel(Entity e, Model c);
  void CreateTransform(Entity e, Transform c);
  void CreateCamera(Entity e, Camera c);

  std::vector<Entity> entities;

  std::array<std::optional<Model>, 50>     models;
  std::array<std::optional<Transform>, 50> transforms;
  std::array<std::optional<Camera>, 50>    cameras;
};