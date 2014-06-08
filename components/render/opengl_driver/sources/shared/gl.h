#ifndef RENDER_GL_DRIVER_OPENGL_HEADER
#define RENDER_GL_DRIVER_OPENGL_HEADER

#ifndef OPENGL_ES_SUPPORT
  #ifndef OPENGL_ES2_SUPPORT
    #include <shared/profile/gl/gl.h>
  #else
    #include <shared/profile/gles2/gl.h>
  #endif
#else
  #include <shared/profile/gles/gl.h>
#endif

#endif
