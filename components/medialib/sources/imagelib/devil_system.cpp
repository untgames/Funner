#include <common/heap.h>
#include <common/file.h>
#include <il/il.h>
#include <il/ilu.h>
#include "shared.h"
#include "devil_system.h"

using namespace common;

namespace 
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///‘ункции работы с пам€тью и файлами, замен€ющии стандартные DevIL функции
///////////////////////////////////////////////////////////////////////////////////////////////////
ILvoid* ILAPIENTRY CustomAllocate (ILuint size)
{
  void* ret_value = NULL;

  try
  {
    ret_value = MemoryManager::Allocate (size);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomAllocate.");
  }

  return ret_value;
}

ILvoid ILAPIENTRY CustomDeallocate (ILvoid* ptr)
{
  try
  {
    MemoryManager::Deallocate (ptr);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomDeallocate.");
  }
}

ILvoid ILAPIENTRY CustomFileClose (ILHANDLE file_ptr)
{
  try
  {
    delete static_cast<StdFile*> (file_ptr);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomFileClose.");
  }
}

ILboolean ILAPIENTRY CustomFileEof (ILHANDLE file_ptr)
{
  ILboolean ret_value = true;

  try
  {
    ret_value = ((StdFile*)file_ptr)->Eof ();
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomFileEof.");
  }

  return ret_value;
}

ILint ILAPIENTRY CustomReadFileGetc (ILHANDLE file_ptr)
{
  char ret_value;

  try
  {
    ((StdFile*)file_ptr)->Read (&ret_value, sizeof (char));
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomReadFileGetc.");
  }

  return ret_value;
}

ILHANDLE ILAPIENTRY CustomReadFileOpen (const ILstring file_name)
{
  StdFile* ret_value = NULL;

  try
  {
    ret_value = new StdFile (file_name, FILE_MODE_READ_ONLY);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomReadFileOpen.");
  }

  return ret_value;
}

ILint ILAPIENTRY CustomReadFileRead (void* data, ILuint size, ILuint count, ILHANDLE file_ptr)
{
  ILint ret_value = 0;

  if (size == 0 || count == 0)
    return 0;

  try
  {
    ret_value = ((StdFile*)file_ptr)->Read (data, size * count);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomReadFileRead.");
  }

  return ret_value / size;
}

ILint ILAPIENTRY CustomFileSeek (ILHANDLE file_ptr, ILint offset, ILint origin)
{
  ILint ret_value = 1;
  FileSeekMode seek_mode[3] = {FILE_SEEK_SET, FILE_SEEK_CUR, FILE_SEEK_END};

  try
  {
     int cur_pos = ((StdFile*)file_ptr)->Tell ();
     cur_pos == ((StdFile*)file_ptr)->Seek ((filepos_t)offset, seek_mode[origin]) ? ret_value = 0 : ret_value = 1;
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomFileSeek.");
  }

  return ret_value;
}

ILint ILAPIENTRY CustomFileTell (ILHANDLE file_ptr)
{
  ILint ret_value;

  try
  {
     ret_value = ((StdFile*)file_ptr)->Tell ();
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomFileTell.");
  }

  return ret_value;
}

ILHANDLE ILAPIENTRY CustomWriteFileOpen (const ILstring file_name)
{
  StdFile* ret_value = NULL;

  try
  {
    size_t folder_name_index = GetFolder (file_name);

    if (folder_name_index)
    {
      stl::string folder_name (file_name, folder_name_index);

      if (!FileSystem::IsDir (folder_name.c_str ()))
        FileSystem::Mkdir (folder_name.c_str ());
    }

    ret_value = new StdFile (file_name, FILE_MODE_WRITE_ONLY);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomWriteFileOpen.");
  }

  return ret_value;
}

ILint ILAPIENTRY CustomWriteFilePutc (ILubyte buffer, ILHANDLE file_ptr)
{
  char ret_value = 0;

  try
  {
    (((StdFile*)file_ptr)->Write (&buffer, sizeof (ILubyte)) == sizeof (ILubyte)) ? ret_value = buffer : ret_value = EOF;
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomWriteFilePutc.");
  }

  return ret_value;
}

ILint ILAPIENTRY CustomWriteFileWrite (const void* buffer, ILuint size, ILuint count, ILHANDLE file_ptr)
{
  ILint ret_value = 0;

  if (size == 0 || count == 0)
    return 0;

  try
  {
    ret_value = ((StdFile*)file_ptr)->Write (buffer, size * count);
  }
  catch (std::exception& exception)
  {
    ImageSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    ImageSystemSingleton::Instance ().DebugLog ("Unknown exception at CustomWriteFileWrite.");
  }

  return ret_value / size;
}

}

DevILSystemImpl::DevILSystemImpl ()
{
  ilInit       ();
  iluInit      ();
  ilEnable     (IL_FILE_OVERWRITE);
  ilEnable     (IL_ORIGIN_SET);
  ilOriginFunc (IL_ORIGIN_UPPER_LEFT);

  ilSetMemory (CustomAllocate, CustomDeallocate);

  FileSystem::SetDefaultFileBufferSize (0);                                               //!!!!¬ременный хак!!!!

  ilSetRead   (CustomReadFileOpen,  CustomFileClose, CustomFileEof, CustomReadFileGetc, 
               CustomReadFileRead,  CustomFileSeek,  CustomFileTell);
  ilSetWrite  (CustomWriteFileOpen, CustomFileClose, CustomWriteFilePutc, 
               CustomFileSeek,      CustomFileTell,  CustomWriteFileWrite);
}

DevILSystemImpl::~DevILSystemImpl ()
{
  ilShutDown     ();
}
