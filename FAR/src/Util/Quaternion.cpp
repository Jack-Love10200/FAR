///
/// @file   Quaternion.cpp
/// @brief  Quaternion class for representing rotations
/// @author Jack Love
/// @date   11.10.2025
///

#include "PCH/PCH.hpp"
#include "Quaternion.hpp"

namespace FAR
{
  Quat Quat::operator*(const Quat& other) const
  {
    glm::vec3 v = glm::vec3(x, y, z);
    glm::vec3 qv = glm::vec3(other.x, other.y, other.z);
    return Quat(w * other.w - glm::dot(v, qv), glm::vec3(w * qv + other.w * v + glm::cross(v, qv)));
  }

  glm::vec3 Quat::operator*(const glm::vec3& v) const
  {
    glm::vec3 qVec(x, y, z);
    glm::vec3 t = 2.0f * glm::cross(qVec, v);
    return v + w * t + glm::cross(qVec, t);
  }

  glm::vec3 Quat::GetVectorPart()
  {
    return glm::vec3(x, y, z);
  }

  float Quat::Length() const
  {
    return std::sqrt(w * w + x * x + y * y + z * z);
  }

  Quat Quat::Normalize() const
  {
    float n = Length();
    if (n > std::numeric_limits<float>::epsilon())
    {
      return Quat(w / n, x / n, y / n, z / n);
    }
    return Quat(1.0f, 0.0f, 0.0f, 0.0f); // Return identity if norm is zero
  }

  Quat Quat::Conjugate() const
  {
    return Quat(w, -x, -y, -z);
  }

  Quat Quat::operator*(float scalar) const
  {
    return Quat(w * scalar, x * scalar, y * scalar, z * scalar);
  }

  Quat operator*(float scalar, const Quat& q)
  {
    return Quat(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
  }

  Quat operator/(const Quat& q, float scalar)
  {
    return Quat(q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar);
  }

  Quat Quat::operator+(const Quat& other) const
  {
    return Quat(w + other.w, x + other.x, y + other.y, z + other.z);
  }

  float Quat::Dot(const Quat& q1, const Quat& q2)
  {
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
  }

  Quat Quat::Slerp(const Quat& q1, const Quat& q2, float alpha)
  {
    Quat z = q2;

    float cosTheta = Dot(q1, q2);

    // If cosTheta < 0, the interpolation will take the long way around the sphere.
    // To fix this, one quat must be negated.
    if (cosTheta < 0.0f)
    {
      z = q2 * -1;
      cosTheta = -cosTheta;
    }

    // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
    if (cosTheta > 1.0f - std::numeric_limits<float>::epsilon())
    {
      // Linear interpolation
      return Quat(
        glm::mix(q1.w, z.w, alpha),
        glm::mix(q1.x, z.x, alpha),
        glm::mix(q1.y, z.y, alpha),
        glm::mix(q1.z, z.z, alpha));
    }
    else
    {
      // Essential Mathematics, page 467
      float angle = acos(cosTheta);
      return (sin((static_cast<float>(1) - alpha) * angle) * q1 + sin(alpha * angle) * z) / sin(angle);
    }
  }

  Quat Quat::FromAxisAngle(float angle, const glm::vec3& axis)
  {
    float halfAngle = angle * 0.5f;
    float s = std::sin(halfAngle);
    return Quat(std::cos(halfAngle), axis.x * s, axis.y * s, axis.z * s).Normalize();
  }

  Quat Quat::FromMatrix(glm::mat4 m)
  {
    float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
    float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
    float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
    float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
    {
      fourBiggestSquaredMinus1 = fourXSquaredMinus1;
      biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
    {
      fourBiggestSquaredMinus1 = fourYSquaredMinus1;
      biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
    {
      fourBiggestSquaredMinus1 = fourZSquaredMinus1;
      biggestIndex = 3;
    }

    float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    float mult = 0.25f / biggestVal;

    switch (biggestIndex)
    {
    case 0:
      return Quat(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
    case 1:
      return Quat((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
    case 2:
      return Quat((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
    case 3:
      return Quat((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
    default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
      assert(false);
      return Quat(1, 0, 0, 0);
    }
  }

  glm::mat4 Quat::ToMatrix() const
  {
    Quat& q = const_cast<Quat&>(*this);

    glm::mat4 Result(1.0f);
    float qxx(q.x * q.x);
    float qyy(q.y * q.y);
    float qzz(q.z * q.z);
    float qxz(q.x * q.z);
    float qxy(q.x * q.y);
    float qyz(q.y * q.z);
    float qwx(q.w * q.x);
    float qwy(q.w * q.y);
    float qwz(q.w * q.z);

    Result[0][0] = 1.0f - 2.0f * (qyy + qzz);
    Result[0][1] = 2.0f * (qxy + qwz);
    Result[0][2] = 2.0f * (qxz - qwy);

    Result[1][0] = 2.0f * (qxy - qwz);
    Result[1][1] = 1.0f - 2.0f * (qxx + qzz);
    Result[1][2] = 2.0f * (qyz + qwx);

    Result[2][0] = 2.0f * (qxz + qwy);
    Result[2][1] = 2.0f * (qyz - qwx);
    Result[2][2] = 1.0f - 2.0f * (qxx + qyy);
    return Result;
  }
}
