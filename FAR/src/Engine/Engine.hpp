///
/// @file   Engine.hpp
/// @brief  Entitiy/Component/System/Resource manager (ECS) game engine core.
/// @author Jack Love
/// @date   11.10.2025
///
#pragma once

#include "PCH/PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Resources/iResource.hpp"

//Entities are just indexes into sparese component arrays
typedef unsigned long long Entity;

namespace FAR
{
  class Engine
  {
  public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    Engine() = default;

    //singleton instance, temporary
    static Engine* eng;
    static void SetInstance(Engine* instance){eng = instance;}
    static Engine* GetInstance(){return eng;}

    float dt = 0.016f;//delta time

    //Engine steps
    void Init();
    void PreUpdate();
    void Update();
    void PostUpdate();
    void Exit();

    /// @brief  Create a new entity for use in the ECS
    /// @retval  - The new entity's ID
    Entity CreateEntity();

    //TODO: Enforce registered types are valid component/system/resource types
    
    /// @brief  Register a system type with the engine, instatiating an instance of it
    /// @tparam T - The type of the system to register
    template<typename SystemType>
    void RegisterSystemType();

    /// @brief  Register ac component type with the engine, constructing an intance of it
    /// @tparam ResourceType - The type of the resource to register
    /// @tparam Construction - Parameter pack for to pass to the resource's constructor
    /// @param  params       - Parameter pack for to pass to the resource's constructor
    template<typename ResourceType, typename... Construction>
    inline void RegisterResource(Construction&&... params);

    /// @brief  Get a resource by type
    /// @tparam ResourceType - The type of resource to get
    /// @retval              - The resource of the specified type
    template<typename ResourceType>
    ResourceType* GetResource();

    /// @brief  Register a component type with the engine, creating a storage array for it
    /// @tparam ComponentType - The type of component to register
    template<typename ComponentType>
    void RegisterComponentType();

    /// @brief  Add a component to an entity
    /// @tparam ComponentType - The type of component to add
    /// @param  e             - The entity to add the component to
    /// @param  component     - The component to add
    template<typename ComponentType>
    void AddComponent(Entity e, const ComponentType& component);

    /// @brief  Get all of the entities that have all of the specified component types
    /// @tparam ComponentTypes - The types of components the entities must have
    /// @retval                - All of the entities that have all of the specified component types
    template<typename... ComponentTypes>
    std::vector<Entity> GetEntities();

    /// @brief  Check if an entity has a component of the specified type
    /// @tparam ComponentType - The type of component to check for
    /// @param  e             - The entity to check
    /// @retval               - Whether the entity has the specified component type or not
    template<typename ComponentType>
    bool HasComponent(Entity e);

    /// @brief  Get a reference to a specified component on a specified entity
    /// @tparam ComponentType - The component type to get a ref to
    /// @param  e             - The entity to get the component from
    /// @retval               - A reference to the specified component on the specified entity
    template<typename ComponentType>
    ComponentType& GetComponent(Entity e);

    //somewhat temporary getters for entities and component types
    const std::vector <std::type_index>& GetComponentTypes() { return componentTypes; }
    const std::vector<Entity>& GetAllEntities() { return entities; }

  private:
    std::vector<Entity> entities; //all of the entities in the ecs

    std::chrono::high_resolution_clock::time_point last; //last frame time point
    std::chrono::high_resolution_clock::time_point current; //current frame time point

    std::vector<std::type_index> componentTypes; //all of the registered component types

    std::vector<iSystem*> systems;//instances of all registered systems

    std::unordered_map<std::type_index, std::unique_ptr<iResource>> resources;//instances of all registered resources

    std::map<std::type_index, std::function<void(Entity, const void*)>> componentAddFuncs;//lambdas for adding components to the static component arrays
    std::map<std::type_index, std::function<void* (Entity)>> componentGetFuncs;//lambdas for getting components from the static component arrays
  };
}// namespace FAR


#include "Engine.inl"