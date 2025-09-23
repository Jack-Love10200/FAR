#include "PCH.hpp"

#include "GL/glew.h"
#include "WindowResource.hpp"

namespace FAR
{
  WindowResource::WindowResource()
  {

    std::cout << "window stuff" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(2000, 1200, "test", nullptr, nullptr);
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