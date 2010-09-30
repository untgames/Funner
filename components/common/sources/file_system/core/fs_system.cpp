#include "shared.h"

using namespace stl;
using namespace common;
using namespace xtl;

namespace
{

const char*  FILE_SYSTEM_ADDONS_MASK              = "common.file_systems.*"; //маска имён компонентов, пользовательских файловых систем
const char*  ANONYMOUS_FILES_PREFIX               = "/anonymous";            //префикс имён анонимных файлов
const size_t MAX_SYMBOLIC_LINKS_REPLACEMENT_COUNT = 64;                      //максимальное количество подстановок символьных ссылок

}

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
  
inline SymbolicLink::SymbolicLink (const char* in_prefix, const char* in_link)
  : prefix (in_prefix)
  , link (in_link)  
{  
}

/*
    Инициализация / завершение
*/

FileSystemImpl::FileSystemImpl ()
{
  default_file_buffer_size = DEFAULT_FILE_BUF_SIZE;
  anonymous_file_system    = AnonymousFileSystemPtr (new AnonymousFileSystem, false);

  ClosedFileImpl::Instance ();

  Mount ("/std",Platform::GetFileSystem ());
  Mount ("/io",Platform::GetIOSystem ());
  Mount (ANONYMOUS_FILES_PREFIX,anonymous_file_system.get ());

  default_path = "/std";
}

FileSystemImpl::~FileSystemImpl ()
{  
  RemoveAllSearchPaths ();
  UnmountAll ();
}

/*
    Загрузка файловых систем
*/

void FileSystemImpl::LoadFileSystems ()
{
  static bool initialized = false;
  
  if (initialized)
    return;
    
  initialized = true;

  static ComponentLoader custom_file_systems_loader (FILE_SYSTEM_ADDONS_MASK);
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
  string new_path = path ? FileSystem::GetNormalizedFileName (path) : "/";

  if (new_path [0] != '/')
    new_path = CompressPath (format ("%s/%s",default_path.c_str (),new_path.c_str ()).c_str ());

  if (!new_path.empty () && new_path.end ()[-1] == '/')
    new_path.erase (new_path.end ()-1);

  FileInfo info;

  if (!GetFileInfo (new_path.c_str (),info))
    throw xtl::format_operation_exception ("common::FileSystem::SetCurrentDir","Dir '%s' does not exist",new_path.c_str ());

  if (!info.is_dir)
    throw xtl::format_operation_exception ("common::FileSystem::SetCurrentDir","Can not set current dir '%s' because it is not dir",new_path.c_str ());

  swap (default_path,new_path);
}

/*
    Работа с путями анонимных файлов
*/

stl::string FileSystemImpl::AddAnonymousFilePath (const FileImplPtr& file)
{
  return common::format ("%s/%s", ANONYMOUS_FILES_PREFIX, anonymous_file_system->AddFilePath (file));
}

void FileSystemImpl::RemoveAnonymousFilePath (const FileImplPtr& file)
{
  anonymous_file_system->RemoveFilePath (file);
}

void FileSystemImpl::RemoveAllAnonymousFilePaths ()
{
  anonymous_file_system->RemoveAllFilePaths ();
}

/*
    Регистрация типов пак-файлов
*/

