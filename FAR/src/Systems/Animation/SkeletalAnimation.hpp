#pragma once
#include "PCH.hpp"

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

    void LoadAnimationData(const aiScene* scene, SkeletalAnimator& animator);
    void LoadAnimation(aiAnimation* animation, SkeletalAnimator& animator);

    void UpdateNode(Model& model, SkeletalAnimator& animator, int nodeIndex);

    void ComputeIncrementalInterpolationConstants(SkeletalAnimator::Animation::Channel& channel, float tps);

    //void LoadBoneNodes(const aiNode* node, SkeletalAnimator& animator, int parentIndex, const VQS& parentTransform);

  };

}