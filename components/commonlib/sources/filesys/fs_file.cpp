#include "shared.h"

using namespace common;
using namespace stl;

/*
    File
*/

/*
    Конструкторы, деструктор и присваивание
*/

File::File ()
  : impl (ClosedFileImpl::Instance ())
{  
  FileSystemSingleton::Instance ().RegisterFile (*this);
}

File::File (FileImplPtr _impl)
  : impl (_impl)
{
  FileSystemSingleton::Instance ().RegisterFile (*this);
}

File::File (const File& file)
  : impl (file.impl)
{
  FileSystemSingleton::Instance ().RegisterFile (*this);
}

File::~File ()
{
  if (FileSystemSingleton::IsInitialized ()) //если менеджера файловой системы уже уничтожен регистрация не нужна
    FileSystemSingleton::Instance ().UnregisterFile (*this);
}

File& File::operator = (const File& file)
{
  impl = file.impl;
  
  return *this;
}

/*
    Режим работы файла
*/

filemode_t File::Mode () const
{
  return impl->Mode ();
}

bool File::CanRead () const
{
  return (impl->Mode () & FILE_MODE_READ) != 0;
}

bool File::CanWrite () const
{
  return (impl->Mode () & FILE_MODE_WRITE) != 0;
}

bool File::CanResize () const
{
  return (impl->Mode () & FILE_MODE_RESIZE) != 0;
}

bool File::CanSeek () const
{
  return (impl->Mode () & FILE_MODE_SEEK) != 0;
}

bool File::CanRewind () const
{
  return (impl->Mode () & FILE_MODE_REWIND) != 0;
}

bool File::IsReadOnly () const
{
  filemode_t mode = impl->Mode ();

  return (mode & FILE_MODE_READ) && !(mode & (FILE_MODE_WRITE|FILE_MODE_RESIZE));
}

bool File::IsWriteOnly () const
{
  filemode_t mode = impl->Mode ();

  return (mode & FILE_MODE_WRITE) && !(mode & FILE_MODE_READ);
}

bool File::IsClosed () const
{
  return impl->IsClosedFileType ();
}

bool File::IsBuffered () const
{
  return impl->GetBufferSize () != 0;
}

/*
    Чтение / запись
*/

size_t File::Read (void* buf,size_t size)
{
  if (!(impl->Mode () & FILE_MODE_READ))
    RaiseNotSupported ("File::Read","This file can't be read");

  if (!buf)
  {
    if (size) RaiseNullArgument ("File::Read","buffer");    
    else      return 0;
  }
    
  return impl->Read (buf,size);
}

size_t File::Write (const void* buf,size_t size)
{
  if (!(impl->Mode () & FILE_MODE_WRITE))
    RaiseNotSupported ("File::Write","This file can't be write");

  if (!buf)
  {
    if (size) RaiseNullArgument ("File::Write","buffer");
    else      return 0;
  }

  return impl->Write (buf,size);
}

/*
    Работа с файловым указателем
*/

void File::Rewind ()
{
  if (!(impl->Mode () & FILE_MODE_REWIND))
    RaiseNotSupported ("File::Rewind","This file can't be rewind");

  return impl->Rewind ();
}

filepos_t File::Seek (filepos_t pos,FileSeekMode seek_mode)
{
  if (!(impl->Mode () & FILE_MODE_SEEK))
    RaiseNotSupported ("File::Seek","This file can't be seek");
    
  size_t position = 0;

  switch (seek_mode)
  {
    case FILE_SEEK_SET: position = pos; break;
    case FILE_SEEK_CUR: position = impl->Tell () + pos; break;
    case FILE_SEEK_END: position = impl->Size () + pos; break;
    default:            RaiseInvalidArgument ("File::Seek","seek_mode",seek_mode);
  }

  if (position > Size () && (impl->Mode () & FILE_MODE_RESIZE))
    impl->Resize (position);

  return impl->Seek (position);
}

filepos_t File::Tell () const
{
  return impl->Tell ();
}

/*
    Размер файла / проверка на конец файла
*/

filesize_t File::Size () const
{
  return impl->Size ();
}

void File::Resize (filesize_t new_size)
{
  if (!(impl->Mode () & FILE_MODE_RESIZE))
    RaiseNotSupported ("File::Resize","This file can't be resize");

  impl->Resize (new_size);
}

bool File::Eof () const
{
  return impl->Eof ();
}

/*
    Работа с файловыми буферами
*/

void File::Flush ()
{
  impl->Flush ();
}

/*
    Закрытие файла
*/

void File::Close ()
{
  if (impl->IsClosedFileType ())
    return;

  impl->Flush ();    

  impl = ClosedFileImpl::Instance ();
}

/*
    Сравнение двух файлов на эквивалентность
*/

bool File::operator == (const File& file) const
{
  return impl == file.impl;
}

bool File::operator != (const File& file) const
{
  return impl != file.impl;
}

/*
    FileImpl
*/

FileImpl::FileImpl (filemode_t _mode)
  : mode (_mode)
  { }

size_t FileImpl::Read (void*,size_t)
{
  return 0;
}

size_t FileImpl::Write (const void*,size_t)
{
  return 0;
}
    
filepos_t FileImpl::Tell ()
{
  return 0;
}

filepos_t FileImpl::Seek (filepos_t)
{
  return Tell ();
}

void FileImpl::Rewind ()
{
  if (Seek (0))
    FileException ("Internal seek error").Raise ("FileImpl::Rewind");
}

filesize_t FileImpl::Size ()
{
  return 0;
}

void FileImpl::Resize (filesize_t)
{
}

bool FileImpl::Eof ()
{
  return (filepos_t)Size () == Tell ();
}
    
void FileImpl::Flush ()
{
}
