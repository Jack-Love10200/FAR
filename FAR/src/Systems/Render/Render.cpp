#include "PCH.hpp"
#include "Render.hpp"

#include "Engine/Engine.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>

#include "stb_image.h"

#include "Components/SkeletalAnimator.hpp"

namespace FAR
{

  glm::mat4 ToGlm(const aiMatrix4x4& m) {
    return glm::mat4(
      m.a1, m.b1, m.c1, m.d1,
      m.a2, m.b2, m.c2, m.d2,
      m.a3, m.b3, m.c3, m.d3,
      m.a4, m.b4, m.c4, m.d4
    );
  }

  GLuint Render::CreateShaderProgram(const std::filesystem::path& vertex, const std::filesystem::path& fragment)
  {
    GLint error;

    std::ifstream vshaderfile(vertex.string());
    std::ifstream fshaderfile(fragment.string());

    std::string vshaderstr((std::istreambuf_iterator<char>(vshaderfile)), std::istreambuf_iterator<char>());
    std::string fshaderstr((std::istreambuf_iterator<char>(fshaderfile)), std::istreambuf_iterator<char>());

    const char* vshader = vshaderstr.c_str();
    const char* fshader = fshaderstr.c_str();

    GLuint vertshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertshader, 1, &vshader, nullptr);
    glCompileShader(vertshader);
    glGetShaderiv(vertshader, GL_COMPILE_STATUS, &error);
    if (!error)
    {
      char infoLog[512];
      glGetShaderInfoLog(vertshader, 512, nullptr, infoLog);
      std::cout << "Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    GLuint fragshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragshader, 1, &fshader, nullptr);
    glCompileShader(fragshader);
    glGetShaderiv(vertshader, GL_COMPILE_STATUS, &error);
    if (!error)
    {
      char infoLog[512];
      glGetShaderInfoLog(vertshader, 512, nullptr, infoLog);
      std::cout << "Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertshader);
    glAttachShader(shaderProgram, fragshader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &error);
    if (!error)
    {
      std::cerr << "program error" << std::endl;
      char buffer[1024];
      glGetProgramInfoLog(shaderProgram, 1024, 0, buffer);
      std::cerr << buffer << std::endl;
      throw std::runtime_error(buffer);
    }

    glDeleteShader(vertshader);
    glDeleteShader(fragshader);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    //bind texture samplers to texture units
    glUseProgram(shaderProgram);
    GLint texturesUniformLocation = 6;
    GLint samplers[16];
    for (int i = 0; i < 16; i++) samplers[i] = i;
    glUniform1iv(texturesUniformLocation, 16, samplers);
    glUseProgram(0);

    return shaderProgram;
  }

  void Render::LoadModel(const std::filesystem::path& filepath, Model& model)
  {
    Assimp::Importer importer;
    //const aiScene* samba = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_PreTransformVertices);
    //const aiScene* samba = importer.ReadFile(filepath.string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_GlobalScale);
    const aiScene* samba = importer.ReadFile(filepath.string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_GlobalScale);

    //m.verticies.resize(samba->mMeshes[0]->mNumVertices);
    //m.indicies.resize(samba->mMeshes[0]->mNumFaces * 3);


    //print out all of the bone names
    for (unsigned int i = 0; i < samba->mNumMeshes; i++)
    {
      for (unsigned int j = 0; j < samba->mMeshes[i]->mNumBones; j++)
      {
        std::cout << "bone: " << samba->mMeshes[i]->mBones[j]->mName.C_Str() << std::endl;
      }
    }

    stbi_set_flip_vertically_on_load(true);

    LoadNodes(samba->mRootNode, samba, model, -1, VQS());

    //if (samba->mNumAnimations > 0)
    //  LoadAnimationData(samba, model);

    //model.animation.globalInverseTransform = glm::inverse(ToGlm(samba->mRootNode->mTransformation));

    //meshes
    for (unsigned int i = 0; i < samba->mNumMeshes; i++)
    {

      PutNodesInModelSpace(model, samba->mMeshes[i]);

      meshInfo m;
      unsigned int numVerts = m.verticies.size();
      std::cout << "num uv channels: " << samba->mMeshes[i]->GetNumUVChannels() << std::endl;

      for (unsigned int j = 0; j < samba->mMeshes[i]->mNumVertices; j++)
      {
        if (!samba->mMeshes[i]->HasTextureCoords(0))
        {
          std::cout << "mesh has no texture coords in channel 0" << std::endl;
        }
        else
        {
          glm::vec4 pos = glm::vec4(samba->mMeshes[i]->mVertices[j].x, samba->mMeshes[i]->mVertices[j].y, samba->mMeshes[i]->mVertices[j].z, 1.0f);
          //pos = ToGlm(samba->mRootNode->mTransformation) * pos;

          m.verticies.push_back(vertex{ pos
            , glm::vec4(samba->mMeshes[i]->mTextureCoords[0][j].x, samba->mMeshes[i]->mTextureCoords[0][j].y, 0, 0) });
        }
      }

      for (unsigned int j = 0; j < samba->mMeshes[i]->mNumFaces; j++)
      {
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[0]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[1]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[2]);
      }

      ApplyBoneWeightsToVerticies(m, samba->mMeshes[i], model);

      CreateVAO(m, model);
      //model.textures.insert(model.textures.end(), std::make_pair(model.VAOs.back().first, std::vector<GLuint>()));

      std::vector<GLuint> empty;
      model.textures.insert(model.textures.end(), std::make_pair(model.VAOs.back().first, empty));
      //model.textures.emplace(model.VAOs.back(), empty);

      aiMaterial* material = samba->mMaterials[samba->mMeshes[i]->mMaterialIndex];
      aiString texPath;

      unsigned int texCount = material->GetTextureCount(aiTextureType_DIFFUSE);

      for (unsigned int t = 0; t < texCount; t++)
      {
        aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, t, &texPath);

        std::cout << texPath.C_Str() << std::endl;

        if (texFound != AI_SUCCESS)
        {
          continue;
        }

        if (*texPath.C_Str() == '*')
        {
          //embedded texture
          std::cout << "embedded texture found" << std::endl;

          int texIndex = atoi(texPath.C_Str() + 1);
          if (texIndex < 0 || texIndex >= (int)samba->mNumTextures)
          {
            continue;
          }
          aiTexture* tex = samba->mTextures[texIndex];
          if (!tex)
          {
            continue;
          }

          int width, height, channels;
          stbi_uc* texData = stbi_load_from_memory(
            (const stbi_uc*)tex->pcData,
            tex->mWidth,
            &width, &height, &channels,
            3);

          if (!texData)
            continue;

          GLuint currentTex;

          glGenTextures(1, &currentTex);
          glBindTexture(GL_TEXTURE_2D, currentTex);
          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
            0, GL_RGB, GL_UNSIGNED_BYTE, texData);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

          model.textures[model.VAOs.back().first].push_back(currentTex);
        }
        else
        {
          //non-embedded texture

          std::filesystem::path texFullPath = filepath.parent_path() / texPath.C_Str();

          int width;
          int height;
          int channels;
          stbi_uc* sambaa = stbi_load(texFullPath.string().c_str(),
            &width, &height, &channels,
            3);

          if (!sambaa)
          {
            continue;
          }

          // GPU buffer for texture
          GLuint currentTex;

          glGenTextures(1, &currentTex);
          glBindTexture(GL_TEXTURE_2D, currentTex);
          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
            0, GL_RGB, GL_UNSIGNED_BYTE, sambaa);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

          model.textures[model.VAOs.back().first].push_back(currentTex);
        }
      }
    }

    //move object to (0, 0, 0) and scale to 1x1x1
    //glm::vec4 max = {0.0f, 0.0f, 0.0f, 0.0f};
    //glm::vec4 min = { 0.0f, 0.0f, 0.0f, 0.0f};
    //for (const vertex& vert : m.verticies)
    //{
    //  max.x = std::max(vert.position.x, max.x);
    //  max.y = std::max(vert.position.y, max.y);
    //  max.z = std::max(vert.position.z, max.z);

    //  min.x = std::min(vert.position.x, min.x);
    //  min.y = std::min(vert.position.y, min.y);
    //  min.z = std::min(vert.position.z, min.z);
    //}

    //glm::vec4 center((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2, 0.0f);
    //float scaleFactor = std::max(std::abs(max.x - min.x), std::max(std::abs(max.y - min.y),  std::abs(max.z - min.z)));

    //for (vertex& vert : m.verticies)
    //{
    //  vert.position -= center;
    //  vert.position /= scaleFactor;
    //  vert.position.w = 1.0f;
    //}

    //return m;
  }

  //void Render::LoadAnimationData(const aiScene* scene, Model& model)
  //{
  //  model.animation.channels.clear();
  //  model.animation.duration = scene->mAnimations[0]->mDuration;
  //  model.animation.ticksPerSecond = scene->mAnimations[0]->mTicksPerSecond;

  //  //for each channel
  //  for (unsigned int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
  //  {
  //    aiNodeAnim* channel = scene->mAnimations[0]->mChannels[i];
  //    Model::Animation::Channel newChannel;
  //    newChannel.nodeName = channel->mNodeName.C_Str();
  //    //position keys
  //    for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
  //    {
  //      aiVector3D pos = channel->mPositionKeys[j].mValue;
  //      newChannel.positionKeys.push_back(std::make_pair(channel->mPositionKeys[j].mTime, glm::vec3(pos.x, pos.y, pos.z)));
  //    }
  //    //rotation keys
  //    for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
  //    {
  //      aiQuaternion rot = channel->mRotationKeys[j].mValue;
  //      newChannel.rotationKeys.push_back(std::make_pair(channel->mRotationKeys[j].mTime, glm::quat(rot.w, rot.x, rot.y, rot.z)));
  //    }
  //    //scaling keys
  //    for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
  //    {
  //      aiVector3D scale = channel->mScalingKeys[j].mValue;
  //      newChannel.scalingKeys.push_back(std::make_pair(channel->mScalingKeys[j].mTime, glm::vec3(scale.x, scale.y, scale.z)));
  //    }
  //    model.animation.channels.push_back(newChannel);
  //  }

    //make sure animations are in the same order as the node
    //for (const Model::Node& node : model.nodes)
    //{
    //  bool alreadyAdded = false;
    //  for (const Model::Animation::Channel& channel : model.animation.channels)
    //  {
    //    if (node.name == channel.nodeName)
    //    {
    //      alreadyAdded = true;
    //      break;
    //    }
    //  }

    //  if (alreadyAdded)
    //    continue;

    //  Model::Animation::Channel emptyChannel;
    //  emptyChannel.nodeName = node.name;
    //  emptyChannel.positionKeys.push_back({0.0f, node.transform.v});
    //  emptyChannel.rotationKeys.push_back({0.0f, node.transform.q});
    //  emptyChannel.scalingKeys.push_back({ 0.0f, glm::vec3(node.transform.s) });

    //  model.animation.channels.push_back(emptyChannel);
    //}
    //}

  void Render::CreateVAO(const meshInfo& m, Model& model)
  {
    //get the mesh
    //vbo
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m.verticies.size() * sizeof(vertex), m.verticies.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //ibo
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indicies.size() * sizeof(unsigned int), m.indicies.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //vao
    GLuint currentVAO;
    glGenVertexArrays(1, &currentVAO);
    glBindVertexArray(currentVAO);

    //bind vbo to vao
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    //config attrib ptrs
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, boneWeights));
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(vertex), (void*)offsetof(vertex, boneIds));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    //bind ibo to vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    //done with vao
    glBindVertexArray(0);

    model.VAOs.push_back(std::make_pair(currentVAO, m.indicies.size()));
  }

  void Render::LoadNodes(const aiNode* node, const aiScene* scene, Model& model, int parentIndex, VQS parentTransform)
  {

    bool nameFound = false;
    //if the name of the node is not in the bone list, dont add it
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
      for (unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
      {
        //std::cout << "comparing " << node->mName.C_Str() << " to " << scene->mMeshes[i]->mBones[j]->mName.C_Str() << std::endl;
        //if (node->mName == scene->mMeshes[i]->mBones[j]->mName || !strcmp(node->mName.C_Str(), "Armature"))
        if (node->mName == scene->mMeshes[i]->mBones[j]->mName)
        {
          nameFound = true;
          break;
        }
      }
    } 

    //if (!nameFound)
    //{
    //  for (unsigned int i = 0; i < node->mNumChildren; i++)
    //  {
    //    LoadNodes(node->mChildren[i], scene, model, parentIndex);
    //  }
    //  return;
    //}

    Model::Node newNode;
    newNode.parent = parentIndex;
    //newNode.transform = ToGlm(node->mTransformation);

    VQS localTransform = VQS(ToGlm(node->mTransformation));
    newNode.transform = parentTransform * localTransform;

    newNode.name = node->mName.C_Str();


    int currentIndex = model.nodes.size();

    model.nodes.push_back(newNode);

    if (parentIndex != -1)
      model.nodes[parentIndex].children.push_back(currentIndex);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      //LoadNodes(node->mChildren[i], scene, model, currentIndex, newNode.transform);
      LoadNodes(node->mChildren[i], scene, model, currentIndex, VQS());
    }
  }

  void Render::RenderNodes(Model& model, Transform& trans)
  {
    std::vector<glm::vec4> points;

    //for (Model::Node& node : model.nodes)
    //{
    //  if (node.parent != -1)
    //  {
    //    points.push_back(glm::vec4(node.transform.v, 1.0f));
    //    points.push_back(glm::vec4(model.nodes[node.parent].transform.v, 1.0f));
    //  }
    //}

    BuildBonePointList(model, points, 0, VQS());

    for (glm::vec4& point : points)
    {
      point = trans.modelMatrix * point;
    }

    for (int i = 0; i < points.size(); i += 2)
    {
      renderResc->DrawRay(points[i], points[i + 1]);
    }

    //renderResc->rays = points;
  }

  void Render::BuildBonePointList(Model& model, std::vector<glm::vec4>& points, int index, VQS parentTrans)
  {
    VQS localTrans = model.nodes[index].transform;
    VQS globalTrans = parentTrans * localTrans;

    glm::vec4 pos = glm::vec4(globalTrans.v, 1.0f);
    //glm::vec4 pos = glm::vec4(localTrans.v, 1.0f);

    for (int& i : model.nodes[index].children)
    {
      VQS childTrans = model.nodes[i].transform;
      VQS childGlobalTrans = globalTrans * childTrans;
      glm::vec4 childPos = glm::vec4(childGlobalTrans.v, 1.0f);
      //glm::vec4 childPos = glm::vec4(childTrans.v, 1.0f);
      

      points.push_back(pos);
      points.push_back(childPos);

      BuildBonePointList(model, points, i, globalTrans);
    }
  }

  void Render::ApplyBoneWeightsToVerticies(meshInfo& m, const aiMesh* mesh, Model& model)
  {
    //for each bone
    for (int i = 0; i < mesh->mNumBones; i++)
    {
      aiBone* bone = mesh->mBones[i];
      //for each weight in the bone
      for (int j = 0; j < bone->mNumWeights; j++)
      {
        aiVertexWeight weight = bone->mWeights[j];
        //add the bone index to the vertex's bone id list
        for (int k = 0; k < 4; k++)
        {
          if (m.verticies[weight.mVertexId].boneIds[k] == -1)
          {
            //search for the node with the same name as the bone
            int found = -1;
            for (int n = 0; n < model.nodes.size(); n++)
            {
              if (!strcmp(bone->mName.C_Str(), model.nodes[n].name.c_str()))
              {
                //std::cout << "found bone " << bone->mName.C_Str() << " at index " << n << std::endl;
                found = n;
                break;
              }
            }
            


            m.verticies[weight.mVertexId].boneIds[k] = found;
            m.verticies[weight.mVertexId].boneWeights[k] = weight.mWeight;
            break;
          }
        }
      }
    }
  }

  void Render::PutNodesInModelSpace(Model& model, aiMesh* mesh)
  {
    //multipy by the inverse bind pose
    for (int i = 0; i < mesh->mNumBones; i++)
    {
      aiBone* bone = mesh->mBones[i];
      //find the node with the same name as the bone
      for (int j = 0; j < model.nodes.size(); j++)
      {
        if (model.nodes[j].name == bone->mName.C_Str())
        {
          glm::mat4 invBindPose = ToGlm(bone->mOffsetMatrix);
          //model.nodes[j].transform = model.nodes[j].transform * glm::inverse(invBindPose);
          //model.nodes[j].transform = model.nodes[j].transform * invBindPose;
          model.nodes[j].inverseBindPose = invBindPose;
          break;
        }
      }
    }
  }

  void Render::CreateLinesVAO()
  {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, renderResc->rays.size() * sizeof(glm::vec4), renderResc->rays.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //vao
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);

    //bind vbo to vao
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //config attrib ptrs
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
    glEnableVertexAttribArray(0);

    //done with vao
    glBindVertexArray(0);
  }


  //void Render::AnimUpdateNode(Model& model, int nodeIndex, float animationTime, const VQS& parentTransform)
  //{
  //  Model::Node& node = model.nodes[nodeIndex];

  //  glm::mat4 translation = glm::mat4(1.0f);
  //  glm::mat4 rotation = glm::mat4(1.0f);
  //  glm::mat4 scaling = glm::mat4(1.0f);

  //  glm::vec3 v(0.0f, 0.0f, 0.0f);
  //  glm::quat q(1.0f, 0.0f, 0.0f, 0.0f);
  //  float s = 1.0f;
  //  //glm::vec3 s(1.0f, 1.0f, 1.0f);

  //  //v = node.transform.v;
  //  //q = node.transform.q;
  //  //s = node.transform.s;

  //  // Find the channel for this node (if any)
  //  Model::Animation::Channel* channel = nullptr;
  //  if (nodeIndex < model.animation.channels.size() && model.animation.channels[nodeIndex].nodeName == node.name) 
  //  {
  //    channel = &model.animation.channels[nodeIndex];
  //  }
  //  else 
  //  {
  //    // fallback: search by name
  //    std::string assimpextra = "_$AssimpFbx$_Rotation";
  //    std::string assimpextrascale = "_$AssimpFbx$_Scaling";
  //    std::string assimpextraposition = "_$AssimpFbx$_Translation";

  //    for (auto& ch : model.animation.channels) 
  //    {

  //      //if (node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextra.size()) ||
  //      //  node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextrascale.size()) ||
  //      //  node.name == ch.nodeName.substr(0, ch.nodeName.size() - assimpextraposition.size()))
  //      if (ch.nodeName == node.name) 
  //      {
  //        channel = &ch;
  //        break;
  //      }
  //    }
  //  }

  //  if (channel) 
  //  {
  //    // Interpolate position

  //    if (channel->positionKeys.size() == 1)
  //    {
  //      translation = glm::translate(channel->positionKeys[0].second);
  //      v = channel->positionKeys[0].second;

  //      //if (v == glm::vec3(0.0f, 0.0f, 0.0f))
  //      //{
  //      //  v = glm::vec3(0.0f, 1.0f, 0.0f);
  //      //}
  //    }

  //    for (int j = 0; j < channel->positionKeys.size() - 1; j++)
  //    {
  //      if (animationTime >= channel->positionKeys[j].first && animationTime <= channel->positionKeys[j + 1].first) 
  //      {
  //        float alpha = (animationTime - channel->positionKeys[j].first) / (channel->positionKeys[j + 1].first - channel->positionKeys[j].first);
  //        glm::vec3 pos = glm::mix(channel->positionKeys[j].second, channel->positionKeys[j + 1].second, alpha);
  //        //translation = glm::translate(glm::mat4(1.0f), pos);
  //        v = pos;
  //        break;
  //      }
  //    }

  //    // Interpolate rotation

  //    if (channel->rotationKeys.size() == 1)
  //    {
  //      rotation = glm::mat4_cast(channel->rotationKeys[0].second);
  //      q = channel->rotationKeys[0].second;
  //    }

  //    for (int j = 0; j < channel->rotationKeys.size() - 1; j++) 
  //    {
  //      if (animationTime >= channel->rotationKeys[j].first && animationTime <= channel->rotationKeys[j + 1].first) 
  //      {
  //        float alpha = (animationTime - channel->rotationKeys[j].first) / (channel->rotationKeys[j + 1].first - channel->rotationKeys[j].first);
  //        glm::quat rot = glm::slerp(channel->rotationKeys[j].second, channel->rotationKeys[j + 1].second, alpha);
  //        //rotation = glm::mat4_cast(rot);
  //        q = rot;
  //        break;
  //      }
  //    }

  //    // Interpolate scaling

  //    if (channel->scalingKeys.size() == 1)
  //    {
  //      scaling = glm::scale(channel->scalingKeys[0].second);
  //      s = (channel->scalingKeys[0].second.x + channel->scalingKeys[0].second.y + channel->scalingKeys[0].second.z) / 3.0f; //assume uniform scale for now
  //    }

  //    for (int j = 0; j < channel->scalingKeys.size() - 1; j++) 
  //    {
  //      if (animationTime >= channel->scalingKeys[j].first && animationTime <= channel->scalingKeys[j + 1].first) 
  //      {
  //        float alpha = (animationTime - channel->scalingKeys[j].first) / (channel->scalingKeys[j + 1].first - channel->scalingKeys[j].first);
  //        glm::vec3 scale = glm::mix(channel->scalingKeys[j].second, channel->scalingKeys[j + 1].second, alpha);
  //        //scaling = glm::scale(glm::mat4(1.0f), scale);
  //        s = (scale.x + scale.y + scale.z) / 3.0f; //assume uniform scale for now
  //        break;
  //      }
  //    }

  //  VQS localTransform = VQS(v, q, s);
  //  VQS globalTransform = parentTransform * localTransform;
  //  //VQS globalTransform = localTransform * parentTransform;

  //  node.transform = globalTransform;
  //  }
  //  
  //  //node.skinningTransform = globalTransform * node.inverseBindPose;
  //  //node.transform = model.animation.globalInverseTransform * globalTransform * node.inverseBindPose;

  //  // Recurse for children
  //  for (int childIdx : node.children) {
  //    //AnimUpdateNode(model, childIdx, animationTime, globalTransform);
  //  }
  //}

  void Render::ApplyNodeHeirarchy(std::vector<Model::Node>& nodes, int nodeIndex, const VQS& parentTransform)
  {
    Model::Node& node = nodes[nodeIndex];
    VQS localTransform = node.transform;
    VQS globalTransform = parentTransform * localTransform;
    //node.skinningTransform = globalTransform * node.inverseBindPose;
    //node.transform = model.animation.globalInverseTransform * globalTransform * node.inverseBindPose;
    
    node.transform = globalTransform;

    // Recurse for children
    for (int childIdx : node.children) {
      ApplyNodeHeirarchy(nodes, childIdx, globalTransform);
    }
  }

  //void Render::AnimUpdate()
  //{
  //  float dt = Engine::GetInstance()->dt;
  //  std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Model>();

  //  for (const Entity& ent : modelEntities)
  //  {
  //    Model& model = Engine::GetInstance()->GetComponent<Model>(ent);

  //    model.animationTime += dt * model.animation.ticksPerSecond;
  //    //model.animationTime = 100.0f; //debug

  //    if (model.animationTime > model.animation.duration)
  //      model.animationTime = fmod(model.animationTime, model.animation.duration);

  //    if (model.animation.channels.empty()) continue;

  //    // Start recursion from root nodes (parent == -1)
  //    for (int i = 0; i < model.nodes.size(); ++i) 
  //    {
  //      //if (model.nodes[i].parent == -1) 
  //        AnimUpdateNode(model, i, model.animationTime, VQS());
  //    }

  //    //ApplyNodeHeirarchy(model.nodes, 0, VQS());
  //  }
  //}



  //void Render::AnimUpdate()
  //{
  //  float dt = Engine::GetInstance()->dt;

  //  std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Model>();

  //  for (const Entity& ent : modelEntities)
  //  {
  //    Model& model = Engine::GetInstance()->GetComponent<Model>(ent);

  //    model.animationTime += dt * model.animation.ticksPerSecond;
  //    //model.animationTime = 100.0f; //debug

  //    if (model.animationTime > model.animation.duration)
  //      model.animationTime = fmod(model.animationTime, model.animation.duration);

  //    if (model.animation.channels.size() == 0) continue;

  //    for (int i = 0; i < model.nodes.size(); i++)
  //    {
  //      //model.nodes[i].transform = glm::mat4(1.0f);

  //      glm::mat4 translation = glm::mat4(1.0f);
  //      glm::mat4 rotation = glm::mat4(1.0f);
  //      glm::mat4 scaling = glm::mat4(1.0f);

  //      //find the channel for this node
  //      Model::Animation::Channel& channel = model.animation.channels[i];

  //      //find the position keyframes surrounding the current animation time
  //      for (int j = 0; j < channel.positionKeys.size() - 1; j++)
  //      {
  //        if (model.animationTime >= channel.positionKeys[j].first && model.animationTime <= channel.positionKeys[j + 1].first)
  //        {
  //          float alpha = (model.animationTime - channel.positionKeys[j].first) / (channel.positionKeys[j + 1].first - channel.positionKeys[j].first);
  //          glm::vec3 pos = glm::mix(channel.positionKeys[j].second, channel.positionKeys[j + 1].second, alpha);
  //          translation = glm::translate(glm::mat4(1.0f), pos);
  //          break;
  //        }
  //      }

  //      //find the rotation keyframes surrounding the current animation time
  //      for (int j = 0; j < channel.rotationKeys.size() - 1; j++)
  //      {
  //        if (model.animationTime >= channel.rotationKeys[j].first && model.animationTime <= channel.rotationKeys[j + 1].first)
  //        {
  //          float alpha = (model.animationTime - channel.rotationKeys[j].first) / (channel.rotationKeys[j + 1].first - channel.rotationKeys[j].first);
  //          glm::quat rot = glm::slerp(channel.rotationKeys[j].second, channel.rotationKeys[j + 1].second, alpha);
  //          rotation = glm::mat4_cast(rot);
  //          break;
  //        }
  //      }

  //      //find the scaling keyframes surrounding the current animation time
  //      for (int j = 0; j < channel.scalingKeys.size() - 1; j++)
  //      {
  //        if (model.animationTime >= channel.scalingKeys[j].first && model.animationTime <= channel.scalingKeys[j + 1].first)
  //        {
  //          float alpha = (model.animationTime - channel.scalingKeys[j].first) / (channel.scalingKeys[j + 1].first - channel.scalingKeys[j].first);
  //          glm::vec3 scale = glm::mix(channel.scalingKeys[j].second, channel.scalingKeys[j + 1].second, alpha);
  //          scaling = glm::scale(glm::mat4(1.0f), scale);
  //          break;
  //        }
  //      }

  //      model.nodes[i].transform = translation * rotation * scaling;
  //      //model.nodes[i].transform = model.nodes[i].transform * model.nodes[i].inverseBindPose;
  //      //model.nodes[i].transform = model.nodes[i].transform * glm::inverse(model.nodes[i].inverseBindPose);
  //    }
  //  }
  //}

  void Render::Init()
  {
    std::cout << "render inti" << std::endl;
    windowResc = Engine::GetInstance()->GetResource<WindowResource>();
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();

    renderResc->basicShaderProgram = CreateShaderProgram("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    renderResc->lineShaderProgram = CreateShaderProgram("assets/shaders/Ray.vert", "assets/shaders/Ray.frag");
  }

  void Render::PreUpdate()
  {
    std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Model>();

    for (const Entity& e : modelEntities)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);

      if (model.path != "" && model.VAOs.size() == 0)
      {
        LoadModel(model.path, model);

      }
    }
  }

  void Render::Update()
  {
    //AnimUpdate();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //modeling
    glUseProgram(renderResc->basicShaderProgram);

    std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Transform, Model>();

    //for each transform
    //for (std::optional<Transform>& transform : Engine::GetInstance()->transforms)
    for (const Entity& e : modelEntities)
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      transform.modelMatrix = glm::mat4(1.0f);
      transform.modelMatrix = glm::translate(transform.modelMatrix, transform.position);
      transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
      //transform.modelMatrix = transform.modelMatrix * glm::eulerAngleXYZ(glm::radians(transform.rotation.x), glm::radians(transform.rotation.y), glm::radians(transform.rotation.z));
      transform.modelMatrix = transform.modelMatrix * glm::mat4_cast(transform.rotationQuaternion);
    }

    //assuming exactly one main cam for now
    std::vector<Entity> cameraEntities = Engine::GetInstance()->GetEntities<Transform, Camera>();
    for (const Entity& e : cameraEntities)
    {
      Camera& camera = Engine::GetInstance()->GetComponent<Camera>(e);
      Transform& cameratransform = Engine::GetInstance()->GetComponent<Transform>(e);

      if (!camera.isMain) continue;

      //viewing
      viewMatrix = glm::mat4(1.0f);
      viewMatrix = glm::lookAt(cameratransform.position, cameratransform.position + camera.forward, camera.up);
      //projection
      projectionMatrix = glm::mat4(1.0f);
      projectionMatrix = glm::perspective(glm::radians(camera.fov), 1.0f, camera.nearPlane, camera.farPlane);
      glUniformMatrix4fv(2, 1, GL_FALSE, &viewMatrix[0][0]);
      glUniformMatrix4fv(3, 1, GL_FALSE, &projectionMatrix[0][0]);
    }

    for (const Entity& e : modelEntities)
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);

      if (model.VAOs.size() == 0) continue;

      //just assemble all of the node matrices into a big array for now

      if (Engine::GetInstance()->HasComponent<SkeletalAnimator>(e))
      {
        glm::mat4 nodeMatrices[300] = { glm::mat4(1.0f) };

        std::vector<Model::Node> nodesCopy = model.nodes;

        ApplyNodeHeirarchy(nodesCopy, 0, VQS());

        for (int i = 0; i < model.nodes.size(); i++)
        {
          //nodeMatrices[i] = model.nodes[i].skinningTransform;
          nodeMatrices[i] = nodesCopy[i].transform.ToMatrix() * nodesCopy[i].inverseBindPose;
        }

        glUniformMatrix4fv(50, 100, GL_FALSE, &nodeMatrices[0][0][0]);
        glUniform1i(200, 1);
      }
      else
      {
        glUniform1i(200, 0);
      }

      glUniform1i(5, model.textured ? 1 : 0);
      glUniform4fv(4, 1, &model.color[0]);
      glUniformMatrix4fv(1, 1, GL_FALSE, &transform.modelMatrix[0][0]);

      for (const auto& [vao, indexCount] : model.VAOs)
      {
        if (model.textured && model.textures.contains(vao))
        {
          int texIndex = 0;
          for (const GLuint& tex : model.textures[vao])
          {
            glActiveTexture(GL_TEXTURE0 + texIndex);
            glBindTexture(GL_TEXTURE_2D, tex);
            texIndex++;
          }
        }
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
      }
      //if (model.path == "assets/jack_samba.glb")
      RenderNodes(model, transform);
    }

    glUseProgram(0);
  }

  void Render::PostUpdate()
  {
    //line rendering

    glDisable(GL_DEPTH_TEST);

    glUseProgram(renderResc->lineShaderProgram);
    CreateLinesVAO();


    glUniformMatrix4fv(1, 1, false, &viewMatrix[0][0]);
    glUniformMatrix4fv(2, 1, false, &projectionMatrix[0][0]);

    glBindVertexArray(lineVAO);

    glDrawArrays(GL_LINES, 0, renderResc->rays.size());

    glBindVertexArray(0);

    glDeleteVertexArrays(1, &lineVAO);
    glUseProgram(0);

    glfwSwapBuffers(windowResc->window);
    glfwPollEvents();
    renderResc->rays.clear();

    glEnable(GL_DEPTH_TEST);
  }

  void Render::Exit()
  {
    if (windowResc->window)
    {
      glfwDestroyWindow(windowResc->window);
      glfwTerminate();
      windowResc->window = nullptr;
    }
  }

} // namespace FAR