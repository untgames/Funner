namespace detail
{

/*
    Экземпляр синглтона
*/

template <class T,template <class> class CreationPolicy>
struct SingletonImpl: public xtl::singleton_default<SingletonImpl<T, CreationPolicy> >
{
  T*                object;
  Lockable          lockable;
  SingletonListNode node;
  bool              is_in_init;

  SingletonImpl ()
    : object (0)
    , is_in_init (false)
  {
    SingletonLog::LogEvent (SingletonLog::Event_AfterServiceInit, typeid (Singleton<T, CreationPolicy>));  
  }
  
  ~SingletonImpl ()
  {
    SingletonLog::LogEvent (SingletonLog::Event_BeforeServiceDone, typeid (Singleton<T, CreationPolicy>));
  }
};

}

/*
    Распределение памяти для синглтона при помощи new/delete
*/

template <class T>
inline T* SingletonCreateUsingNew<T>::Create ()
{
  return new T;
}

template <class T>
inline void SingletonCreateUsingNew<T>::Destroy (T* object)
{
  delete object;
}

/*
    Распределение памяти для синглтона при помощи malloc/free
*/

template <class T>
inline T* SingletonCreateUsingMalloc<T>::Create ()
{
  T* buffer = (T*)malloc (sizeof (T));

  if (!buffer)
    throw std::bad_alloc ();

  return new (buffer) T;
}

template <class T>
inline void SingletonCreateUsingMalloc<T>::Destroy (T* object)
{
  object->~T ();
  free (object);
}

/*
    Размещение синглтона в статической памяти
*/

template <class T>
inline T* SingletonStatic<T>::Create ()
{
  struct Test;
  union  MaxAlign
  {
    char        object_buffer [sizeof (T)];
    short       short_value;
    int         int_value;
    long        long_value;
    float       float_value;
    double      double_value;
    long double long_double_value;
    int         Test::*member_ptr;
    int         (Test::*member_function_ptr)(int);
  };

  static MaxAlign static_buffer;

  return new (&static_buffer) T;
}

template <class T>
inline void SingletonStatic<T>::Destroy (T* object)
{
  object->~T ();
}

/*
    Singleton
*/

/*
    Инициализация и уничтожение синглтона
*/

template <class T,template <class> class CreationPolicy>
void Singleton<T,CreationPolicy>::Init ()
{
  detail::SingletonImpl<T,CreationPolicy>& impl = detail::SingletonImpl<T,CreationPolicy>::instance ();
  
  Lock lock (impl.lockable);

  if (impl.object)
    return;

  if (impl.is_in_init)
    throw stl::runtime_error ("singleton recursive init");

  impl.is_in_init = true;

  try
  {
    SingletonLog::LogEvent (SingletonLog::Event_BeforeCreate, typeid (Singleton<T, CreationPolicy>));
  
    impl.object = CreationPolicy<T>::Create ();

    if (!impl.object)
      throw stl::runtime_error ("unable to create singleton");

    impl.node.RegisterSingleton (typeid (Singleton<T, CreationPolicy>), &Singleton<T,CreationPolicy>::Destroy);

    SingletonLog::LogEvent (SingletonLog::Event_AfterCreate, typeid (Singleton<T, CreationPolicy>));    

    impl.is_in_init = false;
  }
  catch (...)
  {
    impl.is_in_init = false;
    throw;
  }
}

template <class T,template <class> class CreationPolicy>
void Singleton<T,CreationPolicy>::Destroy ()
{
  detail::SingletonImpl<T,CreationPolicy>& impl = detail::SingletonImpl<T,CreationPolicy>::instance ();
  
  Lock lock (impl.lockable);

  if (!impl.object)
    return;

  try
  {
    SingletonLog::LogEvent (SingletonLog::Event_BeforeDestroy, typeid (Singleton<T, CreationPolicy>));

    impl.node.UnregisterSingleton ();
    CreationPolicy<T>::Destroy (impl.object);

    SingletonLog::LogEvent (SingletonLog::Event_AfterDestroy, typeid (Singleton<T, CreationPolicy>));
  }
  catch (...)
  {
  }

  impl.object = 0;
}

template <class T,template <class> class CreationPolicy>
inline bool Singleton<T,CreationPolicy>::IsInitialized ()
{
  detail::SingletonImpl<T,CreationPolicy>& impl = detail::SingletonImpl<T,CreationPolicy>::instance ();

  Lock lock (impl.lockable);

  return impl.object != 0;
}

/*
    Получение экземпляра
*/

template <class T,template <class> class CreationPolicy>
Singleton<T,CreationPolicy>::Instance::Instance ()
{
  detail::SingletonImpl<T,CreationPolicy>& impl = detail::SingletonImpl<T,CreationPolicy>::instance ();
  
  impl.lockable.Lock ();

  if (!impl.object)
  {
    try
    {
      Init ();
    }
    catch (...)
    {
      impl.lockable.Unlock ();
      throw;
    }
  }
}

template <class T,template <class> class CreationPolicy>
inline Singleton<T,CreationPolicy>::Instance::Instance (const Instance&)
{
  detail::SingletonImpl<T,CreationPolicy>::instance ().lockable.Lock ();
}

template <class T,template <class> class CreationPolicy>
inline Singleton<T,CreationPolicy>::Instance::~Instance ()
{
  detail::SingletonImpl<T,CreationPolicy>::instance ().lockable.Unlock ();
}

template <class T,template <class> class CreationPolicy>
inline T* Singleton<T,CreationPolicy>::Instance::operator -> () const
{
  return detail::SingletonImpl<T,CreationPolicy>::instance ().object;
}

template <class T,template <class> class CreationPolicy>
inline T& Singleton<T,CreationPolicy>::Instance::operator * () const
{
  return *detail::SingletonImpl<T,CreationPolicy>::instance ().object;
}
