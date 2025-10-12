///
/// @file   EditorSystem.hpp
/// @brief  GUI system for editing entities, components, scenes, etc.
/// @author Jack Love
/// @date   11.10.2025
///
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

    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;

  private:

    void RenderSceneView();
    void RenderSceneHierarchy();
    void RenderInspector();
    void RenderDetailsPanel();

    //The entity currently selected in the heirarchy view
    Entity selected = 0;

    WindowResource* windowResc = nullptr;
    RenderResource* renderResc = nullptr;
  };
} // namespace FAR
