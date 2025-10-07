#pragma once

#include "PCH.hpp"

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