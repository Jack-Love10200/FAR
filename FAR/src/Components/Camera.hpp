///
/// @file   Camera.hpp
/// @brief  Basic camera component for perspective proj
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once

#include "PCH/PCH.hpp"

struct Camera
{
  float fov{ 90.0f };
  float aspect {1.0f};
  float nearPlane{ 1.0f };
  float farPlane{ 1000.0f };
  glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
  glm::vec3 up{ 0.0f, 1.0f, 0.0f };

  bool isMain{ false };
};