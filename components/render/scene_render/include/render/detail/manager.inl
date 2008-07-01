/*
    Чтение типизированных свойств
*/

template <class T>
inline T get_property (IRenderContext& context, const char* name)
{
  return get<T> (context.GetProperty (name));
}

template <class T>
inline T get_property (IRenderContext& context, const char* name, const T& default_value)
{
  return get<T> (context.GetProperty (name), default_value);
}
