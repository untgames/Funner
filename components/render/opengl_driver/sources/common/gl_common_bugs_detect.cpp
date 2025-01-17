#include "shared.h"

namespace render
{

namespace low_level
{

namespace opengl
{

namespace
{

/*
    Определение багов и несоответствий спецификации
*/

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

/*//определение бага функции glGetTexImage при работе с 3D-текстурой
bool detect_texture3d_bug ()
{
  if (!glTexImage3D && !glTexImage3DEXT)
    return false;

  while (glGetError () != GL_NO_ERROR);            

  static const size_t TEX_SIZE = 2, MAX_TEXEL_SIZE = 8, RGB_TEXEL_SIZE = 3;  
  static const unsigned char SRC_BUFFER_MARKER = 0, DST_BUFFER_MARKER = 1;

  static char src_buffer [TEX_SIZE * TEX_SIZE * TEX_SIZE * MAX_TEXEL_SIZE],
              dst_buffer [sizeof src_buffer];

  memset (src_buffer, SRC_BUFFER_MARKER, sizeof src_buffer);
  memset (dst_buffer, DST_BUFFER_MARKER, sizeof dst_buffer);
  
  size_t texture_id = 0;

  glGenTextures (1, &texture_id);
  glBindTexture (GL_TEXTURE_3D, texture_id);

  glPixelStorei (GL_PACK_ROW_LENGTH,    0);
  glPixelStorei (GL_PACK_ALIGNMENT,     1);
  glPixelStorei (GL_PACK_SKIP_ROWS,     0);
  glPixelStorei (GL_PACK_SKIP_PIXELS,   0);
  glPixelStorei (GL_UNPACK_ROW_LENGTH,  0);
  glPixelStorei (GL_UNPACK_ALIGNMENT,   1);
  glPixelStorei (GL_UNPACK_SKIP_ROWS,   0);
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);

  glPixelStorei (GL_PACK_SKIP_IMAGES,    0);
  glPixelStorei (GL_PACK_IMAGE_HEIGHT,   0);
  glPixelStorei (GL_UNPACK_SKIP_IMAGES,  0);
  glPixelStorei (GL_UNPACK_IMAGE_HEIGHT, 0);  
    
  PFNGLTEXIMAGE3DPROC glTexImage3D_fn = glTexImage3D ? glTexImage3D : (PFNGLTEXIMAGE3DPROC)glTexImage3DEXT;
  
  glTexImage3D_fn (GL_TEXTURE_3D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, src_buffer);
  glGetTexImage   (GL_TEXTURE_3D, 0, GL_RGB, GL_UNSIGNED_BYTE, dst_buffer);

  glDeleteTextures (1, &texture_id);  

  return glGetError () != GL_NO_ERROR || dst_buffer [TEX_SIZE * TEX_SIZE * TEX_SIZE * RGB_TEXEL_SIZE] != DST_BUFFER_MARKER;
}*/

#endif

}

/*
    Определение багов драйверов OpenGL
*/

void detect_opengl_bugs (stl::string& extensions)
{
    //определение бага отключено, из-за crash при вызове glGetTexImage на старых драйверах ATI

//  if (detect_texture3d_bug ())
//    extensions += " GLBUG_texture3D_get_tex_image";
}

}

}

}
