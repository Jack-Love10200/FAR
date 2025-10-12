///
/// @file   VQS.cpp
/// @brief  Vector, Quaternion, Scale transform representation and operations
/// @author Jack Love
/// @date   11.10.2025
///

#include "PCH/PCH.hpp"
#include "Util/VQS.hpp"

#include "glm/glm.hpp"
#include "MathHelpers.hpp"

namespace FAR
{

  VQS::VQS(const glm::mat4& matrix)
  {
    // Extract translation
    v = glm::vec3(matrix[3]);

    // Extract scale
    s.x = glm::length(glm::vec3(matrix[0]));
    s.y = glm::length(glm::vec3(matrix[1]));
    s.z = glm::length(glm::vec3(matrix[2]));

    // Extract rotation
    glm::mat3 rotMatrix = glm::mat3(matrix);
    rotMatrix[0] /= s.x;
    rotMatrix[1] /= s.y;
    rotMatrix[2] /= s.z;
    q = Quat::FromMatrix(rotMatrix);
  }

  glm::mat4 VQS::ToMatrix() const
  {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), v);
    glm::mat4 rotationMatrix = q.ToMatrix();
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(s));
    return translationMatrix * rotationMatrix * scaleMatrix;
  }

  VQS VQS::operator*(const VQS& other) const
  {
    glm::vec3 newV = *this * other.v; //apply this transformation to other's translation
    FAR::Quat newQ = this->q * other.q; //combine rotations
    glm::vec3 newS = this->s * other.s; //combine scales
    return VQS(newV, newQ, newS);
  }

  glm::vec3 VQS::operator*(const glm::vec3& vec) const
  {
    glm::vec3 scaled = vec * s; //non-uniform scale

    glm::vec3 rotated = q * scaled;// *glm::inverse(q);
    glm::vec3 translated = rotated + v;
    return translated;
  }

  VQS VQS::IncrementalInterpolate(VQS left, VQS right)
  {
    VQS retval;
    retval.v = left.v + right.v;
    retval.q = right.q * left.q;
    retval.s = left.s * right.s;
    return retval;
  }


  VQS VQS::Interpolate(VQS left, VQS right, float alpha)
  {
    VQS retval;
    //lerp position
    retval.v = glm::mix(left.v, right.v, alpha);
    //slerp rotation
    retval.q = FAR::Quat::Slerp(left.q, right.q, alpha);
    //elerp scale
    retval.s = elerp(left.s, right.s, alpha);
    return retval;
  }
}