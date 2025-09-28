#pragma once

#include "PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Resources/iResource.hpp"


typedef unsigned long long Entity;

namespace FAR
{

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
    void RegisterSystemType()
    {
      static T system;
      systems.push_back(&system);
    }


    using void_unique_ptr = std::unique_ptr<void, void(*)(void*)>;

    template <typename T, typename... Args>
    void_unique_ptr make_unique_void_ptr(Args&&... args)
    {
      std::unique_ptr<T> ptr = std::make_unique<T>(std::forward<Args>(args)...);

      return void_unique_ptr(ptr.release(), [](void* p) { delete static_cast<T*>(p); });
    }


    template<typename T, typename... Construction>
    inline void RegisterResource(Construction&&... params)
    {
      std::type_index typeIndex = typeid(T);
      //std::unique_ptr<T> ptr = std::make_unique<T>();
      resources.emplace(typeIndex, make_unique_void_ptr<T>(std::forward<Construction>(params)...));
    }

    template<typename T>
    T* GetResource()
    {
      std::type_index typeIndex = std::type_index(typeid(T));
      return static_cast<T*> (resources.at(typeIndex).get());
    }

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

    std::vector<iSystem*> systems;

    std::unordered_map<std::type_index, void_unique_ptr > resources;


    std::map<std::type_index, std::function<void(Entity, const void*)>> componentCreationFuncs;
    std::map<std::type_index, std::function<void* (Entity)>> componentGetFuncs;
  };
}// namespace FAR