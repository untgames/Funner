/*
    Поиск методов и полей
*/

inline jmethodID find_static_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetStaticMethodID (class_, name, signature);

  if (!result)
  {
    if (env->ExceptionOccurred ())
      env->ExceptionClear ();

    throw xtl::format_operation_exception ("JNIEnv::GetStaticMethodID", "Static method '%s %s' not found", name, signature);
  }

  return result;
}

inline jmethodID find_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetMethodID (class_, name, signature);

  if (!result)
  {
    if (env->ExceptionOccurred ())
      env->ExceptionClear ();

    throw xtl::format_operation_exception ("JNIEnv::GetMethodID", "Method '%s %s' not found", name, signature);
  }

  return result;
}

inline jfieldID find_field (JNIEnv* env, jclass class_, const char* name, const char* type)
{
  jfieldID result = env->GetFieldID (class_, name, type);

  if (!result)
  {
    if (env->ExceptionOccurred ())
      env->ExceptionClear ();

    throw xtl::format_operation_exception ("JNIEnv::GetFieldID", "Field '%s %s' not found", type, name);
  }

  return result;
}
