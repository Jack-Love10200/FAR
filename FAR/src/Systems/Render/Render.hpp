#pragma once
#include "PCH.hpp"

#include "GL/glew.h"
#include "GL/GL.h"
#include "GLFW/glfw3.h"

#include "Systems/iSystem.hpp"

#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"

#include "Resources/WindowResource.hpp"
#include "Resources/RenderResource.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace FAR
{
  class Render : public iSystem
  {
  public:
    Render() = default;
    ~Render() override = default;
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;

  private:

    struct vertex
    {
      glm::vec4 position;
      //glm::vec4 normal;
      glm::vec4 uv;
      
      float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
      int boneIds[4] = { -1, -1, -1, -1 };
    };

    struct meshInfo
    {
      std::vector<vertex> verticies;
      std::vector<unsigned int> indicies;
    };

    GLuint CreateShaderProgram(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
    void LoadModel(const std::filesystem::path& filepath, Model& model);

    void LoadAnimationData(const aiScene* scene, Model& model);
    void CreateVAO(const meshInfo& m, Model& model);

    void LoadNodes(const aiNode* node, const aiScene* scene, Model& model, int parentIndex);

    void RenderNodes(Model& model, Transform& trans);
    void BuildBonePointList(Model& model, std::vector<glm::vec4>& points, int index, VQS parentTrans);

    void ApplyBoneWeightsToVerticies(meshInfo& m, const aiMesh* mesh);

    void PutNodesInModelSpace(Model& model, aiMesh* mesh);

    void AnimUpdateNode(Model& model, int nodeIndex, float animationTime, const VQS& parentTransform);


    void CreateLinesVAO();

    void AnimUpdate();


    GLuint lineVAO;

    WindowResource* windowResc;
    RenderResource* renderResc;

    glm::mat4 viewMatrix{ 1.0f };
    glm::mat4 projectionMatrix{ 1.0f };
  };
}