void FileSystemImpl::RegisterPackFile (const char* extension,const PackFileCreater& creater)
{
  if (!extension)
    throw xtl::make_null_argument_exception ("common::FileSystem::RegisterPackFile","extension");

  if (!creater)
    throw xtl::make_null_argument_exception ("common::FileSystem::RegisterPackFile","creater");

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
    throw xtl::make_null_argument_exception ("common::FileSystem::UnregisterPackFile","extension");

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
  LoadFileSystems ();

  string path = FileSystem::GetNormalizedFileName (_path), type = suffix (path);

  if (type.empty ())
    return;

  type.erase (type.begin ());

  volatile size_t path_hash = strhash (path.c_str ()), type_hash = strhash (type.c_str ());

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

        pack_files.push_front (PackFile (strihash (path.c_str ()),search_path_hash,pack_file_system));

        return;
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
    throw xtl::make_null_argument_exception ("common::FileSystem::AddSearchPath","path");

  if (!log_handler)
    throw xtl::make_null_argument_exception ("common::FileSystem::AddSearchPath","log_handler");

  string path = FileSystem::GetNormalizedFileName (_path), mount_path;

  ICustomFileSystemPtr owner_file_system = FindFileSystem (path.c_str (),mount_path);
  FileInfo           file_info;
  
  if (!owner_file_system || !owner_file_system->GetFileInfo (mount_path.c_str (),file_info))
  {
    path = FileSystem::GetNormalizedFileName (_path);
    
    log_handler (format ("Search path '%s' doesn't exist (try to create)",path.c_str ()).c_str ());
    
    AddPackFile (path.c_str (),0,log_handler);
    
    return;
  }
  
  if (path [0] != '/')
    path = format ("%s/%s",default_path.c_str (),path.c_str ());

  size_t path_hash = strhash (path.c_str ());

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
    throw xtl::make_null_argument_exception ("common::FileSystem::RemoveSearchPath","path");

  string path = FileSystem::GetNormalizedFileName (_path);

  if (path [0] != '/')
    path = format ("%s/%s",default_path.c_str (),path.c_str ());

  size_t hash = strhash (path.c_str ());

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
    throw xtl::make_null_argument_exception ("common::FileSystem::Mount","path_prefix");

  if (!file_system)
    throw xtl::make_null_argument_exception ("common::FileSystem::Mount","file_system");

  if (*_path_prefix != '/')
    throw xtl::make_argument_exception ("common::FileSystem::Mount","path_prefix",_path_prefix,"Mount path must start from '/'");

  string prefix = FileSystem::GetNormalizedFileName (_path_prefix);
  size_t hash   = strhash (prefix.c_str ());

  prefix += '/';

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (!strncmp (i->prefix.c_str (),prefix.c_str (),i->prefix.size ()))
      throw xtl::format_operation_exception ("common::FileSystem::Mount","Can not mount file system '%s' "
                                 "because it intersects with already mounted file system '%s'",
                                 prefix.c_str (),i->prefix.c_str ());

  mounts.push_front (MountFileSystem (prefix.c_str (),hash,file_system));
}

void FileSystemImpl::Mount (const char* path_prefix,const char* _path,bool link,const char* force_extension)
{
  static const char* METHOD_NAME = "common::FileSystem::Mount";

  if (!path_prefix)
    throw xtl::make_null_argument_exception (METHOD_NAME,"path_prefix");

  if (!_path)
    throw xtl::make_null_argument_exception (METHOD_NAME,"path");

  LoadFileSystems ();    

  string path = FileSystem::GetNormalizedFileName (_path);

  FileInfo info;

  if (GetFileInfo (_path,info) && info.is_dir || link)
  {
    if (!*path_prefix)
      throw xtl::make_argument_exception (METHOD_NAME, "path_prefix", path_prefix, "Path prefix must be non empty");

      //добавление символьной ссылки

    stl::string prefix = path_prefix;

    if (!prefix.empty () && prefix [prefix.size ()-1] != '/')
      prefix += "/";

    for (SymbolicLinkList::iterator iter=symbolic_links.begin (), end=symbolic_links.end (); iter!=end; ++iter)
      if (!xstrncmp (prefix.c_str (), iter->prefix.c_str (), iter->prefix.size ()))
        throw xtl::format_operation_exception (METHOD_NAME, "Path '%s' has already linked to '%s'", path_prefix, iter->link.c_str ());

    symbolic_links.push_back (SymbolicLink (prefix.c_str (), path.c_str ()));
  }
  else
  {
    string type;
    
    if (force_extension)
    {
      type = force_extension;
    }
    else
    {
      type = suffix (path);
      
      if (!type.empty ())
        type.erase (type.begin ());    
    }
    
    size_t path_hash = strhash (path.c_str ()), type_hash = strhash (type.c_str ());

    for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
      if (i->file_name_hash == path_hash)
      {
        Mount (path_prefix, i->file_system.get ());
        return;
      }

    for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
      if (i->extension_hash == type_hash)
      {
        ICustomFileSystemPtr pack_file_system (i->creater (path.c_str ()),false);

        pack_files.push_front (PackFile (strihash (path.c_str ()),0,pack_file_system));                  

        try
        {
          Mount (path_prefix, pack_file_system.get ());          
        }
        catch (...)
        {
          pack_files.pop_front ();
          throw;
        }
        
        return;
      }

    throw xtl::format_operation_exception (METHOD_NAME, "Fail to open pack-file '%s'. Undefined type '%s'",_path,type.c_str ());
  }
}

