#include "shared.h"

using namespace common;

CustomFileImpl::CustomFileImpl (ICustomFileSystem* _file_system,const char* _file_name,filemode_t mode_flags)
  : FileImpl (mode_flags), auto_close (true)
{
  if (!_file_system)
    RaiseNullArgument ("CustomFileImpl::CustomFileImpl","file_system");

  if (!_file_name)
    RaiseNullArgument ("CustomFileImpl::CustomFileImpl","file_name");

  file_system = _file_system;
  file_handle = file_system->FileOpen (_file_name,mode_flags,0);

  file_system->AddRef ();
}

CustomFileImpl::CustomFileImpl (ICustomFileSystem* _file_system,file_t _handle,filemode_t mode_flags,bool _auto_close)
  : FileImpl (mode_flags), auto_close (_auto_close)
{
  if (!_file_system)
    RaiseNullArgument ("CustomFileImpl::CustomFileImpl","file_system");

  file_system = _file_system;
  file_handle = _handle;

  file_system->AddRef ();  
}

CustomFileImpl::~CustomFileImpl ()
{
  try
  {
    if (auto_close)
      file_system->FileClose (file_handle);
  }
  catch (...)
  {
  }

  try
  {
    file_system->Release ();
  }
  catch (...)
  {
  }
}

size_t CustomFileImpl::Read (void* buf,size_t size)
{
  return file_system->FileRead (file_handle,buf,size);
}

size_t CustomFileImpl::Write (const void* buf,size_t size)
{
  return file_system->FileWrite (file_handle,buf,size);
}

filepos_t CustomFileImpl::Tell ()
{
  return file_system->FileTell (file_handle);
}

filepos_t CustomFileImpl::Seek (filepos_t new_pos)
{
  return file_system->FileSeek (file_handle,new_pos);
}

void CustomFileImpl::Rewind ()
{
  file_system->FileRewind (file_handle);
}

filesize_t CustomFileImpl::Size ()
{
  return file_system->FileSize (file_handle);
}

void CustomFileImpl::Resize (filesize_t new_size)
{
  file_system->FileResize (file_handle,new_size);
}

bool CustomFileImpl::Eof ()
{
  return file_system->FileEof (file_handle);
}

void CustomFileImpl::Flush ()
{
  file_system->FileFlush (file_handle);
}

size_t CustomFileImpl::GetBufferSize ()
{
  return file_system->FileBufferSize (file_handle);
}
