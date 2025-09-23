#include "PCH.hpp"
#include "Render.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Engine/Engine.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>

#include "stb_image.h"

namespace FAR
{

  void Render::WindowSetup()
  {

  }

  void Render::CreateShaderProgram()  
  {
    GLint error;

    std::ifstream vshaderfile("assets/shaders/basic.vert");
    std::ifstream fshaderfile("assets/shaders/basic.frag");

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

    shaderprogram = glCreateProgram();
    glAttachShader(shaderprogram, vertshader);
    glAttachShader(shaderprogram, fragshader);
    glLinkProgram(shaderprogram);
    glGetProgramiv(shaderprogram, GL_LINK_STATUS, &error);
    if (!error)
    {
      std::cerr << "program error" << std::endl;
      char buffer[1024];
      glGetProgramInfoLog(shaderprogram, 1024, 0, buffer);
      std::cerr << buffer << std::endl;
      throw std::runtime_error(buffer);
    }

    glDeleteShader(vertshader);
    glDeleteShader(fragshader);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    //bind texture samplers to texture units
    glUseProgram(shaderprogram);
    GLint texturesUniformLocation = 6;
    GLint samplers[16];
    for (int i = 0; i < 16; i++) samplers[i] = i;
    glUniform1iv(texturesUniformLocation, 16, samplers);
    glUseProgram(0);
  }

  void Render::LoadModel(const std::filesystem::path& filepath, Model& model)
  {
    Assimp::Importer importer;
    const aiScene* samba = importer.ReadFile(filepath.string(), aiProcess_Triangulate);

    //m.verticies.resize(samba->mMeshes[0]->mNumVertices);
    //m.indicies.resize(samba->mMeshes[0]->mNumFaces * 3);

    stbi_set_flip_vertically_on_load(true);

    //meshes
    for (unsigned int i = 0; i < samba->mNumMeshes; i++)
    {
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
          m.verticies.push_back(vertex{ glm::vec4(samba->mMeshes[i]->mVertices[j].x, samba->mMeshes[i]->mVertices[j].y, samba->mMeshes[i]->mVertices[j].z, 1.0f)
            , glm::vec3(samba->mMeshes[i]->mTextureCoords[0][j].x, samba->mMeshes[i]->mTextureCoords[0][j].y, i) });
        }
      }

      for (unsigned int j = 0; j < samba->mMeshes[i]->mNumFaces; j++)
      {
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[0]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[1]);
        m.indicies.push_back(numVerts + samba->mMeshes[i]->mFaces[j].mIndices[2]);
      }

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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //bind ibo to vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    //done with vao
    glBindVertexArray(0);

    model.VAOs.push_back(std::make_pair(currentVAO, m.indicies.size()));
  }

  void Render::Init()
  {
    std::cout << "render inti" << std::endl;
    windowResc = Engine::GetInstance()->GetResource<WindowResource>();

    WindowSetup();
    CreateShaderProgram();
  }

  void Render::PreUpdate()
  {
    std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Model>();

    for (const Entity& e : modelEntities)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);

      if (model.path != "" && model.VAOs.size() == 0)
        LoadModel(model.path, model);
    }
  }

  void Render::Update()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //modeling
    glUseProgram(shaderprogram);

    std::vector<Entity> modelEntities = Engine::GetInstance()->GetEntities<Transform, Model>();

    //for each transform
    //for (std::optional<Transform>& transform : Engine::GetInstance()->transforms)
    for (const Entity& e : modelEntities)
    {
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      transform.modelMatrix = glm::mat4(1.0f);
      transform.modelMatrix = glm::translate(transform.modelMatrix, transform.position);
      transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
      transform.modelMatrix = transform.modelMatrix * glm::eulerAngleXYZ(glm::radians(transform.rotation.x), glm::radians(transform.rotation.y), glm::radians(transform.rotation.z));
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

      glUniformMatrix4fv(1, 1, GL_FALSE, &transform.modelMatrix[0][0]);

      glUniform1i(5, model.textured ? 1 : 0);
      glUniform4fv(4, 1, &model.color[0]);

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
    }

    glUseProgram(0);
  }

  void Render::PostUpdate()
  {
    glfwSwapBuffers(windowResc->window);
    glfwPollEvents();
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