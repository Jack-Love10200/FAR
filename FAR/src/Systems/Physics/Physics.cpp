///
/// @file   Physics.cpp
/// @brief  Physics calculations for springs using euler or runge kutta integration methods
/// @author Jack Love
/// @date   12.12.2025
///



#include "PCH/PCH.hpp"

#include "Physics.hpp"
#include "Resources/RenderResource.hpp"
#include "imgui.h"

namespace FAR
{

  Physics::Derivative Physics::RK4InitialEvaluate(Entity e, std::map<Entity, State>& entityStates)
  {
    //dx = v and dv = a
    Derivative out;
    out.dPosition = entityStates[e].velocity;
    out.dVelocity = ComputeAcceleration(e, entityStates);
    return out;
  }

  Physics::Derivative Physics::RK4PartialEvaluate(Entity e, std::map<Entity, State>& entityStates, float dt, const std::map<Entity, Derivative>& derivs)
  {
    //dx = v and dv = a
    Derivative out;
    out.dPosition = entityStates[e].velocity + derivs.at(e).dVelocity * dt;
    out.dVelocity = ComputeAcceleration(e, entityStates);
    return out;
  }

  void Physics::AdvanceState(const std::map<Entity, State>& initial, const std::map<Entity, Derivative>& k, float dt, std::map<Entity, State>& out)
  {
    for (auto& [e, s] : initial)
    {
      out[e].position = s.position + k.at(e).dPosition * dt;
      out[e].velocity = s.velocity + k.at(e).dVelocity * dt;
    }
  }

  void Physics::RK4Integrate(float dt)
  {
    std::map<Entity, State> initialEntityStates;
    std::map<Entity, State> state;

    std::map<Entity, Derivative> k1derivs;
    std::map<Entity, Derivative> k2derivs;
    std::map<Entity, Derivative> k3derivs;
    std::map<Entity, Derivative> k4derivs;

    std::vector<Entity> ents = Engine::GetInstance()->GetEntities<Transform, PointMass>();

    //snapshot initial state
    for (Entity e : ents)
    {
      Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      initialEntityStates[e].position = t.position;
      initialEntityStates[e].velocity = pm.velocity;
    }

    //calculate k1-k4 runge kutta derivatives
    for (Entity e : ents)
      k1derivs[e] = RK4InitialEvaluate(e, initialEntityStates);

    AdvanceState(initialEntityStates, k1derivs, dt * 0.5f, state);
    for (Entity e : ents)
      k2derivs[e] = RK4PartialEvaluate(e, state, dt * 0.5f, k1derivs);

    AdvanceState(initialEntityStates, k2derivs, dt * 0.5f, state);
    for (Entity e : ents)
      k3derivs[e] = RK4PartialEvaluate(e, state, dt * 0.5f, k2derivs);

    AdvanceState(initialEntityStates, k3derivs, dt, state);
    for (Entity e : ents)
      k4derivs[e] = RK4PartialEvaluate(e, state, dt, k3derivs);


    for (Entity e : ents)
    {
      Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      State& state = initialEntityStates[e];
      Derivative k1 = k1derivs[e];
      Derivative k2 = k2derivs[e];
      Derivative k3 = k3derivs[e];
      Derivative k4 = k4derivs[e];

      //runge kutta weighted average
      glm::vec3 dxdt = (k1.dPosition + 2.0f * k2.dPosition + 2.0f * k3.dPosition + k4.dPosition) / 6.0f;
      glm::vec3 dvdt = (k1.dVelocity + 2.0f * k2.dVelocity + 2.0f * k3.dVelocity + k4.dVelocity) / 6.0f;

      //apply changes to actual component storage
      state.position += dxdt * dt;
      state.velocity += dvdt * dt;
      t.position = state.position;
      pm.velocity = state.velocity;
    }
  }

  glm::vec3 Physics::CalculateSpringForce(const Spring::SpringAttachment& spring, const glm::vec3& tA, const glm::vec3& pmA, const glm::vec3& tB, const glm::vec3& pmB)
  {
    //F = -k * x
    float currentLength = glm::length(tB - tA);
    float x = currentLength - spring.restLength;

    //spring force
    glm::vec3 direction = glm::normalize(tB - tA);
    glm::vec3 springForce = spring.springCoeff * x * direction;

    //Damping force
    glm::vec3 relativeVelocity = -(pmB - pmA);
    glm::vec3 dampingForce = -spring.dampingCoeff * relativeVelocity;

    glm::vec3 force = springForce + dampingForce;
    return force;
  }

  glm::vec3 Physics::CalculateSpringForces(Entity e, std::map<Entity, State>& entityStates)
  {
    glm::vec3 totalForce = glm::vec3(0.0f);

    //all forces from own attachments
    if (Engine::GetInstance()->HasComponent<Spring>(e))
    {
      Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);

      for (const Spring::SpringAttachment& springAttachment : spring.attachments)
      {
        totalForce += CalculateSpringForce(springAttachment, entityStates.at(e).position, entityStates.at(e).velocity,
          entityStates.at(springAttachment.attachedEntity).position, entityStates.at(springAttachment.attachedEntity).velocity);
      }
    }

