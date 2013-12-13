#include "shared.h"

using namespace stl;
using namespace common;
using namespace xtl;

namespace
{

const char*  FILE_SYSTEM_ADDONS_MASK              = "common.file_systems.*"; //маска имён компонентов, пользовательских файловых систем
const char*  ANONYMOUS_FILES_PREFIX               = "/anonymous";            //префикс имён анонимных файлов
const size_t MAX_SYMBOLIC_LINKS_REPLACEMENT_COUNT = 64;                      //максимальное количество подстановок символьных ссылок
const char*  LOG_NAME                             = "common.file_systems";   //имя лога

struct RemoveFileWithPrefix
{
  FileSystemImpl *file_system;
  stl::string    prefix_name;

  RemoveFileWithPrefix (FileSystemImpl* in_file_system, const char* in_prefix_name)
    : file_system (in_file_system)
    , prefix_name (in_prefix_name)
  {}

  void operator () (const char* file_name, const FileInfo&)
  {
    stl::string full_name = prefix_name + file_name;

    file_system->Remove (full_name.c_str ());
  }
};

void background_copy_file_notify (const FileSystem::BackgroundCopyFileCallback& callback, ActionThread thread, const BackgroundCopyState& copy_state)
{
  if (thread == ActionThread_Background)
    callback (copy_state);
  else
    ActionQueue::PushAction (xtl::bind (callback, copy_state.Clone ()), ActionThread_Main);
}

//Рабочие данные процесса фонового копирования файла
struct BackgroundCopyFileData : public xtl::reference_counter
{
  BackgroundCopyState                    copy_state;            //Состояние копирования
  File                                   input_file;            //Файл для чтения
  File                                   output_file;           //Файл для записи
  stl::string                            source_file_name;      //Имя исходного файла
  stl::string                            destination_file_name; //Имя целевого файла
  FileSystem::BackgroundCopyFileCallback callback;              //Колбек
  ActionThread                           thread;                //Нить вызова колбека
  size_t                                 buffer_size;           //Размер буфера копирования
  size_t                                 bytes_copied;          //Объем скопированных данных
  xtl::uninitialized_storage<char>       buffer;                //Буфер копирования
  bool                                   started;               //Началось ли копирование
  bool                                   finished;              //Завершилось ли копирование

  BackgroundCopyFileData (const char* in_source_file_name, const char* in_destination_file_name,
                          const FileSystem::BackgroundCopyFileCallback& in_callback,
                          ActionThread in_thread, size_t in_buffer_size)
    : source_file_name (in_source_file_name)
    , destination_file_name (in_destination_file_name)
    , callback (in_callback)
    , thread (in_thread)
    , buffer_size (in_buffer_size)
    , bytes_copied (0)
    , buffer (in_buffer_size)
    , started (false)
    , finished (false)
    {}

  ~BackgroundCopyFileData ()
  {
    if (!finished)
    {
      output_file.Close ();
      FileSystem::Remove (destination_file_name.c_str ());
    }
  }
};

typedef xtl::intrusive_ptr<BackgroundCopyFileData> BackgroundCopyFileDataPtr;

void background_copy_file_impl (Action& action, BackgroundCopyFileDataPtr data)
{
  try
  {
    if (data->started)
    {
      if (data->input_file.Eof ())
      {
        data->output_file.Close ();

        data->copy_state.SetStatus (BackgroundCopyStateStatus_Finished);
        background_copy_file_notify (data->callback, data->thread, data->copy_state);
        action.Cancel ();

        data->finished = true;
      }
      else
      {
        size_t read_size = data->input_file.Read (data->buffer.data (), data->buffer.size ());

        if (!read_size && !data->input_file.Eof ())
          throw xtl::format_operation_exception ("", "Internal error: can't read input file");

        data->output_file.Write (data->buffer.data (), read_size);

        data->bytes_copied += read_size;

        data->copy_state.SetBytesCopied (data->bytes_copied);
        background_copy_file_notify (data->callback, data->thread, data->copy_state);
      }
    }
    else
    {
      data->input_file  = StdFile    (data->source_file_name.c_str (), FileMode_Read);
      data->output_file = OutputFile (data->destination_file_name.c_str ());

      if (data->output_file.Size ())
        data->output_file.Resize (0);

      data->copy_state.SetStatus (BackgroundCopyStateStatus_Started);
      data->copy_state.SetFileSize (data->input_file.Size ());

      background_copy_file_notify (data->callback, data->thread, data->copy_state);

      data->copy_state.SetStatus (BackgroundCopyStateStatus_InProgress);

      data->started = true;
    }
  }
  catch (std::exception& e)
  {
    data->copy_state.SetStatus (BackgroundCopyStateStatus_Failed);
    data->copy_state.SetStatusText (e.what ());
    background_copy_file_notify (data->callback, data->thread, data->copy_state);
    action.Cancel ();
  }
  catch (...)
  {
    data->copy_state.SetStatus (BackgroundCopyStateStatus_Failed);
    data->copy_state.SetStatusText ("Unknown exception");
    background_copy_file_notify (data->callback, data->thread, data->copy_state);
    action.Cancel ();
  }
}

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
  : log (LOG_NAME)
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

