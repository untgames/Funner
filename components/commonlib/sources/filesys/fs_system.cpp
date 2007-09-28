#include "shared.h"

using namespace stl;
using namespace common;
using namespace xtl;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' used in base member initializer list
#endif

/*
    Реализация вспомогательных классы
*/

inline PackFileType::PackFileType (const char* _extension,size_t _hash,const FileSystem::PackFileCreater& _creater)
  : creater (_creater), extension_hash (_hash), extension (_extension)
  { }
  
inline SearchPath::SearchPath (const char* _path,size_t _hash)
  : hash (_hash), path (_path)
  { }
  
inline PackFile::PackFile (size_t _file_name_hash,size_t _search_path_hash,ICustomFileSystemPtr _file_system) 
  : file_name_hash (_file_name_hash), search_path_hash (_search_path_hash), file_system (_file_system)
  { }
    
inline MountFileSystem::MountFileSystem (const char* _prefix,size_t _hash,ICustomFileSystemPtr _file_system)
  : hash (_hash), prefix (_prefix), file_system (_file_system), mount_point_file_system (*this)
{
  mount_point_info.time_create = 0;
  mount_point_info.time_modify = 0;
  mount_point_info.time_access = 0;
  mount_point_info.is_dir      = true;
  mount_point_info.size        = 0;
}

inline MountFileSystem::MountFileSystem (const MountFileSystem& fs)
  : hash (fs.hash), prefix (fs.prefix), file_system (fs.file_system), mount_point_info (fs.mount_point_info),
    mount_point_file_system (*this)
  { }

/*
    Приведение файлового имени к стандартному виду
*/

string FileSystemImpl::ConvertFileName (const char* file_name)
{
  string res = file_name;

  for (string::iterator i=res.begin ();i!=res.end ();++i)
    switch (*i)
    {
      case '\\': *i = '/'; break;
      default:   *i = tolower (*i); break;
    }

  return res;
}

/*
    Инициализация / завершение
*/

FileSystemImpl::FileSystemImpl ()
{
  default_file_buffer_size = DEFAULT_FILE_BUF_SIZE;

  ClosedFileImpl::Instance ();

  RegisterPackFile ("zip",CreateZipFileSystem);
  RegisterPackFile ("jar",CreateZipFileSystem);
  RegisterPackFile ("pk3",CreateZipFileSystem);

  Mount          ("/std",Platform::GetFileSystem ());
  SetDefaultPath ("/std");
  Mount          ("/io",Platform::GetIOSystem ());
}

FileSystemImpl::~FileSystemImpl ()
{
  CloseAllFiles ();
  RemoveAllSearchPaths ();
  UnmountAll ();  
}

/*
    Регистрация открытых файлов / закрытие всех открытых файлов
*/

void FileSystemImpl::RegisterFile (File& file)
{
  open_files.insert (&file);
}

void FileSystemImpl::UnregisterFile (File& file)
{
  open_files.erase (&file);
}

void FileSystemImpl::CloseAllFiles ()
{
  for_each (open_files.begin (),open_files.end (),mem_fun (&File::Close));
}

/*
    Установление дефолтного пути поиска
*/

const char* FileSystemImpl::CompressPath (const char* path)
{
  if (!path || !*path)
    return "";

  size_t path_size = strlen (path), del_count = 0;

  const char *marker = path + path_size,
             *src    = marker - 1;
  
  compress_path.fast_resize (path_size);
  
  char* dst = compress_path.end ();
  
  for (;src!=path-1;--src)
    if (*src == '/')
    {      
      size_t size = marker - src;
      
      marker = src;
      
      switch (size)
      {
        case 2:
          if (!strncmp (src+1,".",1))
            continue;
          break;            
        case 3:            
          if (!strncmp (src+1,"..",2))
          {
            del_count++;
            continue;
          }
          break;
        default:
          break;
      }

      if (!del_count) 
      {
        dst -= size;
        memcpy (dst,src,size);
      }
      else del_count--;
    }

  return dst;
}

