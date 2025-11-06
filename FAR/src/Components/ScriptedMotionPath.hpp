///
/// @file   ScriptedMotionPath.hpp
/// @brief  Component representing a cubic spline path and an entity's state of motion along it
/// @author Jack Love
/// @date   28.10.2025
///

#pragma once

#include "PCH/PCH.hpp"

//just here to test how much performance not recreating the lin sys solver saves
#include "Eigen/Dense"


struct ScriptedMotionPath
{
  std::vector<glm::vec3> controlPoints;
  std::vector<std::pair<float, float>> velocityKeys; //time, velocity

  //TODO: This wouldn't need to have the pos during game time, 
  //but still needed during editor time for visualization
  struct KeyPoint
  {
    glm::vec3 pos;
    float arcLenght;
    float u;
  };
  
  //computed key points along the spline, used for arc length parameterization
  //and rendering the curve in editor
  std::vector<KeyPoint> keyPoints;

  //current t value along the curve (0-1)
  float t = 0.0f;

  //the total area under the velocity curve for normalization
  float velCurveIntegral;

  //spline coefficints
  Eigen::VectorXd vecX;
  Eigen::VectorXd vecY;
  Eigen::VectorXd vecZ;

  //flag to indicate that the spline needs to be recomputed
  bool isDirty = true;

  //current speed along the path
  float currentSpeed;

  //total time to traverse the path
  float totalTime = 1.0f;
};