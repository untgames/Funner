#include "material_impl.h"

using namespace media::spine;

/*
   Constructor
*/

MaterialImpl::MaterialImpl (const char* in_name, const char* in_texture_path, media::spine::BlendMode in_blend_mode, TexcoordWrap in_texcoord_wrap_u, TexcoordWrap in_texcoord_wrap_v)
  : name (in_name)
  , texture_path (in_texture_path)
  , blend_mode (in_blend_mode)
  , texcoord_wrap_u (in_texcoord_wrap_u)
  , texcoord_wrap_v (in_texcoord_wrap_v)
  {}
