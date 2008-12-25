/*
    Вспомогательный класс, гарантирующий конструирование объекта до main в случае наличия в коде singleton::instance
*/

namespace detail
{

template <class T> struct singleton_default_object_creator
{
  singleton_default_object_creator () { singleton_default<T>::instance (); }
  
  void do_nothing () const {}  
  
  static singleton_default_object_creator creator;
};

template <class T> singleton_default_object_creator<T> singleton_default_object_creator<T>::creator;

}

/*
    Получение экземпляра объекта
*/

template <class T>
T& singleton_default<T>::instance ()
{
  static T object;

  detail::singleton_default_object_creator<T>::creator.do_nothing ();

  return object;
}
