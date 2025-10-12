///
/// @file   WindowResource.hpp
/// @brief  Resource for storing/manipulating the window, currently only using GLFW.
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once
#include "PCH/PCH.hpp"
#include "Resources/iResource.hpp"

#include "GLFW/glfw3.h"

namespace FAR
{
  class WindowResource : public iResource
  {
  public:
    WindowResource();
    ~WindowResource() = default;

    int width{ 2000 };
    int height{ 1200 };
    std::string title{ "FAR OUT ENGINE" };

    GLFWwindow* window = nullptr;

    bool isWindowClosing()
    {
      return glfwWindowShouldClose(window);
    }

  private:
  };
}
#pragma once
