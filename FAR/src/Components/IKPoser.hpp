///
/// @file   IKPoser.hpp
/// @brief  Defines a component for inverse kinematics posing
/// @author Jack Love
/// @date   3.11.2025
///
#pragma once

#include "PCH/PCH.hpp"

#include "Engine/Engine.hpp"

struct IKPoser
{
  Entity target;

  //list of indexes into the skeleton's bone list that this IK poser will manipulate
  std::vector<int> manipulator;

  glm::vec3 currentEEPos = glm::vec3(0.0f, 0.0f, 0.0f);

};