void FileSystemImpl::SetDefaultPath (const char* path)
{
  string new_path = path ? ConvertFileName (path) : "/";  

  if (new_path [0] != '/')
    new_path = CompressPath (format ("%s/%s",default_path.c_str (),new_path.c_str ()).c_str ());

  if (!new_path.empty () && new_path.end ()[-1] == '/')
    new_path.erase (new_path.end ()-1);

  FileInfo info;

  if (!GetFileInfo (new_path.c_str (),info))
    Raise<FileNotFoundException> ("FileSystem::SetCurrentDir","Dir '%s' does not exist",new_path.c_str ());

  if (!info.is_dir)
    Raise<FileNotDirException> ("FileSystem::SetCurrentDir","Can not set current dir '%s' because it is not dir",new_path.c_str ());
    
  swap (default_path,new_path);
}

/*
    Регистрация типов пак-файлов
*/

void FileSystemImpl::RegisterPackFile (const char* extension,const PackFileCreater& creater)
{
  if (!extension)
    RaiseNullArgument ("FileSystem::RegisterPackFile","extension");

  if (!creater)
    RaiseNullArgument ("FileSystem::RegisterPackFile","creater");

  size_t hash = strihash (extension);
  
  for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
    if (i->extension_hash == hash)
    {
      i->creater = creater;
      return;
    }

  pack_types.push_front (PackFileType (extension,hash,creater));
}

void FileSystemImpl::UnregisterPackFile (const char* extension)
{
  if (!extension)
    RaiseNullArgument ("FileSystem::UnregisterPackFile","extension");

  size_t hash = strihash (extension);
    
  for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
    if (i->extension_hash == hash)
    {
      pack_types.erase (i);
      return;
    }
}

/*
    Добавление пак-файлов и путей поиска
*/

void FileSystemImpl::AddPackFile (const char* _path,size_t search_path_hash,const FileSystem::LogHandler& log_handler)
{
  string path = ConvertFileName (_path), type = suffix (path);
         
  if (type.empty ())
    return;

  type.erase (type.begin ());

  size_t path_hash = strhash (path), type_hash = strhash (type);
    
  for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
    if (i->file_name_hash == path_hash)
    {
      log_handler (format ("Pack file '%s' has been already added",_path).c_str ());
      return;
    }

  for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
    if (i->extension_hash == type_hash)
    {
      try
      {        
        ICustomFileSystemPtr pack_file_system (i->creater (path.c_str ()),false);

        pack_files.push_front (PackFile (strihash (path),search_path_hash,pack_file_system));

        return;
      }
      catch (FileException& exception)
      {
        log_handler (format ("File exception at open pack-file '%s': %s",_path,exception.Message ()).c_str ());
      }
      catch (std::exception& exception)
      {
        log_handler (format ("Exception at open pack-file '%s': %s",_path,exception.what ()).c_str ());
      }
      catch (...)
      {
        log_handler (format ("Unknown exception at open pack-file '%s'",_path).c_str ());
      }
    }

  log_handler (format ("Fail to open pack-file '%s'. Undefined extension '%s'",_path,type.c_str ()).c_str ());
}

