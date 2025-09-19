#include "PCH.hpp"
#include "Engine.hpp"

//Engine depends on the systems for now, will make systems register themselves later
#include "Render.hpp"

void Engine::Init()
{
  RenderInit();
}

void Engine::Update()
{
  RenderUpdate();
}

void Engine::Exit()
{
  RenderExit();
}

void Engine::CreateEntity()
{
  static Entity currentEntity = 0;
  entities.push_back(currentEntity);
  currentEntity++;
}
