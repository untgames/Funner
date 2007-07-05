#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "shared.h"
#include <common/exception.h>
#include <common/strlib.h>

#ifdef _WIN32
  #include <io.h>
  #include <direct.h>
#endif

#ifdef _MSC_VER
  #pragma warning (disable : 4996) //declare deprecated
#endif

#ifdef __GNUC__
  #include <unistd.h>
#endif

using namespace common;
using namespace stl;

/*
    Утилиты
*/

inline bool TestFlag (size_t mask,size_t flag)
{
  return (mask & flag) == flag;
}

/*
    Операции работы с файлом
*/

StdioFileSystem::file_t StdioFileSystem::FileOpen (const char* file_name,filemode_t mode,size_t)
{
  char text_mode [8], *text_mode_pos = text_mode;
  
  if (TestFlag (mode,FILE_MODE_READ|FILE_MODE_WRITE)) 
  {
    *text_mode_pos++ = 'w';
    *text_mode_pos++ = '+';
  }
  else if (TestFlag (mode,FILE_MODE_READ))  *text_mode_pos++ = 'r';
  else if (TestFlag (mode,FILE_MODE_WRITE))
  {
    if (TestFlag (mode,FILE_MODE_CREATE)) *text_mode_pos++ = 'w';
    else                                  *text_mode_pos++ = 'a';
  }

  *text_mode_pos++ = 'b';
  *text_mode_pos++ = '\0';

  FILE* file = fopen (file_name,text_mode);

  if (!file)
  {
    switch (errno)
    {
      case EACCES: Raise<FileLoadException>     ("StdioFileSystem::FileOpen","Access denied to file '%s' mode=%s",file_name,strfilemode (mode).c_str ()); break;
      case EMFILE: Raise<FileLoadException>     ("StdioFileSystem::FileOpen","Unable to open file '%s'. Too many open files",file_name); break;
      case ENOENT: Raise<FileNotFoundException> ("StdioFileSystem::FileOpen","File '%s' not found",file_name); break;
      default:     Raise<FileLoadException>     ("StdioFileSystem::FileOpen","Unable to open fie '%s' mode=%s. Reason unknown",file_name,strfilemode (mode).c_str ()); break;
    }
  }
  
  rewind (file);

  return (file_t)file;
}

void StdioFileSystem::FileClose (file_t file)
{
  fclose ((FILE*)file);
}

size_t StdioFileSystem::FileRead (file_t file,void* buf,size_t size)
{
  clearerr ((FILE*)file);

  size_t read_size = fread (buf,1,size,(FILE*)file);
  
  if (ferror ((FILE*)file))
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileRead","file"); break;
      case EINVAL: RaiseNullArgument    ("StdioFileSystem::FileRead","buffer"); break;
      default:     Raise<FileException> ("StdioFileSystem::FileRead","Unknown error"); break;
    }
  }
  
  return (size_t)read_size;
}

size_t StdioFileSystem::FileWrite (file_t file,const void* buf,size_t size)
{
  size_t write_size = fwrite (buf,1,size,(FILE*)file);
  
  if (ferror ((FILE*)file))
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileWrite","file"); break;
      case EINVAL: RaiseNullArgument    ("StdioFileSystem::FileWrite","buffer"); break;
      case ENOSPC: Raise<FileNoSpaceException> ("StdioFileSystem::FileWrite","No enough space for write %u bytes to file",size); break;
      default:     Raise<FileException> ("StdioFileSystem::FileWrite","Unknown error"); break;
    }
  }
  
  return write_size;
}

void StdioFileSystem::FileRewind (file_t file)
{
  rewind ((FILE*)file);

  if (ferror ((FILE*)file))
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileRewind","file"); break;
      default:     Raise<FileException> ("StdioFileSystem::FileRewind","Unknown error"); break;
    }
  }
}

filepos_t StdioFileSystem::FileSeek (file_t file,filepos_t pos)
{
  if (fseek ((FILE*)file,pos,SEEK_SET))
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileSeek","file"); break;
      default:     Raise<FileException> ("StdioFileSystem::FileSeek","Unknown error"); break;
    }    
  }

  return pos;
}

filepos_t StdioFileSystem::FileTell (file_t file)
{
  filepos_t pos = (filepos_t)ftell ((FILE*)file);
  
  if (pos == -1)
  {
    switch (errno)
    {
      case EBADF: RaiseInvalidArgument ("StdioFileSystem::FileTell","file"); break;
      default:    Raise<FileException> ("StdioFileSystem::FileTell","Unknown error"); break;
    }    
  }

  return (filepos_t)pos;
}

filesize_t StdioFileSystem::FileSize (file_t file)
{
  struct stat s;

  if (fstat (fileno ((FILE*)file),&s))
  {
    switch (errno)
    {
      case EBADF: RaiseInvalidArgument ("StdioFileSystem::FileSize","file"); break;
      default:    Raise<FileException> ("StdioFileSystem::FileSize","Unknown error"); break;
    }
  }

  return s.st_size;
}

