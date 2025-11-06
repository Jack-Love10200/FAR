///
/// @file   RenderResource.hpp
/// @brief  Resource for storing/manipulating rendering data, specifically OpenGL states
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once

#include "PCH/PCH.hpp"
#include "GL/glew.h"
#include "Resources/iResource.hpp"

namespace FAR
{
  class RenderResource : public iResource
  {

  public:
    
    /// @brief Creates a framebuffer object with color/depth attachments
    /// @param width  - The width of the framebuffer
    /// @param height - The height of the framebuffer
    void CreateFrameBuffer(int width, int height);

    
    GLuint basicShaderProgram;
    GLuint lineShaderProgram;

    int vpwidth;
    int vpheight;

    GLuint fbo, colorTex, rboDepth;

    struct point 
    {
      glm::vec4 position;
      glm::vec4 color;
    };

    //array of points representing debug lines to draw on top of the scene
    std::vector<point> rays;

    std::vector<point> points;


    /// @brief Draw a ray(line segment) in world space on top of the scene
    /// @param start - One end of the line segment
    /// @param end   - The other end of the line segment
    void DrawRay(point p1, point p2);

    /// @brief Draw a point in world space on top of the scene
    /// @param point - The position of the point
    void DrawPoint(point p)
    {
      points.push_back(p);
    }

  private:

  };
}