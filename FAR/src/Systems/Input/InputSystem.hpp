///
/// @file   InputSystem.hpp
/// @brief  System for handling input from keyboard and mouse and storing it in the InputResource
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once

#include "PCH/PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Engine/Engine.hpp"

#include "Resources/InputResource.h"
#include "Resources/WindowResource.hpp"

namespace FAR
{
  class InputSystem : public iSystem
  {
  public:
    InputSystem() = default;
    ~InputSystem() override = default;

    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;
  private:

    InputResource* inputResc = nullptr;
    WindowResource* windowResc = nullptr;
  };
}