#pragma once

#include "PCH.hpp"
#include "GL/glew.h"
#include "Util/VQS.hpp"

struct Model
{
  std::string path{ "" };
  GLuint texArray;

  //gpu vao and corresponding index count
  std::vector<std::pair<GLuint, unsigned int>> VAOs;

  //map vao to textures for that mesh
  std::map<GLuint, std::vector<GLuint>> textures;

  struct Node
  {
    std::string name{ "" };
    std::vector<int> children;

    VQS transform;

    glm::mat4 inverseBindPose{ 1.0f };
    int parent = -1;
  };

  std::vector<Node> nodes;

  int indexCount{ 0 };
  bool textured = false;
  glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};