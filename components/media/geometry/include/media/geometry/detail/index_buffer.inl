template <class T>
inline const T* IndexBuffer::Data () const
{
  return 0;  
}

template <>
inline const unsigned int* IndexBuffer::Data<const unsigned int> () const
{
  if (DataType () == IndexType_UInt32)
    return reinterpret_cast<const unsigned int*> (Data ());

  return 0;
}

template <>
inline const unsigned short* IndexBuffer::Data<const unsigned short> () const
{
  if (DataType () == IndexType_UInt16)
    return reinterpret_cast<const unsigned short*> (Data ());

  return 0;
}

template <>
inline const unsigned char* IndexBuffer::Data<const unsigned char> () const
{
  if (DataType () == IndexType_UInt8)
    return reinterpret_cast<const unsigned char*> (Data ());

  return 0;
}

template <class T>
inline T* IndexBuffer::Data ()
{
  return const_cast<T*> (const_cast<const IndexBuffer&> (*this).Data<const T> ());
}
