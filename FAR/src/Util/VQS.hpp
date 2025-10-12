///
/// @file   VQS.hpp
/// @brief  Vector, Quaternion, Scale transform representation and operations
/// @author Jack Love
/// @date   11.10.2025
///
#pragma once
#include "PCH/PCH.hpp"
#include "glm/glm.hpp"

namespace FAR
{
  class VQS
  {
  public:

    glm::vec3 v; // translation
    FAR::Quat q; // rotation
    glm::vec3 s; // scale

    VQS() : v(0.0f), q(1.0f, 0.0f, 0.0f, 0.0f), s(1.0f, 1.0f, 1.0f) {}
    VQS(const glm::vec3& translation, const FAR::Quat& rotation, glm::vec3 scale)
      : v(translation), q(rotation), s(scale)
    {
    }

    VQS(const glm::mat4& matrix);

    // Convert to a 4x4 transformation matrix
    glm::mat4 ToMatrix() const;

    // Combine two VQS transformations
    VQS operator*(const VQS& other) const;

    // Apply the VQS transformation to a 3D vector
    glm::vec3 operator*(const glm::vec3& vec) const;

    //interpolate between two VQS transformations
    static VQS Interpolate(VQS left, VQS right, float alpha);
    static VQS IncrementalInterpolate(VQS left, VQS right);

  private:

  };
}