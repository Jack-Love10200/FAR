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
#include "Systems/ScriptedMotion/ScriptedMotion.hpp"

//components
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"
#include "Components/SkeletalAnimator.hpp"
#include "Components/ScriptedMotionPath.hpp"

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
void BusyWaitFrameRateControl(float fps)
{
  int nanoseconds_per_frame = (int)(1.0f / fps * 1'000'000'000.0f);
  using clock = std::chrono::steady_clock;
  static auto next_time = clock::now();
  static const auto frame_duration = std::chrono::steady_clock::duration(nanoseconds_per_frame);

  next_time += frame_duration;

  auto now = clock::now();
  if (now < next_time) {
    while (clock::now() < next_time) {
      std::atomic_signal_fence(std::memory_order_seq_cst); // prevent optimization
    }
  }
  else {
    // If we’re behind, re-sync
    next_time = now;
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
  engine.RegisterComponentType<ScriptedMotionPath>();

  engine.RegisterSystemType<FAR::EditorSystem>();
  engine.RegisterSystemType<FAR::Render>();
  engine.RegisterSystemType<FAR::SkeletalAnimation>();
  engine.RegisterSystemType<FAR::ScriptedMotion>();
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
    BusyWaitFrameRateControl(144.0f);
  }

  engine.Exit();
}