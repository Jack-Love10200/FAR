///
/// @file   Model.hpp
/// @brief  Component for model data, i.e. meshes, textures, and node/bone hierarchy
/// @author Jack Love
/// @date   11.10.2025
///
#pragma once

#include "PCH/PCH.hpp"
#include "GL/glew.h"
#include "Util/VQS.hpp"


//TODO: Simplify this component
struct Model
{
  //path to the model file
  std::string path{ "" };

  //vaos for all meshes and their corresponding index counts
  std::vector<std::pair<GLuint, unsigned int>> VAOs;

  //map vao to textures for that mesh
  std::map<GLuint, std::vector<GLuint>> textures;
 
  struct Node
  {
    //name of the node, should match bone names if this is a bone
    std::string name{ "" };

    //Vector, quaternion, scale position
    FAR::VQS transform;

    //transform to bring a vertex from model space to bone space
    glm::mat4 inverseBindPose{ 1.0f };

    //heirarchy
    std::vector<int> children;
    int parent = -1;
  };

  std::vector<Node> nodes;

  bool textured = false;

  //base color for untextured models
  glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};