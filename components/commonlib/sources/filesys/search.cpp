#include "shared.h"

using namespace stl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательный класс поиска файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileSearchHelper
{  
  public:
    FileSearchHelper (FileListBuilder& builder,const char* mask,size_t search_flags);

    void Search (ICustomFileSystem*,const char* dir);

  private:  
    void Insert          (const char* file_name,const FileInfo&);
    void InsertRecursive (const char* file_name,const FileInfo&);

  private:  
    typedef ICustomFileSystem::FileSearchHandler SearchHandler;
  
    FileListBuilder&   builder;
    size_t             search_flags;
    ICustomFileSystem* file_system;  
    const char*        mask;
    string             full_mask;
};

/*
    Поиск файла
*/

inline FileSearchHelper::FileSearchHelper (FileListBuilder& _builder,const char* _mask,size_t _search_flags)
  : builder (_builder), search_flags (_search_flags), file_system (0), mask (_mask)
  { }

void FileSearchHelper::Search (ICustomFileSystem* _file_system,const char* dir)
{
  file_system = _file_system;
  full_mask   = format ("%s%s",dir,mask);

  if (search_flags & FILE_SEARCH_SUBDIRS)
    file_system->Search (format ("%s*",dir).c_str (),SearchHandler (*this,&FileSearchHelper::InsertRecursive));
  else
    file_system->Search (full_mask.c_str (),SearchHandler (*this,&FileSearchHelper::Insert));
}

void FileSearchHelper::InsertRecursive (const char* file_name,const FileInfo& info)
{
  if (!file_name)
    return;

  if (wcimatch (file_name,full_mask.c_str ()))
    Insert (file_name,info);

  if (info.is_dir)
    file_system->Search (format ("%s/*",file_name).c_str (),SearchHandler (*this,&FileSearchHelper::InsertRecursive));
}

void FileSearchHelper::Insert (const char* file_name,const FileInfo& info)
{
  if (!file_name)
    return;

  if (!info.is_dir)
  {
    if (search_flags & FILE_SEARCH_FILES)
      builder.Insert (file_name,info);
  }
  else
  {
    if (search_flags & FILE_SEARCH_DIRS)
      builder.Insert (file_name,info);
  }
}

void FileSystemImpl::MountSearch (FileListBuilder& builder,const char* wc_mask,const char* prefix,size_t flags)
{
  FileSearchHelper search_helper (builder,wc_mask,flags);
  size_t           prefix_size = strlen (prefix);
  
  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
  {
    if (!common::strnicmp (i->prefix.c_str (),prefix,i->prefix.size ()))  
    {
      builder.SetPrefix    (i->prefix.c_str ());
      search_helper.Search (get_pointer (i->file_system),prefix+i->prefix.size ());
    }
    else if (!common::strnicmp (i->prefix.c_str (),prefix,prefix_size))
    {
      if (flags & FILE_SEARCH_DIRS && wcimatch (i->prefix.c_str ()+prefix_size,wc_mask))
      {
        builder.SetPrefix     ("");
        builder.InsertSubname (i->prefix.c_str (),i->prefix.size ()-1,i->mount_point_info);
      }
      
      if (flags & FILE_SEARCH_SUBDIRS)
      {       
        builder.SetPrefix    (i->prefix.c_str ());
        search_helper.Search (get_pointer (i->file_system),"");
      }
    }
  }
}

FileList FileSystemImpl::Search (const char* src_mask,size_t flags)
{
  if (!src_mask)
    RaiseNullArgument ("FileSystem::Search","mask");

  string full_mask  = ConvertFileName (src_mask),
         src_prefix = dir (full_mask),
         mask       = notdir (full_mask);

  FileListBuilder builder;

    //базовый поиск в смонтированных файловых системах
  
  if (src_prefix [0] == '/')
  {    
    MountSearch (builder,mask.c_str (),src_prefix.c_str (),flags);
  }
  else
  {
    if (src_prefix == "./")
      src_prefix.clear ();
    
    if (src_prefix.empty ())
    {
      if (!(flags & FILE_SEARCH_FULL_PATHS))
        builder.SetTruncateSize (default_path.size ()+1);

      MountSearch (builder,mask.c_str (),default_path.c_str (),flags);
      
      for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      {
        if (!(flags & FILE_SEARCH_FULL_PATHS))
          builder.SetTruncateSize (i->path.size ()+1);

        MountSearch (builder,mask.c_str (),i->path.c_str (),flags);
      }
    }
    else
    {
      if (!(flags & FILE_SEARCH_FULL_PATHS))
        builder.SetTruncateSize (default_path.size ()+1);

      MountSearch (builder,mask.c_str (),format ("%s/%s",default_path.c_str (),src_prefix.c_str ()).c_str (),flags);

      for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      {
        if (!(flags & FILE_SEARCH_FULL_PATHS))
          builder.SetTruncateSize (i->path.size ()+1);

        MountSearch (builder,mask.c_str (),format ("%s/%s",i->path.c_str (),src_prefix.c_str ()).c_str (),flags);
      }
    }    

      //поиск в паках
      
    if (!(flags & FILE_SEARCH_NO_PACKS))
    {
      FileSearchHelper search_helper (builder,mask.c_str (),flags);
      
      builder.SetTruncateSize (0);
      builder.SetPrefix ("");

      for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
        search_helper.Search (get_pointer (i->file_system),src_prefix.c_str ());
    }
  }

  return builder.Build ((flags & FILE_SEARCH_SORT) != 0);
}