    //all forces from any other entitie's spring attachments that go to this entity
    for (Entity otherE : springEnts)
    {
      if (otherE == e)
        continue;
      Spring& otherSpring = Engine::GetInstance()->GetComponent<Spring>(otherE);
      for (const Spring::SpringAttachment& springAttachment : otherSpring.attachments)
      {
        if (springAttachment.attachedEntity == e)
        {
          totalForce -= CalculateSpringForce(springAttachment, entityStates.at(otherE).position, entityStates.at(otherE).velocity,
            entityStates.at(springAttachment.attachedEntity).position, entityStates.at(springAttachment.attachedEntity).velocity); //negative because force is applied to other end
        }
      }
    }

    return totalForce;
  }


  glm::vec3 Physics::ComputeAcceleration(Entity e, std::map<Entity, State>& entityStates)
  {
    //if static, no movement, return zero
    if (Engine::GetInstance()->HasComponent<PointMass>(e))
    {
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      if (pm.isStatic)
        return glm::vec3(0.0f);
    }

    glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

    //gravity
    const glm::vec3 gravity = glm::vec3(0.0f, -2.0f, 0.0f);
    force += Engine::GetInstance()->GetComponent<PointMass>(e).mass * gravity;

    //spring forces
    force += CalculateSpringForces(e, entityStates);

    //f = ma -> a = f/m
    return force / Engine::GetInstance()->GetComponent<PointMass>(e).mass;
  }

  void Physics::Init()
  {
    
  }

  void Physics::PreUpdate()
  {

  }

  void Physics::Update()
  {
    //debug ui
    ImGui::Begin("Physics Debug");
    ImGui::Checkbox("Show Springs", &showSprings);
    ImGui::Checkbox("Use RK4 Integration", &rk4Integrate);
    ImGui::End();

    float dt = Engine::GetInstance()->dt;

    if (rk4Integrate)
    {
      springEnts = Engine::GetInstance()->GetEntities<Spring>();
      RK4Integrate(dt);
    }
    else //euler
    {
      EulerIntegrate(dt);
    }

    if (showSprings)
    {
      //draw lines for spring
      //for object with a spring and position
      for (Entity e : Engine::GetInstance()->GetEntities<Spring, Transform>())
      {
        Transform& tA = Engine::GetInstance()->GetComponent<Transform>(e);
        Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);

        //for each spring attachment the component has
        for (Spring::SpringAttachment& springAttachment : spring.attachments)
        {
          Entity attachedEntity = springAttachment.attachedEntity;
          if (!Engine::GetInstance()->HasComponent<Transform>(attachedEntity))
            continue;

          //render the spring attachment between the 2 entities
          Transform& tB = Engine::GetInstance()->GetComponent<Transform>(attachedEntity);
          RenderResource* renderResc = Engine::GetInstance()->GetResource<RenderResource>();
          renderResc->DrawRay({ .position = glm::vec4(tA.position, 1.0f), .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
            { .position = glm::vec4(tB.position, 1.0f), .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) });
        }
      }
    }
  }


  void Physics::PostUpdate()
  {

  }

  void Physics::Exit()
  {

  }

  void Physics::EulerIntegrate(float dt)
  {
    //for each spring object
    for (Entity e : Engine::GetInstance()->GetEntities<Transform, PointMass, Spring>())
    {
      Transform& tA = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pmA = Engine::GetInstance()->GetComponent<PointMass>(e);
      Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);

      //for each spring attachment the object has
      for (Spring::SpringAttachment& springAttachment : spring.attachments)
      {
        Entity attachedEntity = springAttachment.attachedEntity;

        if (!Engine::GetInstance()->HasComponent<Transform>(attachedEntity) || !Engine::GetInstance()->HasComponent<PointMass>(attachedEntity))
          continue;

        //Calculate the force from the spring attachment and accumulate to both objects total forces
        Transform& tB = Engine::GetInstance()->GetComponent<Transform>(attachedEntity);
        PointMass& pmB = Engine::GetInstance()->GetComponent<PointMass>(attachedEntity);
        glm::vec3 force = CalculateSpringForce(springAttachment, tA.position, pmA.velocity, tB.position, pmB.velocity);
        pmA.totalForce += force;
        pmB.totalForce -= force; //equal and opposite
      }
    }
    std::vector<Entity> pmEnts = Engine::GetInstance()->GetEntities<Transform, PointMass>();

    for (Entity e : pmEnts)
    {
      Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);

      if (pm.isStatic)
      {
        // Static objects do not move
        pm.velocity = glm::vec3(0.0f);
        pm.acceleration = glm::vec3(0.0f);
        pm.totalForce = glm::vec3(0.0f);
        continue;
      }
      else
      {
        pm.totalForce += glm::vec3(0.0f, -2.0f * pm.mass, 0.0f); //gravity

        //standard euler accumulation
        pm.acceleration = pm.totalForce / pm.mass;
        pm.velocity += pm.acceleration * dt;
        t.position += pm.velocity * dt;

        //clear forces for the next frame
        pm.totalForce = glm::vec3(0.0f);
      }

    }
  }
}