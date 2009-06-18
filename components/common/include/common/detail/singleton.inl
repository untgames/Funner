/*
    Экземпляр синглтона
*/

template <class T,template <class> class CreationPolicy>
T* Singleton<T,CreationPolicy>::instance = 0;

template <class T,template <class> class CreationPolicy>
bool Singleton<T,CreationPolicy>::is_in_init = false;

template <class T,template <class> class CreationPolicy>
Lockable Singleton<T,CreationPolicy>::lockable;

template <class T,template <class> class CreationPolicy>
SingletonListNode Singleton<T,CreationPolicy>::node;

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
  SingletonLog::LogEvent (SingletonLog::Event_BeforeCreate, typeid (Singleton<T, CreationPolicy>));  
  
  Lock lock (lockable);

  if (instance)
    return;

  if (is_in_init)
    throw stl::runtime_error ("singleton recursive init");

  is_in_init = true;

  try
  {
    instance = CreationPolicy<T>::Create ();

    if (!instance)
      throw stl::runtime_error ("unable to create singleton");

    node.RegisterSingleton (typeid (Singleton<T, CreationPolicy>), &Singleton<T,CreationPolicy>::Destroy);

    is_in_init = false;
  }
  catch (...)
  {
    is_in_init = false;
    throw;
  }
  
  SingletonLog::LogEvent (SingletonLog::Event_AfterCreate, typeid (Singleton<T, CreationPolicy>));      
}

template <class T,template <class> class CreationPolicy>
void Singleton<T,CreationPolicy>::Destroy ()
{
  SingletonLog::LogEvent (SingletonLog::Event_BeforeDestroy, typeid (Singleton<T, CreationPolicy>));  

  Lock lock (lockable);

  if (!instance)
    return;

  try
  {
    node.UnregisterSingleton ();
    CreationPolicy<T>::Destroy (instance);
  }
  catch (...)
  {
  }

  instance = 0;

  SingletonLog::LogEvent (SingletonLog::Event_AfterDestroy, typeid (Singleton<T, CreationPolicy>));
}

template <class T,template <class> class CreationPolicy>
inline bool Singleton<T,CreationPolicy>::IsInitialized ()
{
  Lock lock (lockable);

  return instance != 0;
}

/*
    Получение экземпляра
*/

template <class T,template <class> class CreationPolicy>
Singleton<T,CreationPolicy>::Instance::Instance ()
{
  lockable.Lock ();

  if (!instance)
  {
    try
    {
      Init ();
    }
    catch (...)
    {
      lockable.Unlock ();
      throw;
    }
  }
}

template <class T,template <class> class CreationPolicy>
inline Singleton<T,CreationPolicy>::Instance::Instance (const Instance&)
{
  Singleton<T,CreationPolicy>::lockable.Lock ();
}

template <class T,template <class> class CreationPolicy>
inline Singleton<T,CreationPolicy>::Instance::~Instance ()
{
  Singleton<T,CreationPolicy>::lockable.Unlock ();
}

template <class T,template <class> class CreationPolicy>
inline T* Singleton<T,CreationPolicy>::Instance::operator -> () const
{
  return Singleton<T,CreationPolicy>::instance;
}

template <class T,template <class> class CreationPolicy>
inline T& Singleton<T,CreationPolicy>::Instance::operator * () const
{
  return *Singleton<T,CreationPolicy>::instance;
}
