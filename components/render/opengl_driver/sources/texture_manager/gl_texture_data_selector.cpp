#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

TextureDataSelector::TextureDataSelector (const TextureDesc& in_desc, const TextureData* in_data)
  : format (in_desc.format)
  , data (in_data)
  , data_size (0)
  , index (0)
  , images_count (0)
  , offset (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureDataSelector::TextureDataSelector";    
  
  if (data && !data->data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data.data");

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

bool TextureDataSelector::GetLevelData (size_t width, size_t height, size_t depth, TextureLevelData& out)
{
  if (!data)
  {
    out.data = 0;
    out.size = 0;

    return false;    
  }

  size_t size = get_image_size (width, height, depth, format);  

  if (data->sizes && data->sizes [index] >= size)
    size = data->sizes [index];    

  data_size = size;  

  out.size = data_size;
  out.data = (char*)data->data + offset;

  return true;
}

void TextureDataSelector::Next ()
{
  offset += data_size;
  
  index++;

  if (index == images_count)
    data = 0;
}
