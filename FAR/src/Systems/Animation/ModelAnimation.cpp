#include "PCH.hpp"

#include "ModelAnimation.hpp"
#include "Engine/Engine.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace FAR
{
  void ModelAnimation::Init()
  {
    
  }

  void ModelAnimation::PreUpdate()
  {
    std::vector<Entity> animationEntities = Engine::GetInstance()->GetEntities<Animation, Model>();

    for (const Entity& e : animationEntities)
    {
      Animation& animation = Engine::GetInstance()->GetComponent<Animation>(e);
      if (animation.animations.size() == 0)
      {
        LoadAnimations(animation);
      }

    }
  }

  void ModelAnimation::Update()
  {

  }

  void ModelAnimation::PostUpdate()
  {

  }

  void ModelAnimation::Exit()
  {

  }

  void ModelAnimation::LoadAnimations(Animation& animation)
  {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animation.filePath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);


  }

}