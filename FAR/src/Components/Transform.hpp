///
/// @file   Transform.hpp
/// @brief  Standard transform component for position, rotation, and scale.
/// @author Jack Love
/// @date   11.10.2025
///
#pragma once
#include "PCH/PCH.hpp"

//TODO: Consider alignment
struct Transform
{
  glm::vec3 position{ 0.0f, 0.0f, 0.0f };

  FAR::Quat rotationQuaternion{ 1.0f, 0.0f, 0.0f, 0.0f };

  glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
  glm::mat4 modelMatrix{ 1.0f };
};