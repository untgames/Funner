#include "shared.h"
#include <memory.h>
#include <stl/algorithm>

using namespace stl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации списка файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class common::FileListImpl: public ReferenceCounter
{
  friend class FileListBuilder;
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов / взятие элемента
///////////////////////////////////////////////////////////////////////////////////////////////////  
    size_t              Size () const { return items.size (); }
    const FileListItem& Item (size_t index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на эквивалентность
///////////////////////////////////////////////////////////////////////////////////////////////////    
    bool IsEqual (const FileListImpl&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание списков
///////////////////////////////////////////////////////////////////////////////////////////////////
    static FileListImpl* GetEmptyList ();
    static FileListImpl* Create ();
    
  private:
    FileListBuilder::FileInfoArray items;      //массив информации о файлах
    stl::string                    file_names; //имена файлов
};

/*
    FileListImpl
*/

const FileListItem& FileListImpl::Item (size_t index) const
{
  if (index >= items.size ())
    RaiseOutOfRange ("FileList::Item","index",index,items.size ());

  return items [index];
}

inline bool IsItemsEqual (const FileListItem& a,const FileListItem& b)
{
  return !memcmp (&a.info,&b.info,sizeof (FileInfo));
}

bool FileListImpl::IsEqual (const FileListImpl& list) const
{
  return file_names == list.file_names && equal (items.begin (),items.end (),list.items.begin (),IsItemsEqual);
}

FileListImpl* FileListImpl::Create ()
{
  return new FileListImpl;
}

FileListImpl* FileListImpl::GetEmptyList ()
{
  return Singleton<FileListImpl,SingletonCreateUsingNew>::InstancePtr ();
}

/*
    FileListIterator
*/

FileListIterator::FileListIterator ()
  : list_impl (FileListImpl::GetEmptyList ()), pos (0)
  { }

FileListIterator::FileListIterator (const FileList& list)
  : list_impl (list.impl), pos (0)
  { }

FileListIterator& FileListIterator::operator = (const FileList& list)
{
  list_impl = list.impl;  
  pos       = 0;

  return *this;
}

FileListIterator& FileListIterator::operator ++ ()
{
  pos++;
  return *this;
}

FileListIterator& FileListIterator::operator -- ()
{
  pos--;
  return *this;
}

FileListIterator FileListIterator::operator ++ (int)
{
  FileListIterator tmp = *this;

  pos++;

  return tmp;
}

FileListIterator FileListIterator::operator -- (int)
{
  FileListIterator tmp = *this;

  pos--;

  return tmp;
}

const FileListItem* FileListIterator::operator -> () const
{
  return &list_impl->Item (pos);
}

const FileListItem& FileListIterator::operator * () const
{
  return list_impl->Item (pos);
}
    
FileListIterator::operator bool () const
{
  return pos < list_impl->Size ();
}
    
bool FileListIterator::operator == (const FileListIterator& x) const
{
  return list_impl == x.list_impl && pos == x.pos;  
}

bool FileListIterator::operator < (const FileListIterator& x) const
{
  return list_impl == x.list_impl && pos < x.pos;
}

bool FileListIterator::operator != (const FileListIterator& x) const
{
  return !(*this == x);
}

bool FileListIterator::operator > (const FileListIterator& x) const
{
  return x < *this;
}

bool FileListIterator::operator <= (const FileListIterator& x) const
{
  return !(x < *this);
}

bool FileListIterator::operator >= (const FileListIterator& x) const
{
  return !(*this < x);
}

/*
    FileList
*/

FileList::FileList ()
  : impl (FileListImpl::GetEmptyList ())
  { }

FileList::FileList (FileListImpl* _impl)
  : impl (_impl,false)
  { }

size_t FileList::Size () const
{
  return impl->Size ();
}

const FileListItem& FileList::Item (size_t index) const
{
  return impl->Item (index);
}

FileList::Iterator FileList::GetIterator () const
{
  return *this;  
}

bool FileList::operator == (const FileList& list) const
{
  return list.impl == impl || impl->IsEqual (*list.impl);
}

bool FileList::operator != (const FileList& list) const
{
  return !(*this == list);
}

/*
    FileListBuilder
*/

FileListBuilder::FileListBuilder ()
{
  truncate_size = 0;
  prefix_hash   = strihash ("");
}

void FileListBuilder::SetPrefix (const char* _prefix)
{
  prefix      = _prefix ? _prefix : "";        
  prefix_hash = prefix.size () > truncate_size ? strihash (prefix.c_str ()+truncate_size) : 0xFFFFFFFF;
}

void FileListBuilder::SetTruncateSize (size_t size)
{
  truncate_size = size;
  prefix_hash   = prefix.size () > truncate_size ? strihash (prefix.c_str ()+truncate_size) : 0xFFFFFFFF;
}

void FileListBuilder::InsertInternal (const char* file_name,size_t file_name_size,const FileInfo& file_info)
{    
  size_t file_name_hash = prefix_hash;

  if (prefix.size () >= truncate_size)
  {
    file_name_hash = strihash (file_name,file_name_hash);
  }
  else if (file_name_size > truncate_size - prefix.size ())
  {
    file_name_hash = strihash (file_name+truncate_size-prefix.size (),file_name_hash);
  }

  if (items_hash.find (file_name_hash) != items_hash.end ())
    return;

  items_hash.insert (file_name_hash);

  FileListItem item   = {(const char*)file_names.size (),file_info};
  size_t       offset = truncate_size;

  if (prefix.size () > offset)
  {
    file_names.append (prefix.c_str ()+offset,prefix.size ()-offset);
    offset = 0;
  }
  else offset -= prefix.size ();

  if (file_name_size > offset)
    file_names.append (file_name+offset,file_name_size-offset);

  file_names.push_back (0);
  items.push_back      (item);  
}

void FileListBuilder::Insert (const char* file_name,const FileInfo& file_info)
{
  if (file_name)
    InsertInternal (file_name,strlen (file_name),file_info);
}

void FileListBuilder::InsertSubname (const char* file_name,size_t file_name_size,const FileInfo& file_info)
{
  if (file_name)
    InsertInternal (file_name,file_name_size,file_info);
}

inline bool CompareItems (const FileListItem& a,const FileListItem& b)
{
  return common::strcmp (a.name,b.name) < 0;
}

FileList FileListBuilder::Build (bool need_sort)
{
  FileListImpl* list = FileListImpl::Create ();

  swap (list->file_names,file_names);
  swap (list->items,items);

  const char* string_base = list->file_names.c_str ();

  for (FileInfoArray::iterator i=list->items.begin ();i!=list->items.end ();++i)
    i->name = string_base + (size_t)i->name;

  if (need_sort)
    sort (list->items.begin (),list->items.end (),CompareItems);

  items_hash.clear ();

  return list;
}

/*
    intrusive_ptr<FileListImpl> control
*/

void common::intrusive_ptr_add_ref (FileListImpl* impl)
{
  impl->AddRef ();
}

void common::intrusive_ptr_release (FileListImpl* impl)
{
  impl->Release ();
}
