///
/// @file   Render.cpp
/// @brief  System for rendering models to the screen using OpenGL
/// @author Jack Love
/// @date   11.10.2025
///
#include "PCH/PCH.hpp"
#include "Render.hpp"

#include "Engine/Engine.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>

#include "stb_image.h"

#include "Components/SkeletalAnimator.hpp"

#include "Util/MathHelpers.hpp"

namespace FAR
{

  GLuint Render::CreateShaderProgram(const std::filesystem::path& vertex, const std::filesystem::path& fragment)
  {
    GLint error;

    //read in shader files
    std::ifstream vshaderfile(vertex.string());
    std::ifstream fshaderfile(fragment.string());
    std::string vshaderstr((std::istreambuf_iterator<char>(vshaderfile)), std::istreambuf_iterator<char>());
    std::string fshaderstr((std::istreambuf_iterator<char>(fshaderfile)), std::istreambuf_iterator<char>());
    const char* vshader = vshaderstr.c_str();
    const char* fshader = fshaderstr.c_str();

    //upload and compile vertex shader
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

    //upload and compile fragment shader
    GLuint fragshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragshader, 1, &fshader, nullptr);
    glCompileShader(fragshader);
    glGetShaderiv(vertshader, GL_COMPILE_STATUS, &error);
    if (!error)
    {
      char infoLog[512];
      glGetShaderInfoLog(vertshader, 512, nullptr, infoLog);
      std::cout << "Fragment Shader Compilation Failed\n" << infoLog << std::endl;
    }

    //link shaders into a program
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


    //clean up shaders, we don't need them anymore
    glDeleteShader(vertshader);
    glDeleteShader(fragshader);

    //TODO:
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

    //for each mesh
    for (unsigned int i = 0; i < samba->mNumMeshes; i++)
    {

      GetInverseBindPositions(model, samba->mMeshes[i]);

      meshInfo m;
      unsigned int numVerts = m.verticies.size();

      //get all verticies
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

      //get all faces
      for (unsigned int j = 0; j < samba->mMeshes[i]->mNumFaces; j++)
      {
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[0]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[1]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[2]);
      }

      ApplyBoneWeightsToVerticies(m, samba->mMeshes[i], model);

      CreateVAO(m, model);


      //get all of the textures for this mesh, upload to gpu, and store in model.textures
      std::vector<GLuint> empty;
      model.textures.insert(model.textures.end(), std::make_pair(model.VAOs.back().first, empty));
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

          //read in texture data
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

          //create buffer and upload to gpu
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

          //save texture in model
          model.textures[model.VAOs.back().first].push_back(currentTex);

