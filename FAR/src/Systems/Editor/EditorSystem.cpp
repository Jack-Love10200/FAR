#include "PCH.hpp"

#include "EditorSystem.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Engine/Engine.hpp"


#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"
#include "Components/SkeletalAnimator.hpp"


namespace FAR
{

  void EditorSystem::Init()
  {
    const char* glsl_version = "#version 460 core";
    windowResc = Engine::GetInstance()->GetResource<WindowResource>();
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* c = ImGui::CreateContext();
    ImGui::SetCurrentContext(c);


    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(windowResc->window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);
    
  }

  void EditorSystem::PreUpdate()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void EditorSystem::Update()
  {
    ImGui::DockSpaceOverViewport();

    RenderSceneView();

    RenderSceneHierarchy();

    RenderInspector();

  }

  void EditorSystem::PostUpdate()
  {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(windowResc->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void EditorSystem::Exit()
  {

  }

  void EditorSystem::RenderSceneView()
  {
    ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoScrollbar);
    ImVec2 windowsize = ImGui::GetContentRegionAvail();
    renderResc->CreateFrameBuffer(windowsize.x, windowsize.y);
    ImGui::Image((void*)(intptr_t)renderResc->colorTex, windowsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
  }

  void EditorSystem::RenderSceneHierarchy()
  {
    ImGui::Begin("Scene Hierarchy");

    const std::vector<Entity>& entities = Engine::GetInstance()->GetAllEntities();

    for (const Entity& e : entities)
    {
      std::string label = "Entity " + std::to_string(e);

      bool is_selected = (selected == e);
      if (ImGui::Selectable(label.c_str(), &is_selected))
        selected = e; // only one can be selected
    }

    ImGui::End();
  }

  void EditorSystem::RenderInspector()
  {
    ImGui::Begin("Inspector");

    //Transform component
    if (selected != -1 && Engine::GetInstance()->HasComponent<Transform>(selected))
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(selected);
      if (ImGui::TreeNode("Transform Component"))
      {
        ImGui::DragFloat3("Position", &transform.position[0], 0.1f);
        ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
        ImGui::DragFloat3("Scale", &transform.scale[0], 0.1f);
        ImGui::TreePop();
      }
    }

    //Camera component
    if (selected != -1 && Engine::GetInstance()->HasComponent<Camera>(selected))
    {
      Camera& camera = Engine::GetInstance()->GetComponent<Camera>(selected);
      if (ImGui::TreeNode("Camera Component"))
      {
        ImGui::DragFloat("FOV", &camera.fov, 0.1f);
        ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f);
        ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f);
        ImGui::Checkbox("Is Main", &camera.isMain);
        ImGui::TreePop();
      }
    }

    //Model Component
    if (selected != -1 && Engine::GetInstance()->HasComponent<Model>(selected))
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(selected);
      if (ImGui::TreeNode("Model Component"))
      {
        ImGui::Text(model.path.c_str());
        ImGui::Checkbox("Textured", &model.textured);
        ImGui::TreePop();
      }
    }

    if (selected != -1 && Engine::GetInstance()->HasComponent<SkeletalAnimator>(selected))
    {
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(selected);
      if (ImGui::TreeNode("Skeltal Animator Component"))
      {
        ImGui::Text(sk.path.c_str());
        ImGui::Checkbox("Looping", &sk.looping);

        if (ImGui::BeginCombo("Animation", sk.animations[sk.currentAnimation].name.c_str()))
        {
          for (int i = 0; i < sk.animations.size(); i++)
          {
            const bool is_selected = (sk.currentAnimation == i);
            const char* animName = sk.animations[i].name.c_str();

            if (ImGui::Selectable(animName, is_selected))
              sk.currentAnimation = i;

            // Make sure the current item is focused if it’s selected
            if (is_selected)
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }
        ImGui::TreePop();
      }
    }


    ImGui::End();
  }

}
