///
/// @file   Engine.cpp
/// @brief  Entitiy/Component/System/Resource manager (ECS) game engine core.
/// @author Jack Love
/// @date   11.10.2025
///
#include "PCH/PCH.hpp"
#include "Engine.hpp"
#include <ranges>

namespace FAR
{
  Engine* Engine::eng = nullptr;

  void Engine::Init()
  {
    for (iSystem* system : systems)
    {
      system->Init();
    }

    current = std::chrono::high_resolution_clock::now();
  }

  void Engine::PreUpdate()
  {
    for (iSystem* system : systems)
    {
      system->PreUpdate();
    }
  }

  void Engine::Update()
  {
    for (iSystem* system : systems)
    {
      system->Update();
    }
  }

  void Engine::PostUpdate()
  {
    for (iSystem* system : systems | std::views::reverse)
    {
      system->PostUpdate();
    }

    last = current;
    current = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> dtf = current - last;

    dt = dtf.count();
    dt = 1.0f / 60.0f;
  }

  void Engine::Exit()
  {
    for (iSystem* system : systems | std::views::reverse)
    {
      system->Exit();
    }
  }

  //TODO: make entity recycling system
  Entity Engine::CreateEntity()
  {
    static Entity currentEntity = 0;
    entities.push_back(currentEntity);
    currentEntity++;

    return currentEntity - 1;
  }

}
