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
#include "Resources/InputResource.h"

#include "Engine/Engine.hpp"

// IMGUI
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

//Guizmo
#include <ImGuizmo.h>

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
    void RenderGuizmo();

    //The entity currently selected in the heirarchy view
    Entity selected = 0;

    WindowResource* windowResc = nullptr;
    RenderResource* renderResc = nullptr;
    InputResource* inputResc = nullptr;

    ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;
    glm::vec3 gizmoStartPos;
    glm::quat gizmoStartRot;
    glm::vec3 gizmoStartScl;

    glm::vec3 gizmoTrackPos;
    glm::quat gizmoTrackRot;

    bool gizmoActive = false;
  };
} // namespace FAR
