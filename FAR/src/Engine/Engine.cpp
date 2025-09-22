#include "PCH.hpp"
#include "Engine.hpp"

//Engine depends on the systems for now, will make systems register themselves later
#include "Render.hpp"

Engine* Engine::eng = nullptr;

void Engine::Init()
{
  RenderInit();
}

void Engine::PreUpdate()
{
  RenderPreUpdate();
}

void Engine::Update()
{
  RenderUpdate();
}

void Engine::PostUpdate()
{
  RenderPostUpdate();
}

void Engine::Exit()
{
  RenderExit();
}

Entity Engine::CreateEntity()
{
  static Entity currentEntity = 0;
  entities.push_back(currentEntity);
  currentEntity++;

  return currentEntity - 1;
}
