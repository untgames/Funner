/*
    Копирование данных между буферами
*/

inline void IBuffer::CopyFrom (IBuffer& source, size_t offset, size_t size)
{
  source.CopyFrom (*this, offset, size);
}

