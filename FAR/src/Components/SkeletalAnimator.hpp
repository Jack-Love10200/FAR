///
/// @file   SkeletalAnimator.hpp
/// @brief  Component for skeletal animation data and current animation state
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once
#include "PCH/PCH.hpp"
#include "Util/VQS.hpp"

struct SkeletalAnimator
{
  //path to the model file this animation came from
  std::string path{ "" };

  //ammount into the current animation in ticks
  float animationTime{ 0.0f };

  struct Animation
  {
    //name of the animation from the model file
    std::string name{ "" };
    double duration{ 0.0f };
    float ticksPerSecond{ 0.0f };

    struct Channel
    {
      //name of the node this channel affects
      std::string nodeName{ "" };

      //index of the current keyframe in the keyFrames vector
      int currentKeyFrame = 0;

      //all keyframes for this channel with timestamp
      std::vector<std::pair<float, FAR::VQS>> keyFrames;

      //precomputed incremental interpolation values
      std::vector<FAR::VQS> incrementalValues;
    };

    std::vector<Channel> channels;
  };

  //all animations loaded from the model file
  std::vector<Animation> animations;

  //index of the current animation in the animations vector
  int currentAnimation = 0;

  //whether the animation is currently playing
  bool playing;
  //whether the animation should loop
  bool looping;
};