  string path = FileSystem::GetNormalizedFileName (_path), mount_path, prefix;

  log.Printf ("Attempt to add search path '%s'", path.c_str ());

  FileInfo file_info;

  memset (&file_info, 0, sizeof (file_info));

  ICustomFileSystemPtr owner_file_system = FindFileSystem (path.c_str (),mount_path,&prefix,true);

  if (!owner_file_system || (!mount_path.empty () && !owner_file_system->GetFileInfo (mount_path.c_str (),file_info)))
  {
    path = FileSystem::GetNormalizedFileName (_path);

    log.Printf ("Search path '%s' doesn't exist (try to create)",path.c_str ());
    log_handler (format ("Search path '%s' doesn't exist (try to create)",path.c_str ()).c_str ());

    AddPackFile (path.c_str (),0,log_handler);

    return;
  }  

  size_t path_hash = strhash (path.c_str ());
  
  if (path [0] != '/')
    path = format ("%s%s",prefix.c_str (),path.c_str ());        

  if (file_info.is_dir || mount_path.empty ())
  {
    for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      if (i->hash == path_hash)
      {
        log.Printf ("Search path '%s' has been already added (path='%s')",path.c_str (),i->path.c_str ());
        log_handler (format ("Search path '%s' has been already added",path.c_str ()).c_str ());
        return;
      }

    FileListBuilder list_builder;

    for (PackFileTypeList::iterator i=pack_types.begin ();i!=pack_types.end ();++i)
      owner_file_system->Search (format ("%s/*.%s",mount_path.c_str (),i->extension.c_str ()).c_str (),
      ICustomFileSystem::FileSearchHandler (bind (&FileListBuilder::Insert, &list_builder, _1, _2)));

    log.Printf ("Add search path '%s'", path.c_str ());

    search_paths.push_front (SearchPath (path.c_str (),path_hash));

    for (FileList::Iterator list_iter=list_builder.Build (true);list_iter;++list_iter)
    {
      if (!list_iter->info.is_dir)
        AddPackFile (list_iter->name,path_hash,log_handler);
    }
  }
  else AddPackFile (path.c_str (),0,log_handler);
}

