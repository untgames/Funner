#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

TextureDataSelector::TextureDataSelector (const TextureData* in_data)
  : data (in_data)
  , index (0)
  , offset (0)
{
}

const void* TextureDataSelector::GetData () const
{
  if (!data || !data->data)
    return 0;        

  return (char*)data->data + offset;
}

void TextureDataSelector::Next (size_t size)
{
  if (data->sizes [index] >= size)
    size = data->sizes [index];

  offset += size;

  index++;
}

