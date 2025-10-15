#include "PCH/PCH.hpp"

#include "ScriptedMotion.hpp"

#include "Components/ScriptedMotionPath.hpp"

#include "Engine/Engine.hpp"

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

      for (int i = 0; i < smp.points.size() - 1; i++)
      {
        renderResc->DrawRay(glm::vec4(smp.points[i], 1.0f), glm::vec4(smp.points[i + 1], 1.0f));
      }
    }
  }

  void ScriptedMotion::PostUpdate()
  {

  }

  void ScriptedMotion::Exit()
  {

  }
}