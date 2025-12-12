#pragma once

#include "PCH/PCH.hpp"
#include "Engine/Engine.hpp"



  struct Spring
  {
    struct SpringAttachment
    {
      Entity attachedEntity;
      float springCoeff = 5.0f;
      float dampingCoeff = 0.5f;
      float restLength = 1.5f;
    };
    std::vector<SpringAttachment> attachments;
  };