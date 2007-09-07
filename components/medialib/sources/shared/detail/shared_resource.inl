/*
    SharedResource
*/

inline SharedResource::SharedResource ()
  : ref_count (1)
  {}

inline SharedResource::SharedResource (const SharedResource& resource)
  : ref_count (1)
  {}

/*
    Копирование ресурса
*/

template <class T>
inline T* clone_resource (T* resource, CloneMode mode, const char* source)
{
  T* new_resource = 0;

  switch (mode)
  {
    case CloneMode_Copy:
      new_resource = new T (*resource);
      break;
    case CloneMode_Instance:
      new_resource = resource;
      resource->ref_count++;
      break;
    default:
      common::RaiseInvalidArgument (source, "mode", mode);
      break;
  }

  return new_resource;
}

/*
    Освобождение ресурса
*/

template <class T>
inline void release_resource (T* resource)
{
  if (!--resource->ref_count)
    delete resource;
}

/*
    SharedResourceHolder
*/

/*
    Конструкторы
*/

template <class T>
inline SharedResourceHolder<T>::SharedResourceHolder (media::CloneMode in_clone_mode)
  : clone_mode (in_clone_mode)
{
  CheckMode (clone_mode, "media::SharedResourceHolder::SharedResourceHolder");
}

template <class T>
inline SharedResourceHolder<T>::SharedResourceHolder (const T& in_resource, media::CloneMode in_clone_mode)
  : resource (in_resource, in_clone_mode),
    clone_mode (in_clone_mode)
{
  CheckMode (clone_mode, "media::SharedResourceHolder::SharedResourceHolder");
}

template <class T>
inline SharedResourceHolder<T>::SharedResourceHolder (const SharedResourceHolder& holder)
  : resource (holder.resource, CloneMode_Instance),
    clone_mode (holder.clone_mode)
  {}

template <class T>
inline SharedResourceHolder<T>::SharedResourceHolder (const SharedResourceHolder& holder, ForceCloneFlag)
  : resource (holder.resource, holder.clone_mode),
    clone_mode (holder.clone_mode)
  {}

/*
    Присваивание
*/

template <class T>
inline SharedResourceHolder<T>& SharedResourceHolder<T>::operator = (const SharedResourceHolder& holder)
{
  resource.Assign (holder.resource, CloneMode_Instance);
  
  clone_mode = holder.clone_mode;
  
  return *this;
}

template <class T>
inline void SharedResourceHolder<T>::Assign (const T& in_resource, media::CloneMode in_clone_mode)
{
  CheckMode (in_clone_mode, "media::SharedResourceHolder::SharedResourceHolder");

  resource.Assign (in_resource, in_clone_mode);

  clone_mode = in_clone_mode;
}

/*
    Проверка режима копирования
*/

template <class T>
inline void SharedResourceHolder<T>::CheckMode (media::CloneMode mode, const char* source)
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
    Режим копирования
*/

template <class T>
inline media::CloneMode SharedResourceHolder<T>::CloneMode () const
{
  return clone_mode;
}

/*
    Хранимый ресурс
*/

template <class T>
inline const T& SharedResourceHolder<T>::Resource () const
{
  return resource;
}

template <class T>
inline T& SharedResourceHolder<T>::Resource ()
{
  return resource;
}
