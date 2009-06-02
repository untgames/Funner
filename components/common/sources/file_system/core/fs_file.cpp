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
  try
  {
    if (FileSystemSingleton::IsInitialized ()) //если менеджера файловой системы уже уничтожен регистрация не нужна
      FileSystemSingleton::Instance ().UnregisterFile (*this);
  }
  catch (...)
  {
  }
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
  return (impl->Mode () & FileMode_Read) != 0;
}

bool File::CanWrite () const
{
  return (impl->Mode () & FileMode_Write) != 0;
}

bool File::CanResize () const
{
  return (impl->Mode () & FileMode_Resize) != 0;
}

bool File::CanSeek () const
{
  return (impl->Mode () & FileMode_Seek) != 0;
}

bool File::CanRewind () const
{
  return (impl->Mode () & FileMode_Rewind) != 0;
}

bool File::IsReadOnly () const
{
  filemode_t mode = impl->Mode ();

  return (mode & FileMode_Read) && !(mode & (FileMode_Write | FileMode_Resize));
}

bool File::IsWriteOnly () const
{
  filemode_t mode = impl->Mode ();

  return (mode & FileMode_Write) && !(mode & FileMode_Read);
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
  if (!(impl->Mode () & FileMode_Read))
    throw xtl::format_not_supported_exception ("File::Read","This file can't be read");

  if (!buf)
  {
    if (size) throw xtl::make_null_argument_exception ("File::Read","buffer");
    else      return 0;
  }

  return impl->Read (buf,size);
}

size_t File::Write (const void* buf,size_t size)
{
  if (!(impl->Mode () & FileMode_Write))
    throw xtl::format_not_supported_exception ("File::Write","This file can't be write");

  if (!buf)
  {
    if (size) throw xtl::make_null_argument_exception ("File::Write","buffer");
    else      return 0;
  }

  return impl->Write (buf,size);
}

/*
    Работа с файловым указателем
*/

void File::Rewind ()
{
  if (!(impl->Mode () & FileMode_Rewind))
    throw xtl::format_not_supported_exception ("File::Rewind","This file can't be rewind");

  return impl->Rewind ();
}

filepos_t File::Seek (filepos_t pos,FileSeekMode seek_mode)
{
  if (!(impl->Mode () & FileMode_Seek))
    throw xtl::format_not_supported_exception ("File::Seek","This file can't be seek");

  size_t position = 0;

  switch (seek_mode)
  {
    case FileSeekMode_Set:     position = pos; break;
    case FileSeekMode_Current: position = impl->Tell () + pos; break;
    case FileSeekMode_End:     position = impl->Size () + pos; break;
    default:                   throw xtl::make_argument_exception ("File::Seek","seek_mode",seek_mode);
  }

  if (position > Size () && (impl->Mode () & FileMode_Resize))
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
  if (!(impl->Mode () & FileMode_Resize))
    throw xtl::format_not_supported_exception ("File::Resize","This file can't be resize");

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
    throw xtl::format_operation_exception ("FileImpl::Rewind", "Internal seek error");
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
