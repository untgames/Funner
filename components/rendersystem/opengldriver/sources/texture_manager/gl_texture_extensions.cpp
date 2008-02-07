#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

TextureExtensions::TextureExtensions (const ContextManager& manager)
{
    //установка текущего контекста
    
  manager.MakeContextCurrent ();
  
    //получение флагов поддержки расширений

  static Extension ARB_texture_compression      = "GL_ARB_texture_compression",  
                   EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc",  
                   EXT_texture_rectangle        = "GL_EXT_texture_rectangle",
                   NV_texture_rectangle         = "GL_NV_texture_rectangle",
                   EXT_texture3D                = "GL_EXT_texture3D",
                   EXT_texture_cube_map         = "GL_EXT_texture_cube_map",
                   ARB_texture_cube_map         = "GL_ARB_texture_cube_map",
                   ARB_texture_non_power_of_two = "GL_ARB_texture_non_power_of_two",
                   ARB_depth_texture            = "GL_ARB_depth_texture",
                   ARB_multitexture             = "GL_ARB_multitexture",
                   EXT_packed_depth_stencil     = "GL_EXT_packed_depth_stencil",
                   SGIS_generate_mipmap         = "GL_SGIS_generate_mipmap",
                   Version_1_2                  = "GL_VERSION_1_2",
                   Version_1_3                  = "GL_VERSION_1_3",
                   Version_1_4                  = "GL_VERSION_1_4",
                   Version_2_0                  = "GL_VERSION_2_0";
                   
  bool has_arb_texture_compression  = manager.IsSupported (ARB_texture_compression) || manager.IsSupported (Version_1_3);  

  has_ext_texture_compression_s3tc  = has_arb_texture_compression && manager.IsSupported (EXT_texture_compression_s3tc);
  has_ext_texture_rectangle         = manager.IsSupported (EXT_texture_rectangle) || manager.IsSupported (NV_texture_rectangle);
  has_ext_texture3d                 = manager.IsSupported (EXT_texture3D) || manager.IsSupported (Version_1_2);
  has_arb_texture_cube_map          = manager.IsSupported (ARB_texture_cube_map) || manager.IsSupported (EXT_texture_cube_map) ||
                                      manager.IsSupported (Version_1_3);
  has_arb_texture_non_power_of_two  = manager.IsSupported (ARB_texture_non_power_of_two) || manager.IsSupported (Version_2_0);                                      
  has_arb_depth_texture             = manager.IsSupported (ARB_depth_texture) || manager.IsSupported (Version_1_4);  
  has_arb_multitexture              = manager.IsSupported (ARB_multitexture) || manager.IsSupported (Version_1_3);
  has_ext_packed_depth_stencil      = manager.IsSupported (EXT_packed_depth_stencil);
  has_sgis_generate_mipmap          = manager.IsSupported (SGIS_generate_mipmap) || manager.IsSupported (Version_1_4);  
}
