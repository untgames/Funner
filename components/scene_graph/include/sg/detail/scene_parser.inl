/*
    Установка значения
*/

template <class T>
inline void SceneParserCache::SetValue (const common::ParseNode& decl, const T& value)
{
  SetValueCore (decl, new detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type> (value));
}

/*
    Поиск значения
*/

template <class T>
inline T* SceneParserCache::FindValue (const common::ParseNode& decl)
{
  detail::ISceneAttachment* attachment = FindValueCore (decl, typeid (typename detail::SceneAttachmentType<T>::Type));
  
  if (attachment)
    return &static_cast<detail::SceneAttachmentImpl<typename detail::SceneAttachmentType<T>::Type>*> (attachment)->Value ();

  return 0;
}

template <class T>
inline const T* SceneParserCache::FindValue (const common::ParseNode& decl) const
{
  return const_cast<SceneParserCache&> (*this).FindValue<T> (decl);
}
