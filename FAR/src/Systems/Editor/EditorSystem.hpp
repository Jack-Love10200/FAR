#pragma once

#include "Systems/iSystem.hpp"
#include "Resources/RenderResource.hpp"
#include "Resources/WindowResource.hpp"


namespace FAR
{
  class EditorSystem : public iSystem
  {
  public:
    EditorSystem() = default;
    ~EditorSystem() = default;
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;
  private:

    WindowResource* windowResc = nullptr;
    RenderResource* renderResc = nullptr;

  };
} // namespace FAR
