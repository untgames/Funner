#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

Buffer::Buffer(const ContextManager& context_manager, const BufferDesc& desc)
  : ContextObject(context_manager)
{
  try
  {
    SetDesc(desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::Buffer::Buffer(const ContextManager& context_manager, const BufferDesc& desc)");
  }
}

void Buffer::GetDesc(BufferDesc& desc)
{
  desc = buffer_desc;
}
  
void Buffer::SetDesc(const BufferDesc& desc)
{
  if (desc.usage_mode >= UsageMode_Num)
    RaiseInvalidArgument("render::low_level::opengl::Buffer::SetDesc (const BufferDesc& desc)", "desc.usage_mode", desc.usage_mode);      

  buffer_desc = desc;
}
