#include "shared.h"

using namespace common;

namespace 
{

/*
    Протоколирование исключения
*/

void log_exception (const char* source, std::exception& exception)
{
  media::ImageSystemSingleton::Instance ().Printf ("Exception at %s: %s", source, exception.what ());
}

void log_exception (const char* source)
{
  media::ImageSystemSingleton::Instance ().Printf ("Unknown exception at %s", source);
}

/*
    Функции работы с памятью и файлами, заменяющии стандартные DevIL функции
*/

ILvoid* ILAPIENTRY devil_allocate (ILuint size)
{
  try
  {
    return MemoryManager::Allocate (size);
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_allocate", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_allocate");
  }
  
  return 0;
}

ILvoid ILAPIENTRY devil_deallocate (ILvoid* ptr)
{
  try
  {
    MemoryManager::Deallocate (ptr);
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_deallocate", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_deallocate");
  }  
}

ILHANDLE ILAPIENTRY devil_file_open_read_only (const ILstring file_name)
{
  try
  {
    return new StdFile (file_name, FILE_MODE_READ_ONLY);
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_open_read_only", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_open_read_only");
  }
  
  return 0;
}

ILHANDLE ILAPIENTRY devil_file_open_write_only (const ILstring file_name)
{
  try
  {
    return new StdFile (file_name, FILE_MODE_WRITE_ONLY);
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_open_write_only", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_open_write_only");
  }
  
  return 0;
}

ILvoid ILAPIENTRY devil_file_close (ILHANDLE file_ptr)
{
  try
  {
    delete (StdFile*)file_ptr;
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_close", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_close");
  }
}

ILboolean ILAPIENTRY devil_file_eof (ILHANDLE file_ptr)
{
  try
  {
    return ((StdFile*)file_ptr)->Eof ();
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_eof", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_eof");
  }
  
  return 0;
}

ILint ILAPIENTRY devil_file_getc (ILHANDLE file_ptr)
{
  try
  {
    char ret_value;

    return ((StdFile*)file_ptr)->Read (&ret_value, sizeof (char)) == sizeof (char) ? ret_value : 0;
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_getc", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_getc");
  }
  
  return 0;
}

ILint ILAPIENTRY devil_file_read (void* data, ILuint size, ILuint count, ILHANDLE file_ptr)
{
  try
  {
    if (size == 0 || count == 0)
      return 0;
    
    return ((StdFile*)file_ptr)->Read (data, size * count) / size;
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_read", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_read");
  }
  
  return 0;
}

ILint ILAPIENTRY devil_file_seek (ILHANDLE file_ptr, ILint offset, ILint origin)
{
  try
  {
    static FileSeekMode seek_mode [] = {FILE_SEEK_SET, FILE_SEEK_CUR, FILE_SEEK_END};

    filepos_t seek_pos;
    
    switch (seek_mode [origin])
    {
      case FILE_SEEK_SET: seek_pos = offset; break;
      case FILE_SEEK_CUR: seek_pos = ((StdFile*)file_ptr)->Tell () + offset; break;
      case FILE_SEEK_END: seek_pos = ((StdFile*)file_ptr)->Size () + offset; break;
    }

    return seek_pos != ((StdFile*)file_ptr)->Seek (seek_pos); //return 1 if error
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_seek", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_seek");
  }

  return 1;
}

ILint ILAPIENTRY devil_file_tell (ILHANDLE file_ptr)
{
  try
  {
    return ((StdFile*)file_ptr)->Tell ();
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_tell", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_tell");
  }

  return 0;
}

ILint ILAPIENTRY devil_file_putc (ILubyte buffer, ILHANDLE file_ptr)
{
  try
  {
    return ((StdFile*)file_ptr)->Write (&buffer, sizeof (ILubyte)) == sizeof (ILubyte) ? buffer : EOF;
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_putc", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_putc");
  }

  return EOF;
}

ILint ILAPIENTRY devil_file_write (const void* buffer, ILuint size, ILuint count, ILHANDLE file_ptr)
{
  try
  {
    if (size == 0 || count == 0)
      return 0;

    return ((StdFile*)file_ptr)->Write (buffer, size * count) / size;
  }
  catch (std::exception& exception)
  {
    log_exception ("media::devil_file_write", exception);
  }
  catch (...)
  {
    log_exception ("media::devil_file_write");
  }

  return 0;
}

//синглтон, обеспечивающий инициализацию DevIL
struct DevILSystemImpl
{
  DevILSystemImpl  ()
  {
    ilInit       ();
    iluInit      ();
    ilEnable     (IL_FILE_OVERWRITE);
    ilEnable     (IL_ORIGIN_SET);
    ilOriginFunc (IL_ORIGIN_LOWER_LEFT);

    ilSetMemory (&devil_allocate, &devil_deallocate);

      //!!!!Временный хак!!!! (https://unt.game-host.org/trac/funner/ticket/31)
    FileSystem::SetDefaultFileBufferSize (0);

    ilSetRead  (&devil_file_open_read_only, &devil_file_close, &devil_file_eof, &devil_file_getc,
                &devil_file_read, &devil_file_seek, &devil_file_tell);
    ilSetWrite (&devil_file_open_write_only, &devil_file_close, &devil_file_putc, &devil_file_seek,
                &devil_file_tell, &devil_file_write);      
  }

  ~DevILSystemImpl ()
  {
    ilShutDown ();
  }
};

typedef common::Singleton<DevILSystemImpl> DevILSystemSingleton;

}

namespace media
{

void devil_init ()
{
  DevILSystemSingleton::Instance ();
}

}
