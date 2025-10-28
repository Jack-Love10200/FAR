#pragma once

#include "PCH/PCH.hpp"

#define PATH_RESOLUTION 500

//just here to test how much not recreating the lin sys solver saves
#include "Eigen/Dense"


struct ScriptedMotionPath
{
  std::vector<glm::vec3> controlPoints;
  std::vector<std::pair<float, float>> velocityKeys; //time, velocity

  struct KeyPoint
  {
    glm::vec3 pos;
    float arcLenght;
    float u;
  };
  
  std::vector<KeyPoint> keyPoints;

  //std::array<float, PATH_RESOLUTION> arcLengths;
  std::vector<std::pair<float, float>> arcLenTable;
  std::array<glm::vec3, PATH_RESOLUTION> pathPoints;

  float t = 0.0f;

  float currentPos = 0.0f;

  float velCurveIntegral;

  Eigen::VectorXd vecX;
  Eigen::VectorXd vecY;
  Eigen::VectorXd vecZ;

  bool isDirty = true;

  float currentSpeed;

  float totalTime = 1.0f;
};