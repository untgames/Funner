#include "../shared.h"

/*
    Useful links:
      http://www.netmite.com/android/mydroid/donut/frameworks/base/core/jni/com_google_android_gles_jni_EGLImpl.cpp
      http://gitorious.org/android-eeepc/base/blobs/71b8a66f99ebfe60e90cb4710d5a001ff1c4b203/opengl/libagl/egl.cpp
      http://www.java2s.com/Open-Source/Android/android-core/platform-frameworks-base/com/google/android/gles_jni/EGLImpl.java.htm
      http://www.netmite.com/android/mydroid/frameworks/base/libs/ui/EGLDisplaySurface.cpp    
*/

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace syslib
{

namespace android
{

/// получение окружения текущей нити
JNIEnv& get_env ();

}

}

namespace
{

/*
    Константы
*/

const char* EGL_CLASS_NAME         = "com/google/android/gles_jni/EGLImpl";
const char* EGL_CONFIG_CLASS_NAME  = "com/google/android/gles_jni/EGLConfigImpl";
const char* EGL_DISPLAY_CLASS_NAME = "com/google/android/gles_jni/EGLDisplayImpl";

/*
    JNI wrappers
*/

///Стратегия владения локальной JNI ссылкой
template <class T> struct jni_local_ref_strategy
{
  static T*   clone   (T* ptr) { return (T*)syslib::android::get_env ().NewLocalRef (ptr); }
  static void release (T* ptr) { syslib::android::get_env ().DeleteLocalRef (ptr); }  
};

/// Локальная JNI ссылка
template <class T> class local_ref: public xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy>
{
  typedef xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy> base;
  public:
    typedef typename base::element_type element_type;

    local_ref () {}
    local_ref (element_type* ptr, bool addref = true) : base (ptr, addref) {}
    local_ref (const local_ref& ref) : base (ref) {}

    template <class T1> local_ref (const local_ref<T1>& ref) : base (ref) {}    
};

}

namespace render
{

namespace low_level
{

namespace opengl
{

namespace egl
{

/*
    Создание экранной поверхности (для Android)
*/

EGLSurface eglCreateWindowSurfaceAndroid (EGLDisplay egl_display, EGLConfig egl_config, const void* window_handle, EGLint format)
{
  try
  {
    jobject view = (jobject)window_handle;
    
    if (!view)
      throw xtl::make_null_argument_exception ("", "window");

      //создание поверхности через JNI

    JNIEnv& env = syslib::android::get_env ();

    local_ref<jclass> egl_class         = env.FindClass (EGL_CLASS_NAME);
    local_ref<jclass> egl_config_class  = env.FindClass (EGL_CONFIG_CLASS_NAME);
    local_ref<jclass> egl_display_class = env.FindClass (EGL_DISPLAY_CLASS_NAME);

    if (!egl_class)
      throw xtl::format_operation_exception ("", "EGLImpl class '%s' not found in JNI environment", EGL_CLASS_NAME);
      
    if (!egl_config_class)
      throw xtl::format_operation_exception ("", "EGLConfigImpl class '%s' not found in JNI environment", EGL_CONFIG_CLASS_NAME);
      
    if (!egl_display_class)
      throw xtl::format_operation_exception ("", "EGLDisplayImpl class '%s' not found in JNI environment", EGL_DISPLAY_CLASS_NAME);

    jmethodID egl_class_constructor         = env.GetMethodID (egl_class.get (), "<init>", "()V");
    jmethodID egl_config_class_constructor  = env.GetMethodID (egl_config_class.get (), "<init>", "(I)V");
    jmethodID egl_display_class_constructor = env.GetMethodID (egl_display_class.get (), "<init>", "(I)V");
    jmethodID egl_create_window_surface     = env.GetMethodID (egl_class.get (), "eglCreateWindowSurface", "(Ljavax/microedition/khronos/egl/EGLDisplay;Ljavax/microedition/khronos/egl/EGLConfig;Ljava/lang/Object;[I)Ljavax/microedition/khronos/egl/EGLSurface;");

    if (!egl_class_constructor)
      throw xtl::format_operation_exception ("", "EGLImpl default constructor not found in class '%s'", EGL_CLASS_NAME);
      
    if (!egl_config_class_constructor)
      throw xtl::format_operation_exception ("", "EGLConfigImpl constructor not found in class '%s'", EGL_CONFIG_CLASS_NAME);
      
    if (!egl_display_class_constructor)
      throw xtl::format_operation_exception ("", "EGLDisplayImpl constructor not found in class '%s'", EGL_DISPLAY_CLASS_NAME);
      
    if (!egl_create_window_surface)
      throw xtl::format_operation_exception ("", "EGLImpl::eglCreateWindowSurface not found in class '%s'", EGL_CLASS_NAME);

    local_ref<jobject> egl = env.NewObject (egl_class.get (), egl_class_constructor);

    if (!egl)
      throw xtl::format_operation_exception ("", "EGLImpl constructor failed");

    local_ref<jobject> egl_config_wrapper = env.NewObject (egl_config_class.get (), egl_config_class_constructor, egl_config);

    if (!egl_config_wrapper)
      throw xtl::format_operation_exception ("", "EGLConfigImpl constructor failed");

    local_ref<jobject> egl_display_wrapper = env.NewObject (egl_display_class.get (), egl_display_class_constructor, egl_display);

    if (!egl_display_wrapper)
      throw xtl::format_operation_exception ("", "EGLDisplayImpl constructor failed");      

    eglWaitGL ();

    local_ref<jobject> egl_surface_wrapper = env.CallObjectMethod (egl.get (), egl_create_window_surface, egl_display_wrapper.get (), egl_config_wrapper.get (), view, 0);

    if (!egl_surface_wrapper)
      throw xtl::format_operation_exception ("", "EGLImpl::eglCreateWindowSurface failed");

    local_ref<jclass> egl_surface_class = env.GetObjectClass (egl_surface_wrapper.get ());

    if (!egl_surface_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for EGLSurfaceImpl)");

    jfieldID egl_surface_field = env.GetFieldID (egl_surface_class.get (), "mEGLSurface", "I");

    if (!egl_surface_field)
      throw xtl::format_operation_exception ("", "Field 'mEGLSurface' not found for EGLSurfaceImpl");

    return (EGLSurface)env.GetIntField (egl_surface_wrapper.get (), egl_surface_field);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::egl::eglCreateWindowSurfaceAndroid");
    throw;
  }
}

}

}

}

}