void FileSystemImpl::RemoveSearchPath (const char* _path)
{
  if (!_path)
    throw xtl::make_null_argument_exception ("common::FileSystem::RemoveSearchPath","path");

  string path = FileSystem::GetNormalizedFileName (_path);

  size_t hash = strhash (path.c_str ());

  log.Printf ("Remove search path '%s'", path.c_str ());

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

  if ((GetFileInfo (_path,info) && info.is_dir) || link)
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

        Mount (path_prefix, pack_file_system.get ());          

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

  for (PackFileList::iterator i=pack_files.begin (), end=pack_files.end (); i!=end; ++i)
  {
    if (i->file_name_hash == hash)
    {
      pack_files.erase (i);
      break;
    }
  }
    
  for (SymbolicLinkList::iterator i=symbolic_links.begin (), end=symbolic_links.end (); i!=end; ++i)
    if (!xstrncmp (prefix.c_str (), i->prefix.c_str (), i->prefix.size ()))
    {
      symbolic_links.erase (i);
      break;
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
   Файловые атрибуты
*/

void FileSystemImpl::SetFileAttribute (const char* src_file_name, const char* attribute, const void* data, size_t size)
{
  static const char* METHOD_NAME = "common::FileSystem::SetFileAttribute";

  if (!src_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  if (!attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "new_name");

  if (size && !data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

  string file_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name, file_name);

    file_system->SetFileAttribute (file_name.c_str (), attribute, data, size);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

void FileSystemImpl::GetFileAttribute (const char* src_file_name, const char* attribute, void* data, size_t size)
{
  static const char* METHOD_NAME = "common::FileSystem::GetFileAttribute";

  if (!src_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  if (!attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "new_name");

  if (size && !data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

  string file_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name, file_name);

    file_system->GetFileAttribute (file_name.c_str (), attribute, data, size);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

bool FileSystemImpl::HasFileAttribute (const char* src_file_name, const char* attribute)
{
  static const char* METHOD_NAME = "common::FileSystem::HasFileAttribute";

  if (!src_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  if (!attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "new_name");

  string file_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name, file_name);

    return file_system->HasFileAttribute (file_name.c_str (), attribute);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

void FileSystemImpl::RemoveFileAttribute (const char* src_file_name, const char* attribute)
{
  static const char* METHOD_NAME = "common::FileSystem::RemoveFileAttribute";

  if (!src_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  if (!attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "new_name");

  string file_name;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name, file_name);

    file_system->RemoveFileAttribute (file_name.c_str (), attribute);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
    Определение принадлежности файла к файловой системе
*/

ICustomFileSystemPtr FileSystemImpl::FindMountFileSystem (const char* file_name, string& result_file_name, string* prefix_name, bool ignore_mount_point_fs)
{
  LoadFileSystems ();

  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
  {
    if (!xtl::xstrncmp (i->prefix.c_str (),file_name,i->prefix.size ()-1))
    {
      switch (file_name [i->prefix.size ()-1])
      {
        case '\0': //запрос к точке монтирования
          if (!ignore_mount_point_fs)
          {
            result_file_name = file_name;

            if (prefix_name)
              *prefix_name = "";

            return &i->mount_point_file_system;
          }
          else 
          {
            result_file_name = file_name + i->prefix.size () - 1;

            if (prefix_name)
              *prefix_name = i->prefix;

            return i->file_system;
          }
        case '/': //запрос к смонтированной файловой системе
          result_file_name = file_name + i->prefix.size ();

          if (prefix_name)
            *prefix_name = i->prefix;

          return i->file_system;
        default:
          break;
      }
    }
  }

  if (prefix_name)
    *prefix_name = "";

  return NULL;
}

ICustomFileSystemPtr FileSystemImpl::FindFileSystem (const char* src_file_name,string& result_file_name,string* prefix_name,bool ignore_mount_point_fs)
{
  static const char* METHOD_NAME = "common::FileSystemImpl::FindFileSystem";  
  
  LoadFileSystems ();  

  string file_name = FileSystem::GetNormalizedFileName (src_file_name);

    //обработка символьных ссылок

  size_t replacement_count = 0;      

  for (SymbolicLinkList::iterator iter=symbolic_links.begin (), end=symbolic_links.end (); iter!=end;)
  {
    if (!xstrncmp (file_name.c_str (), iter->prefix.c_str (), iter->prefix.size () - 1))    
    {
      if (file_name.size () >= iter->prefix.size () && file_name [iter->prefix.size () - 1] == '/')
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
  }  

    //пытаемся найти файл не используя путей поиска

  if (file_name [0] == '/')
  {
    ICustomFileSystemPtr return_value = FindMountFileSystem (file_name.c_str (), result_file_name, prefix_name, ignore_mount_point_fs);

    if (!return_value)
      throw xtl::make_argument_exception (METHOD_NAME, "src_file_name", src_file_name, common::format ("Can't find mount file system for this file (resolved name is '%s')", file_name.c_str ()).c_str ());

    return return_value;
  }
    
  string full_name, mount_name;
  
  ICustomFileSystemPtr owner_file_system;

    //пытаемся найти файл в списке путей поиска

  for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
  {
    full_name         = format ("%s/%s",i->path.c_str (),file_name.c_str ());
    owner_file_system = FindMountFileSystem (full_name.c_str (),mount_name,0,ignore_mount_point_fs);
    
    if (owner_file_system && owner_file_system->IsFileExist (mount_name.c_str ()))
    {
      swap (result_file_name,mount_name);
      
      if (prefix_name)
        *prefix_name = i->path + '/';        
      
      return owner_file_system;
    }
  }

    //пытаемся найти файл в списке пак-файлов
    
  for (SearchPathList::iterator iter=search_paths.begin ();iter!=search_paths.end ();++iter)
  {
    full_name = format ("%s/%s",iter->path.c_str (),file_name.c_str ());    
    
    for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
    {
      if (i->file_system->IsFileExist (full_name.c_str ()))
      {
        swap (full_name,result_file_name);
        
        if (prefix_name)
          *prefix_name = iter->path + '/';
          
        return i->file_system;
      }
    }
  }
  
  for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
    if (i->file_system->IsFileExist (file_name.c_str ()))
    {
      swap (result_file_name,file_name);
            
      if (prefix_name)
        *prefix_name = "";
      
      return i->file_system;
    }  
  
    //пытаемся найти файл по дефолтному пути поиска
    
  full_name         = format ("%s/%s",default_path.c_str (),file_name.c_str ());
  owner_file_system = FindMountFileSystem (full_name.c_str (),mount_name,0,ignore_mount_point_fs);
  
  if (owner_file_system && owner_file_system->IsFileExist (mount_name.c_str ()))
  {
    swap (result_file_name,mount_name);
    
    if (prefix_name)
      *prefix_name = default_path + '/';
    
    return owner_file_system;
  }  

    //возвращаем ссылку на файловую систему включающую путь по умолчанию

  if (!owner_file_system)
    throw format_operation_exception (METHOD_NAME, "File '%s' does not belong to any file system",src_file_name);

  swap (result_file_name,mount_name);
  
  if (prefix_name)
    *prefix_name = default_path + '/';

  return owner_file_system;
}

/*
    Открытие файла
*/

FileImplPtr FileSystemImpl::OpenFile (const char* src_file_name,filemode_t mode_flags,size_t buffer_size)
{
  if (!src_file_name)
    throw xtl::make_null_argument_exception ("FileSystemImpl::OpenFile","file_name");

  string                 file_name;
  bool                   has_crypto_params = false;
  ICustomFileSystemPtr   file_system;
  AnonymousFileSystemPtr anonymous_file_system;

  {
    FileSystemSingleton::Instance instance;

    has_crypto_params     = instance->HasCryptoParameters (src_file_name);
    file_system           = instance->FindFileSystem (src_file_name,file_name);
    anonymous_file_system = instance->anonymous_file_system;

    size_t preferred_buffer_size = instance->GetFileBufferSize (src_file_name);

    if (preferred_buffer_size > buffer_size)
      buffer_size = preferred_buffer_size;
  }

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

    if (has_crypto_params)
    {
      FileCryptoParameters params = FileSystemSingleton::Instance ()->GetCryptoParameters (src_file_name);

      base_file = FileImplPtr (new CryptoFileImpl (base_file, buffer_size, params.ReadMethod (), params.WriteMethod (), params.Key (), params.KeyBits ()), false);
    }

    size_t self_buffer_size = base_file->GetBufferSize ();
    
    if (!buffer_size || self_buffer_size >= buffer_size || (self_buffer_size >= base_file->Size () && !(base_file->Mode () & FileMode_Resize)))
      return base_file;

    if (base_file->Size () <= buffer_size && !(mode_flags & (FileMode_Resize|FileMode_Write)))
      return FileImplPtr (new MemFileImpl (base_file), false);

    return FileImplPtr (new BufferedFileImpl (base_file,buffer_size), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystemImpl::OpenFile()");
    throw;
  }

  return 0;
}

FileImplPtr FileSystemImpl::OpenFile (const char* src_file_name,filemode_t mode_flags)
{
  size_t buffer_size = FileSystemSingleton::Instance ()->GetDefaultFileBufferSize ();

  return OpenFile (src_file_name,mode_flags, buffer_size);
}

void FileSystemImpl::SetDefaultFileBufferSize (size_t buffer_size)
{
  default_file_buffer_size = buffer_size;
}

void FileSystemImpl::SetFileBufferSize (const char* file_name, size_t buffer_size)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::SetFileBufferSize", "file_name");
    
  if (!buffer_size)
  {
    file_buffer_sizes.erase (file_name);
    return;
  }
  
  file_buffer_sizes [file_name] = buffer_size;
}

size_t FileSystemImpl::GetFileBufferSize (const char* file_name) const
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::SetFileBufferSize", "file_name");
    
  FileBufferSizeMap::const_iterator iter = file_buffer_sizes.find (file_name);
  
  if (iter == file_buffer_sizes.end ())
    return GetDefaultFileBufferSize ();
    
  return iter->second;
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

  string file_name, prefix;

  try
  {
    ICustomFileSystemPtr file_system = FindFileSystem (src_file_name, file_name, &prefix);

    if (!file_name.empty () && file_name [file_name.size () - 1] == '/')
      file_name.pop_back ();

    FileInfo info;
    bool     is_dir = false;

    if (file_system->GetFileInfo (file_name.c_str (), info))
      is_dir = info.is_dir;

    if (is_dir)
    {
      stl::string search_mask = common::format ("%s/*", file_name.c_str ());

      file_system->Search (search_mask.c_str (), RemoveFileWithPrefix (this, prefix.c_str ()));

      file_system->Remove (file_name.c_str ());
    }
    else
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
    
    if (!dir_name.empty () && dir_name [dir_name.size () - 1] == '/')
      dir_name.pop_back ();

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

static void DummyLogHandler (const char* message)
{
  try
  {
    Log (LOG_NAME).Print (message);
  }
  catch (...)
  {
  }
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

void FileSystem::SetFileBufferSize (const char* file_name, size_t buffer_size)
{
  FileSystemSingleton::Instance ()->SetFileBufferSize (file_name, buffer_size);
}

size_t FileSystem::GetFileBufferSize (const char* file_name)
{
  return FileSystemSingleton::Instance ()->GetFileBufferSize (file_name);
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

static void InternalGetFileHash (File& file,size_t max_hash_size,FileHash& hash)
{
  char       buf [FILE_HASH_BUF_SIZE];
  Md5Context md5;
  size_t     crc32_hash = 0xFFFFFFFF;
  
  size_t read_size = 0;

  while (!file.Eof () && read_size < max_hash_size)
  {
    size_t available_size = max_hash_size - read_size;
    
    available_size = available_size < sizeof (buf) ? available_size : sizeof (buf);
    
    size_t size = file.Read (buf,available_size);

    md5.Update (buf,size);

    crc32_hash  = crc32 (buf,size,crc32_hash);
    read_size  += size;
  }

  md5.Finish (hash.md5);

  hash.crc32 = crc32_hash;
}

void FileSystem::GetFileHash (File& file,size_t max_hash_size,FileHash& hash)
{
  if (!file.CanRead ())
    throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Unable to compute FileHash. Reason: file can't be read");

  filepos_t current_position = file.Tell ();

  if (current_position)
  {
    if (!file.CanSeek ())
      throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be seek");

    file.Seek (0);

    InternalGetFileHash (file,max_hash_size,hash);

    file.Seek (current_position);
  }
  else
  {
    if (!file.CanRewind ())
      throw xtl::format_not_supported_exception ("common::FileSystem::GetFileHash","Can't compute FileHash. Reason: file can't be rewind");

    InternalGetFileHash (file,max_hash_size,hash);

    file.Rewind ();
  }
}

void FileSystem::GetFileHash (File& file,FileHash& hash)
{
  GetFileHash (file,~0u,hash);
}

void FileSystem::GetFileHash (const char* file_name,size_t max_hash_size,FileHash& hash)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("common::FileSystem::GetFileHash","file_name");

  try
  {
    InputFile file (file_name);

    InternalGetFileHash (file, max_hash_size, hash);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::FileSystem::GetFileHash");
    throw;
  }
}

void FileSystem::GetFileHash (const char* file_name,FileHash& hash)
{
  GetFileHash(file_name,~0u,hash);
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
//      case '\\':
//        *i = '/';
//        break;
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
   Файловые атрибуты
*/

void FileSystem::SetFileAttribute (const char* file_name, const char* attribute, const void* data, size_t size)
{
  FileSystemSingleton::Instance ()->SetFileAttribute (file_name, attribute, data, size);
}

void FileSystem::GetFileAttribute (const char* file_name, const char* attribute, void* data, size_t size)
{
  FileSystemSingleton::Instance ()->GetFileAttribute (file_name, attribute, data, size);
}

bool FileSystem::HasFileAttribute (const char* file_name, const char* attribute)
{
  return FileSystemSingleton::Instance ()->HasFileAttribute (file_name, attribute);
}

void FileSystem::RemoveFileAttribute (const char* file_name, const char* attribute)
{
  FileSystemSingleton::Instance ()->RemoveFileAttribute (file_name, attribute);
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

    StdFile input_file (source_file_name, FileMode_Read);
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

Action FileSystem::BackgroundCopyFile (const char* source_file_name, const char* destination_file_name, const BackgroundCopyFileCallback& callback, ActionThread thread, size_t buffer_size)
{
  if (!source_file_name)
    throw xtl::make_null_argument_exception ("", "source_file_name");

  if (!destination_file_name)
    throw xtl::make_null_argument_exception ("", "destination_file_name");

  if (!buffer_size)
    buffer_size = GetDefaultFileBufferSize ();

  if (!buffer_size)
    buffer_size = 4096;

  BackgroundCopyFileDataPtr data (new BackgroundCopyFileData (source_file_name, destination_file_name, callback, thread, buffer_size), false);

  return ActionQueue::PushAction (xtl::bind (&background_copy_file_impl, _1, data), ActionThread_Background, 0, 0);
}