void StdioFileSystem::FileResize (file_t file,filesize_t new_size)
{
#ifdef _WIN32
  if (chsize (fileno ((FILE*)file),new_size) == -1)
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileResize","file"); break;
      case EINVAL: RaiseNullArgument    ("StdioFileSystem::FileResize","buffer"); break;
      case ENOSPC: Raise<FileNoSpaceException> ("StdioFileSystem::FileResize","No enough space for resize file up to %u bytes",new_size); break;
      default:     Raise<FileException> ("StdioFileSystem::FileResize","Unknown error"); break;
    }
  }    
#else
  Raise<PlatformNotSupportedException> ("StdioFileSystem::FileResize","There is no default implementation for this operation");
#endif  
}

bool StdioFileSystem::FileEof (file_t file)
{
  return feof ((FILE*)file) != 0;
}

void StdioFileSystem::FileFlush (file_t file)
{
  if (fflush ((FILE*)file))
  {
    switch (errno)
    {
      case EBADF:  RaiseInvalidArgument ("StdioFileSystem::FileFlush","file",(int)file,"Invalid file descriptor"); break;
      case ENOSPC: Raise<FileNoSpaceException> ("StdioFileSystem::FileFlush","Unable to flush file buffers due to write failure"); break;
      default:     Raise<FileException> ("StdioFileSystem::FileFlush","Unknown error"); break;
    }
  }
}

/*
    Управление расположением файла
*/

void StdioFileSystem::Remove (const char* file_name)
{
  if (unlink (file_name))
  {
    switch (errno)
    {
      case EACCES: Raise<FileException>         ("StdioFileSystem::Remove","Access to file '%s' denied",file_name); break;
      case ENOENT: Raise<FileNotFoundException> ("StdioFileSystem::Remove","File '%s' not found",file_name); break;
      default:     Raise<FileException>         ("StdioFileSystem::Remove","Unknown error"); break;
    }
  }
}

void StdioFileSystem::Rename (const char* file_name,const char* new_name)
{
  if (rename (file_name,new_name))
  {
    switch (errno)
    {
      case EACCES: Raise<FileException>         ("StdioFileSystem::Rename","Access to file '%s' denied",file_name); break;
      case ENOENT: Raise<FileNotFoundException> ("StdioFileSystem::Rename","File '%s' not found",file_name); break;
      case EINVAL: Raise<FileException>         ("StdioFileSystem::Rename","Invalid name <old_name>='%s' <new_name>='%s'",file_name,new_name); break;
      default:     Raise<FileException>         ("StdioFileSystem::Rename","Unknown error"); break;
    }
  }
}

void StdioFileSystem::Mkdir (const char* dir_name)
{
#ifdef _WIN32
  if (mkdir (dir_name))
  {
    switch (errno)
    {
      case EEXIST: Raise<FileException> ("StdioFileSystem::Mkdir","Path '%s' already exist",dir_name); break;
      case ENOENT: Raise<FileNotFoundException> ("StdioFileSystem::Mkdir","Path '%s' not found",dir_name); break;
      default:     Raise<FileException> ("StdioFileSystem::Mkdir","Unknown error"); break;
    }
  }
#else
  Raise<PlatformNotSupportedException> ("StdioFileSystem::Mkdir","There is no default implementation for this operation");
#endif  
}

/*
    Получение информации о файле
*/

bool StdioFileSystem::IsFileExist (const char* file_name)
{
  struct stat s;

  return !stat (file_name,&s);  
}

bool StdioFileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  struct stat s;

  if (stat (file_name,&s))
    return false;

  info.size         = s.st_size;
  info.time_modify  = s.st_mtime;
  info.time_access  = s.st_atime;
  info.time_create  = s.st_ctime;
  info.is_dir       = (s.st_mode & S_IFDIR) != 0;

  return true;
}

/*
    Поиск файлов
*/

#ifdef _WIN32

void StdioFileSystem::Search (const char* mask,const FileSearchHandler& insert_handler)
{
  _finddata_t find_data;
  FileInfo    info;  

  intptr_t search_handle = _findfirst (mask,&find_data);

  if (search_handle == -1)
    return;

  string dir_name = dir (mask);
  
  if (dir_name == "./")
    dir_name = "";

  do
  {    
    if (strcmp (find_data.name,".") && strcmp (find_data.name,"..") && !(find_data.attrib & _A_HIDDEN))
    {
      info.size         = find_data.size;
      info.time_modify  = find_data.time_write;
      info.time_access  = find_data.time_access;
      info.time_create  = find_data.time_create;
      info.is_dir       = (find_data.attrib & _A_SUBDIR) != 0;
      
      if (!dir_name.empty ()) insert_handler (format ("%s%s",dir_name.c_str (),find_data.name).c_str (),info);
      else                    insert_handler (find_data.name,info);
    }
  } while (_findnext (search_handle,&find_data) != -1);
  
  _findclose (search_handle);
}

#else

void StdioFileSystem::Search (const char* mask,const FileSearchHandler& handler)
{
  Raise<PlatformNotSupportedException> ("StdioFileSystem::Search","There is no default implementation for this operation");
}

#endif
