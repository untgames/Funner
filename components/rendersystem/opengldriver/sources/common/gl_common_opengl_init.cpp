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
}

}

}

}
