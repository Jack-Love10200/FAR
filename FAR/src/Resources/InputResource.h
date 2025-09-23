#pragma once

#include "PCH.hpp"
#include "Resources/iResource.hpp"
#include "KeyCodes.hpp"

namespace FAR
{
  class InputResource// : public iResource
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

    std::unordered_map<KEYCODE, bool> currentKeyStates;
    std::unordered_map<KEYCODE, bool> previousKeyStates;

    std::unordered_map<KEYCODE, bool> currentMouseButtonStates;
    std::unordered_map<KEYCODE, bool> previousMouseButtonStates;

    glm::vec3 mousePosition{ 0.0f, 0.0f, 0.0f };
    glm::vec3 previousMousePosition{ 0.0f, 0.0f, 0.0f };
  private:
  };
}