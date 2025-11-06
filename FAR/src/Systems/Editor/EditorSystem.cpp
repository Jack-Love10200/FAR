///
/// @file   EditorSystem.cpp
/// @brief  GUI system for editing entities, components, scenes, etc.
/// @author Jack Love
/// @date   11.10.2025
///

#include "PCH/PCH.hpp"

#include "EditorSystem.hpp"



//TODO: Create registry for this instead of having all imlemented in the editor system
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"
#include "Components/SkeletalAnimator.hpp"
#include "Components/ScriptedMotionPath.hpp"
#include "Components/IKPoser.hpp"

//for piecewise linear integral for the scripted motion path speed curve
#include "Util/MathHelpers.hpp"


namespace FAR
{

  void EditorSystem::Init()
  {
    const char* glsl_version = "#version 460 core";
    windowResc = Engine::GetInstance()->GetResource<WindowResource>();
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();
    inputResc = Engine::GetInstance()->GetResource<InputResource>();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* c = ImGui::CreateContext();
    ImGui::SetCurrentContext(c);

    ImPlot::CreateContext();


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
    ImGuizmo::BeginFrame();
  }

  void EditorSystem::Update()
  {
    ImGui::DockSpaceOverViewport();
    RenderSceneView();
    RenderSceneHierarchy();
    RenderInspector();
    RenderDetailsPanel();
  }

  void EditorSystem::PostUpdate()
  {
    int display_w, display_h;
    glfwGetFramebufferSize(windowResc->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui::Render();
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void EditorSystem::Exit()
  {
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void EditorSystem::RenderSceneView()
  {
    ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoScrollbar);
    ImVec2 windowsize = ImGui::GetContentRegionAvail();
    renderResc->CreateFrameBuffer(windowsize.x, windowsize.y);

    ImVec2 pos = ImGui::GetCursorScreenPos();
    //pos = ImGui::GetWindowPos();
    
    
    ImGui::Image((void*)(intptr_t)renderResc->colorTex, windowsize, ImVec2(0, 1), ImVec2(1, 0));

    ImGuizmo::SetRect(pos.x, pos.y, windowsize.x, windowsize.y);

    RenderGuizmo();


    ImGui::End();
  }

  void EditorSystem::RenderGuizmo()
  {
    ImGuizmo::SetDrawlist();

    //super temp, just get the test cam for now
    Camera& cam = Engine::GetInstance()->GetComponent<Camera>(0);
    Transform& camTransform = Engine::GetInstance()->GetComponent<Transform>(0);

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::lookAt(camTransform.position, camTransform.position + cam.forward, cam.up);

    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(cam.fov), cam.aspect, cam.nearPlane, cam.farPlane);

    Transform& selectedTransform = Engine::GetInstance()->GetComponent<Transform>(selected);

    glm::mat4 modelMatrix = glm::translate(selectedTransform.position);
    //glm::mat4 modelMatrix = selectedTransform.modelMatrix;

    glm::vec3 oldPos = selectedTransform.position;
    glm::vec3 oldScl = selectedTransform.scale;
    //glm::vec3 oldRot = 

    if (ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
      guizmoOperation, ImGuizmo::WORLD,
      glm::value_ptr(modelMatrix)))
    {
      if (!gizmoActive)
      {
        gizmoTrackPos = glm::vec3(0.0f);
        gizmoTrackRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        gizmoStartPos = selectedTransform.position;
        //gizmoStartRot = glm::eulerAngles(glm::quat_cast(selectedTransform.rotationQuaternion.ToMatrix()));
        gizmoStartScl = selectedTransform.scale;
        gizmoActive = true;
        std::cout << "Gizmo activated\n";
      }
      //selectedTransform.matManuallyModified = true;

      //decompose matrix back into position, rotation, scale
      glm::vec3 dummyPosition;
      glm::vec3 dummyRotation;
      glm::vec3 dummyScale;

      ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix),
        glm::value_ptr(dummyPosition),
        glm::value_ptr(dummyRotation),
        glm::value_ptr(dummyScale));

      if (guizmoOperation == ImGuizmo::TRANSLATE)
      {
        gizmoTrackPos += dummyPosition - oldPos;

        //selectedTransform.position = gizmoStartPos + dummyPosition;
        selectedTransform.position = gizmoTrackPos + gizmoStartPos;
        //std::cout << dummyPosition.x << std::endl;
      }

      else if (guizmoOperation == ImGuizmo::SCALE)
      {
        gizmoTrackPos += dummyScale - oldScl;
        selectedTransform.scale = gizmoStartScl * dummyScale;
      }

      else if (guizmoOperation == ImGuizmo::ROTATE)
      {
        glm::quat rotQuat = glm::quat(glm::radians(dummyRotation));
        selectedTransform.rotationQuaternion = selectedTransform.rotationQuaternion * rotQuat;
      }