void FileSystemImpl::AddSearchPath (const char* _path,const LogHandler& log_handler)
{
  if (!_path)
    RaiseNullArgument ("FileSystem::AddSearchPath","path");

  if (!log_handler)
    RaiseNullArgument ("FileSystem::AddSearchPath","log_handler");

  string path = ConvertFileName (_path), mount_path;

  if (path [0] != '/')
    path = format ("%s/%s",default_path.c_str (),path.c_str ());

  ICustomFileSystemPtr owner_file_system = FindMountFileSystem (path.c_str (),mount_path);
  FileInfo           file_info;

  if (!owner_file_system || !owner_file_system->GetFileInfo (mount_path.c_str (),file_info))
  {
    log_handler (format ("Search path '%s' doesn't exist",path.c_str ()).c_str ());
    return;
  }
  
  size_t path_hash = strhash (path);

  if (file_info.is_dir)
  {
    for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      if (i->hash == path_hash)
      {
        log_handler (format ("Search path '%s' has been already added",path.c_str ()).c_str ());
        return;
      }

    FileListBuilder list_builder;

    for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
      owner_file_system->Search (format ("%s/*.%s",mount_path.c_str (),i->extension.c_str ()).c_str (),
                                ICustomFileSystem::FileSearchHandler (bind (&FileListBuilder::Insert, &list_builder, _1, _2)));

    search_paths.push_front (SearchPath (path.c_str (),path_hash));

    for (FileList::Iterator list_iter=list_builder.Build (true);list_iter;++list_iter)
      if (!list_iter->info.is_dir)
        AddPackFile (list_iter->name,path_hash,log_handler);
  }
  else AddPackFile (path.c_str (),0,log_handler);
}

void FileSystemImpl::RemoveSearchPath (const char* _path)
{
  if (!_path)
    RaiseNullArgument ("FileSystem::RemoveSearchPath","path");

  string path = ConvertFileName (_path);
  
  if (path [0] != '/')
    path = format ("%s/%s",default_path.c_str (),path.c_str ());

  size_t hash = strhash (path);
    
  for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
    if (i->hash == hash)
    {
      search_paths.erase (i);
      break;
    }
    
  for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();)
    if (i->search_path_hash == hash || i->file_name_hash == hash)
    {
      PackFileList::iterator j = i;

      ++j;

      pack_files.erase (i);

      i = j;
    }
    else ++i;
}

void FileSystemImpl::RemoveAllSearchPaths ()
{
  pack_files.clear ();
  search_paths.clear ();
}

/*
    Работа со смонтированными файловыми системами
*/

void FileSystemImpl::Mount (const char* _path_prefix,ICustomFileSystemPtr file_system)
{
  if (!_path_prefix)
    RaiseNullArgument ("FileSystem::Mount","path_prefix");
    
  if (!file_system)
    RaiseNullArgument ("FileSystem::Mount","file_system");

  if (*_path_prefix != '/')
    RaiseInvalidArgument ("FileSystem::Mount","path_prefix",_path_prefix,"Mount path must start from '/'");

  string prefix = ConvertFileName (_path_prefix);
  size_t hash   = strhash (prefix);

  prefix += '/';

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (!strncmp (i->prefix.c_str (),prefix.c_str (),i->prefix.size ()))
      Raise<FileMountException> ("FileSystem::Mount","Can not mount file system '%s' "
                                 "because it intersects with already mounted file system '%s'",
                                 prefix.c_str (),i->prefix.c_str ());

  mounts.push_front (MountFileSystem (prefix.c_str (),hash,file_system));
}

void FileSystemImpl::Unmount (const char* _path_prefix)
{
  if (!_path_prefix)
    return;
        
  string prefix = ConvertFileName (_path_prefix);
  size_t hash   = strhash (prefix);

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (i->hash == hash && !strncmp (prefix.c_str (),i->prefix.c_str (),i->prefix.size ()-1))
    {
      mounts.erase (i);
      return;
    }
}

void FileSystemImpl::Unmount (ICustomFileSystemPtr file_system)
{
  if (!file_system)
    return;
    
  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();)
    if (i->file_system == file_system)
    {
      MountList::iterator j = i;

      ++j;

      mounts.erase (i);

      i = j;
    }
    else ++i;
}

void FileSystemImpl::UnmountAll ()
{
  mounts.clear ();
}

