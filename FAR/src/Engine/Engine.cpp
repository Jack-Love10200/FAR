#include "PCH.hpp"
#include "Engine.hpp"

#include <ranges>


//Engine depends on the systems for now, will make systems register themselves later
#include "Systems/Render/Render.hpp"

namespace FAR
{
  Engine* Engine::eng = nullptr;

  void Engine::Init()
  {
    for (iSystem* system : systems)
    {
      system->Init();
    }
  }

  void Engine::PreUpdate()
  {
    dt = 0.00060606060606061; //placeholder for now, will implement proper timing later
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
    for (iSystem* system : systems)
    {
      system->PostUpdate();
    }
  }

  void Engine::Exit()
  {
    for (iSystem* system : systems | std::views::reverse)
    {
      system->Exit();
    }
  }

  Entity Engine::CreateEntity()
  {
    static Entity currentEntity = 0;
    entities.push_back(currentEntity);
    currentEntity++;

    return currentEntity - 1;
  }

}
