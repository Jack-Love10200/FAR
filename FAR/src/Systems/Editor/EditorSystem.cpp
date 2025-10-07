#include "PCH.hpp"

#include "EditorSystem.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Engine/Engine.hpp"

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

    int windowWidth, windowHeight;
    windowWidth = 1280;
    windowHeight = 720;
    //glfwGetWindowSize(windowResc->window, (int*)&windowWidth, (int*)&windowHeight);

    ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoScrollbar);

    ImVec2 windowsize = ImGui::GetContentRegionAvail();

    renderResc->CreateFrameBuffer(windowsize.x, windowsize.y);

    ImGui::Image((void*)(intptr_t)renderResc->colorTex, windowsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ImGui::ShowDemoWindow();
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

}
