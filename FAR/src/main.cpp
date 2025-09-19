
#include "PCH.hpp"

#include "GL/glew.h"
#include "GL/GL.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

//camera geometry
glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };
glm::vec3 cameraLook{ 0.0f, 0.0f, -1.0f };
glm::vec3 cameraUp{ 0.0f, 1.0f, 0.0f };

float farPlane = 100.0f;
float nearPlane = 1.0f;
float fov = 90.0f;

//object geometry
glm::vec3 objPos{ 0.0f, 0.0f, -3.0f };
glm::vec3 objScale{ 1.0f, 1.0f, 1.0f };
glm::vec3 objRot{ -90.0f, 0.0f, 0.0f };

glm::mat4 Modeling;
glm::mat4 Viewing;
glm::mat4 Projection;

int indexCount;

struct vertex
{
  glm::vec4 position;
  //glm::vec4 normal;
  glm::vec2 uv;
};

struct mesh
{
  std::vector<vertex> verticies;
  std::vector<unsigned int> indicies;
  GLuint texture;
};

const char* vshader =
"#version 430 core\n"
"layout(location = 0) in vec4 vertex_position;\n"
"layout(location = 1) in vec2 uv;\n"
""
"layout(location = 1) uniform mat4 Modeling;\n"
"layout(location = 2) uniform mat4 Viewing;\n"
"layout(location = 3) uniform mat4 Projection;\n"
"out vec2 frag_uv;\n"
"void main()\n"
"{\n"
"   gl_Position = Projection * Viewing * Modeling * vertex_position;\n"
"   frag_uv = uv;\n"
"}\n";

const char* fshader =
"#version 430 core\n"
"layout(location = 0) uniform sampler2D tex;\n"
"in vec2 frag_uv;\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"   frag_color = texture(tex, frag_uv);\n"
"}\n";

GLuint shaderprogram;
GLuint texture;

void CreateShaderProgram()
{
  GLint error;

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
}

GLuint vao;

mesh LoadModel()
{
  Assimp::Importer importer;
  const aiScene* samba = importer.ReadFile("assets/jack_samba.glb", 0);

  if (samba->HasTextures())
  {
    std::cout << "model has textures" << std::endl;
    samba->mTextures[0]->mHeight;
  }

  mesh m;
  m.verticies.resize(samba->mMeshes[0]->mNumVertices);
  m.indicies.resize(samba->mMeshes[0]->mNumFaces * 3);

  stbi_set_flip_vertically_on_load(1);

  int width, height, channels;
  stbi_uc* sambaa = stbi_load_from_memory(
    (const stbi_uc*)samba->mTextures[0]->pcData,
    samba->mTextures[0]->mWidth,
    &width, &height, &channels,
    3);

  // GPU buffer for texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
    0, GL_RGB, GL_UNSIGNED_BYTE, sambaa);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  for (unsigned int i = 0; i < samba->mMeshes[0]->mNumVertices; i++)
  {
    m.verticies[i].position = glm::vec4(samba->mMeshes[0]->mVertices[i].x, samba->mMeshes[0]->mVertices[i].y, samba->mMeshes[0]->mVertices[i].z, 1.0f);
  }

  for (unsigned int i = 0; i < samba->mMeshes[0]->mNumFaces; i++)
  {
    m.indicies[i * 3 + 0] = samba->mMeshes[0]->mFaces[i].mIndices[0];
    m.indicies[i * 3 + 1] = samba->mMeshes[0]->mFaces[i].mIndices[1];
    m.indicies[i * 3 + 2] = samba->mMeshes[0]->mFaces[i].mIndices[2];
  }

  for (unsigned int i = 0; i < samba->mMeshes[0]->mNumVertices; i++)
  {
    if (samba->mMeshes[0]->HasTextureCoords(0))
    {
      m.verticies[i].uv = glm::vec2(samba->mMeshes[0]->mTextureCoords[0][i].x, samba->mMeshes[0]->mTextureCoords[0][i].y);
    }
    else
    {
      m.verticies[i].uv = glm::vec2(0.0f, 0.0f);
    }
  }

  indexCount = m.indicies.size();

  //move object to (0, 0, 0) and scale to 1x1x1
  glm::vec4 max = {};
  glm::vec4 min = {};
  for (vertex vert : m.verticies)
  {
    max.x = std::max(vert.position.x, max.x);
    max.y = std::max(vert.position.y, max.y);
    max.z = std::max(vert.position.z, max.z);

    min.x = std::min(vert.position.x, min.x);
    min.y = std::min(vert.position.y, min.y);
    min.z = std::min(vert.position.z, min.z);
  }

  glm::vec4 center((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2, 0.0f);
  float scaleFactor = std::max(max.x - min.x, std::max(max.y - min.y, max.z - min.z));

  for (vertex& vert : m.verticies)
  {
    vert.position -= center;
    vert.position /= scaleFactor;
  }

  return m;
}

void CreateVAO()
{
  //get the mesh
  mesh m = LoadModel();

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
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

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
}




void Update()
{
  //modeling
  Modeling = glm::mat4(1.0f);
  Modeling = glm::translate(Modeling, objPos);
  Modeling = glm::scale(Modeling, objScale);
  Modeling = Modeling * glm::eulerAngleXYZ(glm::radians(objRot.x), glm::radians(objRot.y), glm::radians(objRot.z));
  //viewing
  Viewing = glm::mat4(1.0f);
  Viewing = glm::lookAt(cameraPos, cameraPos + cameraLook, cameraUp);
  //projection
  Projection = glm::mat4(1.0f);
  Projection = glm::perspective(glm::radians(fov), 1.0f, nearPlane, farPlane);
  glUseProgram(shaderprogram);
  glUniformMatrix4fv(1, 1, GL_FALSE, &Modeling[0][0]);
  glUniformMatrix4fv(2, 1, GL_FALSE, &Viewing[0][0]);
  glUniformMatrix4fv(3, 1, GL_FALSE, &Projection[0][0]);
}

void Render()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glUseProgram(shaderprogram);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

int main()
{

  glfwInit();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow* window = glfwCreateWindow(2000, 1200, "test", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  const char* err;
  glfwGetError(&err);
  if (err)
    std::cout << err << std::endl;

  GLenum glewerr = glewInit();
  if (glewerr != GLEW_OK)
  {
    std::cerr << "Error: Unable to initialize GLEW " << glewGetErrorString(glewerr) << std::endl;
    glfwDestroyWindow(window); // destroy window and corresponding GL context
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  CreateShaderProgram();
  CreateVAO();

  while (!glfwWindowShouldClose(window))
  {
    Update();
    Render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

    std::cout << "Hello World!\n";
}
