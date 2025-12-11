#pragma once

#include "PCH/PCH.hpp"
#include "Engine/Engine.hpp"



  struct Spring
  {
    Entity attachment;
    float springCoeff;
    float dampingCoeff; 
    float restLength;
  };