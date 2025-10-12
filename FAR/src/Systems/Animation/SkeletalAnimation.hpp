///
/// @file   SkeletalAnimation.hpp
/// @brief  System for loading and updating skeletal animations on models
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once
#include "PCH/PCH.hpp"

#include "Util/VQS.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Systems/iSystem.hpp"
#include "Components/SkeletalAnimator.hpp"
#include "Components/Model.hpp"

#include "Engine/Engine.hpp"

namespace FAR
{

  class SkeletalAnimation : public iSystem
  {
  public:
    SkeletalAnimation() = default;
    ~SkeletalAnimation() override = default;
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;

  private:

    //load all of the animations in a scene into the animator
    void LoadAnimationData(const aiScene* scene, SkeletalAnimator& animator);

    //load a single animation into the animator
    void LoadAnimation(aiAnimation* animation, SkeletalAnimator& animator);

    //update a single node's transform based on the current animation time
    void UpdateNode(Model& model, SkeletalAnimator& animator, int nodeIndex);

    //precompute incremental interpolation values for a channel
    void ComputeIncrementalInterpolationConstants(SkeletalAnimator::Animation::Channel& channel, float tps);
  };

}