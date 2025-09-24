
#include "PCH.hpp"

#include "RenderResource.hpp"

namespace FAR
{
  void RenderResource::DrawRay(glm::vec4 start, glm::vec4 end)
  {
    rays.push_back(start);
    rays.push_back(end);
  }
}