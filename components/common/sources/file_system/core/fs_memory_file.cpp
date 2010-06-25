#include "shared.h"

using namespace common;

typedef xtl::uninitialized_storage<char> Buffer;

struct MemFileImpl::Impl
{
  char*  start;            //указатель на начало буфера
  char*  finish;           //указатель на конец буфера
  char*  pos;              //указатель на текущую позицию внутри буфера
  Buffer buffer;           //буфер
  
  void resize (size_t new_size)
  {
    size_t old_pos = pos - start;
    
    buffer.resize (new_size);

    start  = buffer.data ();
    pos    = buffer.data () + old_pos;
    finish = buffer.data () + buffer.size ();
  }
};

MemFileImpl::MemFileImpl (void* buffer, size_t buffer_size, filemode_t mode)
  : FileImpl (mode|FileMode_Seek|FileMode_Rewind)
  , impl (new Impl)
{
  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception ("MemFileImpl::MemFileImpl", "buffer");

  if (mode & FileMode_Resize)
    throw xtl::format_not_supported_exception ("MemFileImpl::MemFileImpl", "Memory files with FileMode_Resize mode not supported");

  impl->start  = (char*)buffer;
  impl->finish = impl->start + buffer_size;
  impl->pos    = impl->start;
}

MemFileImpl::MemFileImpl (FileImplPtr base_file)
  : FileImpl (base_file->Mode ()|FileMode_Seek|FileMode_Rewind)
  , impl (new Impl)
{
  try
  {
    if (!base_file)
      throw xtl::make_null_argument_exception ("", "base_file");
      
    if (base_file->Mode () & (FileMode_Resize|FileMode_Write))
      throw xtl::format_not_supported_exception ("", "Memory files with FileMode_Resize|FileMode_Write mode not supported");

    size_t buffer_size = base_file->Size ();

    impl->buffer.resize (buffer_size);

    buffer_size = base_file->Read (impl->buffer.data (),  buffer_size);

    impl->start  = impl->buffer.data ();
    impl->finish = impl->start + buffer_size;
    impl->pos    = impl->start;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("MemFileImpl::MemFileImpl");
    throw;
  }
}

MemFileImpl::MemFileImpl (size_t reserved_buffer_size, filemode_t mode)
  : FileImpl (mode | FileMode_Seek | FileMode_Rewind)
  , impl (new Impl)
{
  impl->buffer.reserve (reserved_buffer_size);
  
  impl->start  = impl->buffer.data ();
  impl->finish = impl->start;
  impl->pos    = impl->start;
}

MemFileImpl::~MemFileImpl ()
{
}

size_t MemFileImpl::Read (void* buf, size_t size)
{
  if (size_t (impl->finish - impl->pos) < size)
    size = impl->finish - impl->pos;

  memcpy (buf, impl->pos, size);
  
  impl->pos += size;

  return size;
}

size_t MemFileImpl::Write (const void* buf, size_t size)
{
  if (size_t (impl->finish - impl->pos) < size)
  {
    if (Mode () & FileMode_Resize)
    {
      size_t new_file_size = impl->finish - impl->pos + size;

      if (new_file_size > impl->buffer.capacity ())
        impl->buffer.reserve (new_file_size * 2);

      Resize (new_file_size);
    }
    else
    {
      size = impl->finish - impl->pos;
    }
  }

  memcpy (impl->pos, buf, size);

  impl->pos += size;

  return size;
}

filepos_t MemFileImpl::Tell ()
{
  return impl->pos - impl->start;
}

filepos_t MemFileImpl::Seek (filepos_t new_pos)
{
  if (impl->finish - impl->start < new_pos)
    return impl->pos - impl->start;

  impl->pos = impl->start + new_pos;  

  return new_pos;
}

void MemFileImpl::Rewind ()
{
  impl->pos = impl->start;
}

filesize_t MemFileImpl::Size ()
{
  return impl->finish - impl->start;
}

void MemFileImpl::Resize (filesize_t new_file_size)
{
  impl->resize (new_file_size);  
}

bool MemFileImpl::Eof ()
{
  return impl->pos == impl->finish;
}

size_t MemFileImpl::GetBufferSize ()
{
  return impl->finish - impl->start;
}

void* MemFileImpl::GetBuffer ()
{
  return impl->start;
}
