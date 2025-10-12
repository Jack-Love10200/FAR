////////////////////////////////////////////////////////////////////////////////
///
/// @file   main.cpp
/// @brief  Entry point for the FAR engine, registers components, systems, 
/// and resources, then enters the main loop.
/// @author Jack Love
/// @date   11.10.2025
///
////////////////////////////////////////////////////////////////////////////////

#include "PCH/PCH.hpp"
#include "Engine/Engine.hpp"

//systems
#include "Systems/Render/Render.hpp"
#include "Systems/Sandbox/Sandbox.hpp"
#include "Systems/Input/InputSystem.hpp"
#include "Systems/Animation/SkeletalAnimation.hpp"
#include "Systems/Editor/EditorSystem.hpp"

//components
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"
#include "Components/SkeletalAnimator.hpp"

//resources
#include "Resources/InputResource.h"
#include "Resources/WindowResource.hpp"
#include "Resources/RenderResource.hpp"

//TASKS
//TODO: Custom logger
//TODO: Asset libraries/loaders
//TODO: Scene management
//TODO: Serialization
//TDOO: RenderResource Reorganization
//TODO: Make IMGUI/STB prebuilt libraies

//TODO: Proper frame rate controler build into engine class
void BusyWaitFrameRateControl(float targetFrameTime)
{
  auto start = std::chrono::high_resolution_clock::now();
  while (true)
  {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - start;
    if (elapsed.count() >= targetFrameTime)
      break;
  }
} 

int main()
{
  FAR::Engine engine;
  FAR::Engine::SetInstance(&engine);

  engine.RegisterComponentType<Transform>();
  engine.RegisterComponentType<Camera>();
  engine.RegisterComponentType<Model>();
  engine.RegisterComponentType<SkeletalAnimator>();

  engine.RegisterSystemType<FAR::EditorSystem>();
  engine.RegisterSystemType<FAR::Render>();
  engine.RegisterSystemType<FAR::SkeletalAnimation>();
  engine.RegisterSystemType<FAR::Sandbox>();
  engine.RegisterSystemType<FAR::InputSystem>();

  engine.RegisterResource<FAR::InputResource>();
  engine.RegisterResource<FAR::WindowResource>();
  engine.RegisterResource<FAR::RenderResource>();

  engine.Init();

  while (!engine.GetResource<FAR::WindowResource>()->isWindowClosing())
  {
    engine.PreUpdate();
    engine.Update();
    engine.PostUpdate();
    BusyWaitFrameRateControl(1.0f / 60.0f);
  }

  engine.Exit();
}