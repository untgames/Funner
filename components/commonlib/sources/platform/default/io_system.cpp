#include <stdio.h>
#include <common/exception.h>
#include <common/strlib.h>
#include <xtl/function.h>

#include "shared.h"

using namespace common;

/*
    Stdio files entries
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� ��⮪����� 䠩�� �⠭���⭮� ������⥪�
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StdioFile
{
  const char* name;   //��� � StdioIOSystem
  FILE*       stream; //��⮪
  filemode_t  mode;   //०�� ࠡ���
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��⮪��� 䠩�� �⠭���⭮� ������⥪�
///////////////////////////////////////////////////////////////////////////////////////////////////
static StdioFile stdio_files [] = {
  {"stdout",stdout,FILE_MODE_WRITE},
  {"stderr",stderr,FILE_MODE_WRITE},
  {"stdin",stdin,FILE_MODE_READ}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������⢮ ��⮪���� 䠩��� �⠭���⭮� ������⥪�
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t STDIO_FILES_NUM = sizeof (stdio_files)/sizeof (StdioFile);

/*
    StdioIOSystem
*/

StdioIOSystem::file_t StdioIOSystem::FileOpen (const char* file_name,filemode_t mode,size_t)
{
  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (!stricmp (file_name,stdio_file->name))
    {
      if (stdio_file->mode & ~mode)
      {
        Raise<FileLoadException> ("StdioIOSystem::FileOpen","Wrong <file_mode>='%s' at open '%s' stream",
                                  strfilemode (mode).c_str (),file_name);
      }

      return (file_t)stdio_file->stream;
    }
  
  Raise<FileNotFoundException> ("StdioIOSystem::FileOpen","File '%s' not found",file_name);
    
  return (file_t)NULL;
}

bool StdioIOSystem::IsFileExist (const char* file_name)
{
  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (!(stricmp (stdio_file->name,file_name)))
      return true;

  return false;
}

void StdioIOSystem::Search (const char* wc_mask,const FileSearchHandler& handler)
{
  FileInfo info;
  
  memset (&info,0,sizeof (info));

  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (!(wcimatch (wc_mask,stdio_file->name)))
      handler (stdio_file->name,info);  
}

bool StdioIOSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  StdioFile* stdio_file = stdio_files;

  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (!(stricmp (stdio_file->name,file_name)))
    {
      memset (&info,0,sizeof (info));
      return true;
    }

  return false;
}

void StdioIOSystem::Remove (const char* file_name)
{
  RaiseNotSupported ("StdioIOSystem::Remove","File '%s' can not be removed",file_name);
}

void StdioIOSystem::Rename (const char* file_name,const char*)
{
  RaiseNotSupported ("StdioIOSystem::Rename","File '%s' can not be renamed",file_name);
}

void StdioIOSystem::Mkdir (const char* dir_name)
{
  RaiseNotSupported ("StdioIOSystem::Mkdir","Unable to make dir '%s'. This file system is static",dir_name);
}
