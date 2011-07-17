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
  {"glAlphaFunc", (const void*)&glAlphaFunc},
  {"glClearColor", (const void*)&glClearColor},
  {"glClearDepthf", (const void*)&glClearDepthf},
  {"glClipPlanef", (const void*)&glClipPlanef},
  {"glColor4f", (const void*)&glColor4f},
  {"glDepthRangef", (const void*)&glDepthRangef},
  {"glFogf", (const void*)&glFogf},
  {"glFogfv", (const void*)&glFogfv},
  {"glFrustumf", (const void*)&glFrustumf},
  {"glGetClipPlanef", (const void*)&glGetClipPlanef},
  {"glGetFloatv", (const void*)&glGetFloatv},
  {"glGetLightfv", (const void*)&glGetLightfv},
  {"glGetMaterialfv", (const void*)&glGetMaterialfv},
  {"glGetTexEnvfv", (const void*)&glGetTexEnvfv},
  {"glGetTexParameterfv", (const void*)&glGetTexParameterfv},
  {"glLightModelf", (const void*)&glLightModelf},
  {"glLightModelfv", (const void*)&glLightModelfv},
  {"glLightf", (const void*)&glLightf},
  {"glLightfv", (const void*)&glLightfv},
  {"glLineWidth", (const void*)&glLineWidth},
  {"glLoadMatrixf", (const void*)&glLoadMatrixf},
  {"glMaterialf", (const void*)&glMaterialf},
  {"glMaterialfv", (const void*)&glMaterialfv},
  {"glMultMatrixf", (const void*)&glMultMatrixf},
  {"glMultiTexCoord4f", (const void*)&glMultiTexCoord4f},
  {"glNormal3f", (const void*)&glNormal3f},
  {"glOrthof", (const void*)&glOrthof},
  {"glPointParameterf", (const void*)&glPointParameterf},
  {"glPointParameterfv", (const void*)&glPointParameterfv},
  {"glPointSize", (const void*)&glPointSize},
  {"glPolygonOffset", (const void*)&glPolygonOffset},
  {"glRotatef", (const void*)&glRotatef},
  {"glScalef", (const void*)&glScalef},
  {"glTexEnvf", (const void*)&glTexEnvf},
  {"glTexEnvfv", (const void*)&glTexEnvfv},
  {"glTexParameterf", (const void*)&glTexParameterf},
  {"glTexParameterfv", (const void*)&glTexParameterfv},
  {"glTranslatef", (const void*)&glTranslatef},
  {"glActiveTexture", (const void*)&glActiveTexture},
  {"glAlphaFuncx", (const void*)&glAlphaFuncx},
  {"glBindBuffer", (const void*)&glBindBuffer},
  {"glBindTexture", (const void*)&glBindTexture},
  {"glBlendFunc", (const void*)&glBlendFunc},
  {"glBufferData", (const void*)&glBufferData},
  {"glBufferSubData", (const void*)&glBufferSubData},
  {"glClear", (const void*)&glClear},
  {"glClearColorx", (const void*)&glClearColorx},
  {"glClearDepthx", (const void*)&glClearDepthx},
  {"glClearStencil", (const void*)&glClearStencil},
  {"glClientActiveTexture", (const void*)&glClientActiveTexture},
  {"glClipPlanex", (const void*)&glClipPlanex},
  {"glColor4ub", (const void*)&glColor4ub},
  {"glColor4x", (const void*)&glColor4x},
  {"glColorMask", (const void*)&glColorMask},
  {"glColorPointer", (const void*)&glColorPointer},
  {"glCompressedTexImage2D", (const void*)&glCompressedTexImage2D},
  {"glCompressedTexSubImage2D", (const void*)&glCompressedTexSubImage2D},
  {"glCopyTexImage2D", (const void*)&glCopyTexImage2D},
  {"glCopyTexSubImage2D", (const void*)&glCopyTexSubImage2D},
  {"glCullFace", (const void*)&glCullFace},
  {"glDeleteBuffers", (const void*)&glDeleteBuffers},
  {"glDeleteTextures", (const void*)&glDeleteTextures},
  {"glDepthFunc", (const void*)&glDepthFunc},
  {"glDepthMask", (const void*)&glDepthMask},
  {"glDepthRangex", (const void*)&glDepthRangex},
  {"glDisable", (const void*)&glDisable},
  {"glDisableClientState", (const void*)&glDisableClientState},
  {"glDrawArrays", (const void*)&glDrawArrays},
  {"glDrawElements", (const void*)&glDrawElements},
  {"glEnable", (const void*)&glEnable},
  {"glEnableClientState", (const void*)&glEnableClientState},
  {"glFinish", (const void*)&glFinish},
  {"glFlush", (const void*)&glFlush},
  {"glFogx", (const void*)&glFogx},
  {"glFogxv", (const void*)&glFogxv},
  {"glFrontFace", (const void*)&glFrontFace},
  {"glFrustumx", (const void*)&glFrustumx},
  {"glGetBooleanv", (const void*)&glGetBooleanv},
  {"glGetBufferParameteriv", (const void*)&glGetBufferParameteriv},
  {"glGetClipPlanex", (const void*)&glGetClipPlanex},
  {"glGenBuffers", (const void*)&glGenBuffers},
  {"glGenTextures", (const void*)&glGenTextures},
  {"glGetError", (const void*)&glGetError},
  {"glGetFixedv", (const void*)&glGetFixedv},
  {"glGetIntegerv", (const void*)&glGetIntegerv},
  {"glGetLightxv", (const void*)&glGetLightxv},
  {"glGetMaterialxv", (const void*)&glGetMaterialxv},
  {"glGetPointerv", (const void*)&glGetPointerv},
  {"glGetString", (const void*)&glGetString},
  {"glGetTexEnviv", (const void*)&glGetTexEnviv},
  {"glGetTexEnvxv", (const void*)&glGetTexEnvxv},
  {"glGetTexParameteriv", (const void*)&glGetTexParameteriv},
  {"glGetTexParameterxv", (const void*)&glGetTexParameterxv},
  {"glHint", (const void*)&glHint},
  {"glIsBuffer", (const void*)&glIsBuffer},
  {"glIsEnabled", (const void*)&glIsEnabled},
  {"glIsTexture", (const void*)&glIsTexture},
  {"glLightModelx", (const void*)&glLightModelx},
  {"glLightModelxv", (const void*)&glLightModelxv},
  {"glLightx", (const void*)&glLightx},
  {"glLightxv", (const void*)&glLightxv},
  {"glLineWidthx", (const void*)&glLineWidthx},
  {"glLoadIdentity", (const void*)&glLoadIdentity},
  {"glLoadMatrixx", (const void*)&glLoadMatrixx},
  {"glLogicOp", (const void*)&glLogicOp},
  {"glMaterialx", (const void*)&glMaterialx},
  {"glMaterialxv", (const void*)&glMaterialxv},
  {"glMatrixMode", (const void*)&glMatrixMode},
  {"glMultMatrixx", (const void*)&glMultMatrixx},
  {"glMultiTexCoord4x", (const void*)&glMultiTexCoord4x},
  {"glNormal3x", (const void*)&glNormal3x},
  {"glNormalPointer", (const void*)&glNormalPointer},
  {"glOrthox", (const void*)&glOrthox},
  {"glPixelStorei", (const void*)&glPixelStorei},
  {"glPointParameterx", (const void*)&glPointParameterx},
  {"glPointParameterxv", (const void*)&glPointParameterxv},
  {"glPointSizex", (const void*)&glPointSizex},
  {"glPolygonOffsetx", (const void*)&glPolygonOffsetx},
  {"glPopMatrix", (const void*)&glPopMatrix},
  {"glPushMatrix", (const void*)&glPushMatrix},
  {"glReadPixels", (const void*)&glReadPixels},
  {"glRotatex", (const void*)&glRotatex},
  {"glSampleCoverage", (const void*)&glSampleCoverage},
  {"glSampleCoveragex", (const void*)&glSampleCoveragex},
  {"glScalex", (const void*)&glScalex},
  {"glScissor", (const void*)&glScissor},
  {"glShadeModel", (const void*)&glShadeModel},
  {"glStencilFunc", (const void*)&glStencilFunc},
  {"glStencilMask", (const void*)&glStencilMask},
  {"glStencilOp", (const void*)&glStencilOp},
  {"glTexCoordPointer", (const void*)&glTexCoordPointer},
  {"glTexEnvi", (const void*)&glTexEnvi},
  {"glTexEnvx", (const void*)&glTexEnvx},
  {"glTexEnviv", (const void*)&glTexEnviv},
  {"glTexEnvxv", (const void*)&glTexEnvxv},
  {"glTexImage2D", (const void*)&glTexImage2D},
  {"glTexParameteri", (const void*)&glTexParameteri},
  {"glTexParameterx", (const void*)&glTexParameterx},
  {"glTexParameteriv", (const void*)&glTexParameteriv},
  {"glTexParameterxv", (const void*)&glTexParameterxv},
  {"glTexSubImage2D", (const void*)&glTexSubImage2D},
  {"glTranslatex", (const void*)&glTranslatex},
  {"glVertexPointer", (const void*)&glVertexPointer},
  {"glViewport", (const void*)&glViewport},
  {"glPointSizePointerOES", (const void*)&glPointSizePointerOES},
};

