#include "PCH.hpp"

class VQS
{
public:

  glm::vec3 v; // translation
  glm::quat q; // rotation
  float s;     // scale
  VQS() : v(0.0f), q(1.0f, 0.0f, 0.0f, 0.0f), s(1.0f) {}
  VQS(const glm::vec3& translation, const glm::quat& rotation, float scale)
    : v(translation), q(rotation), s(scale) {
  }

  VQS(const glm::mat4& matrix)
  {
    // Extract translation
    v = glm::vec3(matrix[3]);
    // Extract scale
    s = glm::length(glm::vec3(matrix[0]));
    // Extract rotation
    glm::mat3 rotMatrix = glm::mat3(matrix);
    rotMatrix[0] /= s;
    rotMatrix[1] /= s;
    rotMatrix[2] /= s;
    q = glm::quat_cast(rotMatrix);
  }

  // Convert to a 4x4 transformation matrix
  glm::mat4 ToMatrix() const 
  {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), v);
    glm::mat4 rotationMatrix = glm::mat4_cast(q);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(s));
    return translationMatrix * rotationMatrix * scaleMatrix;
  }
  // Combine two VQS transformations
  VQS operator*(const VQS& other) const 
  {
    glm::vec3 newV = *this * other.v; // Apply this transformation to other's translation
    glm::quat newQ = this->q * other.q; // Combine rotations
    float newS = this->s * other.s; // Combine scales
    return VQS(newV, newQ, newS);
  }

  glm::vec3 operator*(const glm::vec3 & vec) const
  {
    glm::vec3 scaled = vec * s;
    glm::vec3 rotated = q * scaled * glm::inverse(q);
    glm::vec3 translated = rotated + v;
    return translated;
  }


  // Inverse of the VQS transformation
  VQS inverse() const 
  {

  }

private:

};