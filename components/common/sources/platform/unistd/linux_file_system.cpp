#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <platform/default/shared.h>
#include <xtl/function.h>
#include <common/singleton.h>
#include <common/strlib.h>

using namespace common;
using namespace stl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с файловой системой в линукс
///////////////////////////////////////////////////////////////////////////////////////////////////
class LinuxFileSystem: public StdioFileSystem
{
  public:
    void FileResize (file_t file,filesize_t new_size);
    void Mkdir      (const char* dir_name);
    void Search     (const char* mask,const FileSearchHandler& find_handler);
};

void LinuxFileSystem::FileResize (file_t file,filesize_t new_size)
{
  if (ftruncate (fileno ((FILE*)file),new_size) == -1)
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("LinuxFileSystem::FileResize","file"); break;
      case EINVAL: RaiseNullArgument    ("LinuxFileSystem::FileResize","buffer"); break;
      case ENOSPC: Raise<FileNoSpaceException> ("LinuxFileSystem::FileResize","No enough space for resize file up to %u bytes",new_size); break;
      default:     Raise<FileException> ("LinuxFileSystem::FileResize","Unknown error"); break;
    }    
  }
}

void LinuxFileSystem::Mkdir (const char* dir_name)
{
  if (mkdir (dir_name,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH))
  {
    switch (errno)
    {
      case EEXIST: Raise<FileException> ("LinuxFileSystem::Mkdir","Path '%s' already exist",dir_name); break;
      case ENOENT: Raise<FileNotFoundException> ("LinuxFileSystem::Mkdir","Path '%s' not found",dir_name); break;
      default:     Raise<FileException> ("LinuxFileSystem::Mkdir","Unknown error"); break;
    }
  }
}

void LinuxFileSystem::Search (const char* full_mask,const FileSearchHandler& find_handler)
{
  string dir_name = dir (full_mask), mask = full_mask + dir_name.size (), file_name;

  DIR* dir = opendir (dir_name.c_str ());

  if (!dir)
    return;    

  FileInfo info;
  
  for (struct dirent* entry;entry=readdir (dir);)
  {   
    if (*entry->d_name != '.' && wcmatch (entry->d_name,mask.c_str ()))
    {            
      file_name = dir_name + entry->d_name;      

      StdioFileSystem::GetFileInfo (file_name.c_str (),info);

      find_handler (file_name.c_str (),info);
    }
  }

  closedir (dir);
}

/*
    LinuxPlatform
*/

ICustomFileSystem* LinuxPlatform::GetFileSystem ()
{
  return Singleton<LinuxFileSystem>::InstancePtr ();
}
