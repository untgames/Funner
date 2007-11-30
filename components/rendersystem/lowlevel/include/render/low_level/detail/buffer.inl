/*
    Копирование данных между буферами
*/

inline void IBuffer::CopyFrom (IBuffer& source, size_t source_offset, size_t size, size_t destination_offset)
{
  source.CopyTo (source_offset, size, *this, destination_offset);
}
