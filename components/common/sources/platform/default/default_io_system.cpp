#include "shared.h"

using namespace common;

/*
    Stdio files entries
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание потокового файла стандартной библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StdioFile
{
  const char* name;     //имя в StdioIOSystem
  const char* dir_name; //имя каталога
  FILE*       stream;   //поток
  filemode_t  mode;     //режим работы
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Потоковые файлы стандартной библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
static StdioFile stdio_files [] = {
  {"stdout","stdout/",stdout,FileMode_Write},
  {"stderr","stderr/",stderr,FileMode_Write},
  {"stdin","stdin/",stdin,FileMode_Read}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество потоковых файлов стандартной библиотеки
//////////////////////////////////////////////,/////////////////////////////////////////////////////
const size_t STDIO_FILES_NUM = sizeof (stdio_files)/sizeof (StdioFile);

/*
    StdioIOSystem
*/

namespace
{

//проверка существования файла в домене
bool is_file_exist (const char* file_name,StdioFile& file)
{
  return !xtl::xstricmp (file.name,file_name) || !xtl::xstrnicmp (file.dir_name,file_name,strlen (file.dir_name));
}

}

StdioIOSystem::file_t StdioIOSystem::FileOpen (const char* file_name,filemode_t mode,size_t)
{
  StdioFile* stdio_file = stdio_files;

  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (is_file_exist (file_name,*stdio_file))
    {
      if (stdio_file->mode & ~mode)
      {
        throw xtl::format_exception<FileLoadException> ("common::StdioIOSystem::FileOpen","Wrong <file_mode>='%s' at open '%s' stream",
                                  strfilemode (mode).c_str (),file_name);
      }

      return (file_t)stdio_file->stream;
    }

  throw xtl::format_exception<FileNotFoundException> ("common::StdioIOSystem::FileOpen","File '%s' not found",file_name);
}

void StdioIOSystem::FileClose (file_t)
{
}

bool StdioIOSystem::IsFileExist (const char* file_name)
{
  StdioFile* stdio_file = stdio_files;

  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (is_file_exist (file_name,*stdio_file))
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
    if (is_file_exist (file_name,*stdio_file))
    {
      memset (&info,0,sizeof (info));
      return true;
    }

  return false;
}

void StdioIOSystem::Remove (const char* file_name)
{
  throw xtl::format_not_supported_exception ("common::StdioIOSystem::Remove","File '%s' can not be removed",file_name);
}

void StdioIOSystem::Rename (const char* file_name,const char*)
{
  throw xtl::format_not_supported_exception ("common::StdioIOSystem::Rename","File '%s' can not be renamed",file_name);
}

void StdioIOSystem::Mkdir (const char* dir_name)
{
  throw xtl::format_not_supported_exception ("common::StdioIOSystem::Mkdir","Unable to make dir '%s'. This file system is static",dir_name);
}
