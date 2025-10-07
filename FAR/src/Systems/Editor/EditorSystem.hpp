#pragma once

#include "Systems/iSystem.hpp"
#include "Resources/RenderResource.hpp"
#include "Resources/WindowResource.hpp"

#include "Engine/Engine.hpp"

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

    void RenderSceneView();

    void RenderSceneHierarchy();

    void RenderInspector();

    //super duper temp
    std::array<bool, 50> EntitySelectionStates = { false };

    Entity selected;

    WindowResource* windowResc = nullptr;
    RenderResource* renderResc = nullptr;

  };
} // namespace FAR
