#include <zzip/zzip.h>
#include <zzip/plugin.h>

#include <common/component.h>
#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <stl/hash_map>
#include <stl/vector>
#include <stl/list>
#include <stl/string>

#include <xtl/function.h>
#include <xtl/common_exceptions.h>

using namespace common;
using namespace stl;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "common.file_systems.zip_file_system"; //имя потока протоколирования

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

  return LogHolderSingleton::Instance ()->log;
}

void log_exception (const char* source, std::exception& exception)
{
  try
  {
    get_log ().Printf ("Exception at %s: %s", source, exception.what ());
  }
  catch (...)
  {
  }
}

void log_exception (const char* source)
{
  try
  {
    get_log ().Printf ("Unknown exception at %s", source);
  }
  catch (...)
  {
  }
}

int zip_open_func (zzip_char_t* name, int flags, ...)
{
  try
  {
    static size_t INVALID_MODE = O_WRONLY | O_RDWR | O_APPEND;

    if (flags & INVALID_MODE)
      throw xtl::make_argument_exception ("", "flags", flags);

    return (int)new StdFile (name, FileMode_ReadOnly);
  }
  catch (std::exception& exception)
  {
    log_exception ("common::zip_open_func", exception);
  }
  catch (...)
  {
    log_exception ("common::zip_open_func");
  }

  return -1;
}

int zip_close_func (int fd)
{
  try
  {
    delete (StdFile*)fd;

    return 0;
  }
  catch (std::exception& exception)
  {
    log_exception ("common::zip_close_func", exception);
  }
  catch (...)
  {
    log_exception ("common::zip_close_func");
  }

  return -1;
}

zzip_ssize_t zip_read_func (int fd, void* buf, zzip_size_t len)
{
  if (!len)
    return 0;

  try
  {
    return ((StdFile*)fd)->Read (buf, len);
  }
  catch (std::exception& exception)
  {
    log_exception ("common::zip_read_func", exception);
  }
  catch (...)
  {
    log_exception ("common::zip_read_func");
  }

  return 0;
}

zzip_off_t zip_seek_func (int fd, zzip_off_t offset, int whence)
{
  try
  {
    FileSeekMode seek_mode;

    switch (whence)
    {
      case SEEK_SET: seek_mode = FileSeekMode_Set;     break;
      case SEEK_CUR: seek_mode = FileSeekMode_Current; break;
      case SEEK_END: seek_mode = FileSeekMode_End;     break;
      default:
        throw xtl::make_argument_exception ("common::zip_seek_func", "whence", whence);
    }

    return ((StdFile*)fd)->Seek (offset, seek_mode);
  }
  catch (std::exception& exception)
  {
    log_exception ("common::zip_seek_func", exception);
  }
  catch (...)
  {
    log_exception ("common::zip_seek_func");
  }

  return -1;
}

zzip_off_t zip_size_func (int fd)
{
  try
  {
    return ((StdFile*)fd)->Size ();
  }
  catch (std::exception& exception)
  {
    log_exception ("common::zip_size_func", exception);
  }
  catch (...)
  {
    log_exception ("common::zip_size_func");
  }

  return -1;
}

