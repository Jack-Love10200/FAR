#include "PCH.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize2.h"

#include "Engine/Engine.hpp"

//main depends on render for now, will resolve silly dependencies later
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

  while (true)
  {
    engine.PreUpdate();
    engine.Update();
    engine.PostUpdate();
  }

  engine.Exit();
}