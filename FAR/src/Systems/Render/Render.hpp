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
      glm::vec3 uv;
    };

    struct meshInfo
    {
      std::vector<vertex> verticies;
      std::vector<unsigned int> indicies;
    };

    void WindowSetup();
    GLuint CreateShaderProgram(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
    void LoadModel(const std::filesystem::path& filepath, Model& model);
    void CreateVAO(const meshInfo& m, Model& model);

    void CreateLinesVAO();


    GLuint lineVAO;

    WindowResource* windowResc;
    RenderResource* renderResc;

    glm::mat4 viewMatrix{ 1.0f };
    glm::mat4 projectionMatrix{ 1.0f };
  };
}
