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

      int numPositionKeys = channel->mNumPositionKeys;
      int numRotationKeys = channel->mNumRotationKeys;
      int numScalingKeys = channel->mNumScalingKeys;

      int maxKeys = std::max(numPositionKeys, std::max(numRotationKeys, numScalingKeys));

      newChannel.keyFrames.reserve(numPositionKeys);

      //collect all keyframe times
      std::set<float> keyTimes;

      for (size_t i = 0; i < channel->mNumPositionKeys; i++)
      {
        keyTimes.insert(channel->mPositionKeys[i].mTime);
      }
      for (size_t i = 0; i < channel->mNumRotationKeys; i++)
      {
        keyTimes.insert(channel->mRotationKeys[i].mTime);
      }
      for (size_t i = 0; i < channel->mNumScalingKeys; i++)
      {
        keyTimes.insert(channel->mScalingKeys[i].mTime);
      }

      //for each unique key time, find the corresponding pos/rot/scale (or the last one before it)

      for (float time : keyTimes)
      {
        VQS vqs;

        if (numPositionKeys > 0)
        {

          bool posKeyFound = false;
          for (int j = 0; j < numPositionKeys; j++)
          {
            if (channel->mPositionKeys[j].mTime == time)
            {
              aiVector3D p = channel->mPositionKeys[j].mValue;
              vqs.v = glm::vec3(p.x, p.y, p.z);
              posKeyFound = true;
            }
            //if not a perfect match, lerp between the last key and the next key
            else if (channel->mPositionKeys[j].mTime < time)
            {
              aiVector3D p = channel->mPositionKeys[j].mValue;
              glm::vec3 pos1 = glm::vec3(p.x, p.y, p.z);
              if (j + 1 < numPositionKeys)
              {
                aiVector3D p2 = channel->mPositionKeys[j + 1].mValue;
                glm::vec3 pos2 = glm::vec3(p2.x, p2.y, p2.z);
                float t1 = channel->mPositionKeys[j].mTime;
                float t2 = channel->mPositionKeys[j + 1].mTime;
                float alpha = (time - t1) / (t2 - t1);
                vqs.v = glm::mix(pos1, pos2, alpha);
                posKeyFound = true;
              }
              else
              {
                vqs.v = pos1;
                posKeyFound = true;
              }
            }
          }
          if (!posKeyFound)
          {
            aiVector3D p = channel->mPositionKeys[0].mValue;
            vqs.v = glm::vec3(p.x, p.y, p.z);
          }
        }


        if (numRotationKeys > 0)
        {
          bool rotKeyFound = false;
          for (int j = 0; j < numRotationKeys; j++)
          {
            if (channel->mRotationKeys[j].mTime == time)
            {
              aiQuaternion q = channel->mRotationKeys[j].mValue;
              vqs.q = glm::quat(q.w, q.x, q.y, q.z);
              rotKeyFound = true;
            }
            //if not a perfect match, lerp between the last key and the next key
            else if (channel->mRotationKeys[j].mTime < time)
            {
              aiQuaternion q = channel->mRotationKeys[j].mValue;
              glm::quat rot1 = glm::quat(q.w, q.x, q.y, q.z);
              if (j + 1 < numRotationKeys)
              {
                aiQuaternion q2 = channel->mRotationKeys[j + 1].mValue;
                glm::quat rot2 = glm::quat(q2.w, q2.x, q2.y, q2.z);
                float t1 = channel->mRotationKeys[j].mTime;
                float t2 = channel->mRotationKeys[j + 1].mTime;
                float alpha = (time - t1) / (t2 - t1);
                vqs.q = glm::slerp(rot1, rot2, alpha);
                rotKeyFound = true;
              }
              else
              {
                vqs.q = rot1;
                rotKeyFound = true;
              }
            }
          }
          if (!rotKeyFound)
          {
            aiQuaternion q = channel->mRotationKeys[0].mValue;
            vqs.q = glm::quat(q.w, q.x, q.y, q.z);
          }
        }

        if (numScalingKeys > 0)
        {
          //if not a perfect match, lerp between the last key and the next key
          bool scaleKeyFound = false;
          for (int j = 0; j < numScalingKeys; j++)
          {
            if (channel->mScalingKeys[j].mTime == time)
            {
              glm::vec3 s = glm::vec3(channel->mScalingKeys[j].mValue.x, channel->mScalingKeys[j].mValue.y, channel->mScalingKeys[j].mValue.z);
              vqs.s = s;
              scaleKeyFound = true;
            }
            else if (channel->mScalingKeys[j].mTime < time)
            {
              aiVector3D sc = channel->mScalingKeys[j].mValue;
              glm::vec3 scale1 = glm::vec3(sc.x, sc.y, sc.z);
              if (j + 1 < numScalingKeys)
              {
                aiVector3D sc2 = channel->mScalingKeys[j + 1].mValue;
                glm::vec3 scale2 = glm::vec3(sc2.x, sc2.y, sc2.z);
                float t1 = channel->mScalingKeys[j].mTime;
                float t2 = channel->mScalingKeys[j + 1].mTime;
                float alpha = (time - t1) / (t2 - t1);
                vqs.s = glm::mix(scale1, scale2, alpha);
                scaleKeyFound = true;
              }
              else
              {
                vqs.s = scale1;
                scaleKeyFound = true;
              }
            }
          }
          if (!scaleKeyFound)
          {
            glm::vec3 s = glm::vec3(channel->mScalingKeys[0].mValue.x, channel->mScalingKeys[0].mValue.y, channel->mScalingKeys[0].mValue.z);
            vqs.s = s;
          }
        }
        newChannel.keyFrames.push_back(std::make_pair(time, vqs));
      }
      newAnim.channels.push_back(newChannel);
    }
    animator.animations.push_back(newAnim);
  }

  float elerp(float a, float b, float f)
  {
    return a + f * (b - a);
  }

  void SkeletalAnimation::UpdateNode(Model& model, SkeletalAnimator& animator, int nodeIndex)
  {
    Model::Node& node = model.nodes[nodeIndex];
    SkeletalAnimator::Animation& currentAnim = animator.animations[animator.currentAnimation];

    SkeletalAnimator::Animation::Channel* channel = nullptr;
    if (nodeIndex < currentAnim.channels.size() && currentAnim.channels[nodeIndex].nodeName == node.name)
    {
      channel = &currentAnim.channels[nodeIndex];
    }
    else
    {
      //fall back to searching by name
      for (auto& ch : currentAnim.channels)
      {
        if (ch.nodeName == node.name)
        {
          channel = &ch;
          break;
        }
      }
    }

    VQS localTransform;

    if (channel)
    {
      // Interpolate position
      for (int i = 0; i < channel->keyFrames.size() - 1; i++)
      {
        if (animator.animationTime >= channel->keyFrames[i].first && animator.animationTime <= channel->keyFrames[i + 1].first)
        {
          float alpha = (animator.animationTime - channel->keyFrames[i].first) / (channel->keyFrames[i + 1].first - channel->keyFrames[i].first);
          VQS kf1 = channel->keyFrames[i].second;
          VQS kf2 = channel->keyFrames[i + 1].second;
          localTransform = VQS::Interpolate(kf1, kf2, alpha);
          break;
        }
      }
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

      if (sk.playing)
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