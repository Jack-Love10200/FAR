#pragma once

#include "PCH.hpp"
#include "GL/glew.h"

struct Model
{
  std::string path{ "" };
  GLuint texArray;

  //gpu vao and corresponding index count
  std::vector<std::pair<GLuint, unsigned int>> VAOs;

  //map vao to textures for that mesh
  std::map<GLuint, std::vector<GLuint>> textures;

  int indexCount{ 0 };
  bool textured = false;
  glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};