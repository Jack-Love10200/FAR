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

  std::vector<Entity> entities;

  template<typename T>
  void RegisterComponentType()
  {
    static std::array<std::optional<T>, 50> componentArray;

    std::type_index typeIndex = std::type_index(typeid(T));
    componentTypes.push_back(typeIndex);

    componentCreationFuncs.emplace(std::make_pair(typeIndex, [this](Entity e, const void* componentdata) 
      {
        const T* component = static_cast<const T*>(componentdata);
        componentArray[e] = *component;
      }));

    componentGetFuncs.emplace(std::make_pair(typeIndex, [this](Entity e) -> void*
      {
        return componentArray[e].has_value() ? &componentArray[e].value() : nullptr;
      }));

    //componentArrays.emplace(std::make_pair(typeIndex ,new unsigned char[sizeof(T) * 50]));
  }

  template<typename T>
  void CreateComponent(Entity e, const T& component)
  {
    //componentArray[e] = component;
    /*std::type_index typeIndex = std::type_index(typeid(T));
    unsigned char* array = componentArrays[typeIndex];

    unsigned char* componentPtr = array + (sizeof(T) * e);
    new (componentPtr) T(component);
    */

    std::type_index typeIndex = std::type_index(typeid(T));
    componentCreationFuncs[typeIndex](e, &component);
  }

  template<typename... ComponentTypes>
  std::vector<Entity> GetEntities()
  {
    std::vector<Entity> result;

    for (Entity e : entities)
    {
      bool hasAllComponents = (... && HasComponent<ComponentTypes>(e));
      if (hasAllComponents)
        result.push_back(e);
    }
    return result;
  }

  template<typename T>
  T& GetComponent(Entity e)
  {
    std::type_index typeIndex = std::type_index(typeid(T));
    return *(static_cast<T*>(componentGetFuncs[typeIndex](e)));
  }

  template<typename T>
  bool HasComponent(Entity e)
  {
    std::type_index typeIndex = std::type_index(typeid(T));
    return componentGetFuncs[typeIndex](e) != nullptr;
  }
  
  //std::map<std::type_index, unsigned char*> componentArrays;
  std::vector<std::type_index> componentTypes;

  std::map<std::type_index, std::function<void(Entity, const void*)>> componentCreationFuncs;

  std::map<std::type_index, std::function<void*(Entity)>> componentGetFuncs;



  //std::array<std::optional<Model>, 50>     models;
  //std::array<std::optional<Transform>, 50> transforms;
  //std::array<std::optional<Camera>, 50>    cameras;
};