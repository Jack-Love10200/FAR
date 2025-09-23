#include "PCH.hpp"
#include "Engine.hpp"




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
    for (iSystem* system : systems)
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
