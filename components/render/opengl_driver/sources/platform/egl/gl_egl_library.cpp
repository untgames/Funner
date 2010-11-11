#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    Список стандартных точек входа
*/

struct DefaultEntry
{
  const char* name;
  const void* pointer;
};

DefaultEntry default_entries [] = {
  {"glAlphaFunc", &glAlphaFunc},
  {"glClearColor", &glClearColor},
  {"glClearDepthf", &glClearDepthf},
  {"glClipPlanef", &glClipPlanef},
  {"glColor4f", &glColor4f},
  {"glDepthRangef", &glDepthRangef},
  {"glFogf", &glFogf},
  {"glFogfv", &glFogfv},
  {"glFrustumf", &glFrustumf},
  {"glGetClipPlanef", &glGetClipPlanef},
  {"glGetFloatv", &glGetFloatv},
  {"glGetLightfv", &glGetLightfv},
  {"glGetMaterialfv", &glGetMaterialfv},
  {"glGetTexEnvfv", &glGetTexEnvfv},
  {"glGetTexParameterfv", &glGetTexParameterfv},
  {"glLightModelf", &glLightModelf},
  {"glLightModelfv", &glLightModelfv},
  {"glLightf", &glLightf},
  {"glLightfv", &glLightfv},
  {"glLineWidth", &glLineWidth},
  {"glLoadMatrixf", &glLoadMatrixf},
  {"glMaterialf", &glMaterialf},
  {"glMaterialfv", &glMaterialfv},
  {"glMultMatrixf", &glMultMatrixf},
  {"glMultiTexCoord4f", &glMultiTexCoord4f},
  {"glNormal3f", &glNormal3f},
  {"glOrthof", &glOrthof},
  {"glPointParameterf", &glPointParameterf},
  {"glPointParameterfv", &glPointParameterfv},
  {"glPointSize", &glPointSize},
  {"glPolygonOffset", &glPolygonOffset},
  {"glRotatef", &glRotatef},
  {"glScalef", &glScalef},
  {"glTexEnvf", &glTexEnvf},
  {"glTexEnvfv", &glTexEnvfv},
  {"glTexParameterf", &glTexParameterf},
  {"glTexParameterfv", &glTexParameterfv},
  {"glTranslatef", &glTranslatef},
  {"glActiveTexture", &glActiveTexture},
  {"glAlphaFuncx", &glAlphaFuncx},
  {"glBindBuffer", &glBindBuffer},
  {"glBindTexture", &glBindTexture},
  {"glBlendFunc", &glBlendFunc},
  {"glBufferData", &glBufferData},
  {"glBufferSubData", &glBufferSubData},
  {"glClear", &glClear},
  {"glClearColorx", &glClearColorx},
  {"glClearDepthx", &glClearDepthx},
  {"glClearStencil", &glClearStencil},
  {"glClientActiveTexture", &glClientActiveTexture},
  {"glClipPlanex", &glClipPlanex},
  {"glColor4ub", &glColor4ub},
  {"glColor4x", &glColor4x},
  {"glColorMask", &glColorMask},
  {"glColorPointer", &glColorPointer},
  {"glCompressedTexImage2D", &glCompressedTexImage2D},
  {"glCompressedTexSubImage2D", &glCompressedTexSubImage2D},
  {"glCopyTexImage2D", &glCopyTexImage2D},
  {"glCopyTexSubImage2D", &glCopyTexSubImage2D},
  {"glCullFace", &glCullFace},
  {"glDeleteBuffers", &glDeleteBuffers},
  {"glDeleteTextures", &glDeleteTextures},
  {"glDepthFunc", &glDepthFunc},
  {"glDepthMask", &glDepthMask},
  {"glDepthRangex", &glDepthRangex},
  {"glDisable", &glDisable},
  {"glDisableClientState", &glDisableClientState},
  {"glDrawArrays", &glDrawArrays},
  {"glDrawElements", &glDrawElements},
  {"glEnable", &glEnable},
  {"glEnableClientState", &glEnableClientState},
  {"glFinish", &glFinish},
  {"glFlush", &glFlush},
  {"glFogx", &glFogx},
  {"glFogxv", &glFogxv},
  {"glFrontFace", &glFrontFace},
  {"glFrustumx", &glFrustumx},
  {"glGetBooleanv", &glGetBooleanv},
  {"glGetBufferParameteriv", &glGetBufferParameteriv},
  {"glGetClipPlanex", &glGetClipPlanex},
  {"glGenBuffers", &glGenBuffers},
  {"glGenTextures", &glGenTextures},
  {"glGetError", &glGetError},
  {"glGetFixedv", &glGetFixedv},
  {"glGetIntegerv", &glGetIntegerv},
  {"glGetLightxv", &glGetLightxv},
  {"glGetMaterialxv", &glGetMaterialxv},
  {"glGetPointerv", &glGetPointerv},
  {"glGetString", &glGetString},
  {"glGetTexEnviv", &glGetTexEnviv},
  {"glGetTexEnvxv", &glGetTexEnvxv},
  {"glGetTexParameteriv", &glGetTexParameteriv},
  {"glGetTexParameterxv", &glGetTexParameterxv},
  {"glHint", &glHint},
  {"glIsBuffer", &glIsBuffer},
  {"glIsEnabled", &glIsEnabled},
  {"glIsTexture", &glIsTexture},
  {"glLightModelx", &glLightModelx},
  {"glLightModelxv", &glLightModelxv},
  {"glLightx", &glLightx},
  {"glLightxv", &glLightxv},
  {"glLineWidthx", &glLineWidthx},
  {"glLoadIdentity", &glLoadIdentity},
  {"glLoadMatrixx", &glLoadMatrixx},
  {"glLogicOp", &glLogicOp},
  {"glMaterialx", &glMaterialx},
  {"glMaterialxv", &glMaterialxv},
  {"glMatrixMode", &glMatrixMode},
  {"glMultMatrixx", &glMultMatrixx},
  {"glMultiTexCoord4x", &glMultiTexCoord4x},
  {"glNormal3x", &glNormal3x},
  {"glNormalPointer", &glNormalPointer},
  {"glOrthox", &glOrthox},
  {"glPixelStorei", &glPixelStorei},
  {"glPointParameterx", &glPointParameterx},
  {"glPointParameterxv", &glPointParameterxv},
  {"glPointSizex", &glPointSizex},
  {"glPolygonOffsetx", &glPolygonOffsetx},
  {"glPopMatrix", &glPopMatrix},
  {"glPushMatrix", &glPushMatrix},
  {"glReadPixels", &glReadPixels},
  {"glRotatex", &glRotatex},
  {"glSampleCoverage", &glSampleCoverage},
  {"glSampleCoveragex", &glSampleCoveragex},
  {"glScalex", &glScalex},
  {"glScissor", &glScissor},
  {"glShadeModel", &glShadeModel},
  {"glStencilFunc", &glStencilFunc},
  {"glStencilMask", &glStencilMask},
  {"glStencilOp", &glStencilOp},
  {"glTexCoordPointer", &glTexCoordPointer},
  {"glTexEnvi", &glTexEnvi},
  {"glTexEnvx", &glTexEnvx},
  {"glTexEnviv", &glTexEnviv},
  {"glTexEnvxv", &glTexEnvxv},
  {"glTexImage2D", &glTexImage2D},
  {"glTexParameteri", &glTexParameteri},
  {"glTexParameterx", &glTexParameterx},
  {"glTexParameteriv", &glTexParameteriv},
  {"glTexParameterxv", &glTexParameterxv},
  {"glTexSubImage2D", &glTexSubImage2D},
  {"glTranslatex", &glTranslatex},
  {"glVertexPointer", &glVertexPointer},
  {"glViewport", &glViewport},
  {"glPointSizePointerOES", &glPointSizePointerOES},
};

