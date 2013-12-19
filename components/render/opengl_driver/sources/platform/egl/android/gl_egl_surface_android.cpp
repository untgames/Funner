#include "../shared.h"

#include <syslib/platform/android.h>

/*
    Useful links:
      http://www.netmite.com/android/mydroid/donut/frameworks/base/core/jni/com_google_android_gles_jni_EGLImpl.cpp
      http://gitorious.org/android-eeepc/base/blobs/71b8a66f99ebfe60e90cb4710d5a001ff1c4b203/opengl/libagl/egl.cpp
      http://www.java2s.com/Open-Source/Android/android-core/platform-frameworks-base/com/google/android/gles_jni/EGLImpl.java.htm
      http://www.netmite.com/android/mydroid/frameworks/base/libs/ui/EGLDisplaySurface.cpp    
*/

using namespace syslib::android;
using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    Константы
*/

const char* EGL_CLASS_NAME         = "com/google/android/gles_jni/EGLImpl";
const char* EGL_CONFIG_CLASS_NAME  = "com/google/android/gles_jni/EGLConfigImpl";
const char* EGL_DISPLAY_CLASS_NAME = "com/google/android/gles_jni/EGLDisplayImpl";

}

/*
    Описание реализации поверхности
*/

struct EglSurface::Impl
{
  global_ref<jobject> egl;
  global_ref<jobject> egl_surface;
  global_ref<jobject> egl_display;
  EGLSurface          egl_surface_field;
  jmethodID           egl_destroy_surface;
};

/*
    Конструктор / деструктор
*/

EglSurface::EglSurface (EGLDisplay egl_display, EGLConfig egl_config, const void* window_handle)
  : impl (new Impl)
{
  try
  {
    jobject view = (jobject)window_handle;
    
    if (!view)
      throw xtl::make_null_argument_exception ("", "window");

      //создание поверхности через JNI

    JNIEnv& env = get_env ();

    local_ref<jclass> egl_class (env.FindClass (EGL_CLASS_NAME), false);
    local_ref<jclass> egl_config_class (env.FindClass (EGL_CONFIG_CLASS_NAME), false);
    local_ref<jclass> egl_display_class (env.FindClass (EGL_DISPLAY_CLASS_NAME), false);

    if (!egl_class)
      throw xtl::format_operation_exception ("", "EGLImpl class '%s' not found in JNI environment", EGL_CLASS_NAME);
      
    if (!egl_config_class)
      throw xtl::format_operation_exception ("", "EGLConfigImpl class '%s' not found in JNI environment", EGL_CONFIG_CLASS_NAME);
      
    if (!egl_display_class)
      throw xtl::format_operation_exception ("", "EGLDisplayImpl class '%s' not found in JNI environment", EGL_DISPLAY_CLASS_NAME);

    jmethodID egl_class_constructor         = find_method (&env, egl_class.get (), "<init>", "()V");
    jmethodID egl_config_class_constructor  = find_method (&env, egl_config_class.get (), "<init>", "(I)V");
    jmethodID egl_display_class_constructor = find_method (&env, egl_display_class.get (), "<init>", "(I)V");
    jmethodID egl_create_window_surface     = find_method (&env, egl_class.get (), "eglCreateWindowSurface", "(Ljavax/microedition/khronos/egl/EGLDisplay;Ljavax/microedition/khronos/egl/EGLConfig;Ljava/lang/Object;[I)Ljavax/microedition/khronos/egl/EGLSurface;");
    jmethodID egl_destroy_surface           = find_method (&env, egl_class.get (), "eglDestroySurface", "(Ljavax/microedition/khronos/egl/EGLDisplay;Ljavax/microedition/khronos/egl/EGLSurface;)Z");

    local_ref<jobject> egl (env.NewObject (egl_class.get (), egl_class_constructor), false);

    if (!egl)
      throw xtl::format_operation_exception ("", "EGLImpl constructor failed");

    local_ref<jobject> egl_config_wrapper (env.NewObject (egl_config_class.get (), egl_config_class_constructor, egl_config), false);

    if (!egl_config_wrapper)
      throw xtl::format_operation_exception ("", "EGLConfigImpl constructor failed");

    local_ref<jobject> egl_display_wrapper (env.NewObject (egl_display_class.get (), egl_display_class_constructor, egl_display), false);

    if (!egl_display_wrapper)
      throw xtl::format_operation_exception ("", "EGLDisplayImpl constructor failed");      

    eglWaitGL ();

    local_ref<jobject> egl_surface_wrapper (syslib::android::check_errors (env.CallObjectMethod (egl.get (), egl_create_window_surface, egl_display_wrapper.get (), egl_config_wrapper.get (), view, 0)), false);

    if (!egl_surface_wrapper)
      throw xtl::format_operation_exception ("", "EGLImpl::eglCreateWindowSurface failed");

    local_ref<jclass> egl_surface_class (env.GetObjectClass (egl_surface_wrapper.get ()), false);

    if (!egl_surface_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for EGLSurfaceImpl)");

    jfieldID egl_surface_field = env.GetFieldID (egl_surface_class.get (), "mEGLSurface", "I");

    if (!egl_surface_field)
      throw xtl::format_operation_exception ("", "Field 'mEGLSurface' not found for EGLSurfaceImpl");

    impl->egl                 = egl;
    impl->egl_surface         = egl_surface_wrapper;
    impl->egl_display         = egl_display_wrapper;
    impl->egl_surface_field   = (EGLSurface)env.GetIntField (egl_surface_wrapper.get (), egl_surface_field);
    impl->egl_destroy_surface = egl_destroy_surface;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::egl::android::EglSurface::EglSurface");
    throw;
  }
}

EglSurface::~EglSurface ()
{
  try
  {
    JNIEnv& env = get_env ();

    syslib::android::check_errors (env.CallBooleanMethod (impl->egl.get (), impl->egl_destroy_surface, impl->egl_display.get (), impl->egl_surface.get ()));
  }
  catch (...)
  {
  }
}

/*
    Поверхность
*/

EGLSurface EglSurface::GetSurface () const
{
  return impl->egl_surface_field;
}
