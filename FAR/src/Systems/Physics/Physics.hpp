#pragma once

#include "PCH/PCH.hpp"
#include "Systems/iSystem.hpp"

#include "Components/PointMass.hpp"
#include "Components/Transform.hpp"
#include "Components/Spring.hpp"

namespace FAR
{
  class Physics : public iSystem
  {
    public:
    Physics() = default;
    ~Physics() override = default;

    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;


    void EulerIntegrate(Transform& t, PointMass& pm);
    void CalculateSpringForces(Spring::SpringAttachment& spring, Transform& tA, PointMass& pmA, Transform& tB, PointMass& pmB);

  };
}