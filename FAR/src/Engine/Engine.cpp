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
    for (iSystem* system : systems)
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

  Entity Engine::CreateEntity()
  {
    static Entity currentEntity = 0;
    entities.push_back(currentEntity);
    currentEntity++;

    return currentEntity - 1;
  }

}
