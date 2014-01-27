#ifdef _MSC_VER

#include "shared.h"

#include <stdio.h>
#include <wchar.h>

using namespace common;
using namespace stl;

namespace
{

/*
    Утилиты
*/

inline bool TestFlag (size_t mask,size_t flag)
{
  return (mask & flag) == flag;
}

inline stl::wstring GetFullFileName (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::GetFullFileName", "file_name");

  return to_wstring_from_utf8 (file_name);
}

}

/*
    Операции работы с файлом
*/

Win32FileSystem::file_t Win32FileSystem::FileOpen (const char* file_name,filemode_t mode,size_t)
{
  wchar_t text_mode [8], *text_mode_pos = text_mode;
  
  if (TestFlag (mode,FileMode_Read|FileMode_Write)) 
  {
    *text_mode_pos++ = L'w';
    *text_mode_pos++ = L'+';
  }
  else if (TestFlag (mode,FileMode_Read))  *text_mode_pos++ = L'r';
  else if (TestFlag (mode,FileMode_Write))
  {
    if (TestFlag (mode,FileMode_Create)) *text_mode_pos++ = L'w';
    else                                 *text_mode_pos++ = L'a';
  }

  *text_mode_pos++ = L'b';  
  *text_mode_pos++ = L'\0';
  
  stl::wstring file_name_unicode = GetFullFileName (file_name);

  FILE* file = _wfopen (file_name_unicode.c_str (),text_mode);

  if (!file)
  {
    switch (errno)
    {
      case EACCES: throw xtl::format_operation_exception ("common::Win32FileSystem::FileOpen","Access denied to file '%S' mode=%s",file_name_unicode.c_str (),strfilemode (mode).c_str ()); break;
      case EMFILE: throw xtl::format_operation_exception ("common::Win32FileSystem::FileOpen","Unable to open file '%S'. Too many open files",file_name_unicode.c_str ()); break;
      case ENOENT: throw xtl::format_operation_exception ("common::Win32FileSystem::FileOpen","File '%s' not found",file_name); break;
      default:     throw xtl::format_operation_exception ("common::Win32FileSystem::FileOpen","Unable to open file '%S' mode=%s. Reason unknown",file_name_unicode.c_str (),strfilemode (mode).c_str ()); break;
    }
  }
  
  rewind (file);

  return (file_t)file;
}

/*
    Управление расположением файла
*/

void Win32FileSystem::Remove (const char* file_name)
{
  try
  {
    stl::wstring full_name = GetFullFileName (file_name);

    DWORD attrib = GetFileAttributesW (full_name.c_str ());

    if (attrib == INVALID_FILE_ATTRIBUTES)
       return;

    if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
      if (!RemoveDirectoryW (full_name.c_str ()))
        raise_error ("::RemoveDirectory");
    }
    else
    {
      if (!DeleteFileW (full_name.c_str ()))
        raise_error ("::DeleteFile ");
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32FileSystem::Remove");
    throw;
  }
}

void Win32FileSystem::Rename (const char* file_name,const char* new_name)
{
  stl::wstring file_name_unicode = GetFullFileName (file_name),
               new_name_unicode  = GetFullFileName (new_name);

  if (_wrename (file_name_unicode.c_str (),new_name_unicode.c_str ()))
  {
    switch (errno)
    {
      case EACCES: throw xtl::format_operation_exception ("common::Win32FileSystem::Rename","Access to file '%S' denied",file_name_unicode.c_str ()); break;
      case ENOENT: throw xtl::format_operation_exception ("common::Win32FileSystem::Rename","File '%S' not found",file_name_unicode.c_str ()); break;
      case EINVAL: throw xtl::format_operation_exception ("common::Win32FileSystem::Rename","Invalid name <old_name>='%S' <new_name>='%S'",file_name_unicode.c_str (),new_name_unicode.c_str ()); break;
      default:     throw xtl::format_operation_exception ("common::Win32FileSystem::Rename","Unknown error"); break;
    }
  }
}

void Win32FileSystem::Mkdir (const char* dir_name)
{
  stl::wstring dir_name_unicode = GetFullFileName (dir_name);

  if (_wmkdir (dir_name_unicode.c_str ()))
  {
    switch (errno)
    {
      case EEXIST: throw xtl::format_operation_exception ("common::Win32FileSystem::Mkdir","Path '%S' already exist",dir_name_unicode.c_str ()); break;
      case ENOENT: throw xtl::format_operation_exception ("common::Win32FileSystem::Mkdir","Path '%S' not found",dir_name_unicode.c_str ()); break;
      default:     throw xtl::format_operation_exception ("common::Win32FileSystem::Mkdir","Unknown error"); break;
    }
  }
}

/*
    Получение информации о файле
*/

bool Win32FileSystem::IsFileExist (const char* file_name)
{
  struct _stat32 s;
  
  stl::wstring file_name_unicode = GetFullFileName (file_name);  

  return _wstat32 (file_name_unicode.c_str (),&s) == 0;
}

bool Win32FileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  struct _stat32 s;
  
  stl::wstring file_name_unicode = GetFullFileName (file_name);    

  if (_wstat32 (file_name_unicode.c_str (),&s))
    return false;

  info.size         = s.st_size;
  info.time_modify  = s.st_mtime;
  info.time_access  = s.st_atime;
  info.time_create  = s.st_ctime;
  info.is_dir       = (s.st_mode & S_IFDIR) != 0;

  return true;
}

/*
   Информация о файловой системе
*/

filesize_t Win32FileSystem::GetFreeSpace (const char* path)
{
  ULARGE_INTEGER free_space;

  if (!GetDiskFreeSpaceExW (GetFullFileName (path).c_str (), &free_space, 0, 0))
     return (filesize_t)-1;

  return free_space.QuadPart;
}

filesize_t Win32FileSystem::GetTotalSpace (const char* path)
{
  ULARGE_INTEGER total_space;

  if (!GetDiskFreeSpaceExW (GetFullFileName (path).c_str (), 0, &total_space, 0))
     return (filesize_t)-1;

  return total_space.QuadPart;
}

/*
    Поиск файлов
*/

namespace
{

inline stl::wstring wdir (const stl::wstring& src)
{
  size_t len = src.size ();

  for (const wchar_t* s=src.c_str () + len;len--;)
    if (*--s == L'/')
      return wstring (src.c_str (), s-src.c_str () + 1);

  return L"./";
}

}

void Win32FileSystem::Search (const char* mask,const FileSearchHandler& insert_handler)
{
  _wfinddata32_t find_data;
  FileInfo       info;  

  stl::wstring mask_unicode = GetFullFileName (mask);    
  
  intptr_t search_handle = _wfindfirst32 (mask_unicode.c_str (),&find_data);

  if (search_handle == -1)
    return;

  stl::string dir_name = to_utf8_string (wdir (mask_unicode));
  
  if (dir_name == "./")
    dir_name = "";

  do
  {    
    stl::string name = to_utf8_string (find_data.name);

    if (name != "." && name != ".." && !(find_data.attrib & _A_HIDDEN))
    {
      info.size         = find_data.size;
      info.time_modify  = find_data.time_write;
      info.time_access  = find_data.time_access;
      info.time_create  = find_data.time_create;
      info.is_dir       = (find_data.attrib & _A_SUBDIR) != 0;
      
      if (!dir_name.empty ()) insert_handler (format ("%s%s",dir_name.c_str (),name.c_str ()).c_str (),info);
      else                    insert_handler (name.c_str (),info);
    }
  } while (_wfindnext32 (search_handle,&find_data) != -1);
  
  _findclose (search_handle);
}

#endif
