#ifndef COMMONLIB_FILE_SYSTEM_SHARED_HEADER
#define COMMONLIB_FILE_SYSTEM_SHARED_HEADER

#include <common/file.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/hash.h>
#include <common/component.h>

#include <platform/platform.h>

#include <stl/hash_set>
#include <stl/vector>
#include <stl/list>
#include <stl/algorithm>

#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <string.h>

namespace common
{

//implementation forwards
struct MountFileSystem;

//константы
const size_t FILE_HASH_BUF_SIZE    = 1024; //размер буфера используемого для рассчёта хэш суммы файла
const size_t DEFAULT_FILE_BUF_SIZE = 8192; //размер буфера файла по умолчанию

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации файла
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileImpl: public xtl::reference_counter
{
  public:
            FileImpl (filemode_t mode);
    virtual ~FileImpl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим работы файла
///////////////////////////////////////////////////////////////////////////////////////////////////
    filemode_t Mode () const { return mode; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение / запись
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read  (void* buf,size_t size);
    virtual size_t Write (const void* buf,size_t size);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Файловый указатель
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual filepos_t Tell   ();
    virtual filepos_t Seek   (filepos_t new_pos);
    virtual void      Rewind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер файла / проверка на конец файла
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual filesize_t Size   ();
    virtual void       Resize (filesize_t new_size);
    virtual bool       Eof    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс файловых буферов на диск
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t GetBufferSize () { return 0; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка является ли данный класс ClosedFileImpl
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsClosedFileType () { return false; }

  private:
    filemode_t mode; //режим работы файла
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Закрытый файл
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClosedFileImpl: public FileImpl
{
  public:
    ClosedFileImpl ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение основных методов
///////////////////////////////////////////////////////////////////////////////////////////////////  
    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: является ли данный класс ClosedFileImpl
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsClosedFileType () { return true; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение единственного экземпляра
///////////////////////////////////////////////////////////////////////////////////////////////////    
    static FileImpl* Instance ();
    
  private:
    void Raise (const char* source);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация оболочки над пользовательским файлом
///////////////////////////////////////////////////////////////////////////////////////////////////
class CustomFileImpl: public FileImpl
{
  private:    
    typedef ICustomFileSystem::file_t file_t;

  public:
    CustomFileImpl  (ICustomFileSystemPtr file_system,const char* file_name,filemode_t mode_flags);
    CustomFileImpl  (ICustomFileSystemPtr file_system,file_t handle,filemode_t mode_flags,bool auto_close);
    ~CustomFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();
    size_t      GetBufferSize ();

  private:
    ICustomFileSystemPtr file_system; //файловая система
    file_t              file_handle; //хандлер файла в файловой системе
    bool                auto_close;  //необходимо ли закрывать файл при уничтожении объекта
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация отображения файла в памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemFileImpl: public FileImpl
{
  public: 
    MemFileImpl  (FileImplPtr base_file);
    MemFileImpl  (void* buffer,size_t buffer_size,filemode_t mode);
    ~MemFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    bool        Eof    ();
    size_t      GetBufferSize () { return finish-start; }

  private:
    bool   is_auto_deleted;  //флаг определяющий нужно ли автоматически удалять буфер при закрытии файла
    char*  start;        //указатель на начало буфера
    char*  finish;       //указатель на конец буфера
    char*  pos;          //указатель на текущую позицию внутри буфера
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация буферизированного файла
///////////////////////////////////////////////////////////////////////////////////////////////////
class BufferedFileImpl: public FileImpl
{
  public:
    BufferedFileImpl  (FileImplPtr base_file,size_t buffer_size);
    ~BufferedFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();
    size_t      GetBufferSize () { return buffer_size; }

  private:
    size_t ReadBuffer  (filepos_t position,void* buf,size_t size);
    size_t WriteBuffer (filepos_t position,const void* buf,size_t size);
    void   ResetBuffer (filepos_t new_cache_start);
    void   FlushBuffer ();

  private:
    FileImplPtr base_file;   //базовый файл
    filepos_t   file_pos;     //файловая позиция
    filesize_t  file_size;    //размер файла
    char*       buffer;       //буфер файла
    size_t      buffer_size;  //размер буфера файла
    filepos_t   cache_start;  //файловая позиция начала буферизированного участка
    filepos_t   cache_finish; //файловая позиция конца буферизированного участка
    size_t      dirty_start;  //файловая позиция начала обновлённого участка
    size_t      dirty_finish; //файловая позиция конца обновлённого участка
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Билдер файлового списка
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileListBuilder: public xtl::noncopyable
{
  public:
    typedef stl::vector<FileListItem> FileInfoArray;
    
    FileListBuilder ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с префиксом имени файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPrefix       (const char* prefix);
    void        SetTruncateSize (size_t size);
    const char* GetPrefix       () const { return prefix.c_str (); }
    size_t      GetTruncateSize () const { return truncate_size; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление файлов в список
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert        (const char* file_name,const FileInfo& file_info);
    void InsertSubname (const char* file_name,size_t file_name_size,const FileInfo& file_info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Генерация списка файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList Build (bool sort=false);
    
  private:      
    void InsertInternal (const char* file_name,size_t file_name_size,const FileInfo& file_info);    
        
  private:
    typedef stl::hash_set<size_t> NameHash;
  
    NameHash      items_hash;
    FileInfoArray items;
    stl::string   file_names;
    stl::string   prefix;
    size_t        truncate_size;
    size_t        prefix_hash;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип пак-файла
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PackFileType
{
  FileSystem::PackFileCreater creater;        //функтор создания пак-файла
  size_t                      extension_hash; //хэш расширения  
  stl::string                 extension;      //расширение
  
  PackFileType (const char* extension,size_t hash,const FileSystem::PackFileCreater&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Путь поиска
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SearchPath
{
  size_t      hash; //хэш пути поиска
  stl::string path; //путь поиска

  SearchPath (const char* path,size_t hash);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пак-файл
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PackFile
{
  size_t              file_name_hash;   //хэш имени пак-файла
  size_t              search_path_hash; //хэш имени пути поиска ассоциированного с пак-файлом
  ICustomFileSystemPtr file_system;      //интерфейс файловой системы пак-файла
  
  PackFile  (size_t file_name_hash,size_t search_path_hash,ICustomFileSystemPtr file_system);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фиктивная файловая система для получения информации о точке монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
class MountPointFileSystem: public ICustomFileSystem
{
  public:
    MountPointFileSystem (MountFileSystem& owner);
  
    file_t     FileOpen    (const char* name,filemode_t mode_flags,size_t);
    void       FileClose   (file_t);
    size_t     FileRead    (file_t,void* buf,size_t size);
    size_t     FileWrite   (file_t,const void* buf,size_t size);
    void       FileRewind  (file_t);
    filepos_t  FileSeek    (file_t,filepos_t pos);
    filepos_t  FileTell    (file_t);
    filesize_t FileSize    (file_t);
    void       FileResize  (file_t,filesize_t new_size);
    bool       FileEof     (file_t);
    void       FileFlush   (file_t);
    void       Remove      (const char* file_name);
    void       Rename      (const char* file_name,const char* new_name);
    void       Mkdir       (const char* dir_name);
    bool       IsFileExist (const char* file_name);
    bool       GetFileInfo (const char* file_name,FileInfo& info);
    void       Search      (const char* mask,const FileSearchHandler&);
    void       AddRef      () {}
    void       Release     () {}
    
  private:
    MountFileSystem* owner; //владелец
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Смонтированная файловая система
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MountFileSystem
{
  size_t                hash;                    //хэш префикса точки монтирования
  stl::string           prefix;                  //префикс точки монтирования
  ICustomFileSystemPtr  file_system;             //интерфейс файловой системы
  FileInfo              mount_point_info;        //информация о точке монтирования
  MountPointFileSystem  mount_point_file_system; //фиктивная файловая система

  MountFileSystem (const char* prefix,size_t hash,ICustomFileSystemPtr file_system);
  MountFileSystem (const MountFileSystem&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация файловой системы
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileSystemImpl
{  
  public:
    typedef FileSystem::LogHandler      LogHandler;
    typedef FileSystem::PackFileCreater PackFileCreater;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileSystemImpl ();
    ~FileSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка дефолтного пути для поиска файлов, создания файлов и каталогов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetDefaultPath (const char* path);
    const char* GetDefaultPath () const { return default_path.c_str (); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с путями поиска
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSearchPath        (const char* path,const LogHandler& log_handler);
    void RemoveSearchPath     (const char* path);
    void RemoveAllSearchPaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление пользовательских типов пак-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterPackFile   (const char* extension,const PackFileCreater& creater);
    void UnregisterPackFile (const char* extension);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление пользоватльских типов пак-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Mount       (const char* path_prefix,ICustomFileSystemPtr file_system);
    void Unmount     (const char* path_prefix);
    void Unmount     (ICustomFileSystemPtr file_system);
    void UnmountAll  ();
    bool IsPathMount (const char* path) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие файла
///////////////////////////////////////////////////////////////////////////////////////////////////    
    FileImplPtr OpenFile (const char* src_file_name,filemode_t mode_flags);
    FileImplPtr OpenFile (const char* src_file_name,filemode_t mode_flags,size_t buffer_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка размера буфера файла по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetDefaultFileBufferSize (size_t buffer_size);
    size_t GetDefaultFileBufferSize () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление / переименование файла, создание каталога
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о файле
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetFileInfo (const char* file_name,FileInfo& info);
    bool IsFileExist (const char* file_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о файле
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList Search (const char* wc_mask,size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация открытых файлов / закрытие всех открытых файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFile   (File&);
    void UnregisterFile (File&);
    void CloseAllFiles  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Утилиты
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* CompressPath (const char* path); //формирование сокращённого пути

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение файловой системы ассоциированной с указанным путём
///////////////////////////////////////////////////////////////////////////////////////////////////
    ICustomFileSystemPtr FindMountFileSystem (const char* path,stl::string& result_file_name);
    ICustomFileSystemPtr FindFileSystem      (const char* path,stl::string& result_file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление пак-файла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddPackFile (const char* path,size_t search_path_hash,const FileSystem::LogHandler& log_handler);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск в смонтированных файловых системах
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MountSearch (FileListBuilder& builder,const char* wc_mask,const char* prefix,size_t flags);
   
  private:
    typedef stl::hash_set<File*>       OpenFileSet;
    typedef stl::list<SearchPath>      SearchPathList;
    typedef stl::list<PackFile>        PackFileList;
    typedef stl::list<PackFileType>    PackFileTypeList;
    typedef stl::list<MountFileSystem> MountList;  

  private:    
    OpenFileSet       open_files;               //список открытых файлов
    PackFileTypeList  pack_types;               //список типов пак-файлов
    PackFileList      pack_files;               //список пак-файлов и пользовательских файловых систем
    MountList         mounts;                   //список смонтированных файловых систем
    SearchPathList    search_paths;             //список путей поиска
    FileImplPtr       closed_file;              //закрытый файл
    stl::string       default_path;             //путь по умолчанию (аналог текущего каталога)
    stl::string       compress_path;            //буфер для формирования сокращённого пути
    size_t            default_file_buffer_size; //размер буфера файла по умолчанию
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон файловой системы
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef Singleton<FileSystemImpl> FileSystemSingleton;

}

#endif
