#pragma once

#include "PCH/PCH.hpp"


struct PointMass
{
  float mass;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  glm::vec3 totalForce;

  bool isStatic;
};