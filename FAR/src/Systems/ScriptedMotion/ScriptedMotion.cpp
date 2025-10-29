#include "PCH/PCH.hpp"

#include "ScriptedMotion.hpp"

#include "Components/Transform.hpp"

#include "Engine/Engine.hpp"

#include "Util/MathHelpers.hpp"

namespace FAR
{
  void ScriptedMotion::Init()
  {
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();
  }

  void ScriptedMotion::PreUpdate()
  {

  }

  void ScriptedMotion::Update()
  {
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<ScriptedMotionPath>();

    for (Entity e : entities)
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(e);


      if (smp.totalTime <= 0.0f)
        smp.totalTime = 0.1f;

      if (smp.isDirty)
      {
        ComputeSplineCoefficients(smp);

        smp.isDirty = false;
        smp.velCurveIntegral = GetPiecewiseLinearIntegral(smp.velocityKeys, 1.0f);

        smp.keyPoints.clear();

        ComputeArcLengthsAdaptive(smp, 0.0f, 1.0f, 0.0f);
      }

      for (int i = 0; i < smp.controlPoints.size() - 1; i++)
        renderResc->DrawRay(glm::vec4(smp.controlPoints[i], 1.0f), glm::vec4(smp.controlPoints[i + 1], 1.0f));

      for (int i = 0; i < smp.keyPoints.size() - 1; i++)
        renderResc->DrawRay(glm::vec4(smp.keyPoints[i].pos, 1.0f), glm::vec4(smp.keyPoints[i + 1].pos, 1.0f));
    }

    entities = Engine::GetInstance()->GetEntities<ScriptedMotionPath, Transform>();
    for (Entity e : entities)
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(e);
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      smp.t += Engine::GetInstance()->dt * (1.0f / smp.totalTime);
      while (smp.t > 1.0f)
        smp.t -= 1.0f;


      smp.currentSpeed = GetCurrentSpeed(smp);

      float areaprog = GetPiecewiseLinearIntegral(smp.velocityKeys, smp.t) / smp.velCurveIntegral;

      float currentpos = GetUfromArcLength(smp, areaprog);

      while (currentpos > 1.0f)
        currentpos -= 1.0f;

      glm::vec3 currentPos = GetCurvePoint(smp, currentpos);
      glm::vec3 centerOfInterest = GetCurvePoint(smp, currentpos + 0.001f);



      //Orientation control (Forward Center of Interest)
      glm::vec3 w = centerOfInterest - currentPos;

      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

      glm::vec3 u = glm::cross(up, w);
      glm::vec3 v = glm::cross(w, u);

      w = glm::normalize(w);
      u = glm::normalize(u);
      v = glm::normalize(v);

