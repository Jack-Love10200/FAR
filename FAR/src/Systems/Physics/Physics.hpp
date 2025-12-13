///
/// @file   Physics.hpp
/// @brief  Physics calculations for springs using euler or runge kutta integration methods
/// @author Jack Love
/// @date   12.12.2025
///

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

  private:

    struct State
    {
      glm::vec3 position;
      glm::vec3 velocity;
    };

    struct Derivative
    {
      glm::vec3 dPosition;
      glm::vec3 dVelocity;
    };

    /// @brief  Calculate the force from a given spring attachment between 2 entities A and B
    /// @param  spring - The spring attachment connecting A and B
    /// @param  tA     - A's Position
    /// @param  pmA    - A's Velocity
    /// @param  tB     - B's Position
    /// @param  pmB    - B's Velocity
    /// @retval        - The force delived by the spring attachment on A (negate to get B's)
    glm::vec3 CalculateSpringForce(const Spring::SpringAttachment& spring, const glm::vec3& tA, const glm::vec3& pmA, const glm::vec3& tB, const glm::vec3& pmB);
    
    //rk4

    /// @brief Runge Kutta 4th order position integration
    /// @param dt - Delta Time
    void RK4Integrate(float dt);

    /// @brief  Finds K1 for Runge Kutta 4th order
    /// @param  e            - The entity to find k1 for
    /// @param  entityStates - A snapshot of all of the current positions/velocities of all entities
    /// @retval              - K1, the derivitive of position and velocity
    Derivative RK4InitialEvaluate(Entity e, std::map<Entity, State>& entityStates);

    /// @brief  Finds K2-K4 for Runge Kutta 4th order
    /// @param  e            - The entitiy to find K for
    /// @param  entityStates - The current states of all relevant entities at the current point in the runge kutta process
    /// @param  dt           - Delta time
    /// @param  derivs       - The previous Runge Kutta derivatives
    /// @retval              - The new Runge Kutta derivative
    Derivative RK4PartialEvaluate(Entity e, std::map<Entity, State>& entityStates, float dt, const std::map<Entity, Derivative>& derivs);

    /// @brief Advances entities states for current part of the Runge Kutta process
    /// @param base - The initital snapshot state
    /// @param k    - The Runge Kutta derivates to update with
    /// @param dt   - Delta Time
    /// @param out  - Map to store updated states to
    void AdvanceState(const std::map<Entity, State>& initial, const std::map<Entity, Derivative>& k, float dt, std::map<Entity, State>& out);
    
    /// @brief  Compute the acceleration based on spring physics for a given entity in a given state
    /// @param  e            - The entity to compute accel for
    /// @param  entityStates - The states of all entities in the system during which to compute e's accel
    /// @retval              - e's acceleration given entityStates
    glm::vec3 ComputeAcceleration(Entity e, std::map<Entity, State>& entityStates);

    /// @brief  Compute the spring forces based on spring physics for a given entity in a given state using hooke's law
    /// @param  e            - The entity to compute accel for
    /// @param  entityStates - The states of all entities in the system during which to compute e's forces
    /// @retval              - e's total force given entityStates
    glm::vec3 CalculateSpringForces(Entity e, std::map<Entity, State>& entityStates);

    /// @brief Use euler integration to do spring physics
    /// @param dt - Delta Time
    void EulerIntegrate(float dt);

    bool showSprings = true;
    bool rk4Integrate = true;

    std::vector<Entity> springEnts;
  };
}