#include "shared.h"

using namespace common;

MemFileImpl::MemFileImpl (void* buffer,size_t buffer_size,filemode_t mode)
  : FileImpl (mode|FILE_MODE_SEEK|FILE_MODE_REWIND), is_auto_deleted (false)
{
  if (!buffer && buffer_size)
    RaiseNullArgument ("MemFileImpl::MemFileImpl","buffer");

  if (mode & FILE_MODE_RESIZE)
    RaiseNotSupported ("MemFileImpl::MemFileImpl","Memory files with FILE_MODE_RESIZE mode not supported");

  start  = (char*)buffer;
  finish = start + buffer_size;
  pos    = start;
}

MemFileImpl::MemFileImpl (FileImpl* base_file)
  : FileImpl (base_file->Mode ()|FILE_MODE_SEEK|FILE_MODE_REWIND), is_auto_deleted (true)    
{
  if (!base_file)
    RaiseNullArgument ("MemFileImpl::MemFileImpl","base_file");
    
  if (base_file->Mode () & (FILE_MODE_RESIZE|FILE_MODE_WRITE))
    RaiseNotSupported ("MemFileImpl::MemFileImpl","Memory files with FILE_MODE_RESIZE|FILE_MODE_WRITE mode not supported");
    
  size_t buffer_size = base_file->Size ();    
  void*  buffer      = ::operator new (buffer_size);
  
  try
  {
    buffer_size = base_file->Read (buffer,buffer_size);
  }
  catch (Exception& exception)
  {
    ::operator delete (buffer);
    exception.Raise ("MemFileImpl::MemFileImpl");
  }
  catch (...)
  {
    ::operator delete (buffer);
    throw;
  }

  start  = (char*)buffer;
  finish = start + buffer_size;
  pos    = start;
}

MemFileImpl::~MemFileImpl ()
{
  if (is_auto_deleted)
    ::operator delete (start);
}

size_t MemFileImpl::Read (void* buf,size_t size)
{
  if (size_t (finish - pos) < size)
    size = finish - pos;

  memcpy (buf,pos,size);
  
  pos += size;

  return size;
}

size_t MemFileImpl::Write (const void* buf,size_t size)
{
  if (size_t (finish - pos) < size)
    size = finish - pos;

  memcpy (pos,buf,size);

  pos += size;

  return size;
}

filepos_t MemFileImpl::Tell ()
{
  return pos - start;
}

filepos_t MemFileImpl::Seek (filepos_t new_pos)
{
  if (finish - start < new_pos)
    return pos - start;
    
  pos = start + new_pos;  

  return new_pos;
}

void MemFileImpl::Rewind ()
{
  pos = start;
}

filesize_t MemFileImpl::Size ()
{
  return finish - start;
}

bool MemFileImpl::Eof ()
{
  return pos == finish;
}
