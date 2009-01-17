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
  if (instance)
    return;

  if (is_in_init)
    throw stl::runtime_error ("Singleton recursive init");

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
}

template <class T,template <class> class CreationPolicy>
void Singleton<T,CreationPolicy>::Destroy ()
{
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

  instance = NULL;
}

/*
    Получение экземпляра
*/

template <class T,template <class> class CreationPolicy>
inline T& Singleton<T,CreationPolicy>::Instance ()
{
  if (!instance)
    Init ();

  return *instance;
}

/*
    Проверка доступности экземпляра
*/

template <class T,template <class> class CreationPolicy>
inline bool Singleton<T,CreationPolicy>::IsInitialized ()
{
  return instance != 0;
}
