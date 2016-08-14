#ifndef _MSC_VER

#include "shared.h"

using namespace common;
using namespace stl;

/*
    Операции работы с файлом
*/

Win32FileSystem::Win32FileSystem ()
{
  memset (path_prefix,0,sizeof(path_prefix));
  
#ifdef WINCE
  char filename[MAX_PATH*sizeof(wchar_t)];
  
  sprintf (filename,"%s.env",__argv[0]);

  DWORD  len=0;
  HANDLE currentdir_file = CreateFileW (towstring (filename).c_str (), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
  
  memset (filename,0,sizeof(filename));
  
  ReadFile (currentdir_file,filename,MAX_PATH*sizeof(wchar_t),&len,NULL);

  CloseHandle (currentdir_file);

  PropertyMap pmap = parse_init_string (tostring ((wchar_t*)filename).c_str());

  strcpy (path_prefix,pmap.GetString("CurrentDir"));
#endif
}

stl::wstring Win32FileSystem::GetFullFileName(const char* file_name)
{
#ifdef WINCE
  if(file_name[0]=='\\')
    return to_wstring_from_utf8 (file_name);

  return to_wstring_from_utf8 (format ("%s\\%s",path_prefix,file_name));
#else
  return to_wstring_from_utf8 (file_name);
#endif
}

Win32FileSystem::file_t Win32FileSystem::FileOpen (const char* file_name, filemode_t mode, size_t size)
{
  try
  {
    UINT style = 0; //OF_SHARE_DENY_NONE;

    if ((mode & (FileMode_Read | FileMode_Write)) == (FileMode_Read | FileMode_Write))
      style |= GENERIC_READ | GENERIC_WRITE;
    else
    {
      if (mode & FileMode_Read) style |= GENERIC_READ;
      if (mode & FileMode_Write) style |= GENERIC_WRITE;
    }

    UINT dwCreationDisposition = 0;
    if ((mode & FileMode_Write) && (!Win32FileSystem::IsFileExist (file_name)))
      dwCreationDisposition |= CREATE_ALWAYS;
   
    if (mode & FileMode_Create)
      dwCreationDisposition |= CREATE_ALWAYS;

    if (dwCreationDisposition==0)
      dwCreationDisposition = OPEN_EXISTING;

    HANDLE file = CreateFileW (GetFullFileName (file_name).c_str (), style, FILE_SHARE_READ ,NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE)
      raise_error ("::OpenFile");      

    return (file_t)file;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileOpen('%s',%s)", file_name, strfilemode (mode).c_str ());
    throw;
  }
}

void Win32FileSystem::FileClose (file_t file)
{
  try
  {    
    if (!CloseHandle (file))
      raise_error ("::CloseHandle");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileClose");
    throw;
  }
}

size_t Win32FileSystem::FileRead (file_t file, void* buf, size_t size)
{
  try
  {
    DWORD len;

    if (!ReadFile (file, buf, size, &len, 0))
      raise_error ("::ReadFile");

    return len;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileRead");
    throw;
  }
}

size_t Win32FileSystem::FileWrite (file_t file, const void* buf, size_t size)
{
  try
  {
    DWORD len;

    if (!WriteFile (file, buf, size, &len, 0))
      raise_error ("::WriteFile");

    return len;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileWrite");
    throw;
  }
}

void Win32FileSystem::FileRewind (file_t file)
{
  FileSeek (file, 0);
}

filepos_t Win32FileSystem::FileSeek (file_t file, filepos_t pos)
{
  try
  {
    DWORD position_low = SetFilePointer (file, pos, 0, FILE_BEGIN);

    if (position_low == INVALID_SET_FILE_POINTER)
      raise_error ("::SetFilePointer");

    return position_low;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileSeek");
    throw;
  }
}

filepos_t Win32FileSystem::FileTell (file_t file)
{
  try
  {
    DWORD position_low = SetFilePointer (file, 0, 0, FILE_CURRENT);

    if (position_low == INVALID_SET_FILE_POINTER)
      raise_error ("::SetFilePointer");

    return position_low;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileTell");
    throw;
  }
}

filesize_t Win32FileSystem::FileSize (file_t file)
{
  try
  {
    DWORD size = GetFileSize (file, 0);

    if (size == INVALID_FILE_SIZE)
      raise_error ("::GetFileAttributes");

    return size;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileSize");
    throw;
  }
}

void Win32FileSystem::FileResize (file_t file, filesize_t new_size)
{ 
  try
  {
    filepos_t last_pos = Win32FileSystem::FileTell (file);

#ifdef WINCE

    filepos_t seek_result=Win32FileSystem::FileSeek (file, new_size-1); 
    
    if (seek_result != new_size-1)
      throw xtl::format_operation_exception ("", "Internal error: FileSeek failed.");

    BOOL result = SetEndOfFile (file);

    int    buffer     = 0;
    size_t write_size = Win32FileSystem::FileWrite (file, &buffer, 1);

    if (write_size!=1)
      throw xtl::format_operation_exception ("", "Internal error: FileWrite failed.");

#else

    Win32FileSystem::FileSeek (file, new_size); 
    BOOL result = SetEndOfFile (file);

#endif

    Win32FileSystem::FileSeek (file, last_pos);

    if (!result)
      raise_error ("::SetEndOfFile");      
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileResize");
    throw;
  }
}

bool Win32FileSystem::FileEof (file_t file)
{
  return Win32FileSystem::FileTell (file) == (filepos_t)Win32FileSystem::FileSize (file);
}

void Win32FileSystem::FileFlush (file_t file)
{
  try
  {
    if (!FlushFileBuffers (file))
      raise_error ("::FlushFileBuffers ");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileFlush");
    throw;
  }
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
    exception.touch ("common::Win32Platform::Remove");
    throw;
  }
}

