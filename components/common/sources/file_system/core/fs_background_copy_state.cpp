#include "shared.h"

using namespace common;

/*
   Состояние фонового копирования
*/

struct BackgroundCopyState::Impl : public xtl::reference_counter
{
  size_t                    file_size;
  size_t                    bytes_copied;
  BackgroundCopyStateStatus status;
  stl::string               error;

  Impl ()
    : file_size (0)
    , bytes_copied (0)
    , status (BackgroundCopyStateStatus_Started)
    {}
};

/*
   Конструктор/деструктор/копирование
*/

BackgroundCopyState::BackgroundCopyState ()
  : impl (new Impl)
{}

BackgroundCopyState::BackgroundCopyState (const BackgroundCopyState& source)
  : impl (source.impl)
{
  addref (impl);
}

BackgroundCopyState::~BackgroundCopyState ()
{
  release (impl);
}

BackgroundCopyState& BackgroundCopyState::operator = (const BackgroundCopyState& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   Получение размера файла
*/

void BackgroundCopyState::SetFileSize (size_t size)
{
  impl->file_size = size;
}

size_t BackgroundCopyState::FileSize () const
{
  return impl->file_size;
}

/*
   Получение количества скопированных байт
*/

void BackgroundCopyState::SetBytesCopied (size_t size)
{
  impl->bytes_copied = size;
}

size_t BackgroundCopyState::BytesCopied () const
{
  return impl->bytes_copied;
}

/*
   Статус операции
*/

void BackgroundCopyState::SetStatus (BackgroundCopyStateStatus status)
{
  impl->status = status;
}

BackgroundCopyStateStatus BackgroundCopyState::Status () const
{
  return impl->status;
}

/*
   Текст ошибки
*/

void BackgroundCopyState::SetStatusText (const char* error)
{
  if (!error)
    throw xtl::make_null_argument_exception ("common::BackgroundCopyState::SetStatusText", "error");

  impl->error = error;
}

const char* BackgroundCopyState::StatusText () const
{
  return impl->error.c_str ();
}
