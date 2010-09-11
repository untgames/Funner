#ifndef COMMONLIB_FILE_SYSTEM_SHARED_HEADER
#define COMMONLIB_FILE_SYSTEM_SHARED_HEADER

#include <cstring>

#include <stl/algorithm>
#include <stl/auto_ptr.h>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/list>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/crypto.h>
#include <common/file.h>
#include <common/hash.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

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
            FileImpl  (filemode_t mode);
    virtual ~FileImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим работы файла
///////////////////////////////////////////////////////////////////////////////////////////////////
    filemode_t Mode () const { return mode; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение пути к файлу
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetPath   ();
            void        SetPath   (const char* path);
            void        ResetPath ();

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
    filemode_t                 mode;                //режим работы файла
    stl::auto_ptr<stl::string> anonymous_file_path; //сгенерированный путь к файлу
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
    const char* GetPath ();
    size_t      Read    (void* buf,size_t size);
    size_t      Write   (const void* buf,size_t size);
    filepos_t   Tell    ();
    filepos_t   Seek    (filepos_t new_pos);
    void        Rewind  ();
    filesize_t  Size    ();
    void        Resize  (filesize_t new_size);
    bool        Eof     ();
    void        Flush   ();

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

    void        SetPath       (const char* path);
    const char* GetPath       ();
    size_t      Read          (void* buf,size_t size);
    size_t      Write         (const void* buf,size_t size);
    filepos_t   Tell          ();
    filepos_t   Seek          (filepos_t new_pos);
    void        Rewind        ();
    filesize_t  Size          ();
    void        Resize        (filesize_t new_size);
    bool        Eof           ();
    void        Flush         ();
    size_t      GetBufferSize ();

  private:
    ICustomFileSystemPtr file_system;   //файловая система
    file_t               file_handle;   //хандлер файла в файловой системе
    bool                 auto_close;    //необходимо ли закрывать файл при уничтожении объекта
    bool                 has_file_path; //путь к файлу определен
    stl::string          file_path;     //путь к файлу
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация отображения файла в памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemFileImpl: public FileImpl
{
  public: 
    MemFileImpl  (FileImplPtr base_file);
    MemFileImpl  (size_t buffer_reserved_size,filemode_t mode);
    MemFileImpl  (void* buffer,size_t buffer_size,filemode_t mode);
    ~MemFileImpl ();
    
    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    size_t      GetBufferSize ();
    void*       GetBuffer ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация буферизированного файла
///////////////////////////////////////////////////////////////////////////////////////////////////
class BufferedFileImpl: public FileImpl
{
  public:
    BufferedFileImpl  (FileImplPtr base_file,size_t buffer_size);
    ~BufferedFileImpl ();

    const char* GetPath       ();
    size_t      Read          (void* buf,size_t size);
    size_t      Write         (const void* buf,size_t size);
    filepos_t   Tell          ();
    filepos_t   Seek          (filepos_t new_pos);
    void        Rewind        ();
    filesize_t  Size          ();
    void        Resize        (filesize_t new_size);
    bool        Eof           ();
    void        Flush         ();
    size_t      GetBufferSize ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Файл с шифрованием
///////////////////////////////////////////////////////////////////////////////////////////////////
class CryptoFileImpl: public FileImpl
{
  public:
    CryptoFileImpl  (const FileImplPtr& file,size_t buffer_size,const char* read_crypto_method,const char* write_crypto_method,const void* key,size_t key_bits);
    CryptoFileImpl  (const FileImplPtr& file,size_t buffer_size,const char* read_crypto_method,const void* key,size_t key_bits);
    ~CryptoFileImpl ();

    const char* GetPath       ();
    size_t      Read          (void* buf,size_t size);
    size_t      Write         (const void* buf,size_t size);
    filepos_t   Tell          ();
    filepos_t   Seek          (filepos_t new_pos);
    void        Rewind        ();
    filesize_t  Size          ();
    void        Resize        (filesize_t new_size);
    bool        Eof           ();
    void        Flush         ();
    size_t      GetBufferSize ();

  private:
    CryptoFileImpl (const CryptoFileImpl&); //no impl
    CryptoFileImpl& operator = (const CryptoFileImpl&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
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
///Фиктивная файловая система для хранения списка анонимных файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnonymousFileSystem: public ICustomFileSystem, public xtl::reference_counter
{
  public:
    AnonymousFileSystem ();
    ~AnonymousFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с путями анонимных файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* AddFilePath        (const FileImplPtr&);
    void        RemoveFilePath     (const FileImplPtr&);
    void        RemoveAllFilePaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Захва файла по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileImplPtr RetainFile (const char* path,filemode_t mode_flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение основных операций
///////////////////////////////////////////////////////////////////////////////////////////////////
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
    void       AddRef      ();
    void       Release     ();
    
  private:
    struct AnonymousFile
    {
      stl::string path;
      FileImpl*   file;
      
      AnonymousFile (const char* in_path, FileImpl* in_file)
        : path (in_path)
        , file (in_file) { }
    };

    typedef stl::list<AnonymousFile> AnonymousFileList;

  private:
    AnonymousFileList anonymous_files; //список анономных файлов
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Символическая ссылка
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SymbolicLink
{
  stl::string prefix;      //заменяемый префикс
  stl::string link;        //ссылка на файл
  
  SymbolicLink (const char* prefix, const char* link);
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
    void Mount       (const char* path_prefix,const char* path,bool link,const char* force_extension);
    void Unmount     (const char* path_prefix);
    void Unmount     (ICustomFileSystemPtr file_system);
    void UnmountAll  ();
    bool IsPathMount (const char* path) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с путями анонимных файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    stl::string AddAnonymousFilePath        (const FileImplPtr&);
    void        RemoveAnonymousFilePath     (const FileImplPtr&);
    void        RemoveAllAnonymousFilePaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка шифрования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetCryptoParameters       (const char* path, const FileCryptoParameters& parameters);
    bool                 HasCryptoParameters       (const char* path) const;
    FileCryptoParameters GetCryptoParameters       (const char* path) const;
    void                 RemoveCryptoParameters    (const char* path);
    void                 RemoveAllCryptoParameters ();    
    
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
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка файловых систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void LoadFileSystems ();
   
  private:
    typedef stl::list<SearchPath>                                           SearchPathList;
    typedef stl::list<PackFile>                                             PackFileList;
    typedef stl::list<PackFileType>                                         PackFileTypeList;
    typedef stl::list<MountFileSystem>                                      MountList;
    typedef stl::hash_map<stl::hash_key<const char*>, FileCryptoParameters> CryptoMap;
    typedef xtl::intrusive_ptr<AnonymousFileSystem>                         AnonymousFileSystemPtr;
    typedef stl::list<SymbolicLink>                                         SymbolicLinkList;

  private:    
    PackFileTypeList       pack_types;               //список типов пак-файлов
    PackFileList           pack_files;               //список пак-файлов и пользовательских файловых систем
    MountList              mounts;                   //список смонтированных файловых систем
    SymbolicLinkList       symbolic_links;           //список символических ссылок
    SearchPathList         search_paths;             //список путей поиска    
    FileImplPtr            closed_file;              //закрытый файл
    AnonymousFileSystemPtr anonymous_file_system;    //система анонимных файлов    
    CryptoMap              crypto_parameters;        //параметры шифрования файлов
    stl::string            default_path;             //путь по умолчанию (аналог текущего каталога)
    stl::string            compress_path;            //буфер для формирования сокращённого пути
    size_t                 default_file_buffer_size; //размер буфера файла по умолчанию
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон файловой системы
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef Singleton<FileSystemImpl> FileSystemSingleton;

}

#endif
