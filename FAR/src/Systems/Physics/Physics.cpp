#include "PCH/PCH.hpp"

#include "Physics.hpp"

#include "Resources/RenderResource.hpp"

namespace FAR
{
  void Physics::Init()
  {
    
  }

  void Physics::PreUpdate()
  {

  }

  void Physics::Update()
  {
    std::vector<Entity> springEnts = Engine::GetInstance()->GetEntities<Transform, PointMass, Spring>();
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

        RenderResource* renderResc = Engine::GetInstance()->GetResource<RenderResource>();
        renderResc->DrawRay({ .position = glm::vec4(tA.position, 1.0f), .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
          { .position = glm::vec4(Engine::GetInstance()->GetComponent<Transform>(attachedEntity).position, 1.0f), .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) });

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