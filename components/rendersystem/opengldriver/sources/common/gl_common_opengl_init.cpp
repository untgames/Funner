#include "shared.h"

/*
    Инициализация состояния OpenGL
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void init_opengl_state ()
{
    //установка состояния управления передачей пикселей (комнды glDrawPixels, glRedPixels, glTexImage, ...)

  glPixelStorei (GL_PACK_ROW_LENGTH,    0);
  glPixelStorei (GL_PACK_ALIGNMENT,     1);
  glPixelStorei (GL_PACK_SKIP_ROWS,     0);
  glPixelStorei (GL_PACK_SKIP_PIXELS,   0);
  glPixelStorei (GL_UNPACK_ROW_LENGTH,  0);
  glPixelStorei (GL_UNPACK_ALIGNMENT,   1);
  glPixelStorei (GL_UNPACK_SKIP_ROWS,   0);
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);

  bool has_ext_texture3D = GLEW_EXT_texture3D || GLEW_VERSION_1_2; //убрать!!!!

  if (has_ext_texture3D)
  {
    glPixelStorei (GL_PACK_SKIP_IMAGES_EXT,    0);
    glPixelStorei (GL_PACK_IMAGE_HEIGHT_EXT,   0);
    glPixelStorei (GL_UNPACK_SKIP_IMAGES_EXT,  0);
    glPixelStorei (GL_UNPACK_IMAGE_HEIGHT_EXT, 0);
  }  
}

}

}

}
