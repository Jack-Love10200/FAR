///
/// @file   MathHelpers.hpp
/// @brief  A few math helper functions for interpolation and conversions
/// @author Jack Love
/// @date   11.10.2025
///
#include "PCH/PCH.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace FAR
{
  //exponential interpolation
  float elerp(float a, float b, float f);
  //exponential interpolation for vec3
  glm::vec3 elerp(const glm::vec3& a, const glm::vec3& b, float f);

  //convert aiMatrix4x4 to glm::mat4
  glm::mat4 ToGlm(const aiMatrix4x4& m);
}