/*
    ResourceHolder
*/

/*
    Конструкторы
*/

template <class T>
inline ResourceHolder<T>::ResourceHolder (media::CloneMode in_clone_mode)
  : clone_mode (in_clone_mode)
{
  CheckMode (clone_mode, "media::ResourceHolder::ResourceHolder");
}

template <class T>
inline ResourceHolder<T>::ResourceHolder (const T& in_resource, media::CloneMode in_clone_mode)
  : resource (in_resource, in_clone_mode),
    clone_mode (in_clone_mode)
{
  CheckMode (clone_mode, "media::ResourceHolder::ResourceHolder");
}

template <class T>
inline ResourceHolder<T>::ResourceHolder (const ResourceHolder& holder, ForceCloneFlag)
  : resource (holder.resource, holder.clone_mode),
    clone_mode (holder.clone_mode)
  {}

/*
    Проверка режима копирования
*/

template <class T>
inline void ResourceHolder<T>::CheckMode (media::CloneMode mode, const char* source)
{
  switch (mode)
  {
    case CloneMode_Copy:
    case CloneMode_Instance:
      break;
    default:
      common::RaiseInvalidArgument (source, "clone_mode", mode);
      break;
  }  
}

/*
    Присоединение ресурса
*/

template <class T>
inline void ResourceHolder<T>::Attach (const T& in_resource, CloneMode mode)
{
  resource   = clone (in_resource, mode);
  clone_mode = mode;
}

/*
    Хранимый ресурс
*/

template <class T>
inline const T& ResourceHolder<T>::Resource () const
{
  return resource;
}

template <class T>
inline T& ResourceHolder<T>::Resource ()
{
  return resource;
}
