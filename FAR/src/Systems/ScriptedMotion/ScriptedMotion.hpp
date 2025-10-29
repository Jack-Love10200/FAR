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

	Eigen::MatrixXf GetMatrixByNumCtrlPts(size_t numCtrlPts);

	float GetCubicSplineMatrixTerm(int t, int termnum);

  void ComputeSplineCoefficients(ScriptedMotionPath& smp);

  void ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance);

  float GetUfromArcLength(ScriptedMotionPath& smp, float arclength);

  float GetVelocityAtTime(ScriptedMotionPath& smp);

  float GetCurrentSpeed(ScriptedMotionPath& smp);

  glm::vec3 GetCurvePoint(ScriptedMotionPath& smp, float u);

    RenderResource* renderResc = nullptr;
  };
}