const size_t DEFAULT_ENTRIES_COUNT = sizeof (default_entries) / sizeof (*default_entries);

#ifdef ANDROID

///Dynamic library wrapper for Android
class DynamicLibrary
{
  public:
    DynamicLibrary (const char* path) : handle (dlopen (path, RTLD_NOW | RTLD_GLOBAL))
    {
      if (!handle)
        throw xtl::format_operation_exception ("render::low_level::opengl::egl::DynamicLibrary::DynamicLibrary", "Library not found '%s'", path);              
    }
    
    ~DynamicLibrary ()
    {
      dlclose (handle);
    }      
    
    void* GetProcAddress (const char* name)
    {
      if (!name)
        return 0;
        
      return (void*)dlsym (handle, name);
    }

  private:
    DynamicLibrary (const DynamicLibrary&); //no impl
    DynamicLibrary& operator = (const DynamicLibrary&); //no impl
    
  private:
    void* handle;
};

class GlesLibrary
{
  public:
    GlesLibrary ()
      : library ("libGLESv1_CM.so")
    {
    }
    
    void* GetProcAddress (const char* name)
    {
      return library.GetProcAddress (name);
    }

  private:
    DynamicLibrary library;
};

#else

class GlesLibrary
{
  public:
    void GetProcAddress (const char*)
    {
      return 0;
    }
};

#endif

}

/*
    Описание реализации библиотеки
*/

typedef stl::hash_map<stl::hash_key<const char*>, const void*> EntryMap;

struct Library::Impl
{
  NativeDisplayType display;      //соединение с дисплеем
  EntryMap          entries;      //карта стандартных точек входа
  GlesLibrary       gles_library; //динамическая библиотека GLES

///Конструктор
  Impl ()
    : display (0)
  {
#ifdef BEAGLEBOARD
    display = (NativeDisplayType)DisplayManager::DisplayHandle ();
#endif    
    
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
  {
    address = (void*)eglGetProcAddress (name);    
    
    if (!address)
      address = impl->gles_library.GetProcAddress (name);
  }

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
