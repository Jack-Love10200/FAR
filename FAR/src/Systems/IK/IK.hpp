#pragma once

#include "PCH/PCH.hpp"

#include "Systems/iSystem.hpp"

namespace FAR
{
  class IK : public iSystem
  {
    public:
    IK() = default;
    ~IK() override = default;
    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;

  };
}