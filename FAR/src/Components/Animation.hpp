#pragma once

#include "PCH.hpp"

namespace FAR
{
  struct Animation
  {
    struct AnimData
    {
      std::string name;
      float duration;
      float ticksPerSecond;
      // Add more fields as necessary, such as keyframes, channels, etc.
    };


    std::string filePath; // Path to the model file associated with this animation
    std::vector<AnimData> animations;
  };
}

