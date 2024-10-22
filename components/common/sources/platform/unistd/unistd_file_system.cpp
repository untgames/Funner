#include <unistd.h>

#if !defined ( ARM9 ) && !defined ( BADA )
  #include <dirent.h>
#endif

#ifdef ANDROID
  #include <sys/vfs.h>

  #define statvfs statfs
#else
  #include <sys/statvfs.h>
#endif

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

#include <xtl/function.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/default/shared.h>

using namespace common;
using namespace stl;

/*
    Работа с unistd файловой системой
*/

namespace
{

class UnistdFileSystem: public StdioFileSystem
{
  public:
#if !defined ( ARM9 ) && !defined ( BADA )
    void FileResize (file_t file,filesize_t new_size)
    {
      static const char* METHOD_NAME = "UnistdFileSystem::FileResize";

      if (ftruncate (fileno ((FILE*)file),new_size) == -1)
      {
        switch (errno)
        {
          case EBADF:  throw xtl::make_argument_exception (METHOD_NAME,"file"); break;
          case EINVAL: throw xtl::make_null_argument_exception    (METHOD_NAME,"buffer"); break;
          case ENOSPC: throw xtl::format_operation_exception (METHOD_NAME,"No enough space for resize file up to %u bytes",new_size); break;
          default:     throw xtl::format_operation_exception (METHOD_NAME,"Unknown error"); break;
        }
      }
    }
#endif

    void Mkdir (const char* dir_name)
    {
      static const char* METHOD_NAME = "UnistdFileSystem::Mkdir";
      
      if (mkdir (dir_name,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH))
      {
        switch (errno)
        {
          case EEXIST: throw xtl::format_operation_exception (METHOD_NAME,"Path '%s' already exist",dir_name); break;
          case ENOENT: throw xtl::format_operation_exception (METHOD_NAME,"Path '%s' not found",dir_name); break;
          default:     throw xtl::format_operation_exception (METHOD_NAME, "Can't create directory '%s'. Reason: %s", dir_name, common::strerror (errno)); break;
        }
      }
    }

#if !defined ( ARM9 ) && !defined ( BADA )
    void Search (const char* full_mask,const FileSearchHandler& find_handler)
    {
      string dir_name = dir (full_mask), mask = full_mask + dir_name.size (), file_name;

      DIR* dir = opendir (dir_name.c_str ());

      if (!dir)
        return;

      if (dir_name == "./")
        dir_name = "";

      FileInfo info;

      for (struct dirent* entry; (entry = readdir (dir)) != 0;)
      {
        if (strcmp (entry->d_name, ".") && strcmp (entry->d_name, "..") && wcmatch (entry->d_name,mask.c_str ()))
        {
          file_name = dir_name + entry->d_name;

          StdioFileSystem::GetFileInfo (file_name.c_str (),info);

          find_handler (file_name.c_str (),info);
        }
      }

      closedir (dir);
    }
#endif

///Информация о файловой системе
    filesize_t GetFreeSpace (const char* path)
    {
      struct statvfs result;

      if (statvfs (path, &result))
        return (filesize_t)-1;

      return (filesize_t)result.f_frsize * result.f_bavail;
    }

    filesize_t GetTotalSpace (const char* path)
    {
      struct statvfs result;

      if (statvfs (path, &result))
        return (filesize_t)-1;

      return (filesize_t)result.f_frsize * result.f_blocks;
    }
};

}

/*
    Получение файловой системы
*/

ICustomFileSystem* UnistdPlatform::GetFileSystem ()
{
  return &*Singleton<UnistdFileSystem>::Instance ();
}
