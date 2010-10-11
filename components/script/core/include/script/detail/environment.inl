/*
    Регистрация ассоциаций между C++ RTTI и библиотеками
*/
    
template <class T>
inline void Environment::RegisterType (const char* library_id)
{
  RegisterType (xtl::get_type<T> (),                library_id);
  RegisterType (xtl::get_type<const T> (),          library_id);
  RegisterType (xtl::get_type<volatile T> (),       library_id);
  RegisterType (xtl::get_type<volatile const T> (), library_id);
}

template <class T>
inline void Environment::UnregisterType ()
{
  UnregisterType (xtl::get_type<T> ());
  UnregisterType (xtl::get_type<const T> ());
  UnregisterType (xtl::get_type<volatile T> ());
  UnregisterType (xtl::get_type<volatile const T> ());
}
