#pragma once
#include "PCH.hpp"
#include "Util/VQS.hpp"

struct SkeletalAnimator
{
  std::string path{ "" };
  float animationTime{ 0.0f };

  struct Animation
  {
    //glm::mat4 globalInverseTransform{ 1.0f };
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

  int currentAnimation = 0;
  std::vector<Animation> animations;

};