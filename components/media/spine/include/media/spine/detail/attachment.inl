/*
   Get Type and type-specific data. Returned object is owned by callee.
*/

template <class T>
inline const T* Attachment::AttachmentData () const
{
  return TypeSpecificData (xtl::type<T> ());
}

template <class T>
inline T* Attachment::AttachmentData ()
{
  return const_cast<T*> (const_cast<const Attachment&> (*this).AttachmentData<T> ());
}

template <class T>
inline T Attachment::GetAttachmentData () const
{
  const T* data = AttachmentData<T> ();

  if (!data)
    throw xtl::format_operation_exception ("media::spine::Attachment::GetAttachmentData", "Attachment doesn't have data of requested type '%s'", typeid (T).name ());

  return *data;
}
