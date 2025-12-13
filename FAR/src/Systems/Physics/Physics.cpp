#include "PCH/PCH.hpp"

#include "Physics.hpp"

#include "Resources/RenderResource.hpp"]

#include "imgui.h"

namespace FAR
{


  struct State
  {
    glm::vec3 position;  
    glm::vec3 velocity;  
  };

  // Represents the time-derivative of State.
  struct Derivative
  {
    glm::vec3 dPosition;  // derivative of position  = velocity
    glm::vec3 dVelocity;  // derivative of velocity = acceleration
  };

  glm::vec3 calculateSpringForce(const Spring::SpringAttachment& spring, const glm::vec3& tA, const glm::vec3& pmA, const glm::vec3& tB, const glm::vec3& pmB);
  glm::vec3 computeAcceleration(Entity e, std::map<Entity, State>& entityStates);

  std::vector<Entity> springEnts;

  //std::map<Entity, State> entityStates;

  // Compute derivatives for a given state.
  // 't' = current simulation time.
  // 'dt' = time step to project forward for evaluation.
  // 'state' = current or intermediate state.
  Derivative evaluate(Entity e, std::map<Entity, State>& entityStates)
  {
    Derivative out;
    out.dPosition = entityStates[e].velocity;
    out.dVelocity = computeAcceleration(e, entityStates);  // typically F/m
    return out;
  }

  Derivative evaluate(Entity e, std::map<Entity, State>& entityStates, float dt, std::map<Entity, Derivative> derivs)
  {
    State state;
    state.position = entityStates[e].position + derivs[e].dPosition * dt;
    state.velocity = entityStates[e].velocity + derivs[e].dVelocity * dt;

    Derivative out;
    out.dPosition = state.velocity;
    out.dVelocity = computeAcceleration(e, entityStates);
    return out;
  }


  //void computeAcceleration(std::map<Entity, State> entityStates)
  //{

  //}

  void AdvanceState(
    const std::map<Entity, State>& base,
    const std::map<Entity, Derivative>& k,
    float dt,
    std::map<Entity, State>& out)
  {
    for (auto& [e, s] : base)
    {
      out[e].position = s.position + k.at(e).dPosition * dt;
      out[e].velocity = s.velocity + k.at(e).dVelocity * dt;
    }
  }

  // RK4 integrator: integrates position and velocity over time step 'dt'.
  void integrate(float dt)
  {
    std::map<Entity, State> initialEntityStates;

    std::vector<Entity> ents = Engine::GetInstance()->GetEntities<Transform, PointMass>();

    for (Entity e : ents)
    {
      Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      initialEntityStates[e].position = t.position;
      initialEntityStates[e].velocity = pm.velocity;
    }

    std::map<Entity, Derivative> k1derivs;
    for (Entity e : ents)
    {
      k1derivs[e] = evaluate(e, initialEntityStates);
    }

    std::map<Entity, State> state;

    AdvanceState(initialEntityStates, k1derivs, dt * 0.5f, state);

    std::map<Entity, Derivative> k2derivs;
    for (Entity e : ents)
    {
      k2derivs[e] = evaluate(e, state, dt * 0.5f, k1derivs);
    }

    AdvanceState(initialEntityStates, k2derivs, dt * 0.5f, state);

    std::map<Entity, Derivative> k3derivs;
    for (Entity e : ents)
    {
      k3derivs[e] = evaluate(e, state, dt * 0.5f, k2derivs);
    }

    AdvanceState(initialEntityStates, k3derivs, dt, state);

    std::map<Entity, Derivative> k4derivs;
    for (Entity e : ents)
    {
      k4derivs[e] = evaluate(e, state, dt, k3derivs);
    }

    //Derivative k1 = evaluate(e, state, t);
    //Derivative k2 = evaluate(e, state, t, dt * 0.5f, k1derivs);
    //Derivative k3 = evaluate(e, state, t, dt * 0.5f, k2derivs);
    //Derivative k4 = evaluate(e, state, t, dt, k3derivs);

    for (Entity e : ents)
    {
      Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      State& state = initialEntityStates[e];
      Derivative k1 = k1derivs[e];
      Derivative k2 = k2derivs[e];
      Derivative k3 = k3derivs[e];
      Derivative k4 = k4derivs[e];

      glm::vec3 dxdt =
        (k1.dPosition +
          2.0f * k2.dPosition +
          2.0f * k3.dPosition +
          k4.dPosition) / 6.0f;

      glm::vec3 dvdt =
        (k1.dVelocity +
          2.0f * k2.dVelocity +
          2.0f * k3.dVelocity +
          k4.dVelocity) / 6.0f;

      state.position += dxdt * dt;
      state.velocity += dvdt * dt;

      t.position = state.position;
      pm.velocity = state.velocity;
    }


  }

