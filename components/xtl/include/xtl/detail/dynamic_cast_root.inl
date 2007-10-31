/*
    Получение dynamic_cast_root
*/

inline dynamic_cast_root* get_root (dynamic_cast_root& root)
{
  return &root;
}

inline dynamic_cast_root* get_root (dynamic_cast_root* root)
{
  return root;
}

inline const dynamic_cast_root* get_root (const dynamic_cast_root& root)
{
  return &root;
}

inline const dynamic_cast_root* get_root (const dynamic_cast_root* root)
{
  return root;
}

/*
    Получение dynamic_cast_root для интеллектуальных указателей
*/

template <class T>
inline dynamic_cast_root* get_root (const shared_ptr<T>& ptr)
{
  return get_root (get_pointer (ptr));
}

template <class T, class Strategy>
inline dynamic_cast_root* get_root (const intrusive_ptr<T, Strategy>& ptr)
{
  return get_root (get_pointer (ptr));
}

template <class T>
inline const dynamic_cast_root* get_root (const shared_ptr<const T>& ptr)
{
  return get_root (get_pointer (ptr));
}

template <class T, class Strategy>
inline const dynamic_cast_root* get_root (const intrusive_ptr<const T, Strategy>&)
{
  return get_root (get_pointer (ptr));
}

template <class T>
inline dynamic_cast_root* get_root (const stl::auto_ptr<T>& ptr)
{
  return get_root (get_pointer (ptr));
}

template <class T>
const dynamic_cast_root* get_root (const stl::auto_ptr<const T>& ptr)
{
  return get_root (get_pointer (ptr));  
}
