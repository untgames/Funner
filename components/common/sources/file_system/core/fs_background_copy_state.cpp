#include "shared.h"

using namespace common;

/*
   Состояние фонового копирования
*/

struct BackgroundCopyState::Impl : public xtl::reference_counter
{
  filesize_t                file_size;
  filesize_t                bytes_copied;
  BackgroundCopyStateStatus status;
  stl::string               error;

  Impl ()
    : file_size ()
    , bytes_copied ()
    , status (BackgroundCopyStateStatus_Started)
    {}    
};

/*
   Конструктор/деструктор/копирование
*/

BackgroundCopyState::BackgroundCopyState ()
  : impl (new Impl)
{
}

BackgroundCopyState::BackgroundCopyState (Impl* in_impl)
  : impl (in_impl)
{
}

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
  BackgroundCopyState (source).Swap (*this);

  return *this;
}

/*
    Клонирование
*/

BackgroundCopyState BackgroundCopyState::Clone () const
{
  return BackgroundCopyState (new Impl (*impl));
}

/*
   Получение размера файла
*/

void BackgroundCopyState::SetFileSize (filesize_t size)
{
  impl->file_size = size;
}

filesize_t BackgroundCopyState::FileSize () const
{
  return impl->file_size;
}

/*
   Получение количества скопированных байт
*/

void BackgroundCopyState::SetBytesCopied (filesize_t size)
{
  impl->bytes_copied = size;
}

filesize_t BackgroundCopyState::BytesCopied () const
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

/*
    Обмен
*/

void BackgroundCopyState::Swap (BackgroundCopyState& state)
{
  stl::swap (impl, state.impl);
}

namespace common
{

void swap (BackgroundCopyState& state1, BackgroundCopyState& state2)
{
  state1.Swap (state2);
}

}
