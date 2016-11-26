namespace detail
{

/*
    Базовый класс аттачмента
*/

class ISceneAttachment
{
  public:
    virtual ~ISceneAttachment () {}
    
    virtual const std::type_info& ValueType () = 0;
};

template <class T> class SceneAttachmentImpl: public ISceneAttachment
{
  public:
    SceneAttachmentImpl (const T& in_value) : value (in_value) {}
    
    const std::type_info& ValueType () { return typeid (T); }
    
    T& Value () { return value; }
    
  private:
    T value;
};

template <class T> struct SceneAttachmentType                   { typedef T Type; };
template <class T> struct SceneAttachmentType<const T>          { typedef T Type; };
template <class T> struct SceneAttachmentType<volatile T>       { typedef T Type; };
template <class T> struct SceneAttachmentType<const volatile T> { typedef T Type; };

}

/*
    Присоединенные данные
*/

template <class T>
inline void SceneContext::Attach (T& value)
{
  AttachCore (new detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type> (value));
}

template <class T>
inline void SceneContext::Detach ()
{
  DetachCore (typeid (typename detail::SceneAttachmentType<T>::Type));
}

template <class T>
inline T* SceneContext::FindAttachment ()
{
  detail::ISceneAttachment* attachment = FindAttachmentCore (typeid (typename detail::SceneAttachmentType<T>::Type));
  
  if (attachment && &typeid (typename detail::SceneAttachmentType<T>::Type) == &attachment->ValueType ())
    return &static_cast<detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type>*> (attachment)->Value ();    
    
  return 0;
}

template <class T>
inline const T* SceneContext::FindAttachment () const
{
  return const_cast<SceneContext&> (*this).FindAttachment<T> ();
}

template <class T>
inline T& SceneContext::Attachment ()
{
  T* value = FindAttachment<T> ();

  if (!value)
    RaiseAttachmentError (typeid (typename detail::SceneAttachmentType<T>::Type));
    
  return *value;
}

template <class T>
inline const T& SceneContext::Attachment () const
{
  return const_cast<SceneContext&> (*this).Attachment<T> ();
}

/*
    Присоединенные именованные данные
*/

template <class T>
inline void SceneContext::Attach (const char* name, T& value)
{
  AttachCore (name, new detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type> (value));
}

template <class T>
inline void SceneContext::Detach (const char* name)
{
  DetachCore (name, typeid (typename detail::SceneAttachmentType<T>::Type));
}

template <class T>
inline T* SceneContext::FindAttachment (const char* name)
{
  detail::ISceneAttachment* attachment = FindAttachmentCore (name, typeid (typename detail::SceneAttachmentType<T>::Type));
  
  if (attachment && &typeid (typename detail::SceneAttachmentType<T>::Type) == &attachment->ValueType ())
    return &static_cast<detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type>*> (attachment)->Value ();    
    
  return 0;
}

template <class T>
inline const T* SceneContext::FindAttachment (const char* name) const
{
  return const_cast<SceneContext&> (*this).FindAttachment<T> (name);
}

template <class T>
inline T& SceneContext::Attachment (const char* name)
{
  T* value = FindAttachment<T> (name);

  if (!value)
    RaiseAttachmentError (name, typeid (typename detail::SceneAttachmentType<T>::Type));
    
  return *value;
}

template <class T>
inline const T& SceneContext::Attachment (const char* name) const
{
  return const_cast<SceneContext&> (*this).Attachment<T> (name);
}

/*
    Отсоединение всех данных
*/

template <class T>
inline void SceneContext::DetachAll ()
{
  DetachAllCore (typeid (typename detail::SceneAttachmentType<T>::Type));
}
