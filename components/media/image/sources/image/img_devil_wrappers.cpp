#include "shared.h"

using namespace common;
using namespace media;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "media.image.devil"; //имя потока протоколирования

/*
    Протоколирование исключения
*/

struct LogHolder
{
  LogHolder () : log (LOG_NAME) {}

  Log log;
};

Log& get_log ()
{
  typedef common::Singleton<LogHolder> LogHolderSingleton;

  return LogHolderSingleton::Instance ().log;
}

void log_exception (const char* source, std::exception& exception)
{
  get_log ().Printf ("Exception at %s: %s", source, exception.what ());
}

void log_exception (const char* source)
{
  get_log ().Printf ("Unknown exception at %s", source);
}

/*
    Функции работы с памятью и файлами, заменяющии стандартные DevIL функции
*/

void* ILAPIENTRY devil_allocate (ILsizei size)
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

void ILAPIENTRY devil_deallocate (const void* ptr)
{
  try
  {
    MemoryManager::Deallocate (const_cast <void*> (ptr));
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
    return new StdFile (file_name, FileMode_ReadOnly);
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
    return new StdFile (file_name, FileMode_WriteOnly);
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

void ILAPIENTRY devil_file_close (ILHANDLE file_ptr)
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
    static FileSeekMode seek_mode [] = {FileSeekMode_Set, FileSeekMode_Current, FileSeekMode_End};

    filepos_t seek_pos;

    switch (seek_mode [origin])
    {
      default:
      case FileSeekMode_Set:     seek_pos = offset; break;
      case FileSeekMode_Current: seek_pos = ((StdFile*)file_ptr)->Tell () + offset; break;
      case FileSeekMode_End:     seek_pos = ((StdFile*)file_ptr)->Size () - offset; break;
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

/*
   Компонент работы с DevIL картинками
*/

class DevILComponent
{
  public:
    //загрузка компонента
    DevILComponent ()
    {
      ilSetMemory (&devil_allocate, &devil_deallocate); //Необходимо вызывать до вызова ilInit ();

      ilInit ();

      ilSetRead  (&devil_file_open_read_only, &devil_file_close, &devil_file_eof, &devil_file_getc,  //Необходимо вызывать после вызова ilInit ();
                  &devil_file_read, &devil_file_seek, &devil_file_tell);
      ilSetWrite (&devil_file_open_write_only, &devil_file_close, &devil_file_putc, &devil_file_seek,
                  &devil_file_tell, &devil_file_write);

      iluInit      ();
      ilEnable     (IL_FILE_OVERWRITE);
      ilEnable     (IL_ORIGIN_SET);
      ilOriginFunc (IL_ORIGIN_LOWER_LEFT);

      ImageManager::RegisterLoader ("jpg",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("png",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("bmp",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("tga",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("tif",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("dds",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("psd",     &Image::DefaultLoader);
      ImageManager::RegisterLoader ("cubemap", &Image::CubemapLoader);
      ImageManager::RegisterLoader ("skybox",  &Image::SkyBoxLoader);

      ImageManager::RegisterSaver  ("jpg",     &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("png",     &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("bmp",     &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("tga",     &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("cubemap", &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("skybox",  &Image::DefaultSaver);
      ImageManager::RegisterSaver  ("dds",     &Image::DefaultSaver);
    }

    //выгрузка компонента
    ~DevILComponent ()
    {
      ilShutDown ();
    }
};

extern "C"
{

ComponentRegistrator<DevILComponent> DevIL ("media.image.DevIL");

}

}
