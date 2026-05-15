///
/// @file   ScriptedMotion.cpp
/// @brief  System for calculating cubic spline paths and moving entities along them
/// @author Jack Love
/// @date   28.10.2025
///

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

    //point size for debug point rendering
    glPointSize(10.0f);
    glLineWidth(20.0f);
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

      //prevent divide by zero
      if (smp.totalTime <= 0.0f)
        smp.totalTime = 0.1f;

      //only recompute splines that have been changed
      if (smp.isDirty)
      {
        ComputeSplineCoefficients(smp);
        ComputeArcLengthsAdaptive(smp, 0.0f, 1.0f, 0.0f); 

        smp.isDirty = false;
        smp.velCurveIntegral = GetPiecewiseLinearIntegral(smp.velocityKeys, 1.0f);
      }

      //render the control points
      for (int i = 0; i < smp.controlPoints.size(); i++)
        renderResc->DrawPoint({ .position = glm::vec4(smp.controlPoints[i], 1.0f), .color = {1.0f, 0.0f, 0.0f, 1.0f} });

      //render the key points as an approximation of the curve
      for (int i = 0; i < smp.keyPoints.size() - 1; i++)
        renderResc->DrawRay({ .position = glm::vec4(smp.keyPoints[i].pos, 1.0f), .color = {1.0f, 1.0f, 1.0f, 1.0f} }, { .position = glm::vec4(smp.keyPoints[i + 1].pos, 1.0f), .color = {0.0f, 0.0f, 0.0f, 1.0f} });
    }

    entities = Engine::GetInstance()->GetEntities<ScriptedMotionPath, Transform>();
    for (Entity e : entities)
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(e);
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      //advance time, looping
      smp.t += Engine::GetInstance()->dt * (1.0f / smp.totalTime);
      while (smp.t > 1.0f)
        smp.t -= 1.0f;

      //make sure the current speed is up to date for other systems to use
      smp.currentSpeed = GetCurrentSpeed(smp);

      //get the current U based on the velocity curve and current t-value
      float areaprog = GetPiecewiseLinearIntegral(smp.velocityKeys, smp.t) / smp.velCurveIntegral;
      float currentpos = GetUfromArcLength(smp, areaprog);
      while (currentpos > 1.0f)
        currentpos -= 1.0f;

      //TODO: There are float imprecision issues here that cause jitter towards the ends of paths, especially long ones
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

      transform.modelMatrix = glm::mat4(
        glm::vec4(u.x, u.y, u.z, 0.0f),
        glm::vec4(v.x, v.y, v.z, 0.0f),
        glm::vec4(w.x, w.y, w.z, 0.0f),
        glm::vec4(currentPos, 1.0f)
      );

      transform.modelMatrix = transform.modelMatrix * glm::scale(glm::mat4(1.0f), transform.scale);

      transform.matManuallyModified = true;
    }
  }

  Eigen::MatrixXd ScriptedMotion::GetMatrixByNumCtrlPts(size_t numCtrlPts)
  {
    Eigen::MatrixXd matrix(numCtrlPts + 2, numCtrlPts + 2);

    //zero initializ
    matrix.setZero();

    //fill in terms
    for (int i = 0; i < numCtrlPts + 2; i++)
      for (int j = 0; j < numCtrlPts; j++)
        matrix(i, j) = GetCubicSplineMatrixTerm(j, i);

    //natural spline boundary conditions, second derivative = 0 at endpoints
    matrix(2, numCtrlPts) = 1;
    matrix(2, numCtrlPts + 1) = 1;

    size_t k = numCtrlPts - 1;
    for (size_t i = 3; i < numCtrlPts + 2; i++)
    {
      size_t currentterm = k - (i - 3);
      matrix(i, numCtrlPts) = 0;
      matrix(i, numCtrlPts + 1) = static_cast<double>(6 * currentterm);
    }

    return matrix;
  }

  double ScriptedMotion::GetCubicSplineMatrixTerm(int t, int termnum)
  {
    if (termnum == 0)
      return 1.0;

    if (termnum == 1)
      return static_cast<double>(t);

    if (termnum == 2)
      return static_cast<double>(t * t);

    if (termnum == 3)
      return static_cast<double>(t * t * t);


    int c = termnum - 3;
    int tMinusC = t - c;
    if (tMinusC < 0)
    {
      return 0.0;
    }

    return static_cast<double>(tMinusC * tMinusC * tMinusC);
  }

  void ScriptedMotion::ComputeSplineCoefficients(ScriptedMotionPath& smp)
  {
    Eigen::MatrixXd mat = GetMatrixByNumCtrlPts(smp.controlPoints.size());

    //augment our controlpoints with the cubic spline matrix for this num of control points
    //rref it and solve

    Eigen::VectorXd vecX(smp.controlPoints.size() + 2);
    Eigen::VectorXd vecY(smp.controlPoints.size() + 2);
    Eigen::VectorXd vecZ(smp.controlPoints.size() + 2);

    vecX.setZero();
    vecY.setZero();
    vecZ.setZero();

    for (int i = 0; i < smp.controlPoints.size(); i++)
    {
      vecX[i] = smp.controlPoints[i].x;
      vecY[i] = smp.controlPoints[i].y;
      vecZ[i] = smp.controlPoints[i].z;
    }

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> system(mat.transpose());
    smp.vecX = system.solve(vecX);
    smp.vecY = system.solve(vecY);
    smp.vecZ = system.solve(vecZ);
  }

  void ScriptedMotion::ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance)
  {
    const float epsilon = 0.0001f;

    //clear existing key points
    smp.keyPoints.clear();

    //always start with at the beginning of the curve
    ScriptedMotionPath::KeyPoint startkp = { .pos = GetCurvePoint(smp, 0.0f), .arcLenght = 0.0f, .u = 0.0f };

    smp.keyPoints.push_back(startkp);

    struct segment
    {
      float start;
      float end;

      glm::vec3 startPoint;
      glm::vec3 endPoint;
    };

    std::stack<segment> segments;

    //accumulates linear distances of accepted segments for arc lengths
    float accumulatedLength = 0.0f;

    for (int i = smp.controlPoints.size() - 2; i >= 0; i--)
    {
      float startU = (float)i / (smp.controlPoints.size() - 1); 
      float endU = (float)(i + 1) / (smp.controlPoints.size() - 1);

      segments.push(segment{ .start = startU, .end = endU, 
        .startPoint = GetCurvePoint(smp, startU), .endPoint = GetCurvePoint(smp, endU) });
    }

    //start with a segment of the whole curve and go until all segments that we end up subdividing are processed
    //segments.push(segment{ .start = 0.0f, .end = 1.0f, .startPoint = GetCurvePoint(smp, 0.0f), .endPoint = GetCurvePoint(smp, 1.0f)});
    while (!segments.empty())
    {
      //get the next segment
      segment current = segments.top(); segments.pop();

      //calculate the middle point of the segment
      float middle = glm::mix(current.start, current.end, 0.5f);
      glm::vec3 middlePoint = GetCurvePoint(smp, middle);

      //calculate distances
      float startToEnd = glm::length(current.endPoint - current.startPoint);
      float startToMiddle = glm::length(middlePoint - current.startPoint);
      float throughMiddle = glm::length(middlePoint - current.startPoint) + glm::length(current.endPoint - middlePoint);

      //if the distance going through middle is different from straight line (i.e. dont have enough resolution on the curve yet)
      if (glm::length(startToEnd - throughMiddle) > epsilon)
      {
        //subdivide, making sure to push the latter segment first so that the stack processes the earlier segment first
        segments.push(segment{ .start = middle, .end = current.end, 
          .startPoint = middlePoint, .endPoint = current.endPoint});

        segments.push(segment{ .start = current.start, .end = middle, 
          .startPoint = current.startPoint, .endPoint = middlePoint});
      }
      //else we have enough resolution
      else
      {
        //add the end point of this segment as a key point
        accumulatedLength += glm::length(current.endPoint - current.startPoint);
        ScriptedMotionPath::KeyPoint newkp = { .pos = current.endPoint, .arcLenght = accumulatedLength, .u = current.end };
        smp.keyPoints.push_back(newkp);
      }
    }

    //normalize arc lengths to [0, 1]
    double totalLength = smp.keyPoints.back().arcLenght;
    auto normalizeLength = [totalLength](ScriptedMotionPath::KeyPoint& kp) {kp.arcLenght /= totalLength; };
    std::for_each(smp.keyPoints.begin(), smp.keyPoints.end(), normalizeLength);
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
    //un-normalized position
    double current = u * (double)(smp.controlPoints.size() - 1);

    //regular polynomial part
    glm::vec<3, double> point = {0.0, 0.0, 0.0};
    point.x += smp.vecX[0];
    point.x += smp.vecX[1] * current;
    point.x += smp.vecX[2] * current * current;
    point.x += smp.vecX[3] * current * current * current;

    point.y += smp.vecY[0];
    point.y += smp.vecY[1] * current;
    point.y += smp.vecY[2] * current * current;
    point.y += smp.vecY[3] * current * current * current;

    point.z += smp.vecZ[0];
    point.z += smp.vecZ[1] * current;
    point.z += smp.vecZ[2] * current * current;
    point.z += smp.vecZ[3] * current * current * current;

    //truncated power function part
    for (int j = 4; j < smp.controlPoints.size() + 2; j++)
    {
      double t = current - (double)(j - 3);
      if (t < 0)
      {
        continue;
      }
      point.x += smp.vecX[j] * t * t * t;
      point.y += smp.vecY[j] * t * t * t;
      point.z += smp.vecZ[j] * t * t * t;
    }

    return glm::vec3(point);
  }

  void ScriptedMotion::PostUpdate()
  {

  }

  void ScriptedMotion::Exit()
  {

  }
}