      //glm::quat rotQuat = glm::quat_cast(selectedTransform.modelMatrix);

    }
    else if (gizmoActive && (!inputResc->GetMouseButton(KEYCODE::MOUSE_LEFT))) // Reset the guizmo tracker if the user is not manipulating the guizmo
    {
      gizmoActive = false;
      std::cout << "Gizmo deactivated\n";
    }
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
        //ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
        ImGui::DragFloat4("RotationQuat", (float*) & transform.rotationQuaternion, 0.1f);
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

    //Skeletal Animator Component
    if (selected != -1 && Engine::GetInstance()->HasComponent<SkeletalAnimator>(selected))
    {
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(selected);
      if (ImGui::TreeNode("Skeltal Animator Component"))
      {
        ImGui::Text(sk.path.c_str());
        ImGui::Checkbox("Looping", &sk.looping);
        ImGui::Checkbox("Playing", &sk.playing);

        ImGui::DragFloat("Animation Time", &sk.animationTime, 0.1f, 0, sk.animations[sk.currentAnimation].duration);

        ImGui::DragFloat("Playback Speed", &sk.playbackSpeed, 0.1f, 0.1f, 10.0f);

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

    //Scripted Motion Path Component
    if (selected != -1 && Engine::GetInstance()->HasComponent<ScriptedMotionPath>(selected))
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(selected);
      if (ImGui::TreeNode("Scripted Motion Path Component"))
      {
        for (int i = 0; i < smp.controlPoints.size(); i++)
        {
          std::string label = "Control Point " + std::to_string(i);

          if (ImGui::DragFloat3(label.c_str(), &smp.controlPoints[i][0], 0.1f))
            smp.isDirty = true;

          if (ImGui::Button(("Remove##" + std::to_string(i)).c_str()))
          {
            smp.controlPoints.erase(smp.controlPoints.begin() + i);
            smp.isDirty = true;
          }
        }

        if (ImGui::Button("Add Control Point"))
        {
          smp.controlPoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
          smp.isDirty = true;
        }

        for (int i = 0; i < smp.velocityKeys.size(); i++)
        {
          //ImGui::Text("Time: %.2f, Velocity: %.2f", time, vel);

          //ensure time keys are in order
          float timeMin = (i == 0) ? 0.0f : smp.velocityKeys[i - 1].first + 0.01f;
          float timeMax = (i == smp.velocityKeys.size() - 1) ? 1.0f : smp.velocityKeys[i + 1].first - 0.01f;

          float velMin = 0.0f;
          float velMax = 1.0f;

          if (ImGui::DragFloat(("Time##" + std::to_string(i)).c_str(), (float*)&smp.velocityKeys[i].first, 0.01f, timeMin, timeMax))
            smp.isDirty = true;
          //ImGui::SameLine();

          if (ImGui::DragFloat(("Velocity##" + std::to_string(i)).c_str(), (float*)&smp.velocityKeys[i].second, 0.01f, velMin, velMax))
            smp.isDirty = true;
        }

        //velocity/time plot
        if (ImPlot::BeginPlot("Velocity Curve"))
        {
          ImPlot::SetupAxes("Time", "Velocity");
          std::vector<float> times;
          std::vector<float> velocities;
          for (const auto& [time, vel] : smp.velocityKeys)
          {
            times.push_back(time);
            velocities.push_back(vel);
          }
          ImPlot::PlotLine("Velocity", times.data(), velocities.data(), (int)times.size());
          ImPlot::EndPlot();
        }

        float increment = 1.0f / 100.0f;
        float currenttime = 0.0f;

        //postion/time plot
        if (ImPlot::BeginPlot("Position Curve"))
        {
          ImPlot::SetupAxes("Time", "Position");
          std::vector<float> positions;
          std::vector<float> posTimes;

          for (int i = 0; i < 100; i++)
          {
            currenttime += increment;
            float pos = GetPiecewiseLinearIntegral(smp.velocityKeys, currenttime);
            
            positions.push_back(pos);
            posTimes.push_back(currenttime);
          }

          //normalize positions range to [0,1]
          float maxPos = positions.back();
          std::for_each(positions.begin(), positions.end(), [maxPos](float& p) { p /= maxPos; });

          ImPlot::PlotLine("Position", posTimes.data(), positions.data(), (int)posTimes.size());
          ImPlot::EndPlot();
        }

        ImGui::DragFloat("Total Time", &smp.totalTime, 0.1f, 0.1f, 100.0f);

        ImGui::TreePop();
      }
    }

    if (selected != -1 && Engine::GetInstance()->HasComponent<IKPoser>(selected))
    {
      IKPoser& ik = Engine::GetInstance()->GetComponent<IKPoser>(selected);
      if (ImGui::TreeNode("IK Poser Component"))
      {
        ImGui::Text(("Target Entity: " + std::to_string(ik.target)).c_str());
        ImGui::Text("Manipulators:");
        for (int i = 0; i < ik.manipulator.size(); i++)
        {
          ImGui::Text(("%d: Bone Index " + std::to_string(ik.manipulator[i])).c_str());
        }
        ImGui::TreePop();
      }
    }


    ImGui::End();
  }

  void EditorSystem::RenderDetailsPanel()
  {
    ImGui::Begin("Details");
    ImGui::Text("FPS: %.1f", 1.0f / Engine::GetInstance()->dt);

    //radio buttons for gizmo mode
    if (ImGui::RadioButton("Translate", guizmoOperation == ImGuizmo::TRANSLATE))
      guizmoOperation = ImGuizmo::TRANSLATE;
  
    if (ImGui::RadioButton("Rotate", guizmoOperation == ImGuizmo::ROTATE))
      guizmoOperation = ImGuizmo::ROTATE;

    if (ImGui::RadioButton("Scale", guizmoOperation == ImGuizmo::SCALE))
      guizmoOperation = ImGuizmo::SCALE;

    ImGui::End();
  }
}
