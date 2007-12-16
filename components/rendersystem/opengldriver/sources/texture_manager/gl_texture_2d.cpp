#include <common/exception.h>
#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2D::Texture2D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_2D)
  {}

/*
   Работа с данными
*/

void Texture2D::SetData (size_t layer, size_t mip_level, size_t offset, size_t size, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture2D::SetData", "buffer");

  if (initialized)
  {
    if (size + offset > TexelSize () * desc.width * desc.height)
      RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "size + offset", 0, TexelSize () * desc.width * desc.height);
    if (offset % (TexelSize () * desc.width))
      RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "offset");
    if (size % (TexelSize () * desc.width))
      RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "size");

    glTexSubImage2D (GL_TEXTURE_1D, mip_level, 0, offset / (TexelSize () * desc.width), desc.width, size / (TexelSize () * desc.width), GLFormat (), GL_UNSIGNED_BYTE, buffer);
  }
  else if (size != TexelSize () * desc.width * desc.height)
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "size");
  else
  {
    glTexImage2D (GL_TEXTURE_2D, mip_level, GLInternalFormat (), desc.width, desc.height, 0, GLFormat (), GL_UNSIGNED_BYTE, buffer);
    initialized = true;
  }
}

void Texture2D::GetData (size_t layer, size_t mip_level, size_t offset, size_t size, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture2D::SetData", "buffer");
}