bool FileSystemImpl::IsPathMount (const char* path) const
{
  if (!path)
    return false;
        
  string prefix = ConvertFileName (path);
  size_t hash   = strhash (prefix);

  for (MountList::const_iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (i->hash == hash && !strncmp (prefix.c_str (),i->prefix.c_str (),i->prefix.size ()-1))
      return true;

  return false;
}

/*
    Определение принадлежности файла к файловой системе
*/

ICustomFileSystemPtr FileSystemImpl::FindMountFileSystem (const char* file_name,string& result_file_name)
{
  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
  {
    if (!string_wrappers::strnicmp (i->prefix.c_str (),file_name,i->prefix.size ()-1))
    {
      switch (file_name [i->prefix.size ()-1])
      {
        case '\0': //запрос к точке монтирования
          result_file_name = file_name;
          return &i->mount_point_file_system;
        case '/': //запрос к смонтированной файловой системе
          result_file_name = file_name + i->prefix.size ();
          return i->file_system;
        default:
          break;
      }
    }
  }

  return NULL;
}

ICustomFileSystemPtr FileSystemImpl::FindFileSystem (const char* src_file_name,string& result_file_name)
{
  string file_name = ConvertFileName (src_file_name);
  
    //пытаемся найти файл не используя путей поиска
  
  if (file_name [0] == '/')
    return FindMountFileSystem (file_name.c_str (),result_file_name);
 
    //пытаемся найти файл по дефолтному пути поиска
 
  string             full_name         = format ("%s/%s",default_path.c_str (),file_name.c_str ()), mount_name;
  ICustomFileSystemPtr owner_file_system = FindMountFileSystem (full_name.c_str (),mount_name);

  if (owner_file_system && owner_file_system->IsFileExist (mount_name.c_str ()))
  {
    swap (result_file_name,mount_name);
    return owner_file_system;
  }

  string             default_mount_name  = mount_name;
  ICustomFileSystemPtr default_file_system = owner_file_system;

    //пытаемся найти файл в списке путей поиска

  for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
  {
    full_name         = format ("%s/%s",i->path.c_str (),file_name.c_str ());
    owner_file_system = FindMountFileSystem (full_name.c_str (),mount_name);

    if (owner_file_system && owner_file_system->IsFileExist (mount_name.c_str ()))
    {
      swap (result_file_name,mount_name);
      return owner_file_system;
    }
  }

    //пытаемся найти файл в списке пак-файлов

  for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
    if (i->file_system->IsFileExist (file_name.c_str ()))
    {
      swap (result_file_name,file_name);
      return i->file_system;
    }
    
    //возвращаем ссылку на файловую систему включающую путь по умолчанию      

  if (!default_file_system)
    Raise<FileNotFoundException> (NULL,"File '%s' does not belong to any file system",src_file_name);

  swap (result_file_name,default_mount_name);

  return default_file_system;
}

/*
    Открытие файла
*/

FileImplPtr FileSystemImpl::OpenFile (const char* src_file_name,filemode_t mode_flags,size_t buffer_size)
{
  if (!src_file_name)
    RaiseNullArgument ("FileSystemImpl::OpenFile","file_name");

  string file_name;

  ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);

  try
  {    
    ICustomFileSystem::file_t file = file_system->FileOpen (file_name.c_str (),mode_flags,buffer_size);
    
    try
    {
      FileImplPtr base_file (new CustomFileImpl (file_system,file,mode_flags,true),false);
      
      size_t self_buffer_size = base_file->GetBufferSize ();
      
      if (!buffer_size || self_buffer_size >= buffer_size)
        return base_file;

      if (base_file->Size () < default_file_buffer_size && !(mode_flags & (FILE_MODE_RESIZE|FILE_MODE_WRITE)))
        return FileImplPtr (new MemFileImpl (base_file), false);

      return FileImplPtr (new BufferedFileImpl (base_file,buffer_size), false);
    }
    catch (...)
    {
      file_system->FileClose (file);
      throw;
    }
  }
  catch (FileException&)
  {
    throw;
  }
  catch (std::exception& exception)
  {
    Raise<FileLoadException> ("FileSystemImpl::OpenFile","Unknown exception at open file '%s': %s",src_file_name,exception.what ());
  }
  catch (...)
  {
    Raise<FileLoadException> ("FileSystemImpl::OpenFile","Unknown exception at open file '%s'",src_file_name);
  }
  
  return NULL;
}

