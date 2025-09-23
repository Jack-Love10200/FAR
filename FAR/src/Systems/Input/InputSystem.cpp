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

  void InputSystem::PostUpdate()
  {
    inputResc->previousKeyStates = inputResc->currentKeyStates;

    for (int key = 32; key <= 348; key++)
    {
      inputResc->currentKeyStates[(KEYCODE)key] = glfwGetKey(windowResc->window, key);
    }
  }
}