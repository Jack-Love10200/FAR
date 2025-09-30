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

  struct Node
  {
    std::string name{ "" };
    glm::mat4 transform{ 1.0f };
    std::vector<int> children;
    glm::mat4 inverseBindPose{ 1.0f };
    glm::mat4 skinningTransform{ 1.0f };
    int parent;
  };

  std::vector<Node> nodes;

  float animationTime{ 0.0f };

  //animation data
  struct Animation
  {
    glm::mat4 globalInverseTransform{ 1.0f };
    std::string name{ "" };
    float duration{ 0.0f };
    float ticksPerSecond{ 0.0f };
    struct Channel
    {
      std::string nodeName{ "" };
      std::vector<std::pair<float, glm::vec3>> positionKeys;
      std::vector<std::pair<float, glm::quat>> rotationKeys;
      std::vector<std::pair<float, glm::vec3>> scalingKeys;
    };
    std::vector<Channel> channels;
  };

  Animation animation;

  int indexCount{ 0 };
  bool textured = false;
  glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};