#include "PCH/PCH.hpp"

#include "ScriptedMotion.hpp"

#include "Components/ScriptedMotionPath.hpp"

#include "Engine/Engine.hpp"

#include "Eigen/Dense"

namespace FAR
{
  void ScriptedMotion::Init()
  {
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();

    Eigen::MatrixXd m;

    m.setRandom(10, 10);

    Eigen::Matrix<double, 10, 1> v;

    v.setRandom();

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> system(m);

    Eigen::Matrix<double, 10, 1> x = system.solve(v);

    std::cout << "The solution is:\n" << x << std::endl;

    x.array().operator[](0) = 3;

    std::cout << "it is now:\n" << x << std::endl;
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