void FileSystemImpl::Unmount (const char* _path_prefix)
{
  if (!_path_prefix)
    return;

  string prefix = FileSystem::GetNormalizedFileName (_path_prefix);
  size_t hash   = strhash (prefix.c_str ());

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (i->hash == hash && !strncmp (prefix.c_str (),i->prefix.c_str (),i->prefix.size ()-1))
    {
      mounts.erase (i);
      return;
    }
    
  prefix  = _path_prefix;  
  prefix += "/";
    
  for (SymbolicLinkList::iterator i=symbolic_links.begin (), end=symbolic_links.end (); i!=end; ++i)
    if (!xstrncmp (prefix.c_str (), i->prefix.c_str (), i->prefix.size ()))
    {
      symbolic_links.erase (i);
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
  symbolic_links.clear ();
}

bool FileSystemImpl::IsPathMount (const char* path) const
{
  if (!path)
    return false;

  string prefix = FileSystem::GetNormalizedFileName (path);
  size_t hash   = strhash (prefix.c_str ());

  for (MountList::const_iterator i=mounts.begin ();i!=mounts.end ();++i)
    if (i->hash == hash && !strncmp (prefix.c_str (),i->prefix.c_str (),i->prefix.size ()-1))
      return true;

  return false;
}

/*
    Настройка шифрования
*/

void FileSystemImpl::SetCryptoParameters (const char* path, const FileCryptoParameters& parameters)
{
  try
  {
    if (!path)
      throw xtl::make_null_argument_exception ("", "path");

    CryptoMap::iterator iter = crypto_parameters.find (path);

    if (iter != crypto_parameters.end ())
    {
      iter->second = parameters;
    }
    else
    {
      crypto_parameters.insert_pair (path, parameters);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystemImpl::SetCryptoParameters");
    throw;
  }
}

bool FileSystemImpl::HasCryptoParameters (const char* path) const
{
  if (!path)
    return false;

  return crypto_parameters.find (path) != crypto_parameters.end ();
}

FileCryptoParameters FileSystemImpl::GetCryptoParameters (const char* path) const
{
  static const char* METHOD_NAME = "common::FileSystemImpl::GetCryptoParameters";

  if (!path)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path");

  CryptoMap::const_iterator iter = crypto_parameters.find (path);

  if (iter == crypto_parameters.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "path", path, "No crypto parameters assigned");

  return iter->second;
}

void FileSystemImpl::RemoveCryptoParameters (const char* path)
{
  if (!path)
    return;

  crypto_parameters.erase (path);
}

void FileSystemImpl::RemoveAllCryptoParameters ()
{
  crypto_parameters.clear ();
}

/*
    Определение принадлежности файла к файловой системе
*/

ICustomFileSystemPtr FileSystemImpl::FindMountFileSystem (const char* file_name,string& result_file_name)
{
  LoadFileSystems ();

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
  {
    if (!xtl::xstrncmp (i->prefix.c_str (),file_name,i->prefix.size ()-1))
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
  static const char* METHOD_NAME = "common::FileSystemImpl::FindFileSystem";  
  
  LoadFileSystems ();  

  string file_name = FileSystem::GetNormalizedFileName (src_file_name);
  
    //обработка символьных ссылок

  size_t replacement_count = 0;  

  for (SymbolicLinkList::iterator iter=symbolic_links.begin (), end=symbolic_links.end (); iter!=end;)
    if (!xstrncmp (file_name.c_str (), iter->prefix.c_str (), iter->prefix.size () - 1))    
    {
      if (file_name.size () > iter->prefix.size () && file_name [iter->prefix.size () - 1] == '/')
      {
        file_name.replace (0, iter->prefix.size () - 1, iter->link);
      }
      else if (file_name.size () == iter->prefix.size () - 1)
      {
        file_name = iter->link;
      }
      else
      {
        ++iter;
        continue;
      }
      
      file_name = FileSystem::GetNormalizedFileName (file_name.c_str ());

      replacement_count++;
      
      if (replacement_count < MAX_SYMBOLIC_LINKS_REPLACEMENT_COUNT)
        iter = symbolic_links.begin ();
    }    
    else ++iter;
  
    //пытаемся найти файл не используя путей поиска

  if (file_name [0] == '/')
  {
    ICustomFileSystemPtr return_value = FindMountFileSystem (file_name.c_str (),result_file_name);

    if (!return_value)
      throw xtl::make_argument_exception (METHOD_NAME, "src_file_name", src_file_name, "Can't find mount file system for this file");

    return return_value;
  }

    //пытаемся найти файл по дефолтному пути поиска

  string               full_name         = format ("%s/%s",default_path.c_str (),file_name.c_str ()), mount_name;
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
    throw format_operation_exception (METHOD_NAME, "File '%s' does not belong to any file system",src_file_name);

  swap (result_file_name,default_mount_name);

  return default_file_system;
}

/*
    Открытие файла
*/

FileImplPtr FileSystemImpl::OpenFile (const char* src_file_name,filemode_t mode_flags,size_t buffer_size)
{
  if (!src_file_name)
    throw xtl::make_null_argument_exception ("FileSystemImpl::OpenFile","file_name");

  string file_name;

  ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);

  try
  {
    FileImplPtr base_file;
    
    if (file_system == anonymous_file_system.get ())
    {
      base_file = FileImplPtr (new BufferedFileImpl (anonymous_file_system->RetainFile (file_name.c_str (), mode_flags), buffer_size), false);
    }
    else
    {
      volatile ICustomFileSystem::file_t file = file_system->FileOpen (file_name.c_str (),mode_flags,buffer_size);      

      try
      {
        xtl::intrusive_ptr<CustomFileImpl> custom_file (new CustomFileImpl (file_system,file,mode_flags,true),false);
        
        file = 0;
        
        custom_file->SetPath (src_file_name);
        
        base_file = custom_file;
      }
      catch (...)
      {
        if (file)
          file_system->FileClose (file);

        throw;
      }
    }         

    if (HasCryptoParameters (src_file_name))
    {
      FileCryptoParameters params = GetCryptoParameters (src_file_name);

      base_file = FileImplPtr (new CryptoFileImpl (base_file, buffer_size, params.ReadMethod (), params.WriteMethod (), params.Key (), params.KeyBits ()), false);
    }

    size_t self_buffer_size = base_file->GetBufferSize ();

    if (!buffer_size || self_buffer_size >= buffer_size || (self_buffer_size >= base_file->Size () && !(base_file->Mode () & FileMode_Resize)))
      return base_file;

    if (base_file->Size () < default_file_buffer_size && !(mode_flags & (FileMode_Resize|FileMode_Write)))
      return FileImplPtr (new MemFileImpl (base_file), false);

    return FileImplPtr (new BufferedFileImpl (base_file,buffer_size), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystemImpl::OpenFile()");
    throw;
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
    throw xtl::make_null_argument_exception ("common::FileSystem::Remove","file_name");

  string file_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);

    file_system->Remove (file_name.c_str ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::Remove");
    throw;
  }
}

void FileSystemImpl::Rename (const char* src_file_name,const char* new_name)
{
  if (!src_file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::Rename","file_name");

  if (!new_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::Rename","new_name");

  string file_name, new_file_name;

  try
  {
    ICustomFileSystemPtr file_system     = FindFileSystem (src_file_name,file_name);
    ICustomFileSystemPtr new_file_system = FindFileSystem (new_name,new_file_name);
    
    if (new_file_system != file_system)
      throw xtl::format_operation_exception ("", "Can't rename file '%s' to '%s'. Different file systems", src_file_name, new_name);

    file_system->Rename (file_name.c_str (),new_file_name.c_str ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::Rename");
    throw;
  }
}

namespace
{

void MkdirRecursive (ICustomFileSystem& fs, const char* dir_name)
{
  stl::string parent_dir_name = dir (dir_name);
  
  parent_dir_name.pop_back ();
  
  FileInfo info;
  
  if (parent_dir_name != "." && !(fs.GetFileInfo (parent_dir_name.c_str (), info) && info.is_dir))
    MkdirRecursive (fs, parent_dir_name.c_str ());

  fs.Mkdir (dir_name);
}

}

void FileSystemImpl::Mkdir (const char* src_dir_name)
{
  if (!src_dir_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::Mkdir","dir_name");

  string dir_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_dir_name,dir_name);
    
    MkdirRecursive (*file_system, dir_name.c_str ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::Mkdir");
    throw;
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

    if (!src_file_name)
      return false;

    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name,file_name);

    if (!file_system)        
      return false;
      
    if (!file_name.empty () && file_name [file_name.size ()-1] == '/')
      file_name.pop_back ();

    return file_system->IsFileExist (file_name.c_str ());
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
    
    if (!file_name.empty () && file_name [file_name.size () - 1] == '/')
      file_name.pop_back ();

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
  FileSystemSingleton::Instance ()->AddSearchPath (path,log_handler);
}

static void DummyLogHandler (const char*)
{
}

void FileSystem::AddSearchPath (const char* path)
{
  FileSystemSingleton::Instance ()->AddSearchPath (path,DummyLogHandler);
}

void FileSystem::RemoveSearchPath (const char* path)
{
  FileSystemSingleton::Instance ()->RemoveSearchPath (path);
}

void FileSystem::RemoveAllSearchPaths ()
{
  FileSystemSingleton::Instance ()->RemoveAllSearchPaths ();
}

void FileSystem::RegisterPackFile (const char* extension,const PackFileCreater& creater)
{
  FileSystemSingleton::Instance ()->RegisterPackFile (extension,creater);
}

void FileSystem::UnregisterPackFile (const char* extension)
{
  FileSystemSingleton::Instance ()->UnregisterPackFile (extension);
}

void FileSystem::Mount (const char* path_prefix,ICustomFileSystemPtr file_system)
{
  FileSystemSingleton::Instance ()->Mount (path_prefix,file_system);
}

void FileSystem::Mount (const char* path_prefix,const char* path,const char* force_extension)
{
  FileSystemSingleton::Instance ()->Mount (path_prefix,path,false,force_extension);
}

void FileSystem::MountLink (const char* path_prefix, const char* path)
{
  FileSystemSingleton::Instance ()->Mount (path_prefix,path,true,0);
}

void FileSystem::Unmount (const char* path_prefix)
{
  FileSystemSingleton::Instance ()->Unmount (path_prefix);
}

void FileSystem::Unmount (ICustomFileSystemPtr file_system)
{
  FileSystemSingleton::Instance ()->Unmount (file_system);
}

void FileSystem::UnmountAll ()
{
  FileSystemSingleton::Instance ()->UnmountAll ();
}

bool FileSystem::IsPathMount (const char* path_prefix)
{
  return FileSystemSingleton::Instance ()->IsPathMount (path_prefix);
}

void FileSystem::Remove (const char* file_name)
{
  FileSystemSingleton::Instance ()->Remove (file_name);
}

void FileSystem::Rename (const char* file_name,const char* new_name)
{
  FileSystemSingleton::Instance ()->Rename (file_name,new_name);
}

void FileSystem::Mkdir (const char* dir_name)
{
  FileSystemSingleton::Instance ()->Mkdir (dir_name);
}

bool FileSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  return FileSystemSingleton::Instance ()->GetFileInfo (file_name,info);
}

bool FileSystem::IsFileExist (const char* file_name)
{
  return FileSystemSingleton::Instance ()->IsFileExist (file_name);
}

bool FileSystem::IsDir (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ()->GetFileInfo (file_name,info) ? info.is_dir : false;
}

filetime_t FileSystem::GetFileTime (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ()->GetFileInfo (file_name,info) ? info.time_modify : 0;
}

filesize_t FileSystem::GetFileSize (const char* file_name)
{
  FileInfo info;

  return FileSystemSingleton::Instance ()->GetFileInfo (file_name,info) ? info.size : 0;
}

FileList FileSystem::Search (const char* wc_mask,size_t flags)
{
  return FileSystemSingleton::Instance ()->Search (wc_mask,flags);
}

void FileSystem::SetDefaultFileBufferSize (size_t buffer_size)
{
  FileSystemSingleton::Instance ()->SetDefaultFileBufferSize (buffer_size);
}

size_t FileSystem::GetDefaultFileBufferSize ()
{
  return FileSystemSingleton::Instance ()->GetDefaultFileBufferSize ();
}

void FileSystem::SetCurrentDir (const char* path)
{
  FileSystemSingleton::Instance ()->SetDefaultPath (path);
}

const char* FileSystem::GetCurrentDir ()
{
  return FileSystemSingleton::Instance ()->GetDefaultPath ();
}

/*
    Получение MD5 хэша файла
*/

static void InternalGetFileHash (File& file,FileHash& hash)
{
  char       buf [FILE_HASH_BUF_SIZE];
  Md5Context md5;
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
    throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Unable to compute FileHash. Reason: file can't be read");

  filepos_t current_position = file.Tell ();

  if (current_position)
  {
    if (!file.CanSeek ())
      throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be seek");

    file.Seek (0);

    InternalGetFileHash (file,hash);

    file.Seek (current_position);
  }
  else
  {
    if (!file.CanRewind ())
      throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be rewind");

    InternalGetFileHash (file,hash);

    file.Rewind ();
  }
}

void FileSystem::GetFileHash (const char* file_name,FileHash& hash)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::GetFileHash","file_name");

  try
  {
    InputFile file (file_name);

    InternalGetFileHash (file,hash);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::GetFileHash");
    throw;
  }
}

/*
    Приведение файлового имени к стандартному виду
*/

string FileSystem::GetNormalizedFileName (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::GetNormalizedFileName", "file_name");

  string res = file_name;

  for (string::iterator i=res.begin ();i!=res.end ();++i)
    switch (*i)
    {
      case '\\':
        *i = '/';
        break;
      case '<': //замена шаблона <VarName> на значение переменной VarName
      {
        string::iterator end = strchr (i, '>');

        if (!end)
          continue;

        stl::string var_name (i + 1, end);
        const char* var_value = getenv (var_name.c_str ());

        if (!var_value)
          var_value = "";

        size_t offset = i - res.begin ();


        res.replace (i, end + 1, var_value);

        i   = res.begin () + offset;
        end = i + strlen (var_value);

        for (;i!=end; i++)
          switch (*i)
          {
            case '\\': *i = '/'; break;
            default: break;
          }

        --i;

        break;
      }
      default:
        break;
    }

  return res;
}

/*
    Чтение текстового файла в строку
*/

void FileSystem::LoadTextFile (const char* file_name, string& buffer)
{
  try
  {
    InputFile file (file_name);

    buffer.resize (file.Size (), ' ');

    file.Read (&buffer [0], file.Size ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::LoadTextFile");
    throw;
  }
}

string FileSystem::LoadTextFile (const char* file_name)
{
  string buffer;

  LoadTextFile (file_name, buffer);

  return buffer;
}

/*
    Настройка шифрования
*/

void FileSystem::SetCryptoParameters
 (const char* path,
  const char* read_crypto_method,
  const char* write_crypto_method,
  const void* key,
  size_t      key_bits)
{
  SetCryptoParameters (path, FileCryptoParameters (read_crypto_method, write_crypto_method, key, key_bits));
}

void FileSystem::SetCryptoParameters (const char* path, const FileCryptoParameters& parameters)
{
  FileSystemSingleton::Instance ()->SetCryptoParameters (path, parameters);
}

bool FileSystem::HasCryptoParameters (const char* path)
{
  return FileSystemSingleton::Instance ()->HasCryptoParameters (path);
}

FileCryptoParameters FileSystem::GetCryptoParameters (const char* path)
{
  return FileSystemSingleton::Instance ()->GetCryptoParameters (path);
}

void FileSystem::RemoveCryptoParameters (const char* path)
{
  FileSystemSingleton::Instance ()->RemoveCryptoParameters (path);
}

void FileSystem::RemoveAllCryptoParameters ()
{
  FileSystemSingleton::Instance ()->RemoveAllCryptoParameters ();
}

/*
    Получение ключа шифрования по файлу
*/

void FileSystem::GetFileCryptoKey (const char* file_name, filecryptokey_t key)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    FileHash file_hash;

    GetFileHash (file_name, file_hash);

    memcpy (key, file_hash.md5, sizeof (filecryptokey_t) < sizeof (file_hash.md5) ? sizeof (filecryptokey_t) : sizeof (file_hash.md5));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::GetFileCryptoKey");
    throw;
  }
}

/*
    Копирование файла
*/

void FileSystem::CopyFile (const char* source_file_name, const char* destination_file_name, size_t buffer_size)
{
  try
  {
    if (!source_file_name)
      throw xtl::make_null_argument_exception ("", "source_file_name");

    if (!destination_file_name)
      throw xtl::make_null_argument_exception ("", "destination_file_name");
      
    if (!buffer_size)
      buffer_size = GetDefaultFileBufferSize ();
      
    if (!buffer_size)
      buffer_size = 4096;

    InputFile input_file (source_file_name);
    OutputFile output_file (destination_file_name);

    if (output_file.Size ())
      output_file.Resize (0);

    xtl::uninitialized_storage<char> buffer (buffer_size);

    while (!input_file.Eof ())
    {
      size_t read_size = input_file.Read (buffer.data (), buffer.size ());

      if (!read_size)
        break;

      output_file.Write (buffer.data (), read_size);
    }

    if (!input_file.Eof ())
      throw xtl::format_operation_exception ("", "Internal error: can't read input file");
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::FileSystem::CopyFile");
    throw;
  }
}
