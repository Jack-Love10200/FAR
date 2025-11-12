#pragma once

#include "PCH/PCH.hpp"

#include "Systems/iSystem.hpp"

#include "Components/Model.hpp"
#include "Components/IKPoser.hpp"
#include "Components/Transform.hpp"

namespace FAR
{
  class IK : public iSystem
  {
    public:
    IK() = default;
    ~IK() override = default;
    //engine steps
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;


  private:
    void ApplyNodeHeirarchy(std::vector<Model::Node>& nodes, int nodeIndex, const VQS& parentTransform);
    void SolveIK(Model& model, IKPoser& ikp, const glm::vec3& targetPos); 

  };
}