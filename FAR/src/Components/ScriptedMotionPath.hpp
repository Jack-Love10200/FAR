#pragma once

#include "PCH/PCH.hpp"

#define PATH_RESOLUTION 500

//just here to test how much not recreating the lin sys solver saves
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
  
  std::vector<KeyPoint> keyPoints;

  float t = 0.0f;

  float velCurveIntegral;

  Eigen::VectorXf vecX;
  Eigen::VectorXf vecY;
  Eigen::VectorXf vecZ;

  bool isDirty = true;

  float currentSpeed;

  float totalTime = 1.0f;
};