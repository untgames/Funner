/*
    Попытка посещения объекта
*/

namespace detail
{

struct touch_on_default_action
{
  touch_on_default_action (bool& in_flag) : flag (&in_flag)  {}
  
  template <class T> void operator () (T&, Shader::Visitor&) { *flag = true; }
  
  bool* flag;
};

}

template <class T>
inline bool Shader::TryAccept (T& visited, Visitor& visitor)
{
  bool is_not_processed = false;
  
  visitor (visited, detail::touch_on_default_action (is_not_processed));

  return !is_not_processed;
}

/*
    Копирование
*/

inline Shader::Pointer clone (Shader::Pointer ptr, CloneMode mode)
{
  return ptr ? ptr->Clone (mode) : 0;
}
