#include "shared.h"

using namespace common;

/*
   Работа с DXT форматом при остутствии аппаратной поддержки
*/

void render::low_level::opengl::unpack_dxt (PixelFormat format, size_t width, size_t height, const void* dxt_data, void* unpacked_data)
{
  RaiseNotImplemented ("render::low_level::opengl::unpack_dxt");
}

void render::low_level::opengl::pack_dxt (PixelFormat format, size_t width, size_t height, const void* unpacked_data, void* dxt_data)
{
  RaiseNotImplemented ("render::low_level::opengl::pack_dxt");
}

GLint render::low_level::opengl::unpack_internal_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return GL_RGB8;
    case PixelFormat_DXT3: 
    case PixelFormat_DXT5: return GL_RGBA8;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_internal_format", "format"); return 1;
  }
}

GLenum render::low_level::opengl::unpack_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return GL_RGB;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return GL_RGBA;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_format", "format"); return 1;
  }
}

render::low_level::PixelFormat render::low_level::opengl::unpack_pf (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return PixelFormat_RGB8;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return PixelFormat_RGBA8;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_format", "format"); return PixelFormat_RGB8;
  }
}

GLenum render::low_level::opengl::unpack_type (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return GL_UNSIGNED_BYTE;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_type", "format"); return 1;
  }
}

size_t render::low_level::opengl::unpack_texel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return 3;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return 4;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_texel_size", "format"); return 1;
  }
}