const size_t DEFAULT_ENTRIES_COUNT = sizeof (default_entries) / sizeof (*default_entries);

}

/*
    Описание реализации библиотеки
*/

typedef stl::hash_map<stl::hash_key<const char*>, const void*> EntryMap;

struct Library::Impl
{
  Display* display; //соединение с дисплеем
  EntryMap entries; //карта стандартных точек входа

///Конструктор
  Impl ()
    : display ((Display*)DisplayManager::DisplayHandle ())
  {
    for (size_t i=0; i<DEFAULT_ENTRIES_COUNT; i++)
      entries [default_entries [i].name] = default_entries [i].pointer;
  }
};

/*
    Конструктор / деструктор
*/

Library::Library ()  
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Library::Library");
    throw;
  }
}

Library::~Library ()
{
}

/*
    Получение адреса точки входа
*/

const void* Library::GetProcAddress (const char* name, size_t search_flags)
{
  static const char* METHOD_NAME = "render::low_level::opengl::egl::Library::GetProcAddress";
  
  if (!name)
  {
    if (search_flags & EntrySearch_NoThrow)
      return 0;

    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  }
  
  DisplayLock lock (impl->display);

  const void* address = 0;

  if (!address && (search_flags & EntrySearch_Context))
    address = (void*)eglGetProcAddress (name);    

  if (address)
    return address;

  if (search_flags & EntrySearch_Library)
  {
    EntryMap::iterator iter = impl->entries.find (name);

    address = iter != impl->entries.end () ? iter->second : 0;
  }

  if (address)
    return address;

  if (search_flags & EntrySearch_NoThrow)
    return 0;

  throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in EGL", name);
}
