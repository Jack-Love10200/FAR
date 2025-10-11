#pragma once
#include "PCH.hpp"
#include "Util/VQS.hpp"

struct SkeletalAnimator
{
  std::string path{ "" };
  float animationTime{ 0.0f };

  struct Animation
  {
    std::string name{ "" };
    float duration{ 0.0f };
    float ticksPerSecond{ 0.0f };

    struct Channel
    {
      std::string nodeName{ "" };

      int currentKeyFrame = 0;

      std::vector<std::pair<float, VQS>> keyFrames;
      std::vector<std::pair<float, VQS>> incrementalValues;
    };

    std::vector<Channel> channels;
  };

  int currentAnimation = 0;
  std::vector<Animation> animations;

  bool playing;
  bool looping;
};