FileImplPtr FileSystemImpl::OpenFile (const char* src_file_name,filemode_t mode_flags)
{
  return OpenFile (src_file_name,mode_flags,default_file_buffer_size);
}

void FileSystemImpl::SetDefaultFileBufferSize (size_t buffer_size)
{
  default_file_buffer_size = buffer_size;
}

size_t FileSystemImpl::GetDefaultFileBufferSize () const
{
  return default_file_buffer_size;
}

/*
    Управление расположением файлов
*/

void FileSystemImpl::Remove (const char* src_file_name)
{
  if (!src_file_name)
    RaiseNullArgument ("FileSystem::Remove","file_name");
    
  string file_name;
  
  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);
    
    file_system->Remove (file_name.c_str ());
  }
  catch (Exception& exception)
  {
    exception.Raise ("FileSystem::Remove");
  }
}

void FileSystemImpl::Rename (const char* src_file_name,const char* new_name)
{
  if (!src_file_name)
    RaiseNullArgument ("FileSystem::Rename","file_name");
  
  if (!new_name)
    RaiseNullArgument ("FileSystem::Rename","new_name");

  string file_name;
  
  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);
    
    file_system->Rename (file_name.c_str (),new_name);
  }
  catch (Exception& exception)
  {
    exception.Raise ("FileSystem::Rename");
  }
}

void FileSystemImpl::Mkdir (const char* src_dir_name)
{
  if (!src_dir_name)
    RaiseNullArgument ("FileSystem::Mkdir","dir_name");

  string dir_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_dir_name,dir_name);
    
    file_system->Mkdir (dir_name.c_str ());
  }
  catch (Exception& exception)
  {
    exception.Raise ("FileSystem::Mkdir");
  }
}

/*
    Получение информации о файле
*/

bool FileSystemImpl::IsFileExist (const char* src_file_name)
{
  try
  {
    string file_name;
    return src_file_name && FindFileSystem (src_file_name,file_name);    
  }
  catch (...)
  {
    return false;
  }
}

bool FileSystemImpl::GetFileInfo (const char* src_file_name,FileInfo& info)
{
  if (!src_file_name)
    return false;
    
  try
  {
    string file_name;

    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);    

    return file_system->GetFileInfo (file_name.c_str (),info);
  }
  catch (...)
  {
    return false;
  }
}

/*
    FileSystem
*/

/*
    Оболочки над методами работы с файловой системой
*/

void FileSystem::AddSearchPath (const char* path,const LogHandler& log_handler)
{
  FileSystemSingleton::Instance ().AddSearchPath (path,log_handler);
}

static void DummyLogHandler (const char*)
{
}  

void FileSystem::AddSearchPath (const char* path)
{
  FileSystemSingleton::Instance ().AddSearchPath (path,DummyLogHandler);
}

void FileSystem::RemoveSearchPath (const char* path)
{
  FileSystemSingleton::Instance ().RemoveSearchPath (path);
}

void FileSystem::RemoveAllSearchPaths ()
{
  FileSystemSingleton::Instance ().RemoveAllSearchPaths ();
}

void FileSystem::RegisterPackFile (const char* extension,const PackFileCreater& creater)
{
  FileSystemSingleton::Instance ().RegisterPackFile (extension,creater);
}

void FileSystem::UnregisterPackFile (const char* extension)
{
  FileSystemSingleton::Instance ().UnregisterPackFile (extension);
}

void FileSystem::Mount (const char* path_prefix,ICustomFileSystemPtr file_system)
{
  FileSystemSingleton::Instance ().Mount (path_prefix,file_system);
}

