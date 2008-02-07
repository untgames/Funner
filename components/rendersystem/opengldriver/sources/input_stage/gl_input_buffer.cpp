#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

Buffer::Buffer(const ContextManager& context_manager, const BufferDesc& desc)
  : ContextObject(context_manager), buffer_desc(desc)
{
  switch (desc.usage_mode)
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream: break;
    default: RaiseInvalidArgument("render::low_level::opengl::Buffer::Buffer", "desc.usage_mode", desc.usage_mode);
  }
}

void Buffer::GetDesc(BufferDesc& desc)
{
  desc = buffer_desc;
}
