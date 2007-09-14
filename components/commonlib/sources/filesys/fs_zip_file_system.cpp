#include "shared.h"

using namespace common;
using namespace stl;

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
    
    size_t       ref_count;
    ZZIP_DIR*    zip_dir;
    OpenFileList open_files;
    EntryList    entries;
    EntryMap     entry_map;
    string       file_names;
    string       zip_file_name;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
    Получение сообщения об ошибке
*/

static const char* GetZZipErrorMessage (zzip_error_t error) 
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
  zzip_error_t error = ZZIP_NO_ERROR;

  zip_dir = zzip_dir_open (path,&error);
  
  try
  {
    CheckError (error);
    
    ZZIP_DIRENT entry;
    string      file_name;
    
    while (zzip_dir_read (zip_dir,&entry))
    {
      FileListItem item;
      
      item.name = (const char*)file_names.size ();    
      file_name = FileSystemImpl::ConvertFileName (entry.d_name);
            
      if (!file_name.empty () && file_name.end ()[-1] == '/')
        file_name.erase (file_name.end ()-1);

      file_names.append    (file_name);            
      file_names.push_back (0);
      
      item.info.is_dir      = strchr (file_name.c_str (),'/') != NULL;
      item.info.size        = entry.st_size;
      item.info.time_create = 0;
      item.info.time_access = 0;
      item.info.time_modify = 0;

      entry_map [strihash (file_name)] = entries.size ();

      entries.push_back (item);      
    }
    
    const char* string_base = file_names.c_str ();
    
    for (EntryList::iterator i=entries.begin ();i!=entries.end ();++i)
      i->name = string_base + (size_t)i->name;
  }
  catch (Exception& exception)
  {
    exception.Raise ("ZipFileSystem::ZipFileSystem");
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
  if ((mode_flags & FILE_MODE_WRITE) || (mode_flags & FILE_MODE_RESIZE) || (mode_flags & FILE_MODE_CREATE))
    RaiseNotSupported ("ZipFileSystem::FileOpen","Unable to open file '%s' in zip-file '%s' with mode='%s'",
          file_name,zip_file_name.c_str (),strfilemode (mode_flags).c_str ());

  EntryMap::iterator iter = entry_map.find (strihash (file_name));
  
  if (iter == entry_map.end ())
    Raise<FileNotFoundException> ("ZipFileSystem::FileOpen","File '%s' not found in zip-file '%s'",file_name,zip_file_name.c_str ());

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
  RaiseNotSupported ("ZipFileSystem::FileWrite","There is not write operation in zip-files");
  return 0;
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
  RaiseNotSupported ("ZipFileSystem::FileResize","There is not resize operation in zip-files");
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
  RaiseNotSupported ("ZipFileSystem::Remove","Unable to remove file '%s' from zip-file '%s'",
                     file_name,zip_file_name.c_str ());
}

void ZipFileSystem::Rename (const char* file_name,const char* new_name)
{
  RaiseNotSupported ("ZipFileSystem::Rename","Unable to rename file '%s' to '%s' in zip-file '%s'",
                     file_name,new_name,zip_file_name.c_str ());
}

void ZipFileSystem::Mkdir (const char* dir_name)
{
  RaiseNotSupported ("ZipFileSystem::Mkdir","Unable to create dir '%s' in zip-file '%s'",
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
    
  Raise<FileException> (NULL,"ZZip internal error (zip-file '%s'): %s",
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
    Создание файловой системы zip-файла
*/

ICustomFileSystem* FileSystemImpl::CreateZipFileSystem (const char* path)
{
  return new ZipFileSystem (path);
}