      glm::mat4 translation = glm::mat4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(currentPos, 1.0f)
      );

      glm::mat4 rotation = glm::mat4(
        glm::vec4(u.x, u.y, u.z, 0.0f),
        glm::vec4(v.x, v.y, v.z, 0.0f),
        glm::vec4(w.x, w.y, w.z, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
      );

      transform.modelMatrix = translation * rotation;
      transform.matManuallyModified = true;
    }
  }



  void ScriptedMotion::PostUpdate()
  {

  }

  void ScriptedMotion::Exit()
  {

  }

  Eigen::MatrixXf ScriptedMotion::GetMatrixByNumCtrlPts(size_t numCtrlPts)
  {
    Eigen::MatrixXf matrix(numCtrlPts + 2, numCtrlPts + 2);

    //zero initialize
    matrix.setZero();

    for (int i = 0; i < numCtrlPts + 2; i++)
    {
      for (int j = 0; j < numCtrlPts; j++)
      {
        //matrix[i, j] = GetCubicSplineMatrixTerm(j, i);
        matrix(i, j) = GetCubicSplineMatrixTerm(j, i);
      }
    }

    matrix(2, numCtrlPts) = 1;
    matrix(2, numCtrlPts + 1) = 1;

    size_t k = numCtrlPts - 1;
    for (size_t i = 3; i < numCtrlPts + 2; i++)
    {
      size_t currentterm = k - (i - 3);
      matrix(i, numCtrlPts) = 0;
      matrix(i, numCtrlPts + 1) = static_cast<float>(6 * currentterm);
    }

    return matrix;
  }

  float ScriptedMotion::GetCubicSplineMatrixTerm(int t, int termnum)
  {
    if (termnum == 0)
      return 1.0f;

    if (termnum == 1)
      return static_cast<float>(t);

    if (termnum == 2)
      return static_cast<float>(t * t);

    if (termnum == 3)
      return static_cast<float>(t * t * t);


    int c = termnum - 3;
    int tMinusC = t - c;
    if (tMinusC < 0)
    {
      return 0.0f;
    }

    return static_cast<float>(tMinusC * tMinusC * tMinusC);
  }

  void ScriptedMotion::ComputeSplineCoefficients(ScriptedMotionPath& smp)
  {
    Eigen::MatrixXf mat = GetMatrixByNumCtrlPts(smp.controlPoints.size());

    Eigen::VectorXf vecX(smp.controlPoints.size() + 2);
    Eigen::VectorXf vecY(smp.controlPoints.size() + 2);
    Eigen::VectorXf vecZ(smp.controlPoints.size() + 2);

    vecX.setZero();
    vecY.setZero();
    vecZ.setZero();

    smp.vecX.setZero();
    smp.vecY.setZero();
    smp.vecZ.setZero();

    for (int i = 0; i < smp.controlPoints.size(); i++)
    {
      vecX[i] = smp.controlPoints[i].x;
      vecY[i] = smp.controlPoints[i].y;
      vecZ[i] = smp.controlPoints[i].z;
    }

    Eigen::ColPivHouseholderQR<Eigen::MatrixXf> system(mat.transpose());

    smp.vecX = system.solve(vecX);
    smp.vecY = system.solve(vecY);
    smp.vecZ = system.solve(vecZ);
  }

  void ScriptedMotion::ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance)
  {
    const float epsilon = 0.000001f;

    start = 0.0f;
    end = 1.0f;

    int i = 0;

    ScriptedMotionPath::KeyPoint startkp = { .pos = GetCurvePoint(smp, start), .arcLenght = 0.0f, .u = 0.0f };
    smp.keyPoints.push_back(startkp);

    struct segment
    {
      float start;
      float end;

      glm::vec3 sp;
      glm::vec3 ep;
    };

    std::stack<segment> segments;

    segments.push(segment{ .start = 0.0f, .end = 1.0f, .sp = GetCurvePoint(smp, 0.0f), .ep = GetCurvePoint(smp, 1.0f)});

    float al = 0.0f;

    while (!segments.empty())
    {
      //glm::vec3 startPoint = GetCurvePoint(smp, start);
      segment current = segments.top(); segments.pop();

      //glm::vec3 startPoint = GetCurvePoint(smp, current.start);
      //glm::vec3 endPoint = GetCurvePoint(smp, current.end);

      const glm::vec3& startPoint = current.sp;
      const glm::vec3& endPoint = current.ep;


      float middle = glm::mix(current.start, current.end, 0.5f);
      glm::vec3 middlePoint = GetCurvePoint(smp, middle);

      float startToEnd = glm::length(endPoint - startPoint);
      float startToMiddle = glm::length(middlePoint - startPoint);
      float throughMiddle = glm::length(middlePoint - startPoint) + glm::length(endPoint - middlePoint);

      if (glm::length(startToEnd - throughMiddle) > epsilon)
      {
        segments.push(segment{ .start = middle, .end = current.end, .sp = middlePoint, .ep = endPoint});
        segments.push(segment{ .start = current.start, .end = middle, .sp = startPoint, .ep = middlePoint});
      }
      else
      {
        al += glm::length(startToMiddle);
        ScriptedMotionPath::KeyPoint kp = { .pos = middlePoint, .arcLenght = al, .u = middle };
        smp.keyPoints.push_back(kp);
        al += glm::length(endPoint - middlePoint);
        kp = { .pos = endPoint, .arcLenght = al, .u = current.end };
        //smp.keyPoints.push_back(kp);
      }
    }


    float totalLength = smp.keyPoints.back().arcLenght;

    //float totalLength = smp.keyPoints.end()[0].arcLenght;

    auto normalizeLength = [totalLength](ScriptedMotionPath::KeyPoint& kp) {kp.arcLenght /= totalLength; };
    std::for_each(smp.keyPoints.begin(), smp.keyPoints.end(), normalizeLength);

    smp.keyPoints.push_back(ScriptedMotionPath::KeyPoint{ .pos = GetCurvePoint(smp, end), .arcLenght = 1.0f, .u = 1.0f });
    //if (glm::abs(startToEnd - throughMiddle) > epsilon)
    //{
    //  ComputeArcLengthsAdaptive(smp, start, middle, distance);
    //  ComputeArcLengthsAdaptive(smp, middle, end, distance + startToMiddle);
    //}
    //else
    //{
    //  //store length
    //  smp.arcLenTable.push_back(std::make_pair(middle, distance + startToMiddle));
    //  //smp.arcLenTable.push_back(std::make_pair(end, distance + startToEnd));
    //}
  }

  float ScriptedMotion::GetUfromArcLength(ScriptedMotionPath& smp, float arclength)
  {
    //edge cases
    if (smp.keyPoints.size() == 0)
      return 0;
    if (smp.keyPoints.size() == 1)
      return smp.keyPoints[0].u;

    //start at the middle, ready to jump halfway through each half
    size_t i = smp.keyPoints.size() / 2;
    size_t jumpsize = i / 2;

    //search until the return break
    while (true)
    {
      //if arclenth is between i and i+1, interpolate and return u
      if (arclength >= smp.keyPoints[i].arcLenght && arclength <= smp.keyPoints[i + 1].arcLenght)
      {
        float segmentlength = smp.keyPoints[i + 1].arcLenght - smp.keyPoints[i].arcLenght;
        float frac = (arclength - smp.keyPoints[i].arcLenght) / segmentlength;

        return glm::mix(smp.keyPoints[i].u, smp.keyPoints[i + 1].u, frac);
      }
      //if we should be to the right, jump half way through the remaining right side
      else if (arclength > smp.keyPoints[i + 1].arcLenght)
        i += jumpsize;
      //else we should be to the left, jump half way through the remaining left side
      else
        i -= jumpsize;

      //half the jump size for binary search, make sure we always jump at least 1 so no inf loop
      jumpsize /= 2;
      if (jumpsize == 0) jumpsize = 1;
    }
  }

  float ScriptedMotion::GetVelocityAtTime(ScriptedMotionPath& smp)
  {
    for (int i = 0; i < smp.velocityKeys.size() - 1; i++)
    {
      if (smp.t >= smp.velocityKeys[i].first && smp.t <= smp.velocityKeys[i + 1].first)
      {
        float timeDiff = smp.velocityKeys[i + 1].first - smp.velocityKeys[i].first;
        float frac = (smp.t - smp.velocityKeys[i].first) / timeDiff;
        //float velDiff = smp.velocityKeys[i + 1].second - smp.velocityKeys[i].second;
        

        return glm::mix(smp.velocityKeys[i].second, smp.velocityKeys[i + 1].second, frac);
        //return smp.velocityKeys[i].second + velDiff * frac;
      }
    }
    return 0.0f;
  }

  float ScriptedMotion::GetCurrentSpeed(ScriptedMotionPath& smp)
  {
    //linear search for the 2 velocity keys surrounding current time t, return the lerp between their velocities
    for (int i = 0; i < smp.velocityKeys.size() - 1; i++)
    {
      if (smp.t >= smp.velocityKeys[i].first && smp.t <= smp.velocityKeys[i + 1].first)
      {
        float timeDiff = smp.velocityKeys[i + 1].first - smp.velocityKeys[i].first;
        float frac = (smp.t - smp.velocityKeys[i].first) / timeDiff;
        return glm::mix(smp.velocityKeys[i].second, smp.velocityKeys[i + 1].second, frac);
      }
    }
    return 0.0f;
  }

  glm::vec3 ScriptedMotion::GetCurvePoint(ScriptedMotionPath& smp, float u)
  {
    Eigen::VectorXf& solvedX = smp.vecX;
    Eigen::VectorXf& solvedY = smp.vecY;
    Eigen::VectorXf& solvedZ = smp.vecZ;

    float current = u * (smp.controlPoints.size() - 1);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    x += solvedX[0];
    x += solvedX[1] * current;
    x += solvedX[2] * current * current;
    x += solvedX[3] * current * current * current;

    y += solvedY[0];
    y += solvedY[1] * current;
    y += solvedY[2] * current * current;
    y += solvedY[3] * current * current * current;

    z += solvedZ[0];
    z += solvedZ[1] * current;
    z += solvedZ[2] * current * current;
    z += solvedZ[3] * current * current * current;

    for (int j = 4; j < smp.controlPoints.size() + 2; j++)
    {
      float t = current - (float)(j - 3);
      if (t < 0)
      {
        continue;
      }
      x += solvedX[j] * t * t * t;
      y += solvedY[j] * t * t * t;
      z += solvedZ[j] * t * t * t;
    }
    return glm::vec3(x, y, z);
  }
}