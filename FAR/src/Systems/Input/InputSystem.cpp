#include "PCH.hpp"
#include "InputSystem.hpp"
#include "GLFW/glfw3.h"

namespace FAR
{
  void InputSystem::Init()
  {
    inputResc = Engine::GetInstance()->GetResource<InputResource>();
    windowResc = Engine::GetInstance()->GetResource<WindowResource>();

    for (int key = 32; key <= 348; key++)
    {
      inputResc->currentKeyStates[(KEYCODE)key] = false;
      inputResc->previousKeyStates[(KEYCODE)key] = false;
    }
  }

  void InputSystem::PreUpdate()
  {
  }

  void InputSystem::Update()
  {
  }

  void InputSystem::PostUpdate()
  {
    inputResc->previousKeyStates = inputResc->currentKeyStates;

    for (int key = 32; key <= 348; key++)
    {
      inputResc->currentKeyStates[(KEYCODE)key] = glfwGetKey(windowResc->window, key);
    }

    inputResc->previousMousePosition = inputResc->mousePosition;
    
    double xpos, ypos;
    glfwGetCursorPos(windowResc->window, &xpos, &ypos);
    inputResc->mousePosition = glm::vec2((float)xpos, (float)ypos);
    inputResc->mouseDelta = inputResc->mousePosition - inputResc->previousMousePosition;

    inputResc->previousMouseButtonStates = inputResc->currentMouseButtonStates;
    for (int button = 0; button <= 7; button++)
    {
      inputResc->currentMouseButtonStates[(KEYCODE)button] = glfwGetMouseButton(windowResc->window, button);
    }

  }
  void InputSystem::Exit()
  {
  }
}