///
/// @file   MathHelpers.cpp
/// @brief  A few math helper functions for interpolation and conversions
/// @author Jack Love
/// @date   11.10.2025
///
#include "PCH/PCH.hpp"
#include "Util/MathHelpers.hpp"

namespace FAR
{
  float elerp(float a, float b, float f)
  {
    // Handle edge cases safely
    if (std::abs(a) < std::numeric_limits<float>::epsilon() ||
      std::abs(b) < std::numeric_limits<float>::epsilon() ||
      (a * b < 0)) {
      return glm::mix(a, b, f);  // Fallback to linear
    }
    return a * std::pow(b / a, f);
  }

  glm::vec3 elerp(const glm::vec3& a, const glm::vec3& b, float f)
  {
    return glm::vec3(
      elerp(a.x, b.x, f),
      elerp(a.y, b.y, f),
      elerp(a.z, b.z, f)
    );
  }

  glm::mat4 ToGlm(const aiMatrix4x4& m) {
    return glm::mat4(
      m.a1, m.b1, m.c1, m.d1,
      m.a2, m.b2, m.c2, m.d2,
      m.a3, m.b3, m.c3, m.d3,
      m.a4, m.b4, m.c4, m.d4
    );
  }
}