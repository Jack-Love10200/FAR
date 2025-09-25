#pragma once

#include "PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Components/Animation.hpp"
#include "Components/Model.hpp"

namespace FAR
{
  class ModelAnimation : iSystem
  {
  public:
    ModelAnimation() = default;
    ~ModelAnimation() override = default;
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;
  private:

    void LoadAnimations(Animation& animation);

  };
}