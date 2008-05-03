/*
    Регистрация ассоциаций между C++ RTTI и библиотеками
*/
    
template <class T>
inline void Environment::RegisterType (const char* library_id)
{
  RegisterType (typeid (T),                library_id);
  RegisterType (typeid (const T),          library_id);
  RegisterType (typeid (volatile T),       library_id);
  RegisterType (typeid (volatile const T), library_id);
}

template <class T>
inline void Environment::UnregisterType ()
{
  UnregisterType (typeid (T));
  UnregisterType (typeid (const T));
  UnregisterType (typeid (volatile T));
  UnregisterType (typeid (volatile const T));
}
