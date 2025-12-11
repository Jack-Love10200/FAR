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
  struct Manipulator
  {
    Entity target;
    struct ManipulatorBone
    {
      int boneIndex;//which bone in the model's skeleton the part of the manipulator affects

      float minPitch = -2.0f * 3.14159f; //in radians
      float maxPitch = 2.0f * 3.14159f; //in radians

      float minYaw = -2.0f * 3.14159f; //in radians
      float maxYaw = 2.0f * 3.14159f; //in radians


      float minRoll = -2.0f * 3.14159f; //in radians
      float maxRoll = 2.0f * 3.14159f; //in radians
    };
    std::vector<ManipulatorBone> bones;

    int EEIndex; // which bone should be uses for considering end effector convergence

    glm::vec3 currentEEPos = glm::vec3(0.0f, 0.0f, 0.0f);
  };

  std::vector<Manipulator> manipulators; 
};