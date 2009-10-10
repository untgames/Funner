#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

TextureDataSelector::TextureDataSelector (const TextureDesc& in_desc, const TextureData* in_data)
  : images_count (0)
  , format (in_desc.format)
  , is_internal_format (in_desc.format > 0 && in_desc.format < PixelFormat_FirstInternal)
  , data (in_data)
  , index (0)
  , offset (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureDataSelector::TextureDataSelector";    
  
  if (data && is_internal_format && !data->sizes)
    throw xtl::format_operation_exception (METHOD_NAME, "For internal formats TextureData::sizes must be non-null pointer to array with mip-level sizes");      
    
  if (in_desc.generate_mips_enable)
  {
    switch (in_desc.dimension)
    {
      case TextureDimension_Cubemap:
        images_count = 6;
        break;
      default:
        images_count = 1;
        break;
    }
  }
  else
  {
    images_count = ~0u;
  }
}

const void* TextureDataSelector::GetData () const
{
  if (!data || !data->data)
    return 0;        

  return (char*)data->data + offset;
}

void TextureDataSelector::Next (size_t width, size_t height, size_t depth)
{
  if (!data)
    return;

  size_t size = is_internal_format ? 0u : get_image_size (width, height, depth, format);

  if (data->sizes && data->sizes [index] >= size)
    size = data->sizes [index];

  offset += size;

  index++;
  
  if (index == images_count)
    data = 0;
}
