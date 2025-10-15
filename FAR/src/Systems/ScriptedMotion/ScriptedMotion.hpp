#pragma once

#include "PCH/PCH.hpp"

#include "Resources/RenderResource.hpp"

#include "Systems/iSystem.hpp"


namespace FAR
{
  class ScriptedMotion : public iSystem
  {
    public:
    virtual ~ScriptedMotion() = default;

    virtual void Init() override;
    virtual void PreUpdate() override;
    virtual void Update() override;
    virtual void PostUpdate() override;
    virtual void Exit() override;

  private:

    RenderResource* renderResc = nullptr;
  };
}