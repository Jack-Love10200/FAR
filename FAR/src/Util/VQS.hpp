#pragma once

#include "PCH.hpp"

#include "glm/glm.hpp"

class VQS
{
public:

  glm::vec3 v; // translation
  //glm::quat q; // rotation
  FAR::Quat q; // rotation
  //float s;     // scale
  glm::vec3 s;

  //VQS() : v(0.0f), q(1.0f, 0.0f, 0.0f, 0.0f), s(1.0f) {}
  //VQS(const glm::vec3& translation, const glm::quat& rotation, float scale)
  //  : v(translation), q(rotation), s(scale) 
  //{
  //}

  VQS() : v(0.0f), q(1.0f, 0.0f, 0.0f, 0.0f), s(1.0f, 1.0f, 1.0f) {}
  VQS(const glm::vec3& translation, const FAR::Quat& rotation, glm::vec3 scale)
    : v(translation), q(rotation), s(scale) 
  {
  }

  VQS(const glm::mat4& matrix)
  {
    // Extract translation
    v = glm::vec3(matrix[3]);
    // Extract scale
    
    //s = glm::length(glm::vec3(matrix[0]));
    
    s.x = glm::length(glm::vec3(matrix[0]));
    s.y = glm::length(glm::vec3(matrix[1]));
    s.z = glm::length(glm::vec3(matrix[2]));

    // Extract rotation
    glm::mat3 rotMatrix = glm::mat3(matrix);
    rotMatrix[0] /= s.x;
    rotMatrix[1] /= s.y;
    rotMatrix[2] /= s.z;
    q = FAR::Quat::FromMatrix(rotMatrix);
  }

  // Convert to a 4x4 transformation matrix
  glm::mat4 ToMatrix() const 
  {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), v);
    glm::mat4 rotationMatrix = q.ToMatrix();
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(s));
    return translationMatrix * rotationMatrix * scaleMatrix;
  }
  // Combine two VQS transformations
  VQS operator*(const VQS& other) const 
  {
    glm::vec3 newV = *this * other.v; // Apply this transformation to other's translation
    FAR::Quat newQ = this->q * other.q; // Combine rotations

    //float newS = this->s * other.s; // Combine scales
    glm::vec3 newS = this->s * other.s; // Combine scales

    return VQS(newV, newQ, newS);
  }

  glm::vec3 operator*(const glm::vec3 & vec) const
  {
    glm::vec3 scaled = vec * s; //non-uniform scale

    glm::vec3 rotated = q * scaled;// *glm::inverse(q);
    glm::vec3 translated = rotated + v;
    return translated;
  }

  static float elerp(float a, float b, float f)
  {
    // Handle edge cases safely
    if (std::abs(a) < std::numeric_limits<float>::epsilon() ||
      std::abs(b) < std::numeric_limits<float>::epsilon() ||
      (a * b < 0)) {
      return glm::mix(a, b, f);  // Fallback to linear
    }
    return a * std::pow(b / a, f);
  }

  static glm::vec3 elerp(const glm::vec3& a, const glm::vec3& b, float f)
  {
    return glm::vec3(
      elerp(a.x, b.x, f),
      elerp(a.y, b.y, f),
      elerp(a.z, b.z, f)
    );
  }

  //lerp position
  //slerp rotation
  //elerp scale

  static VQS IncrementalInterpolate(VQS left, VQS right)
  {
    VQS retval;
    retval.v = left.v + right.v;
    retval.q = right.q * left.q;
    retval.s = left.s * right.s;
    return retval;
  }

  static VQS Interpolate(VQS left, VQS right, float alpha)
  {
    VQS retval;

    retval.v = glm::mix(left.v, right.v, alpha);
    retval.q = FAR::Quat::Slerp(left.q, right.q, alpha);
    retval.s = elerp(left.s, right.s, alpha);

    return retval;
  }


  // Inverse of the VQS transformation
  VQS inverse() const 
  {

  }

private:

};