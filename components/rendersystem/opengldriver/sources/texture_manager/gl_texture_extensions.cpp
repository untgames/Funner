#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

TextureExtensions::TextureExtensions (const ContextManager& manager)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureExtensions::TextureExtensions";

    //установка текущего контекста
    
  manager.MakeContextCurrent ();
  
    //получение флагов поддержки расширений

  static Extension ARB_texture_compression        = "GL_ARB_texture_compression",
                   EXT_texture_compression_s3tc   = "GL_EXT_texture_compression_s3tc",
                   EXT_texture_rectangle          = "GL_EXT_texture_rectangle",
                   NV_texture_rectangle           = "GL_NV_texture_rectangle",
                   EXT_texture3D                  = "GL_EXT_texture3D",
                   EXT_texture_cube_map           = "GL_EXT_texture_cube_map",
                   ARB_texture_cube_map           = "GL_ARB_texture_cube_map",
                   ARB_texture_non_power_of_two   = "GL_ARB_texture_non_power_of_two",
                   ARB_depth_texture              = "GL_ARB_depth_texture",
                   ARB_multitexture               = "GL_ARB_multitexture",
                   EXT_packed_depth_stencil       = "GL_EXT_packed_depth_stencil",
                   SGIS_generate_mipmap           = "GL_SGIS_generate_mipmap",
                   SGIS_texture_lod               = "GL_SGIS_texture_lod",
                   EXT_texture_lod_bias           = "GL_EXT_texture_lod_bias",
                   EXT_texture_filter_anisotropic = "GL_EXT_texture_filter_anisotropic",
                   EXT_shadow_funcs               = "GL_EXT_shadow_funcs",
                   ARB_texture_border_clamp       = "GL_ARB_texture_border_clamp",
                   ARB_texture_mirrored_repeat    = "GL_ARB_texture_mirrored_repeat",
                   Version_1_2                    = "GL_VERSION_1_2",
                   Version_1_3                    = "GL_VERSION_1_3",
                   Version_1_4                    = "GL_VERSION_1_4",
                   Version_1_5                    = "GL_VERSION_1_5",
                   Version_2_0                    = "GL_VERSION_2_0";

  bool has_arb_texture_compression   = manager.IsSupported (ARB_texture_compression) || manager.IsSupported (Version_1_3);  

  has_ext_texture_compression_s3tc   = has_arb_texture_compression && manager.IsSupported (EXT_texture_compression_s3tc);
  has_ext_texture_rectangle          = manager.IsSupported (EXT_texture_rectangle) || manager.IsSupported (NV_texture_rectangle);
  has_ext_texture3d                  = manager.IsSupported (EXT_texture3D) || manager.IsSupported (Version_1_2);
  has_arb_texture_cube_map           = manager.IsSupported (ARB_texture_cube_map) || manager.IsSupported (EXT_texture_cube_map) ||
                                       manager.IsSupported (Version_1_3);
  has_arb_texture_non_power_of_two   = manager.IsSupported (ARB_texture_non_power_of_two) || manager.IsSupported (Version_2_0);                                      
  has_arb_depth_texture              = manager.IsSupported (ARB_depth_texture) || manager.IsSupported (Version_1_4);  
  has_arb_multitexture               = manager.IsSupported (ARB_multitexture) || manager.IsSupported (Version_1_3);
  has_ext_packed_depth_stencil       = manager.IsSupported (EXT_packed_depth_stencil);
  has_sgis_generate_mipmap           = manager.IsSupported (SGIS_generate_mipmap) || manager.IsSupported (Version_1_4);  

  has_sgis_texture_lod               = manager.IsSupported (SGIS_texture_lod) || manager.IsSupported (Version_1_2);
  has_ext_texture_lod_bias           = manager.IsSupported (EXT_texture_lod_bias) || manager.IsSupported (Version_1_4);
  has_ext_texture_filter_anisotropic = manager.IsSupported (EXT_texture_filter_anisotropic);
  has_ext_shadow_funcs               = manager.IsSupported (EXT_shadow_funcs) || manager.IsSupported (Version_1_5);
  has_arb_texture_border_clamp       = manager.IsSupported (ARB_texture_border_clamp) || manager.IsSupported (Version_1_3);
  has_arb_texture_mirrored_repeat    = manager.IsSupported (ARB_texture_mirrored_repeat) || manager.IsSupported (Version_1_4);
  
    //определение численных параметров  
  
  if (has_ext_texture_filter_anisotropic)
    glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&max_anisotropy);
  else
    max_anisotropy = 0;
    
    //проверка ошибок

  manager.CheckErrors (METHOD_NAME);  
}