void Win32FileSystem::Rename (const char* file_name, const char* new_name)
{
  try
  {
    if (!MoveFileW (GetFullFileName (file_name).c_str (), GetFullFileName (new_name).c_str ()))
      raise_error ("::MoveFile ");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::Rename");
    throw;
  }
}

void Win32FileSystem::Mkdir (const char* dir_name)
{
  try
  {
    if (!CreateDirectoryW (GetFullFileName (dir_name).c_str (), 0))
      raise_error ("::CreateDirectory ");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::Mkdir");
    throw;
  }
}

/*
    Получение информации о файле
*/

bool Win32FileSystem::IsFileExist (const char* file_name)
{
  return GetFileAttributesW ( GetFullFileName (file_name).c_str () ) != INVALID_FILE_ATTRIBUTES;
}

namespace
{

inline filetime_t make_time (FILETIME ft)
{
  return (((filetime_t)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
}

}

bool Win32FileSystem::GetFileInfo (const char* file_name, FileInfo& info)
{
  try
  {
    DWORD attrib = GetFileAttributesW ( GetFullFileName (file_name).c_str () );

    if (attrib == INVALID_FILE_ATTRIBUTES)
       return false;

    info.is_dir = (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
 
    WIN32_FILE_ATTRIBUTE_DATA file_info;

    if (!GetFileAttributesExW ( GetFullFileName (file_name).c_str (), GetFileExInfoStandard, &file_info))
       return false;

    info.time_create = make_time (file_info.ftCreationTime);
    info.time_access = make_time (file_info.ftLastAccessTime);
    info.time_modify = make_time (file_info.ftLastWriteTime);
    info.size        = file_info.nFileSizeLow;

    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::GetFileInfo");
    throw;
  }
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

void Win32FileSystem::Search (const char* mask, const FileSearchHandler& handler)
{
  WIN32_FIND_DATAW find_file_data;
  
  memset (&find_file_data, 0, sizeof (find_file_data));

  FileInfo info;

  HANDLE handle = FindFirstFileW (GetFullFileName (mask).c_str(), &find_file_data);

  if (handle == INVALID_HANDLE_VALUE)
    return;
    
  string dir_name = dir (mask);
  
  if (dir_name == "./")
    dir_name = "";    

  do
  {
    if (wcscmp ( find_file_data.cFileName, L".") && wcscmp ( find_file_data.cFileName, L".."))
    {
      info.is_dir      = (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
      info.time_create = make_time (find_file_data.ftCreationTime);
      info.time_access = make_time (find_file_data.ftLastAccessTime);
      info.time_modify = make_time (find_file_data.ftLastWriteTime);
      info.size        = find_file_data.nFileSizeLow;

      if (!dir_name.empty ()) handler (format ("%s%s", dir_name.c_str (), tostring (find_file_data.cFileName).c_str ()).c_str (), info);
      else                    handler (tostring (find_file_data.cFileName).c_str (),info);
    }
  } while (FindNextFileW (handle, &find_file_data));

  CloseHandle (handle);
}

#endif