void FileSystem::Unmount (const char* path_prefix)
{
  FileSystemSingleton::Instance ().Unmount (path_prefix);
}

void FileSystem::Unmount (ICustomFileSystemPtr file_system)
{
  FileSystemSingleton::Instance ().Unmount (file_system);
}

void FileSystem::UnmountAll ()
{
  FileSystemSingleton::Instance ().UnmountAll ();
}

bool FileSystem::IsPathMount (const char* path_prefix)
{
  return FileSystemSingleton::Instance ().IsPathMount (path_prefix);
}

void FileSystem::Remove (const char* file_name)
{
  FileSystemSingleton::Instance ().Remove (file_name);
}

void FileSystem::Rename (const char* file_name,const char* new_name)
{
  FileSystemSingleton::Instance ().Rename (file_name,new_name);
}

void FileSystem::Mkdir (const char* dir_name)
{
  FileSystemSingleton::Instance ().Mkdir (dir_name);
}

bool FileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  return FileSystemSingleton::Instance ().GetFileInfo (file_name,info);
}

bool FileSystem::IsFileExist (const char* file_name)
{
  return FileSystemSingleton::Instance ().IsFileExist (file_name);
}

bool FileSystem::IsDir (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ().GetFileInfo (file_name,info) ? info.is_dir : false;
}

filetime_t FileSystem::GetFileTime (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ().GetFileInfo (file_name,info) ? info.time_modify : 0;
}

filesize_t FileSystem::GetFileSize (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ().GetFileInfo (file_name,info) ? info.size : 0;
}

FileList FileSystem::Search (const char* wc_mask,size_t flags)
{
  return FileSystemSingleton::Instance ().Search (wc_mask,flags);
}

void FileSystem::SetDefaultFileBufferSize (size_t buffer_size)
{
  FileSystemSingleton::Instance ().SetDefaultFileBufferSize (buffer_size);
}

size_t FileSystem::GetDefaultFileBufferSize ()
{
  return FileSystemSingleton::Instance ().GetDefaultFileBufferSize ();
}

void FileSystem::SetCurrentDir (const char* path)
{
  FileSystemSingleton::Instance ().SetDefaultPath (path);
}

const char* FileSystem::GetCurrentDir ()
{
  return FileSystemSingleton::Instance ().GetDefaultPath ();
}

/*
    Получение MD5 хэша файла
*/

static void InternalGetFileHash (File& file,FileHash& hash)
{
  char       buf [FILE_HASH_BUF_SIZE];
  MD5Context md5;
  size_t     crc32_hash = 0xFFFFFFFF;

  while (!file.Eof ())
  {
    size_t size = file.Read (buf,sizeof (buf));
    
    md5.Update (buf,size);
    
    crc32_hash = crc32 (buf,size,crc32_hash);
  }
  
  md5.Finish (hash.md5);
  
  hash.crc32 = crc32_hash;
}

void FileSystem::GetFileHash (File& file,FileHash& hash)
{
  if (!file.CanRead ())
    RaiseNotSupported ("FileSystem::GetFileHash","Unable to compute FileHash. Reason: file can't be read");
    
  filepos_t current_position = file.Tell ();

  if (current_position)
  {
    if (!file.CanSeek ())
      RaiseNotSupported ("FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be seek");

    file.Seek (0);  

    InternalGetFileHash (file,hash);

    file.Seek (current_position);
  }
  else
  {
    if (!file.CanRewind ())
      RaiseNotSupported ("FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be rewind");

    InternalGetFileHash (file,hash);

    file.Rewind ();
  }
}

void FileSystem::GetFileHash (const char* file_name,FileHash& hash)
{
  if (!file_name)
    RaiseNullArgument ("FileSystem::GetFileHash","file_name");

  try
  {
    InputFile file (file_name);

    InternalGetFileHash (file,hash);
  }
  catch (FileException& exception)
  {
    exception.Raise ("FileSystem::GetFileHash");
  }
}
