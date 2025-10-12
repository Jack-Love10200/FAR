///
/// @file   InputResource.h
/// @brief  Resrouce for storing and retrieving state
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once

#include "PCH/PCH.hpp"
#include "Resources/iResource.hpp"
#include "KeyCodes.hpp"

namespace FAR
{
  class InputResource : public iResource
  {
  public:
    InputResource();// = default;
    ~InputResource() = default;

    bool GetKeyTriggered(KEYCODE key)
    {
      return currentKeyStates[key] && !previousKeyStates[key];
    }

    bool GetKey(KEYCODE key)
    {
      return currentKeyStates[key];
    }

    bool GetKeyReleased(KEYCODE key)
    {
      return !currentKeyStates[key] && previousKeyStates[key];
    }

    bool GetMouseButtonTriggered(KEYCODE button)
    {
      return currentMouseButtonStates[button] && !previousMouseButtonStates[button];
    }

    bool GetMouseButton(KEYCODE button)
    {
      return currentMouseButtonStates[button];
    }

    bool GetMouseButtonReleased(KEYCODE button)
    {
      return !currentMouseButtonStates[button] && previousMouseButtonStates[button];
    }
    

    glm::vec2 mousePosition{ 0.0f, 0.0f};
    glm::vec2 previousMousePosition{ 0.0f, 0.0f};
    glm::vec2 mouseDelta{ 0.0f, 0.0f};

    //key&button states
    std::unordered_map<KEYCODE, bool> currentKeyStates;
    std::unordered_map<KEYCODE, bool> previousKeyStates;
    std::unordered_map<KEYCODE, bool> currentMouseButtonStates;
    std::unordered_map<KEYCODE, bool> previousMouseButtonStates;

  };
}