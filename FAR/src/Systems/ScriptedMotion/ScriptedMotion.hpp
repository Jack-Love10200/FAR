///
/// @file   ScriptedMotion.cpp
/// @brief  System for calculating cubic spline paths and moving entities along them
/// @author Jack Love
/// @date   28.10.2025
///

#pragma once

#include "PCH/PCH.hpp"

#include "Resources/RenderResource.hpp"

#include "Systems/iSystem.hpp"

#include "Eigen/Dense"

#include "Components/ScriptedMotionPath.hpp"
#include <stb_image_resize2.h> 


namespace FAR
{
  class ScriptedMotion : public iSystem
  {
  public:
    virtual ~ScriptedMotion() = default;

    virtual void Init() override;
    virtual void PreUpdate() override;
    virtual void Update() override;
    virtual void PostUpdate() override;
    virtual void Exit() override;

  private:

    //get the cubic spline matrix for a given number of control points
    Eigen::MatrixXd GetMatrixByNumCtrlPts(size_t numCtrlPts);

    //get the term value for a given t and term number in the cubic spline matrix
    double GetCubicSplineMatrixTerm(int t, int termnum);

    //compute the spline coefficients for a given scripted motion path
    void ComputeSplineCoefficients(ScriptedMotionPath& smp);

    //compute the arc length table adaptively for a given scripted motion path
    void ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance);

    //get the U parameter corresponding to a given arc length proportion
    float GetUfromArcLength(ScriptedMotionPath& smp, float arclength);

    //get the speed at the current t value for a given scripted motion path
    float GetCurrentSpeed(ScriptedMotionPath& smp);

    //get a point on the cubic spline curve for a given scripted motion path and u (proportion 0-1) parameter
    glm::vec3 GetCurvePoint(ScriptedMotionPath& smp, float u);

    RenderResource* renderResc = nullptr;
  };
}