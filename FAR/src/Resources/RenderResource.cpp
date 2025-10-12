///
/// @file   RenderResource.cpp
/// @brief  Resource for storing/manipulating rendering data, specifically OpenGL states
/// @author Jack Love
/// @date   11.10.2025
///

#include "PCH/PCH.hpp"

#include "RenderResource.hpp"

namespace FAR
{
  void RenderResource::CreateFrameBuffer(int width, int height)
  {
    //delete existing fbo if it exists
    if (fbo != 0)
    {
      glDeleteFramebuffers(1, &fbo);
      glDeleteTextures(1, &colorTex);
      glDeleteRenderbuffers(1, &rboDepth);
    }

    //store viewport size
    vpwidth = width;
    vpheight = height;

    //create the fbo
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //create color texture
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    //create depth renderbuffer (optional but usually needed)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    //check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cerr << "FBO not complete" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RenderResource::DrawRay(glm::vec4 start, glm::vec4 end)
  {
    rays.push_back(start);
    rays.push_back(end);
  }
}