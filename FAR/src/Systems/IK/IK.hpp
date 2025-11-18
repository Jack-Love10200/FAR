///
/// @file   IK.hpp
/// @brief  Cyclic Coordinate Descent (CCD) Inverse Kinematics System
/// @author Jack Love
/// @date   18.11.2025
///
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
    // Take all of the nodes in list from their local spaces to world space based on their heirarchy
    void ApplyNodeHeirarchy(std::vector<Model::Node>& nodes, int nodeIndex, const VQS& parentTransform);

    // Solve IK for all manipulators in an IKPoser
    void SolveIK(Model& model, IKPoser& ikp, VQS pos); 

    // Solve IK via CCD for a single manipulator
    void SolveManipulator(Model& model, IKPoser::Manipulator& manipulator, VQS pos);
  };
}