#pragma once

namespace FAR
{
  class iResource
  {
  public:
    iResource() = default;
    virtual ~iResource() = default;

    //non-copyable, reference ONLY
    iResource(const iResource&) = delete;
    iResource& operator=(const iResource&) = delete;
  };
}