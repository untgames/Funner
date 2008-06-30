/*
    Установка типизированных свойств
*/

template <class T>
inline void set_property (Viewport& viewport, const char* name, const T& value)
{
  stl::string buffer;

  to_string (buffer, value);

  viewport.SetProperty (name, buffer.c_str ());
}

template <class T>
inline T get_property (Viewport& viewport, const char* name)
{
  return get<T> (viewport.GetProperty (name));
}

template <class T>
inline T get_property (Viewport& viewport, const char* name, const T& default_value)
{
  return get<T> (viewport.GetProperty (name), default_value);
}
