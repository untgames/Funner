/*
   Get Type and type-specific data. Returned object is owned by callee.
*/

template <class T>
inline const T* Attachment::AttachmentData () const
{
  //we specialize implementation for known types, for all other types just return 0
  return 0;
}

template <>
inline const PointAttachmentData* Attachment::AttachmentData<PointAttachmentData> () const
{
  if (Type () == AttachmentType_Point)
    return static_cast<const PointAttachmentData*> (TypeSpecificData ());

  return 0;
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
