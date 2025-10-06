#include "PCH.hpp"

#include "SkeletalAnimation.hpp"

#include "Components/Model.hpp"

  glm::mat4 ToGlm(const aiMatrix4x4& m) {
    return glm::mat4(
      m.a1, m.b1, m.c1, m.d1,
      m.a2, m.b2, m.c2, m.d2,
      m.a3, m.b3, m.c3, m.d3,
      m.a4, m.b4, m.c4, m.d4
    );
  }

namespace FAR
{


  void SkeletalAnimation::LoadAnimationData(const aiScene* scene, SkeletalAnimator& animator)
  {
    for (int i = 0; i < scene->mNumAnimations; i++)
    {
      LoadAnimation(scene->mAnimations[i], animator);
    }
  }

  void SkeletalAnimation::LoadAnimation(aiAnimation* animation, SkeletalAnimator& animator)
  {
    SkeletalAnimator::Animation newAnim;

    newAnim.name = animation->mName.C_Str();
    newAnim.duration = animation->mDuration;
    newAnim.ticksPerSecond = animation->mTicksPerSecond;

    //for each channel
    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
      aiNodeAnim* channel = animation->mChannels[i];
      SkeletalAnimator::Animation::Channel newChannel;
      newChannel.nodeName = channel->mNodeName.C_Str();
      //position keys
      for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
      {
        aiVector3D pos = channel->mPositionKeys[j].mValue;
        newChannel.positionKeys.push_back(std::make_pair(channel->mPositionKeys[j].mTime, glm::vec3(pos.x, pos.y, pos.z)));
      }
      //rotation keys
      for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
      {
        aiQuaternion rot = channel->mRotationKeys[j].mValue;
        newChannel.rotationKeys.push_back(std::make_pair(channel->mRotationKeys[j].mTime, glm::quat(rot.w, rot.x, rot.y, rot.z)));
      }
      //scaling keys
      for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
      {
        aiVector3D scale = channel->mScalingKeys[j].mValue;
        newChannel.scalingKeys.push_back(std::make_pair(channel->mScalingKeys[j].mTime, glm::vec3(scale.x, scale.y, scale.z)));
      }
      newAnim.channels.push_back(newChannel);
    }