  glm::vec3 CalculateSpringForces(Entity e, std::map<Entity, State>& entityStates)
  {


    glm::vec3 totalForce = glm::vec3(0.0f);

    if (Engine::GetInstance()->HasComponent<Spring>(e))
    {
      Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);

      for (Spring::SpringAttachment& springAttachment : spring.attachments)
      {
        totalForce += calculateSpringForce(springAttachment, entityStates[e].position, entityStates[e].velocity, 
          entityStates[springAttachment.attachedEntity].position, entityStates[springAttachment.attachedEntity].velocity);
      }
    }


    //search all other entities for springs connected to this one
    //std::vector<Entity> springEnts = Engine::GetInstance()->GetEntities<Transform, PointMass, Spring>();
    for (Entity otherE : springEnts)
    {
      if (otherE == e)
        continue;
      Spring& otherSpring = Engine::GetInstance()->GetComponent<Spring>(otherE);
      for (Spring::SpringAttachment& springAttachment : otherSpring.attachments)
      {
        if (springAttachment.attachedEntity == e)
        {
          totalForce -= calculateSpringForce(springAttachment, entityStates[otherE].position, entityStates[otherE].velocity,
            entityStates[springAttachment.attachedEntity].position, entityStates[springAttachment.attachedEntity].velocity); //negative because force is applied to other end
        }
      }
    }

