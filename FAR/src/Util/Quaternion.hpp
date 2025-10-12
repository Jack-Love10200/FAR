///
/// @file   Quaternion.hpp
/// @brief  Quaternion class for representing rotations
/// @author Jack Love
/// @date   11.10.2025
///

#pragma once
#include <glm/glm.hpp>

namespace FAR
{
  class Quat
  {
  public:

    float w; // scalar part
    float x;
    float y;
    float z;

    //constructors
    Quat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    Quat(float w, glm::vec3 v) : w(w), x(v.x), y(v.y), z(v.z) {}
    
    //quat-quat operations
    Quat operator*(const Quat& other) const;
    Quat operator+(const Quat& other) const;
    static Quat Slerp(const Quat& q1, const Quat& q2, float alpha);
    static float Dot(const Quat& q1, const Quat& q2);

    //quat-other operations
    glm::vec3 operator*(const glm::vec3& v) const;
    Quat operator*(float scalar) const;
    friend Quat operator*(float scalar, const Quat& q);
    friend Quat operator/(const Quat& q, float scalar);

    //conversions to quat
    static Quat FromAxisAngle(float angle, const glm::vec3& axis);
    static Quat FromMatrix(glm::mat4 m);

    //other operations
    Quat Normalize() const;
    Quat Conjugate() const;
    glm::vec3 GetVectorPart();
    float Length() const;

    //conversions from quat
    glm::mat4 ToMatrix() const;
  };
}