#pragma once

namespace FAR
{
  class iSystem
  {
  public:
    virtual ~iSystem() = default;

    virtual void Init() {};
    virtual void PreUpdate() {};
    virtual void Update() {};
    virtual void PostUpdate() {};
    virtual void Exit() {};
  };
}