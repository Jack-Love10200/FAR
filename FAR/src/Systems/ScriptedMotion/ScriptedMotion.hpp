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

	Eigen::MatrixXd GetMatrixByNumCtrlPts(int numCtrlPts);

	double GetCubicSplineMatrixTerm(int t, int termnum);

	void ComputePath(ScriptedMotionPath& smp, Eigen::MatrixXd mat);

  //area under the velocity curve from 0 to a
  float VelocityCurveArea(ScriptedMotionPath& smp, float a);

  //void ComputeArcLengths(ScriptedMotionPath& smp);
  void ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance);

  float GetUfromArcLength(ScriptedMotionPath& smp, float arclength);

  float GetVelocityAtTime(ScriptedMotionPath& smp);

  float GetCurrentSpeed(ScriptedMotionPath& smp);

  glm::vec3 GetCurvePoint(ScriptedMotionPath& smp, float u);

    RenderResource* renderResc = nullptr;
  };
}