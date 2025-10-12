///
/// @file   SkeletalAnimation.hpp
/// @brief  System for loading and updating skeletal animations on models
/// @author Jack Love
/// @date   11.10.2025
///

#include "PCH/PCH.hpp"

#include "SkeletalAnimation.hpp"
#include "Components/Model.hpp"

#include "Util/MathHelpers.hpp"

namespace FAR
{


  void SkeletalAnimation::LoadAnimationData(const aiScene* scene, SkeletalAnimator& animator)
  {
    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
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
              vqs.q = Quat(q.w, q.x, q.y, q.z);
              rotKeyFound = true;
            }
            //if not a perfect match, lerp between the last key and the next key
            else if (channel->mRotationKeys[j].mTime < time)
            {
              aiQuaternion q = channel->mRotationKeys[j].mValue;
              Quat rot1 = Quat(q.w, q.x, q.y, q.z);
              if (j + 1 < numRotationKeys)
              {
                aiQuaternion q2 = channel->mRotationKeys[j + 1].mValue;
                Quat rot2 = Quat(q2.w, q2.x, q2.y, q2.z);
                float t1 = channel->mRotationKeys[j].mTime;
                float t2 = channel->mRotationKeys[j + 1].mTime;
                float alpha = (time - t1) / (t2 - t1);
                vqs.q = Quat::Slerp(rot1, rot2, alpha);
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
            vqs.q = Quat(q.w, q.x, q.y, q.z);
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

                vqs.s.x = elerp(scale1.x, scale2.x, alpha);
                vqs.s.y = elerp(scale1.y, scale2.y, alpha);
                vqs.s.z = elerp(scale1.z, scale2.z, alpha);

                //vqs.s = glm::mix(scale1, scale2, alpha);
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
      ComputeIncrementalInterpolationConstants(newChannel, newAnim.ticksPerSecond);
      newAnim.channels.push_back(newChannel);
    }
    animator.animations.push_back(newAnim);
  }

  void SkeletalAnimation::UpdateNode(Model& model, SkeletalAnimator& animator, int nodeIndex)
  {
    Model::Node& node = model.nodes[nodeIndex];
    SkeletalAnimator::Animation& currentAnim = animator.animations[animator.currentAnimation];

    VQS prevTransform = node.transform;

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
        if (animator.animationTime >= channel->keyFrames[i].first && animator.animationTime < channel->keyFrames[i + 1].first)
        {
          if (channel->currentKeyFrame != i)
          {
            prevTransform = channel->keyFrames[i].second;
            channel->currentKeyFrame = i;

            node.transform = prevTransform;
            return;
          }

          localTransform = VQS::IncrementalInterpolate(prevTransform, channel->incrementalValues[i]);
        }
      }
      node.transform = localTransform;
    }
  }

  void SkeletalAnimation::ComputeIncrementalInterpolationConstants(SkeletalAnimator::Animation::Channel& channel, float tps)
  {
    float fps = 144.0f;

    for (int i = 0; i < channel.keyFrames.size() - 1; i++)
    {
      float duration = (channel.keyFrames[i + 1].first - channel.keyFrames[i].first);

      float ticksPerFrame = tps / fps;

      int n = duration / ticksPerFrame;
      float a = glm::acos(glm::clamp(Quat::Dot(channel.keyFrames[i].second.q, channel.keyFrames[i + 1].second.q), -1.0f, 1.0f));

      VQS increment;
      VQS kf1 = channel.keyFrames[i].second;
      VQS kf2 = channel.keyFrames[i + 1].second;

      kf1.q.Normalize();
      kf2.q.Normalize();

      if (a < std::numeric_limits<float>::epsilon())
      {
        increment.q = Quat(1, 0, 0, 0);
      }
      else
      {
        float B = a / (float)n;

        glm::vec3 u0 = glm::vec3(kf1.q.x, kf1.q.y, kf1.q.z);
        glm::vec3 un = glm::vec3(kf2.q.x, kf2.q.y, kf2.q.z);

        float w0 = kf1.q.w;
        float wn = kf2.q.w;

        glm::vec3 u;

        u = (kf2.q * kf1.q.Conjugate()).GetVectorPart() / sin(a);

        u = glm::normalize(u);

        float sinB = sin(B);

        glm::vec3 sinBu = sinB * u;

        //islerping rotation
        increment.q = Quat(cos(B), sinBu.x, sinBu.y, sinBu.z);
        increment.q.Normalize();

      }
      //ilerping position
      increment.v = (kf2.v - kf1.v) / (float)n;

      //ielerping scale
      increment.s.x = pow(kf2.s.x / kf1.s.x, 1.0f / float(n));
      increment.s.y = pow(kf2.s.y / kf1.s.y, 1.0f / float(n));
      increment.s.z = pow(kf2.s.z / kf1.s.z, 1.0f / float(n));

      channel.incrementalValues.push_back(increment);
    }
  }

  void SkeletalAnimation::Init()
  {

  }

  void SkeletalAnimation::PreUpdate()
  {
    //check for newly created SkeletalAnimator components that need their animation data loaded
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<SkeletalAnimator, Model>();
    for (const Entity& e : entities)
    {
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(e);
      if (sk.path != "" && sk.animations.size() == 0)
      {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(sk.path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_GlobalScale);
        LoadAnimationData(scene, sk);
        importer.FreeScene();
      }
    }
  }

  void SkeletalAnimation::Update()
  {
    //update all skeletal animators
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<SkeletalAnimator, Model>();
    for (const Entity& e : entities)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);
      SkeletalAnimator& sk = Engine::GetInstance()->GetComponent<SkeletalAnimator>(e);

      float dt = Engine::GetInstance()->dt;

      if (sk.playing)
      sk.animationTime += dt * sk.animations[sk.currentAnimation].ticksPerSecond;

      if (sk.looping)
        sk.animationTime = fmod(sk.animationTime, sk.animations[sk.currentAnimation].duration);
      else if (sk.animationTime > sk.animations[sk.currentAnimation].duration)
        sk.animationTime = sk.animations[sk.currentAnimation].duration;

      if (sk.playing)
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