zzip_ssize_t zip_write_func (int fd, _zzip_const void* buf, zzip_size_t len)
{
  try
  {
    if (!len)
      return 0;

    return ((StdFile*)fd)->Write (buf, len);
  }
  catch (std::exception& exception)
  {
    log_exception ("::zip_write_func", exception);
  }
  catch (...)
  {
    log_exception ("::zip_write_func");
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Файловая система zip-файла
///////////////////////////////////////////////////////////////////////////////////////////////////
class ZipFileSystem: public ICustomFileSystem
{
  public:
            ZipFileSystem  (const char* path);
    virtual ~ZipFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с файлом
///////////////////////////////////////////////////////////////////////////////////////////////////
    file_t     FileOpen   (const char* name,filemode_t mode_flags,size_t);
    void       FileClose  (file_t);
    size_t     FileRead   (file_t,void* buf,size_t size);
    size_t     FileWrite  (file_t,const void* buf,size_t size);
    void       FileRewind (file_t);
    filepos_t  FileSeek   (file_t,filepos_t pos);
    filepos_t  FileTell   (file_t);
    filesize_t FileSize   (file_t);
    void       FileResize (file_t,filesize_t new_size);
    bool       FileEof    (file_t);
    void       FileFlush  (file_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление расположением файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о файле
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFileExist (const char* file_name);
    bool GetFileInfo (const char* file_name,FileInfo& info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск файла (Search возвращает количество найденных файлов)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Search (const char* wc_mask,const FileSearchHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка ошибки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckError ();
    void CheckError (zzip_error_t);

  private:
    struct ZipFile
    {
      ZZIP_FILE* handle;
      size_t     size;
    };

    typedef vector<FileListItem>    EntryList;
    typedef hash_map<size_t,size_t> EntryMap;
    typedef list<ZipFile*>          OpenFileList;

    zzip_plugin_io_handlers io_handlers;
    size_t                  ref_count;
    ZZIP_DIR*               zip_dir;
    OpenFileList            open_files;
    EntryList               entries;
    EntryMap                entry_map;
    string                  file_names;
    string                  zip_file_name;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
    Получение сообщения об ошибке
*/

const char* GetZZipErrorMessage (zzip_error_t error)
{
  switch (error)
  {
    case ZZIP_NO_ERROR:     return "No error";
    case ZZIP_OUTOFMEM:     return "Out of memory";
    case ZZIP_DIR_OPEN:     return "Failed to open zip-file";
    case ZZIP_DIR_STAT:
    case ZZIP_DIR_SEEK:
    case ZZIP_DIR_READ:     return "Unable to read zip file";
    case ZZIP_UNSUPP_COMPR: return "Unsupported compression format";
    case ZZIP_CORRUPTED:    return "Corrupted archive";
    default:                return "Unknown error";
  }
}

/*
    Инициализация / завершение
*/

ZipFileSystem::ZipFileSystem (const char* path)
  : ref_count (1), zip_file_name (path)
{
  zzip_init_io (&io_handlers, 0);

  io_handlers.fd.open     = zip_open_func;
  io_handlers.fd.close    = zip_close_func;
  io_handlers.fd.read     = zip_read_func;
  io_handlers.fd.seeks    = zip_seek_func;
  io_handlers.fd.filesize = zip_size_func;
  io_handlers.fd.write    = zip_write_func;

  zzip_error_t error = ZZIP_NO_ERROR;

  zip_dir = zzip_dir_open_ext_io (path, &error, 0, &io_handlers);

  try
  {
    CheckError (error);

    ZZIP_DIRENT entry;
    string      file_name;

    while (zzip_dir_read (zip_dir,&entry))
    {
      FileListItem item;

      item.name = (const char*)file_names.size ();
      file_name = FileSystem::GetNormalizedFileName (entry.d_name);

      if (!file_name.empty () && file_name.end ()[-1] == '/')
        file_name.erase (file_name.end ()-1);

      file_names.append    (file_name);
      file_names.push_back (0);

      item.info.is_dir      = entry.st_size == 0;
      item.info.size        = entry.st_size;
      item.info.time_create = 0;
      item.info.time_access = 0;
      item.info.time_modify = 0;

      entry_map [strihash (file_name.c_str ())] = entries.size ();

      entries.push_back (item);
    }

    const char* string_base = file_names.c_str ();

    for (EntryList::iterator i=entries.begin ();i!=entries.end ();++i)
      i->name = string_base + (size_t)i->name;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("ZipFileSystem::ZipFileSystem");
    throw;
  }
  catch (...)
  {
    zzip_dir_close (zip_dir);
    throw;
  }
}

ZipFileSystem::~ZipFileSystem ()
{
  for (OpenFileList::iterator i=open_files.begin ();i!=open_files.end ();++i)
  {
    zzip_file_close ((*i)->handle);
    delete *i;
  }

  zzip_dir_close (zip_dir);
}

/*
    Работа с файлом
*/

ZipFileSystem::file_t ZipFileSystem::FileOpen (const char* file_name,filemode_t mode_flags,size_t)
{
  if ((mode_flags & FileMode_Write) || (mode_flags & FileMode_Resize) || (mode_flags & FileMode_Create))
    throw xtl::format_not_supported_exception ("ZipFileSystem::FileOpen","Unable to open file '%s' in zip-file '%s' with mode='%s'",
          file_name,zip_file_name.c_str (),strfilemode (mode_flags).c_str ());

  EntryMap::iterator iter = entry_map.find (strihash (file_name));

  if (iter == entry_map.end ())
    throw xtl::format_operation_exception ("ZipFileSystem::FileOpen","File '%s' not found in zip-file '%s'",file_name,zip_file_name.c_str ());

  ZipFile* file = new ZipFile;

  file->handle = zzip_file_open (zip_dir,file_name,ZZIP_ONLYZIP|ZZIP_CASELESS);
  file->size   = entries [iter->second].info.size;

  if (!file->handle)
  {
    delete file;

    CheckError ();

    return 0;
  }

  try
  {
    open_files.push_back (file);
  }
  catch (...)
  {
    zzip_file_close (file->handle);
    delete file;
    throw;
  }

  return (file_t)file;
}

void ZipFileSystem::FileClose (file_t _file)
{
  ZipFile* file = (ZipFile*)_file;

  open_files.remove (file);

  zzip_file_close (file->handle);

  delete file;
}

size_t ZipFileSystem::FileRead (file_t _file,void* buf,size_t size)
{
  ZipFile* file = (ZipFile*)_file;

  size_t read_size = zzip_file_read (file->handle,(char*)buf,size);

  CheckError ();

  return read_size;
}

size_t ZipFileSystem::FileWrite (file_t,const void* buf,size_t size)
{
  throw xtl::format_not_supported_exception ("ZipFileSystem::FileWrite","There is not write operation in zip-files");
}

void ZipFileSystem::FileRewind (file_t _file)
{
  ZipFile* file = (ZipFile*)_file;

  zzip_rewind (file->handle);

  CheckError ();
}

filepos_t ZipFileSystem::FileSeek (file_t _file,filepos_t pos)
{
  ZipFile* file = (ZipFile*)_file;

  filepos_t new_pos = zzip_seek (file->handle,pos,SEEK_SET);

  CheckError ();

  return new_pos;
}

filepos_t ZipFileSystem::FileTell (file_t _file)
{
  ZipFile* file = (ZipFile*)_file;

  return zzip_tell (file->handle);
}

filesize_t ZipFileSystem::FileSize (file_t _file)
{
  return ((ZipFile*)_file)->size;
}

void ZipFileSystem::FileResize (file_t,filesize_t)
{
  throw xtl::format_not_supported_exception ("ZipFileSystem::FileResize","There is not resize operation in zip-files");
}

bool ZipFileSystem::FileEof (file_t _file)
{
  ZipFile* file = (ZipFile*)_file;

  return (filesize_t)zzip_tell (file->handle) >= file->size;
}

void ZipFileSystem::FileFlush (file_t)
{
}

/*
    Управление расположением файлов
*/

void ZipFileSystem::Remove (const char* file_name)
{
  throw xtl::format_not_supported_exception ("ZipFileSystem::Remove","Unable to remove file '%s' from zip-file '%s'",
                     file_name,zip_file_name.c_str ());
}

void ZipFileSystem::Rename (const char* file_name,const char* new_name)
{
  throw xtl::format_not_supported_exception ("ZipFileSystem::Rename","Unable to rename file '%s' to '%s' in zip-file '%s'",
                     file_name,new_name,zip_file_name.c_str ());
}

void ZipFileSystem::Mkdir (const char* dir_name)
{
  throw xtl::format_not_supported_exception ("ZipFileSystem::Mkdir","Unable to create dir '%s' in zip-file '%s'",
                     dir_name,zip_file_name.c_str ());
}

/*
    Получение информации о файле
*/

bool ZipFileSystem::IsFileExist (const char* file_name)
{
  EntryMap::iterator i = entry_map.find (strihash (file_name));

  return i != entry_map.end ();
}

bool ZipFileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  EntryMap::iterator i = entry_map.find (strihash (file_name));

  if (i == entry_map.end ())
    return false;

  info = entries [i->second].info;

  return true;
}

/*
    Поиск файла
*/

void ZipFileSystem::Search (const char* wc_mask,const FileSearchHandler& find_handler)
{
  for (EntryList::iterator i=entries.begin ();i!=entries.end ();++i)
    if (wcimatch (i->name,wc_mask))
      find_handler (i->name,i->info);
}

/*
    Проверка ошибки
*/

void ZipFileSystem::CheckError ()
{
  CheckError ((zzip_error_t)zzip_error (zip_dir));
}

void ZipFileSystem::CheckError (zzip_error_t error)
{
  if (error == ZZIP_NO_ERROR)
    return;

  throw xtl::format_operation_exception ("", "ZZip internal error (zip-file '%s'): %s",
    zip_file_name.c_str (),GetZZipErrorMessage (error));
}

/*
    Подсчёт ссылок
*/

void ZipFileSystem::AddRef ()
{
  ref_count++;
}

void ZipFileSystem::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
    Компонент, регистрирующий ZipFileSystem
*/

class ZipFileSystemComponent
{
  public:
    ZipFileSystemComponent ()
    {
      FileSystem::RegisterPackFile ("zip", &Create);
      FileSystem::RegisterPackFile ("jar", &Create);
      FileSystem::RegisterPackFile ("pk3", &Create);
      FileSystem::RegisterPackFile ("pak", &Create);
    }

  private:
    static ICustomFileSystem* Create (const char* path)
    {
      return new ZipFileSystem (path);
    }
};

}

extern "C"
{

ComponentRegistrator<ZipFileSystemComponent> ZipFileSystem ("common.file_systems.ZipFileSystem");

}
