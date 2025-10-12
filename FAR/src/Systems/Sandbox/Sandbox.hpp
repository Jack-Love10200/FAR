///
/// @file   Sandbox.hpp
/// @brief  System for throwing throwing together tests, demos, and experiments
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once

#include "PCH/PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Engine/Engine.hpp"

#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"

namespace FAR
{
  class Sandbox : public iSystem
  {
  public:
    Sandbox() = default;
    ~Sandbox() override = default;

    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;

  private:

    Entity cam = 0;
  };
}