    animator.animations.push_back(newAnim);
  }

  void SkeletalAnimation::UpdateNode(Model& model, SkeletalAnimator& animator, int nodeIndex)
  {
    Model::Node& node = model.nodes[nodeIndex];

    glm::mat4 translation = glm::mat4(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 scaling = glm::mat4(1.0f);

    glm::vec3 v(0.0f, 0.0f, 0.0f);
    glm::quat q(1.0f, 0.0f, 0.0f, 0.0f);
    float s = 1.0f;
    //glm::vec3 s(1.0f, 1.0f, 1.0f);

    //v = node.transform.v;
    //q = node.transform.q;
    //s = node.transform.s;

    // Find the channel for this node (if any)
    
    SkeletalAnimator::Animation& currentAnim = animator.animations[animator.currentAnimation];
    SkeletalAnimator::Animation::Channel* channel = nullptr;
    if (nodeIndex < currentAnim.channels.size() && currentAnim.channels[nodeIndex].nodeName == node.name)
    {
      channel = &currentAnim.channels[nodeIndex];
    }
    else
    {
      // fallback: search by name
      std::string assimpextra = "_$AssimpFbx$_Rotation";
      std::string assimpextrascale = "_$AssimpFbx$_Scaling";
      std::string assimpextraposition = "_$AssimpFbx$_Translation";

      for (auto& ch : currentAnim.channels)
      {

        //if (node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextra.size()) ||
        //  node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextrascale.size()) ||
        //  node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextraposition.size()))
        if (ch.nodeName == node.name)
        {
          channel = &ch;
          break;
        }
      }
    }

    if (channel)
    {
      // Interpolate position

      if (channel->positionKeys.size() == 1)
      {
        translation = glm::translate(channel->positionKeys[0].second);
        v = channel->positionKeys[0].second;

        //if (v == glm::vec3(0.0f, 0.0f, 0.0f))
        //{
        //  v = glm::vec3(0.0f, 1.0f, 0.0f);
        //}
      }

      for (int j = 0; j < channel->positionKeys.size() - 1; j++)
      {
        if (animator.animationTime >= channel->positionKeys[j].first && animator.animationTime <= channel->positionKeys[j + 1].first)
        {
          float alpha = (animator.animationTime - channel->positionKeys[j].first) / (channel->positionKeys[j + 1].first - channel->positionKeys[j].first);
          glm::vec3 pos = glm::mix(channel->positionKeys[j].second, channel->positionKeys[j + 1].second, alpha);
          //translation = glm::translate(glm::mat4(1.0f), pos);
          v = pos;
          break;
        }
      }

      // Interpolate rotation

      if (channel->rotationKeys.size() == 1)
      {
        rotation = glm::mat4_cast(channel->rotationKeys[0].second);
        q = channel->rotationKeys[0].second;
      }

      for (int j = 0; j < channel->rotationKeys.size() - 1; j++)
      {
        if (animator.animationTime >= channel->rotationKeys[j].first && animator.animationTime <= channel->rotationKeys[j + 1].first)
        {
          float alpha = (animator.animationTime - channel->rotationKeys[j].first) / (channel->rotationKeys[j + 1].first - channel->rotationKeys[j].first);
          glm::quat rot = glm::slerp(channel->rotationKeys[j].second, channel->rotationKeys[j + 1].second, alpha);
          //rotation = glm::mat4_cast(rot);
          q = rot;
          break;
        }
      }

      // Interpolate scaling

      if (channel->scalingKeys.size() == 1)
      {
        scaling = glm::scale(channel->scalingKeys[0].second);
        s = (channel->scalingKeys[0].second.x + channel->scalingKeys[0].second.y + channel->scalingKeys[0].second.z) / 3.0f; //assume uniform scale for now
      }

      for (int j = 0; j < channel->scalingKeys.size() - 1; j++)
      {
        if (animator.animationTime >= channel->scalingKeys[j].first && animator.animationTime <= channel->scalingKeys[j + 1].first)
        {
          float alpha = (animator.animationTime - channel->scalingKeys[j].first) / (channel->scalingKeys[j + 1].first - channel->scalingKeys[j].first);
          glm::vec3 scale = glm::mix(channel->scalingKeys[j].second, channel->scalingKeys[j + 1].second, alpha);
          //scaling = glm::scale(glm::mat4(1.0f), scale);
          s = (scale.x + scale.y + scale.z) / 3.0f; //assume uniform scale for now
          break;
        }
      }

      VQS localTransform = VQS(v, q, s);
      //VQS globalTransform = parentTransform * localTransform;
      //VQS globalTransform = localTransform * parentTransform;

      //node.transform = globalTransform;
      node.transform = localTransform;
    }
  }

  void SkeletalAnimation::Init()
  {

  }

  void SkeletalAnimation::PreUpdate()
  {
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<SkeletalAnimator, Model>();
    for (const Entity& e : entities)
    {
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(e);

      if (sk.path != "" && sk.animations.size() == 0)
      {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(sk.path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_GlobalScale);
        LoadAnimationData(scene, sk);
      }
    }
  }

  void SkeletalAnimation::Update()
  {
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<SkeletalAnimator, Model>();

    for (const Entity& e : entities)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(e);

      float dt = Engine::GetInstance()->dt;

      sk.animationTime += dt * sk.animations[sk.currentAnimation].ticksPerSecond;

      if (sk.looping)
      {
        sk.animationTime = fmod(sk.animationTime, sk.animations[sk.currentAnimation].duration);
      }
      else
      {
        if (sk.animationTime > sk.animations[sk.currentAnimation].duration)
          sk.animationTime = sk.animations[sk.currentAnimation].duration;
      }

      for (int i = 0; i < model.nodes.size(); i++)
      {
        UpdateNode(model, sk, i);
      }
    }

  }

  void SkeletalAnimation::PostUpdate()
  {

  }

  void SkeletalAnimation::Exit()
  {

  }
}