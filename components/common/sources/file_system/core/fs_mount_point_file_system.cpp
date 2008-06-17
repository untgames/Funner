#include "shared.h"

using namespace stl;
using namespace common;
using namespace xtl;

MountPointFileSystem::MountPointFileSystem (MountFileSystem& _owner)
  : owner (&_owner)
  { }

MountPointFileSystem::file_t MountPointFileSystem::FileOpen (const char* file_name,filemode_t,size_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileOpen","Unable to open mount-point file '%s'",file_name);
}

void MountPointFileSystem::FileClose (file_t)
{
}

size_t MountPointFileSystem::FileRead (file_t,void*,size_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileRead");
}

size_t MountPointFileSystem::FileWrite (file_t,const void* buf,size_t size)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileWrite");
}

void MountPointFileSystem::FileRewind (file_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileRewind");
}

filepos_t MountPointFileSystem::FileSeek (file_t,filepos_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileSeek");
}

filepos_t MountPointFileSystem::FileTell (file_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileTell");
}

filesize_t MountPointFileSystem::FileSize (file_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileSize");
}

void MountPointFileSystem::FileResize (file_t,filesize_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileResize");
}

bool MountPointFileSystem::FileEof (file_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileEof");
}

void MountPointFileSystem::FileFlush (file_t)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::FileFlush");
}

void MountPointFileSystem::Remove (const char* file_name)
{
  FileSystemSingleton::Instance ().Unmount (file_name);
}

void MountPointFileSystem::Rename (const char* file_name,const char* new_name)
{
  ICustomFileSystemPtr file_system = owner->file_system;
  
  FileSystemSingleton::Instance ().Unmount (file_name);
  
  try
  {
    FileSystemSingleton::Instance ().Mount (new_name,file_system);
  }
  catch (FileMountException& exception)
  {
    FileSystemSingleton::Instance ().Mount (file_name,file_system);
    exception.touch ("MountPointFileSystem::Rename");
    throw;
  }
}

void MountPointFileSystem::Mkdir (const char*)
{
}

bool MountPointFileSystem::IsFileExist (const char*)
{
  return true;
}

bool MountPointFileSystem::GetFileInfo (const char* name,FileInfo& info)
{
  info = owner->mount_point_info;
  return true;
}

void MountPointFileSystem::Search (const char* mask,const FileSearchHandler& insert_handler)
{
  throw xtl::format_not_supported_exception ("MountPointFileSystem::Search");
}
