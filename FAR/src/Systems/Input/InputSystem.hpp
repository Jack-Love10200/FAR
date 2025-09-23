#pragma once

#include "PCH.hpp"
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
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;
  private:

    InputResource* inputResc;
    WindowResource* windowResc;
  };
}