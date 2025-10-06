#pragma once

#include "PCH.hpp"
#include "Systems/iSystem.hpp"
#include "Engine/Engine.hpp"

#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Model.hpp"

namespace FAR
{
  class Sandbox : public iSystem
  {
  public:
    Sandbox() = default;
    ~Sandbox() override = default;
    void Init() override;
    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;
    void Exit() override;
  private:

    Entity cam;
    Entity jack;
    Entity adi;
    Entity okayu;
    Entity pika;
    Entity roman;
    Entity egyptian;
    Entity viking;
  };
}