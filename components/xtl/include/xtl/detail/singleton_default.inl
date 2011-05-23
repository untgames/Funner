/*
    Вспомогательный класс, гарантирующий конструирование объекта до main в случае наличия в коде singleton::instance
*/

namespace detail
{

template <class T, bool need_destroy> struct singleton_default_object_creator
{
  singleton_default_object_creator () { singleton_default<T, need_destroy>::instance (); }

  void do_nothing () const {}

  static singleton_default_object_creator creator;
};

template <class T, bool need_destroy> singleton_default_object_creator<T, need_destroy> singleton_default_object_creator<T, need_destroy>::creator;

/*
    Получение экземпляра объекта
*/

template <class T, bool need_destroy> struct singleton_default_instance
{
  static T& get ()
  {
    static T instance;

    return instance;
  }
};

template <class T> struct singleton_default_instance<T, false>
{
  static T& get ()
  {
    union max_align
    {
      char        field1;
      short       field2;
      int         field3;
      long        field4;
      float       field5;
      double      field6;
      long double field7;
      void*       field8;
      char        value_buffer [sizeof (T)];
    };
    
    struct initializer
    {
      max_align buffer;
      T*        instance;
      
      initializer () : instance (new (&buffer) T) {}
    }
#ifdef ARM
#ifdef __GNU_C__
    __attribute__ ((aligned (8)))
#elif defined (_MSC_VER)
    __declspec(align(32))
#else
#error Unknown compiler
#endif
#endif
    ;

    static initializer instance_holder;

    return *instance_holder.instance;
  }
};

}

template <class T, bool need_destroy>
T& singleton_default<T, need_destroy>::instance ()
{
  detail::singleton_default_object_creator<T, need_destroy>::creator.do_nothing ();

  return detail::singleton_default_instance<T, need_destroy>::get ();
}
