#include "shared.h"

using namespace common;

CustomFileImpl::CustomFileImpl (ICustomFileSystemPtr _file_system,const char* _file_name,filemode_t mode_flags)
  : FileImpl (mode_flags)
  , auto_close (true)
  , has_file_path (false)
{
  if (!_file_system)
    throw xtl::make_null_argument_exception ("common::CustomFileImpl::CustomFileImpl","file_system");

  if (!_file_name)
    throw xtl::make_null_argument_exception ("common::CustomFileImpl::CustomFileImpl","file_name");

  file_system = _file_system;
  file_handle = file_system->FileOpen (_file_name,mode_flags,0);
}

CustomFileImpl::CustomFileImpl (ICustomFileSystemPtr _file_system,file_t _handle,filemode_t mode_flags,bool _auto_close)
  : FileImpl (mode_flags)
  , file_system (_file_system)
  , file_handle (_handle)
  , auto_close (_auto_close)
  , has_file_path (false)
{
  if (!_file_system)
    throw xtl::make_null_argument_exception ("common::CustomFileImpl::CustomFileImpl","file_system");
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
}

void CustomFileImpl::SetPath (const char* path)
{
  if (!path)
    throw xtl::make_null_argument_exception ("common::CustomFileImpl::SetPath", "path");
    
  file_path     = path;
  has_file_path = true;
}

const char* CustomFileImpl::GetPath ()
{
  if (!has_file_path)
    SetPath (FileImpl::GetPath ());

  return file_path.c_str ();
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
  if (Mode () & (FileMode_Write | FileMode_Resize | FileMode_Create))
    file_system->FileFlush (file_handle);
}

size_t CustomFileImpl::GetBufferSize ()
{
  return file_system->FileBufferSize (file_handle);
}
