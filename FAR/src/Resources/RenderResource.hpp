#pragma once

#include "PCH.hpp"
#include "GL/glew.h"

namespace FAR
{
  class RenderResource
  {

  public:
    
    GLuint basicShaderProgram;
    GLuint lineShaderProgram;

    //struct ray
    //{
    //  glm::vec3 start;
    //  glm::vec3 end;
    //  glm::vec4 color;
    //};

    int vpwidth;
    int vpheight;

    GLuint fbo, colorTex, rboDepth;

    void CreateFrameBuffer(int width, int height);

    void DrawRay(glm::vec4 start, glm::vec4 end);

    std::vector<glm::vec4> rays;

  private:

  };
}