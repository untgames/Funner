#include "shared.h"

using namespace stl;
using namespace common;
using namespace xtl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательный класс поиска файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileSearchHelper
{  
  public:
    FileSearchHelper (FileListBuilder& builder,const char* mask,size_t search_flags);

    void Search (ICustomFileSystemPtr,const char* dir);

  private:  
    void Insert          (const char* file_name,const FileInfo&);
    void InsertRecursive (const char* file_name,const FileInfo&);

  private:  
    typedef ICustomFileSystem::FileSearchHandler SearchHandler;
  
    FileListBuilder&     builder;
    size_t               search_flags;
    ICustomFileSystemPtr file_system;  
    const char*          mask;
    string               full_mask;
};

/*
    Поиск файла
*/

inline FileSearchHelper::FileSearchHelper (FileListBuilder& _builder,const char* _mask,size_t _search_flags)
  : builder (_builder), search_flags (_search_flags), file_system (0), mask (_mask)
  { }

void FileSearchHelper::Search (ICustomFileSystemPtr _file_system,const char* dir)
{
  file_system = _file_system;
  full_mask   = format ("%s%s",dir,mask);

  if (search_flags & FileSearch_SubDirs)
    file_system->Search (format ("%s*",dir).c_str (),SearchHandler (bind (&FileSearchHelper::InsertRecursive, this, _1, _2)));
  else
    file_system->Search (full_mask.c_str (),SearchHandler (bind (&FileSearchHelper::Insert, this, _1, _2)));
}

void FileSearchHelper::InsertRecursive (const char* file_name,const FileInfo& info)
{
  if (!file_name)
    return;

  if (wcimatch (file_name,full_mask.c_str ()))
    Insert (file_name,info);

  if (info.is_dir)
    file_system->Search (format ("%s/*",file_name).c_str (),SearchHandler (bind (&FileSearchHelper::InsertRecursive, this, _1, _2)));
}

void FileSearchHelper::Insert (const char* file_name,const FileInfo& info)
{
  if (!file_name)
    return;

  if (!info.is_dir)
  {
    if (search_flags & FileSearch_Files)
      builder.Insert (file_name,info);
  }
  else
  {
    if (search_flags & FileSearch_Dirs)
      builder.Insert (file_name,info);
  }
}

void FileSystemImpl::MountSearch (FileListBuilder& builder,const char* wc_mask,const char* prefix,size_t flags)
{
  FileSearchHelper search_helper (builder,wc_mask,flags);
  size_t           prefix_size = strlen (prefix);
  
  for (MountList::iterator i=mounts.begin ();i!=mounts.end ();++i)
  {
    if (!xtl::xstrnicmp (i->prefix.c_str (),prefix,i->prefix.size ()))  
    {
      builder.SetPrefix    (i->prefix.c_str ());
      search_helper.Search (i->file_system,prefix+i->prefix.size ());
    }
    else if (!xtl::xstrnicmp (i->prefix.c_str (),prefix,prefix_size))
    {
      if (flags & FileSearch_Dirs && wcimatch (i->prefix.c_str ()+prefix_size,wc_mask))
      {
        builder.SetPrefix     ("");
        builder.InsertSubname (i->prefix.c_str (),i->prefix.size ()-1,i->mount_point_info);
      }
      
      if (flags & FileSearch_SubDirs)
      {       
        builder.SetPrefix    (i->prefix.c_str ());
        search_helper.Search (i->file_system,"");
      }
    }
  }
}

FileList FileSystemImpl::Search (const char* src_mask,size_t flags)
{
  if (!src_mask)
    throw xtl::make_null_argument_exception ("FileSystem::Search","mask");

  string full_mask  = FileSystem::GetNormalizedFileName (src_mask),
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
      if (!(flags & FileSearch_FullPaths))
        builder.SetTruncateSize (default_path.size ()+1);

      MountSearch (builder,mask.c_str (),default_path.c_str (),flags);
      
      for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      {
        if (!(flags & FileSearch_FullPaths))
          builder.SetTruncateSize (i->path.size ()+1);

        MountSearch (builder,mask.c_str (),i->path.c_str (),flags);
      }
    }
    else
    {
      if (!(flags & FileSearch_FullPaths))
        builder.SetTruncateSize (default_path.size ()+1);

      MountSearch (builder,mask.c_str (),format ("%s/%s",default_path.c_str (),src_prefix.c_str ()).c_str (),flags);

      for (SearchPathList::iterator i=search_paths.begin ();i!=search_paths.end ();++i)
      {
        if (!(flags & FileSearch_FullPaths))
          builder.SetTruncateSize (i->path.size ()+1);

        MountSearch (builder,mask.c_str (),format ("%s/%s",i->path.c_str (),src_prefix.c_str ()).c_str (),flags);
      }
    }    

      //поиск в паках
      
    if (!(flags & FileSearch_NoPacks))
    {
      FileSearchHelper search_helper (builder,mask.c_str (),flags);
      
      builder.SetTruncateSize (0);
      builder.SetPrefix ("");

      for (PackFileList::iterator i=pack_files.begin ();i!=pack_files.end ();++i)
        search_helper.Search (i->file_system,src_prefix.c_str ());
    }
  }

  return builder.Build ((flags & FileSearch_Sort) != 0);
}