          //free image data
          stbi_image_free(sambaa);
        }
      }
    }
    //free importer data
    importer.FreeScene();
  }

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
    Model::Node newNode;

    newNode.parent = parentIndex;
    newNode.name = node->mName.C_Str();

    VQS localTransform = VQS(ToGlm(node->mTransformation));
    newNode.transform = parentTransform * localTransform;

    int currentIndex = model.nodes.size();
    model.nodes.push_back(newNode);

    if (parentIndex != -1)
      model.nodes[parentIndex].children.push_back(currentIndex);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      LoadNodes(node->mChildren[i], scene, model, currentIndex, VQS());
    }
  }

  void Render::RenderNodes(Model& model, Transform& trans)
  {
    std::vector<glm::vec4> points;

    BuildBonePointList(model, points, 3, VQS());

    //put points in world space
    for (glm::vec4& point : points)
    {
      point = trans.modelMatrix * point;
    }

    //draw lines
    for (int i = 0; i < points.size(); i += 2)
    {
      renderResc->DrawRay(points[i], points[i + 1]);
    }
  }

  void Render::BuildBonePointList(Model& model, std::vector<glm::vec4>& points, int index, VQS parentTrans)
  {


    VQS localTrans = model.nodes[index].transform;
    VQS globalTrans = parentTrans * localTrans;

    glm::vec4 pos = glm::vec4(globalTrans.v, 1.0f);

    for (int& i : model.nodes[index].children)
    {
      //skip sword and shield bones for the cs460, they look unsightly
      if (model.nodes[i].name == "sword" || model.nodes[i].name == "shield")
        continue;

      VQS childTrans = model.nodes[i].transform;
      VQS childGlobalTrans = globalTrans * childTrans;
      glm::vec4 childPos = glm::vec4(childGlobalTrans.v, 1.0f);

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

  void Render::GetInverseBindPositions(Model& model, aiMesh* mesh)
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
    //set render target to our offscreen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderResc->fbo);
    glViewport(0, 0, renderResc->vpwidth, renderResc->vpheight);

    //clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set program
    glUseProgram(renderResc->basicShaderProgram);

    //get all of our entities to be rendered
    std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Transform, Model>();

    //recompute model matrices
    for (const Entity& e : modelEntities)
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      transform.modelMatrix = glm::mat4(1.0f);
      transform.modelMatrix = glm::translate(transform.modelMatrix, transform.position);
      transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
      transform.modelMatrix = transform.modelMatrix * transform.rotationQuaternion.ToMatrix();
    }

    //assuming exactly one main cam for now, get view and projection matrices from it and upload to shader
    std::vector<Entity> cameraEntities = Engine::GetInstance()->GetEntities<Transform, Camera>();
    for (const Entity& e : cameraEntities)
    {
      Camera& camera = Engine::GetInstance()->GetComponent<Camera>(e);
      Transform& cameratransform = Engine::GetInstance()->GetComponent<Transform>(e);

      if (!camera.isMain) continue;

      camera.aspect = (float)renderResc->vpwidth / (float)renderResc->vpheight;

      //viewing
      viewMatrix = glm::mat4(1.0f);
      viewMatrix = glm::lookAt(cameratransform.position, cameratransform.position + camera.forward, camera.up);

      //projection
      projectionMatrix = glm::mat4(1.0f);
      projectionMatrix = glm::perspective(glm::radians(camera.fov), camera.aspect, camera.nearPlane, camera.farPlane);
      glUniformMatrix4fv(2, 1, GL_FALSE, &viewMatrix[0][0]);
      glUniformMatrix4fv(3, 1, GL_FALSE, &projectionMatrix[0][0]);
    }

    //rending each entity
    for (const Entity& e : modelEntities)
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);

      if (model.VAOs.size() == 0) continue;

      //TODO: Change this
      //just apply the node heirarchy and get final matricies into one big array here for now
      if (Engine::GetInstance()->HasComponent<SkeletalAnimator>(e))
      {
        glm::mat4 nodeMatrices[300] = { glm::mat4(1.0f) };
        std::vector<Model::Node> nodesCopy = model.nodes;
        ApplyNodeHeirarchy(nodesCopy, 0, VQS());
        for (int i = 0; i < model.nodes.size(); i++)
        {
          nodeMatrices[i] = nodesCopy[i].transform.ToMatrix() * nodesCopy[i].inverseBindPose;
        }
        glUniformMatrix4fv(50, 100, GL_FALSE, &nodeMatrices[0][0][0]);
        glUniform1i(200, 1); //using skinning
      }
      else
      {
        glUniform1i(200, 0); //not using skinning
      }

      //set textured, color, and model matrix uniforms
      glUniform1i(5, model.textured ? 1 : 0);
      glUniform4fv(4, 1, &model.color[0]);
      glUniformMatrix4fv(1, 1, GL_FALSE, &transform.modelMatrix[0][0]);

      //render each of the model's mesh vaos
      for (const auto& [vao, indexCount] : model.VAOs)
      {
        //bind all of the textures for this vao if any
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

      //draw bones/nodes
      RenderNodes(model, transform);
    }
    glUseProgram(0);

    //done rendering to offscreen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



  }


  void Render::PostUpdate()
  {
    //line rendering

    //set render target to our offscreen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderResc->fbo);
    glViewport(0, 0, renderResc->vpwidth, renderResc->vpheight);

    //lines always on top
    glDisable(GL_DEPTH_TEST);

    //lines shader program
    glUseProgram(renderResc->lineShaderProgram);
    CreateLinesVAO();

    //upload view and projection matrices
    glUniformMatrix4fv(1, 1, false, &viewMatrix[0][0]);
    glUniformMatrix4fv(2, 1, false, &projectionMatrix[0][0]);

    //draw lines
    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, renderResc->rays.size());
    glBindVertexArray(0);

    //cleanup
    glDeleteVertexArrays(1, &lineVAO);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    //end of frame stuff
    glfwSwapBuffers(windowResc->window);
    glfwPollEvents();
    renderResc->rays.clear();
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