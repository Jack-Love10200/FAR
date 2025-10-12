///
/// @file   Render.hpp
/// @brief  System for rendering models to the screen using OpenGL
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once
#include "PCH/PCH.hpp"

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

    //engine steps
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

    //load, upload, and link a shader program from vertex and fragment shader files
    GLuint CreateShaderProgram(const std::filesystem::path& vertex, const std::filesystem::path& fragment);

    //load a model from a file into a Model component
    void LoadModel(const std::filesystem::path& filepath, Model& model);

    //create a gpu vao from a mesh
    void CreateVAO(const meshInfo& m, Model& model);

    //load all of the nodes from an assimp scene into a model component
    void LoadNodes(const aiNode* node, const aiScene* scene, Model& model, int parentIndex, VQS parentTransform);

    //render all of a model's nodes as lines
    void RenderNodes(Model& model, Transform& trans);
    //helper for RenderNodes, recursively builds a list of points representing the bones
    void BuildBonePointList(Model& model, std::vector<glm::vec4>& points, int index, VQS parentTrans);

    //apply bone weights from an assimp mesh to a meshInfo struct
    void ApplyBoneWeightsToVerticies(meshInfo& m, const aiMesh* mesh, Model& model);


    void GetInverseBindPositions(Model& model, aiMesh* mesh);

    void ApplyNodeHeirarchy(std::vector<Model::Node>& nodes, int nodeIndex, const VQS& parentTransform);
    void CreateLinesVAO();

    GLuint lineVAO = 0;

    WindowResource* windowResc = nullptr;
    RenderResource* renderResc = nullptr;

    glm::mat4 viewMatrix{ 1.0f };
    glm::mat4 projectionMatrix{ 1.0f };
  };
}