    return totalForce;
  }

  glm::vec3 calculateSpringForce(const Spring::SpringAttachment& spring, const glm::vec3& tA, const glm::vec3& pmA, const glm::vec3& tB, const glm::vec3& pmB)
  {
    float currentLength = glm::length(tB - tA);
    float x = currentLength - spring.restLength;

    glm::vec3 direction = glm::normalize(tB - tA);

    glm::vec3 springForce = spring.springCoeff * x * direction;
    //Damping force
    glm::vec3 relativeVelocity = -(pmB - pmA);
    glm::vec3 dampingForce = -spring.dampingCoeff * relativeVelocity;

    glm::vec3 force = springForce + dampingForce;

    return force;
  }

  glm::vec3 computeAcceleration(Entity e, std::map<Entity, State>& entityStates)
  {

    if (Engine::GetInstance()->HasComponent<PointMass>(e))
    {
      PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);
      if (pm.isStatic)
        return glm::vec3(0.0f);
    }

    // Aggregate all forces acting on the object.
    glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

    // 1. Gravity (constant downward acceleration)
    const glm::vec3 gravity = glm::vec3(0.0f, -2.0f, 0.0f);
    force += Engine::GetInstance()->GetComponent<PointMass>(e).mass * gravity;

    // 2. Drag or linear damping
    //    Proportional to velocity, opposite direction.
    //const float dragCoefficient = 0.8f;
    //force += -dragCoefficient * state.velocity;

    // 3. External/engine-defined forces
    //    This could be from gameplay, input, explosions, wind fields, etc.
    force += CalculateSpringForces(e, entityStates);

    // 4. Springs (optional)
    //    Hooke’s law: F = -k * x - c * v
    //for (const Spring& s : state.attachedSprings)
    //{
    //  glm::vec3 displacement = state.position - s.anchor;
    //  glm::vec3 springForce = -s.k * displacement - s.damping * state.velocity;
    //  force += springForce;
    //}

    // 5. Mass must be non-zero
    return force / Engine::GetInstance()->GetComponent<PointMass>(e).mass;
  }

  void Physics::Init()
  {
    
  }

  void Physics::PreUpdate()
  {

  }

  bool showSprings = true;
  bool rk4Integrate = true;

  void Physics::Update()
  {
    ImGui::Begin("Physics Debug");

    ImGui::Checkbox("Show Springs", &showSprings);
    ImGui::Checkbox("Use RK4 Integration", &rk4Integrate);

    ImGui::End();

    std::vector<Entity> springEnts = Engine::GetInstance()->GetEntities<Transform, PointMass, Spring>();

    if (rk4Integrate)
    {
      float dt = Engine::GetInstance()->dt;
      springEnts = Engine::GetInstance()->GetEntities<Spring>();
      integrate(dt);
    }
    else //euler
    {
      //Calculate spring forces
      for (Entity e : springEnts)
      {
        Transform& tA = Engine::GetInstance()->GetComponent<Transform>(e);
        PointMass& pmA = Engine::GetInstance()->GetComponent<PointMass>(e);
        Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);


        for (Spring::SpringAttachment& springAttachment : spring.attachments)
        {
          Entity attachedEntity = springAttachment.attachedEntity;

          if (!Engine::GetInstance()->HasComponent<Transform>(attachedEntity) || !Engine::GetInstance()->HasComponent<PointMass>(attachedEntity))
            continue;

          Transform& tB = Engine::GetInstance()->GetComponent<Transform>(attachedEntity);
          PointMass& pmB = Engine::GetInstance()->GetComponent<PointMass>(attachedEntity);
          CalculateSpringForces(springAttachment, tA, pmA, tB, pmB);
        }
      }
      std::vector<Entity> pmEnts = Engine::GetInstance()->GetEntities<Transform, PointMass>();

      for (Entity e : pmEnts)
      {
        Transform& t = Engine::GetInstance()->GetComponent<Transform>(e);
        PointMass& pm = Engine::GetInstance()->GetComponent<PointMass>(e);

        if (!pm.isStatic)
          pm.totalForce += glm::vec3(0.0f, -2.0f * pm.mass, 0.0f); //gravity

        EulerIntegrate(t, pm);
      }

    }

    if (showSprings)
    {
      //draw lines for springs
      for (Entity e : springEnts)
      {
        Transform& tA = Engine::GetInstance()->GetComponent<Transform>(e);
        Spring& spring = Engine::GetInstance()->GetComponent<Spring>(e);
        for (Spring::SpringAttachment& springAttachment : spring.attachments)
        {
          Entity attachedEntity = springAttachment.attachedEntity;
          if (!Engine::GetInstance()->HasComponent<Transform>(attachedEntity))
            continue;
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

  void Physics::EulerIntegrate(Transform& t, PointMass& pm)
  {
    float dt = Engine::GetInstance()->dt;

    if (pm.isStatic)
    {
      // Static objects do not move
      pm.velocity = glm::vec3(0.0f);
      pm.acceleration = glm::vec3(0.0f);
      pm.totalForce = glm::vec3(0.0f);
      return;
    }

    // Compute acceleration from accumulated force
    pm.acceleration = pm.totalForce / pm.mass;

    // Integrate velocity
    pm.velocity += pm.acceleration * dt;

    // Integrate position
    t.position += pm.velocity * dt;

    // Clear forces for the next frame
    pm.totalForce = glm::vec3(0.0f);
  }

  void Physics::CalculateSpringForces(Spring::SpringAttachment& spring, Transform& tA, PointMass& pmA, Transform& tB, PointMass& pmB)
  {
    //F = -k * x
    float currentLength = glm::length(tB.position - tA.position);
    float x = currentLength - spring.restLength;

    glm::vec3 direction = glm::normalize(tB.position - tA.position);

    glm::vec3 springForce = spring.springCoeff * x * direction;
    //Damping force
    glm::vec3 relativeVelocity = -(pmB.velocity - pmA.velocity);
    glm::vec3 dampingForce = -spring.dampingCoeff * relativeVelocity;

    glm::vec3 totalForce = springForce + dampingForce;
    pmA.totalForce += totalForce;
    pmB.totalForce -= totalForce;

    //if (pmA.totalForce.x > 9999999.0f || pmA.totalForce.x < -9999999.0f)
    //{
    //  pmA.totalForce = glm::vec3(0.0f);
    //}

  }
}