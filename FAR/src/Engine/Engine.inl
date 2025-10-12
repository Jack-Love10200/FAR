///
/// @file   Engine.inl
/// @brief  Template inline implementations for the ECS Engine.
/// @author Jack Love
/// @date   11.10.2025
///

namespace FAR
{
  template<typename SystemType>
  void Engine::RegisterSystemType()
  {
    static SystemType system;
    systems.push_back(&system);
  }

  template<typename ResourceType, typename... Construction>
  inline void Engine::RegisterResource(Construction&&... params)
  {
    std::type_index typeIndex = typeid(ResourceType);
    std::unique_ptr<ResourceType> ptr = std::make_unique<ResourceType>(std::forward<Construction>(params)...);
    resources.emplace(typeIndex, std::move(ptr));
  }

  template<typename ResourceType>
  ResourceType* Engine::GetResource()
  {
    std::type_index typeIndex = std::type_index(typeid(ResourceType));
    return static_cast<ResourceType*> (resources.at(typeIndex).get());
  }

  template<typename ComponentType>
  void Engine::RegisterComponentType()
  {
    static std::array<std::optional<ComponentType>, 50> componentArray;

    std::type_index typeIndex = std::type_index(typeid(ComponentType));
    componentTypes.push_back(typeIndex);

    componentAddFuncs.emplace(std::make_pair(typeIndex, [this](Entity e, const void* componentdata)
      {
        const ComponentType* component = static_cast<const ComponentType*>(componentdata);
        componentArray[e] = *component;
      }));

    componentGetFuncs.emplace(std::make_pair(typeIndex, [this](Entity e) -> void*
      {
        return componentArray[e].has_value() ? &componentArray[e].value() : nullptr;
      }));
  }

  template<typename ComponentType>
  void Engine::AddComponent(Entity e, const ComponentType& component)
  {
    std::type_index typeIndex = std::type_index(typeid(ComponentType));
    componentAddFuncs[typeIndex](e, &component);
  }

  template<typename... ComponentTypes>
  std::vector<Entity> Engine::GetEntities()
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


  template<typename ComponentType>
  bool Engine::HasComponent(Entity e)
  {
    std::type_index typeIndex = std::type_index(typeid(ComponentType));
    return componentGetFuncs[typeIndex](e) != nullptr;
  }

  template<typename ComponentType>
  ComponentType& Engine::GetComponent(Entity e)
  {
    std::type_index typeIndex = std::type_index(typeid(ComponentType));
    return *(static_cast<ComponentType*>(componentGetFuncs[typeIndex](e)));
  }
}