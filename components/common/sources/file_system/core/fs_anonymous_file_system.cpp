#include "shared.h"

using namespace common;

AnonymousFileSystem::AnonymousFileSystem ()
{ 
}

AnonymousFileSystem::~AnonymousFileSystem ()
{
  RemoveAllFilePaths ();
}

FileImplPtr AnonymousFileSystem::RetainFile (const char* path, filemode_t file_mode)
{
  static const char* METHOD_NAME = "common::AnonymousFileSystem::RetainFile";

  if (!path)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path");
    
  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == path)
    {
      FileImplPtr file     = iter->file;
      size_t      xor_mask = file->Mode () ^ file_mode;
      
      if ((xor_mask & file->Mode ()) != xor_mask)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't open file '%s'. Wrong file mode", path);
      
      return file;
    }

  throw xtl::format_operation_exception (METHOD_NAME, "File '%s' not found", path);
}

AnonymousFileSystem::file_t AnonymousFileSystem::FileOpen (const char* file_name, filemode_t, size_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileOpen", "Unable to open file '%s'", file_name);
}

void AnonymousFileSystem::FileClose (file_t)
{
}

size_t AnonymousFileSystem::FileRead (file_t, void*, size_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileRead");
}

size_t AnonymousFileSystem::FileWrite (file_t, const void* buf, size_t size)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileWrite");
}

void AnonymousFileSystem::FileRewind (file_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileRewind");
}

filepos_t AnonymousFileSystem::FileSeek (file_t, filepos_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileSeek");
}

filepos_t AnonymousFileSystem::FileTell (file_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileTell");
}

filesize_t AnonymousFileSystem::FileSize (file_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileSize");
}

void AnonymousFileSystem::FileResize (file_t, filesize_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileResize");
}

bool AnonymousFileSystem::FileEof (file_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileEof");
}

void AnonymousFileSystem::FileFlush (file_t)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::FileFlush");
}

void AnonymousFileSystem::Remove (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::AnonymousFileSystem::Remove", "file_name");

  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == file_name)
    {
      iter->file->ResetPath ();

      anonymous_files.erase (iter);

      return;
    }
}

void AnonymousFileSystem::Rename (const char* file_name, const char* new_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::AnonymousFileSystem::Rename", "file_name");

  if (!new_name)
    throw xtl::make_null_argument_exception ("common::AnonymousFileSystem::Rename", "new_name");
    
  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == new_name)
      throw xtl::format_operation_exception ("common::AnonymousFileSystem::Rename", "Destination file '%s' already exist", new_name);

  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == file_name)
    {
      stl::string new_name_copied = new_name;
      
      iter->path.swap (new_name_copied);
      
      return;
    }

  throw xtl::format_operation_exception ("common::AnonymousFileSystem::Rename", "File '%s' not found", file_name);
}

void AnonymousFileSystem::Mkdir (const char*)
{
  throw xtl::format_not_supported_exception ("common::AnonymousFileSystem::Mkdir");
}

bool AnonymousFileSystem::IsFileExist (const char* file_name)
{
  if (!file_name)
    return false;
    
  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == file_name)
      return true;

  return false;
}

bool AnonymousFileSystem::GetFileInfo (const char* file_name, FileInfo& info)
{
  if (!file_name)
    return false;
    
  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->path == file_name)
    {
      info.time_create = 0;
      info.time_access = 0;
      info.time_modify = 0;
      info.size        = iter->file->Size ();
      info.is_dir      = false;

      return true;
    }

  return false;
}

void AnonymousFileSystem::Search (const char* mask, const FileSearchHandler& insert_handler)
{
  try
  {
    if (!mask)
      throw xtl::make_null_argument_exception ("", "mask");

    for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
      if (wcimatch (iter->path.c_str (), mask))
      {
        FileInfo info;
        
        info.time_create = 0;
        info.time_access = 0;
        info.time_modify = 0;
        info.size        = iter->file->Size ();
        info.is_dir      = false;        
        
        insert_handler (iter->path.c_str (), info);
      }
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::AnonymousFileSystem::Search");
    throw;
  }
}

const char* AnonymousFileSystem::AddFilePath (const FileImplPtr& file)
{
  for (AnonymousFileList::iterator iter=anonymous_files.begin (),  end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->file == file)
      return iter->path.c_str ();

  stl::string path = common::format ("%p", file.get ());

  anonymous_files.push_back (AnonymousFile (path.c_str (), file.get ()));

  return anonymous_files.back ().path.c_str ();
}

void AnonymousFileSystem::RemoveFilePath (const FileImplPtr& file)
{
  for (AnonymousFileList::iterator iter=anonymous_files.begin (),   end=anonymous_files.end (); iter!=end; ++iter)
    if (iter->file == file)
    {
      iter->file->ResetPath ();
      
      anonymous_files.erase (iter);

      return;
    }
}

void AnonymousFileSystem::RemoveAllFilePaths ()
{
  for (AnonymousFileList::iterator iter=anonymous_files.begin (), end=anonymous_files.end (); iter!=end; ++iter)
    iter->file->ResetPath ();

  anonymous_files.clear ();
}

void AnonymousFileSystem::AddRef ()
{
  addref (this);
}

void AnonymousFileSystem::Release ()
{
  release (this);
}
