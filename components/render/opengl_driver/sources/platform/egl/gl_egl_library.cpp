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
  void*       pointer;
};

DefaultEntry default_entries [] = {
  {"glAlphaFunc", (void*)&glAlphaFunc},
  {"glClearColor", (void*)&glClearColor},
  {"glClearDepthf", (void*)&glClearDepthf},
  {"glClipPlanef", (void*)&glClipPlanef},
  {"glColor4f", (void*)&glColor4f},
  {"glDepthRangef", (void*)&glDepthRangef},
  {"glFogf", (void*)&glFogf},
  {"glFogfv", (void*)&glFogfv},
  {"glFrustumf", (void*)&glFrustumf},
  {"glGetClipPlanef", (void*)&glGetClipPlanef},
  {"glGetFloatv", (void*)&glGetFloatv},
  {"glGetLightfv", (void*)&glGetLightfv},
  {"glGetMaterialfv", (void*)&glGetMaterialfv},
  {"glGetTexEnvfv", (void*)&glGetTexEnvfv},
  {"glGetTexParameterfv", (void*)&glGetTexParameterfv},
  {"glLightModelf", (void*)&glLightModelf},
  {"glLightModelfv", (void*)&glLightModelfv},
  {"glLightf", (void*)&glLightf},
  {"glLightfv", (void*)&glLightfv},
  {"glLineWidth", (void*)&glLineWidth},
  {"glLoadMatrixf", (void*)&glLoadMatrixf},
  {"glMaterialf", (void*)&glMaterialf},
  {"glMaterialfv", (void*)&glMaterialfv},
  {"glMultMatrixf", (void*)&glMultMatrixf},
  {"glMultiTexCoord4f", (void*)&glMultiTexCoord4f},
  {"glNormal3f", (void*)&glNormal3f},
  {"glOrthof", (void*)&glOrthof},
  {"glPointParameterf", (void*)&glPointParameterf},
  {"glPointParameterfv", (void*)&glPointParameterfv},
  {"glPointSize", (void*)&glPointSize},
  {"glPolygonOffset", (void*)&glPolygonOffset},
  {"glRotatef", (void*)&glRotatef},
  {"glScalef", (void*)&glScalef},
  {"glTexEnvf", (void*)&glTexEnvf},
  {"glTexEnvfv", (void*)&glTexEnvfv},
  {"glTexParameterf", (void*)&glTexParameterf},
  {"glTexParameterfv", (void*)&glTexParameterfv},
  {"glTranslatef", (void*)&glTranslatef},
  {"glActiveTexture", (void*)&glActiveTexture},
  {"glAlphaFuncx", (void*)&glAlphaFuncx},
  {"glBindBuffer", (void*)&glBindBuffer},
  {"glBindTexture", (void*)&glBindTexture},
  {"glBlendFunc", (void*)&glBlendFunc},
  {"glBufferData", (void*)&glBufferData},
  {"glBufferSubData", (void*)&glBufferSubData},
  {"glClear", (void*)&glClear},
  {"glClearColorx", (void*)&glClearColorx},
  {"glClearDepthx", (void*)&glClearDepthx},
  {"glClearStencil", (void*)&glClearStencil},
  {"glClientActiveTexture", (void*)&glClientActiveTexture},
  {"glClipPlanex", (void*)&glClipPlanex},
  {"glColor4ub", (void*)&glColor4ub},
  {"glColor4x", (void*)&glColor4x},
  {"glColorMask", (void*)&glColorMask},
  {"glColorPointer", (void*)&glColorPointer},
  {"glCompressedTexImage2D", (void*)&glCompressedTexImage2D},
  {"glCompressedTexSubImage2D", (void*)&glCompressedTexSubImage2D},
  {"glCopyTexImage2D", (void*)&glCopyTexImage2D},
  {"glCopyTexSubImage2D", (void*)&glCopyTexSubImage2D},
  {"glCullFace", (void*)&glCullFace},
  {"glDeleteBuffers", (void*)&glDeleteBuffers},
  {"glDeleteTextures", (void*)&glDeleteTextures},
  {"glDepthFunc", (void*)&glDepthFunc},
  {"glDepthMask", (void*)&glDepthMask},
  {"glDepthRangex", (void*)&glDepthRangex},
  {"glDisable", (void*)&glDisable},
  {"glDisableClientState", (void*)&glDisableClientState},
  {"glDrawArrays", (void*)&glDrawArrays},
  {"glDrawElements", (void*)&glDrawElements},
  {"glEnable", (void*)&glEnable},
  {"glEnableClientState", (void*)&glEnableClientState},
  {"glFinish", (void*)&glFinish},
  {"glFlush", (void*)&glFlush},
  {"glFogx", (void*)&glFogx},
  {"glFogxv", (void*)&glFogxv},
  {"glFrontFace", (void*)&glFrontFace},
  {"glFrustumx", (void*)&glFrustumx},
  {"glGetBooleanv", (void*)&glGetBooleanv},
  {"glGetBufferParameteriv", (void*)&glGetBufferParameteriv},
  {"glGetClipPlanex", (void*)&glGetClipPlanex},
  {"glGenBuffers", (void*)&glGenBuffers},
  {"glGenTextures", (void*)&glGenTextures},
  {"glGetError", (void*)&glGetError},
  {"glGetFixedv", (void*)&glGetFixedv},
  {"glGetIntegerv", (void*)&glGetIntegerv},
  {"glGetLightxv", (void*)&glGetLightxv},
  {"glGetMaterialxv", (void*)&glGetMaterialxv},
  {"glGetPointerv", (void*)&glGetPointerv},
  {"glGetString", (void*)&glGetString},
  {"glGetTexEnviv", (void*)&glGetTexEnviv},
  {"glGetTexEnvxv", (void*)&glGetTexEnvxv},
  {"glGetTexParameteriv", (void*)&glGetTexParameteriv},
  {"glGetTexParameterxv", (void*)&glGetTexParameterxv},
  {"glHint", (void*)&glHint},
  {"glIsBuffer", (void*)&glIsBuffer},
  {"glIsEnabled", (void*)&glIsEnabled},
  {"glIsTexture", (void*)&glIsTexture},
  {"glLightModelx", (void*)&glLightModelx},
  {"glLightModelxv", (void*)&glLightModelxv},
  {"glLightx", (void*)&glLightx},
  {"glLightxv", (void*)&glLightxv},
  {"glLineWidthx", (void*)&glLineWidthx},
  {"glLoadIdentity", (void*)&glLoadIdentity},
  {"glLoadMatrixx", (void*)&glLoadMatrixx},
  {"glLogicOp", (void*)&glLogicOp},
  {"glMaterialx", (void*)&glMaterialx},
  {"glMaterialxv", (void*)&glMaterialxv},
  {"glMatrixMode", (void*)&glMatrixMode},
  {"glMultMatrixx", (void*)&glMultMatrixx},
  {"glMultiTexCoord4x", (void*)&glMultiTexCoord4x},
  {"glNormal3x", (void*)&glNormal3x},
  {"glNormalPointer", (void*)&glNormalPointer},
  {"glOrthox", (void*)&glOrthox},
  {"glPixelStorei", (void*)&glPixelStorei},
  {"glPointParameterx", (void*)&glPointParameterx},
  {"glPointParameterxv", (void*)&glPointParameterxv},
  {"glPointSizex", (void*)&glPointSizex},
  {"glPolygonOffsetx", (void*)&glPolygonOffsetx},
  {"glPopMatrix", (void*)&glPopMatrix},
  {"glPushMatrix", (void*)&glPushMatrix},
  {"glReadPixels", (void*)&glReadPixels},
  {"glRotatex", (void*)&glRotatex},
  {"glSampleCoverage", (void*)&glSampleCoverage},
  {"glSampleCoveragex", (void*)&glSampleCoveragex},
  {"glScalex", (void*)&glScalex},
  {"glScissor", (void*)&glScissor},
  {"glShadeModel", (void*)&glShadeModel},
  {"glStencilFunc", (void*)&glStencilFunc},
  {"glStencilMask", (void*)&glStencilMask},
  {"glStencilOp", (void*)&glStencilOp},
  {"glTexCoordPointer", (void*)&glTexCoordPointer},
  {"glTexEnvi", (void*)&glTexEnvi},
  {"glTexEnvx", (void*)&glTexEnvx},
  {"glTexEnviv", (void*)&glTexEnviv},
  {"glTexEnvxv", (void*)&glTexEnvxv},
  {"glTexImage2D", (void*)&glTexImage2D},
  {"glTexParameteri", (void*)&glTexParameteri},
  {"glTexParameterx", (void*)&glTexParameterx},
  {"glTexParameteriv", (void*)&glTexParameteriv},
  {"glTexParameterxv", (void*)&glTexParameterxv},
  {"glTexSubImage2D", (void*)&glTexSubImage2D},
  {"glTranslatex", (void*)&glTranslatex},
  {"glVertexPointer", (void*)&glVertexPointer},
  {"glViewport", (void*)&glViewport},
//  {"glPointSizePointerOES", (void*)&glPointSizePointerOES},
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
    void* GetProcAddress (const char*)
    {
      return 0;
    }
};

#endif

}

/*
    Описание реализации библиотеки
*/

typedef stl::hash_map<stl::hash_key<const char*>, void*> EntryMap;

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

void* Library::GetProcAddress (const char* name, size_t search_flags)
{
  static const char* METHOD_NAME = "render::low_level::opengl::egl::Library::GetProcAddress";
  
  if (!name)
  {
    if (search_flags & EntrySearch_NoThrow)
      return 0;

    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  }
  
  DisplayLock lock (impl->display);

  void* address = 0;

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
