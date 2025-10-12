///
/// @file   WindowResource.cpp
/// @brief  Resource for storing/manipulating the window, currently only using GLFW.
/// @author Jack Love
/// @date   11.10.2025
///
#include "PCH/PCH.hpp"

#include "GL/glew.h"
#include "WindowResource.hpp"

namespace FAR
{
  WindowResource::WindowResource()
  {
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(2000, 1200, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    const char* err;
    glfwGetError(&err);
    if (err)
      std::cout << err << std::endl;

    GLenum glewerr = glewInit();
    if (glewerr != GLEW_OK)
    {
      std::cerr << "Error: Unable to initialize GLEW " << glewGetErrorString(glewerr) << std::endl;
      glfwDestroyWindow(window); // destroy window and corresponding GL context
      glfwTerminate();
      std::exit(EXIT_FAILURE);
    }
  }
}