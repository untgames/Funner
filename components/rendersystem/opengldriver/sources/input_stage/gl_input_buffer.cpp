#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

Buffer::Buffer(const ContextManager& context_manager, const BufferDesc& desc)
  : ContextObject(context_manager), buffer_desc(desc)
{
  if (desc.usage_mode >= UsageMode_Num)
    RaiseInvalidArgument("render::low_level::opengl::Buffer::Buffer(const ContextManager& context_manager, const BufferDesc& desc)", "desc.usage_mode", desc.usage_mode);
}

void Buffer::GetDesc(BufferDesc& desc)
{
  desc = buffer_desc;
}
