#include "shared.h"
#include <common/exception.h>

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture1D::Texture1D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_1D)
  {}

/*
   Работа с данными
*/

void Texture1D::SetData (size_t layer, size_t mip_level, size_t offset, size_t size, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");

  if (initialized)
  {
    if (size + offset > TexelSize () * desc.width)
      RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "size + offset", 0, TexelSize () * desc.width);
    if (offset%TexelSize ())
      RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "offset");
    if (size % TexelSize ())
      RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "size");

    glTexSubImage1D (GL_TEXTURE_1D, mip_level, offset / TexelSize (), size / TexelSize (), GLFormat (), GL_UNSIGNED_BYTE, buffer);
  }
  else if (size != TexelSize () * desc.width)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "size");
  else
  {
    glTexImage1D (GL_TEXTURE_1D, mip_level, GLInternalFormat (), desc.width, 0, GLFormat (), GL_UNSIGNED_BYTE, buffer);
    initialized = true;
  }
}

void Texture1D::GetData (size_t layer, size_t mip_level, size_t offset, size_t size, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");
}
