#include "shared.h"
#include <stdio.h>

#include <platform/default/shared.h>

using namespace common;
using namespace stl;

/*
    Операции работы с файлом
*/

Win32FileSystem::file_t Win32FileSystem::FileOpen (const char* file_name,filemode_t mode,size_t size)
{
printf("----------------%s\n",file_name);
  OFSTRUCT ofstruct;
  HFILE hFile;
  UINT style;

  if(mode&FileMode_Read)
    style=OF_READ;
  if(mode&FileMode_Write)
    style=OF_WRITE;
  if(mode&(FileMode_Read | FileMode_Write))
    style=OF_READWRITE;

  if(mode&FileMode_Create)
    style|=OF_CREATE;

  if((mode&FileMode_Write)&&(!IsFileExist (file_name)))
    style|=OF_CREATE;

  try
  {
    hFile=OpenFile(file_name,&ofstruct,style);
    if (hFile==HFILE_ERROR)
      raise_error ("::OpenFile");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileOpen");
    throw;
  }

  return (file_t)hFile;
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

size_t Win32FileSystem::FileRead (file_t file,void* buf,size_t size)
{
  try
  {
    DWORD len;

    if (!ReadFile (file,buf,size,&len,NULL))
      raise_error ("::ReadFile");
    return len;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileRead");
    throw;
  }
}

size_t Win32FileSystem::FileWrite (file_t file,const void* buf,size_t size)
{
  try
  {
    DWORD len;

    if (!WriteFile (file,buf,size,&len,NULL) )
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

filepos_t Win32FileSystem::FileSeek (file_t file,filepos_t pos)
{
  try
  {
    DWORD dwLow;
    DWORD dwHigh = 0;

    dwLow = SetFilePointer(file, pos, NULL, FILE_BEGIN);
    if (dwLow == INVALID_SET_FILE_POINTER)
      raise_error ("::SetFilePointer");

    return dwLow;
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
    DWORD dwLow;
    DWORD dwHigh = 0;

    dwLow = SetFilePointer(file, 0, NULL, FILE_CURRENT);
    if (dwLow == INVALID_SET_FILE_POINTER)
      raise_error ("::SetFilePointer");
  
    return dwLow;
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
    DWORD size;
  
    size=GetFileSize(file,NULL);
    if (size==INVALID_FILE_SIZE)
      raise_error ("::GetFileAttributes");
 
    return size;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileSize");
    throw;
  }
}

void Win32FileSystem::FileResize (file_t file,filesize_t new_size)
{
  filepos_t lastPos=FileTell(file);
  FileSeek(file,new_size);
  
  try
  {
    if (!SetEndOfFile (file) )
      raise_error ("::SetEndOfFile");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::FileResize");
    throw;
  }
  FileSeek(file,lastPos);
}

bool Win32FileSystem::FileEof (file_t file)
{
  return (FileTell(file) == FileSize(file));
}

void Win32FileSystem::FileFlush (file_t file)
{
  try
  {
    if (!FlushFileBuffers  (file) )
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
    if (!DeleteFile  (file_name) )
      raise_error ("::DeleteFile ");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::Remove");
    throw;
  }
}

void Win32FileSystem::Rename (const char* file_name,const char* new_name)
{
  try
  {
    if (!MoveFile  (file_name, new_name) )
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
    if (!CreateDirectory   (dir_name, NULL) )
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
  try
  {
    DWORD attrib;

    attrib=GetFileAttributes(file_name);
    if (attrib==INVALID_FILE_ATTRIBUTES)
      return false;
    else
      return true;
//      raise_error ("::GetFileAttributes");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::IsFileExist");
    throw;
  }
}

inline long long MakeInt64(FILETIME ft)
{
  return ((long long)ft.dwHighDateTime << 64) + ft.dwLowDateTime;
}

bool Win32FileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  DWORD attrib;
  try
  {
    attrib=GetFileAttributes(file_name);
    if (attrib == INVALID_FILE_ATTRIBUTES)
      return false;
    info.is_dir=((attrib&FILE_ATTRIBUTE_DIRECTORY)!=0);
 
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if(!GetFileAttributesEx(file_name,GetFileExInfoStandard,&file_info) )
       return false;

    info.time_create = MakeInt64(file_info.ftCreationTime);
    info.time_access = MakeInt64(file_info.ftLastAccessTime);
    info.time_modify = MakeInt64(file_info.ftLastWriteTime);
    info.size = file_info.nFileSizeLow;
    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::GetFileInfo");
    throw;
  }
}

/*
    Поиск файлов
*/

void FindDataToFileInfo (LPWIN32_FIND_DATA lpFindFileData, FileInfo *info)
{
  info->is_dir=((lpFindFileData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0);

  info->time_create = MakeInt64(lpFindFileData->ftCreationTime);
  info->time_access = MakeInt64(lpFindFileData->ftLastAccessTime);
  info->time_modify = MakeInt64(lpFindFileData->ftLastWriteTime);
  info->size = lpFindFileData->nFileSizeLow;
}

void Win32FileSystem::Search (const char* mask,const FileSearchHandler& handler)
{
  throw xtl::format_exception<xtl::bad_platform> ("common::Win32FileSystem::Search","Not implementated");

  WIN32_FIND_DATA FindFileData;
  FileInfo info;
  
  HANDLE handle = FindFirstFile(mask,&FindFileData);
  if (handle == INVALID_HANDLE_VALUE)
    return;

  FindDataToFileInfo(&FindFileData, &info);

  handler(FindFileData.cFileName, info);
//  handler("aksdjalksdjaskldjalksd", info);
                                  
  while(!FindNextFile (handle,&FindFileData) )
  {
    FindDataToFileInfo(&FindFileData, &info);
    handler(FindFileData.cFileName, info);
  }
  CloseHandle( handle );
}
