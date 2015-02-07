#include "shared.h"

using namespace common;

/*
    Константы
*/

namespace
{

const size_t DEFAULT_BUFFER_SIZE = 4096; //размер буферов по умолчанию

}

/*
    Описание реализации файла с шифрованием
*/

typedef xtl::uninitialized_storage<char> Buffer;

struct CryptoFileImpl::Impl
{
  FileImplPtr   source_file;          //исходный файл
  CryptoContext read_crypto_context;  //контекст шифрования
  CryptoContext write_crypto_context; //контекст шифрования
  Buffer        read_write_buffer;    //вспомогательный буфер для чтения / записи данных из исходного файла
  Buffer        data_buffer;          //кэш с данными после шифрования
  filepos_t     file_pos;             //файловая позиция
  filesize_t    file_size;            //размер файла
  filepos_t     data_start_pos;       //позиция начала данных
  filepos_t     data_end_pos;         //позиция конца данных
  filepos_t     dirty_start_pos;      //позиция начала данных, требующих записи
  filepos_t     dirty_end_pos;        //позиция конца данных, требующих записи

///Конструктор
  Impl (const FileImplPtr& in_source_file, size_t buffer_size, const char* read_crypto_method, const char* write_crypto_method, const void* key, size_t key_bits)
    : source_file (in_source_file)
    , read_crypto_context (read_crypto_method, key, key_bits)
    , write_crypto_context (write_crypto_method, key, key_bits)
    , file_pos ()
    , data_start_pos ()
    , data_end_pos ()
    , dirty_start_pos ()
    , dirty_end_pos ()
  {
    file_size = AdjustBlockSizeLow (source_file->Size ());

    size_t block_size = read_crypto_context.BlockSize ();

    if (!block_size)
      throw xtl::format_not_supported_exception ("", "Sequential crypting contexts not supported (method='%s')", read_crypto_method);

    if (block_size != write_crypto_context.BlockSize ())
      throw xtl::format_not_supported_exception ("", "Read crypto context '%s' block size %u defferes from write crypto context '%s' block size %u",
        read_crypto_method, read_crypto_context.BlockSize (), write_crypto_method, write_crypto_context.BlockSize ());

    if (buffer_size < DEFAULT_BUFFER_SIZE)
      buffer_size = DEFAULT_BUFFER_SIZE;

    if (buffer_size > block_size) buffer_size = buffer_size / block_size * block_size;
    else                          buffer_size = block_size;

    data_buffer.resize (buffer_size);
    read_write_buffer.resize (buffer_size);
  }

///Корректировка размеров файла
  filesize_t AdjustBlockSizeHigh (filesize_t size)
  {
    size_t block_size = write_crypto_context.BlockSize ();

    if (!(size % block_size))
      return size;

    return size + block_size - size % block_size;
  }

  filesize_t AdjustBlockSizeLow (filesize_t size)
  {
    size_t block_size = read_crypto_context.BlockSize ();

    return size / block_size * block_size;
  }

///Сброс буфера данных
  void FlushBuffer ()
  {
    try
    {
      if (dirty_start_pos == dirty_end_pos)
        return;

      dirty_start_pos = AdjustBlockSizeLow  (dirty_start_pos);
      dirty_end_pos   = AdjustBlockSizeHigh (dirty_end_pos);

      if (dirty_start_pos < data_start_pos)
        throw xtl::format_operation_exception ("", "Internal error: dirty_start_pos < data_start_pos");

      if (dirty_end_pos > data_end_pos)
        throw xtl::format_operation_exception ("", "Internal error: dirty_end_pos > data_end_pos");

      if ((filesize_t)dirty_end_pos > source_file->Size ())
        source_file->Resize ((filesize_t)dirty_end_pos);

      if (source_file->Seek (dirty_start_pos) != dirty_start_pos)
        throw xtl::format_operation_exception ("", "Can't seek file");      

      filesize_t size        = dirty_end_pos - dirty_start_pos,
                 crypto_size = dirty_end_pos - data_start_pos;

      size_t result = write_crypto_context.Update ((size_t)crypto_size, data_buffer.data () + dirty_start_pos - data_start_pos, read_write_buffer.data ());

      if (result != crypto_size)
        throw xtl::format_operation_exception ("", "Can't encrypt/decrypt data from file");

      result = source_file->Write (read_write_buffer.data (), (size_t)size);

      if (result != size)
        throw xtl::format_operation_exception ("", "Can't write data to file");

      dirty_start_pos = dirty_end_pos = file_pos;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("common::CryptoFileImpl::Impl::FlushBuffer");
      throw;
    }
  }

///Подготовка буфера данных
  void PrepareData (filepos_t pos)
  {
    if (pos >= data_start_pos && pos < data_end_pos)
      return;

    try
    {
        //сброс буфера данных

      FlushBuffer ();

      try
      {
          //расчёт файловой позиции начала буфера файла

        size_t block_size = read_crypto_context.BlockSize ();

        data_start_pos = (pos / block_size) * block_size;

          //изменение размеров буферов

        if (source_file->Mode () & FileMode_Read)
        {
          filesize_t src_file_size  = source_file->Size (),
                     available_size = src_file_size >= (filesize_t)data_start_pos ? src_file_size - data_start_pos : 0;

          if (available_size > data_buffer.size ())
            available_size = data_buffer.size ();                 

          size_t block_size = read_crypto_context.BlockSize ();

          if (available_size % block_size)
            available_size = available_size < block_size ? 0 : available_size / block_size * block_size - block_size;

          if (available_size)
          {
              //чтение и шифрование данных

            if (source_file->Seek (data_start_pos) != data_start_pos)
              throw xtl::format_operation_exception ("", "Can't seek file");

            size_t result = source_file->Read (read_write_buffer.data (), (size_t)available_size);

            if (result)
            {
              size_t updated_result = read_crypto_context.Update (result, read_write_buffer.data (), data_buffer.data ());

              if (result != updated_result)
                throw xtl::format_operation_exception ("", "Can't encrypt/decrypt data from file");
            }

            data_end_pos = AdjustBlockSizeLow (data_start_pos + result);
          }
          else data_end_pos = data_start_pos;                     

          if (size_t (data_end_pos - data_start_pos) < data_buffer.size () && available_size < data_buffer.size ())
            data_end_pos = data_start_pos + data_buffer.size ();
        }
        else
        {
          data_end_pos = data_start_pos + data_buffer.size ();
        }
      }
      catch (...)
      {
        data_start_pos = data_end_pos = 0;
        throw;
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("common::CryptoFileImpl::Impl::PrepareData");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

CryptoFileImpl::CryptoFileImpl (const FileImplPtr& file, size_t buffer_size, const char* read_crypto_method, const char* write_crypto_method, const void* key, size_t key_bits)
  : FileImpl (file->Mode ())
{
  try
  {
    impl = new Impl (file, buffer_size, read_crypto_method, write_crypto_method, key, key_bits);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::CryptoFileImpl");
    throw;
  }
}

CryptoFileImpl::CryptoFileImpl (const FileImplPtr& file, size_t buffer_size, const char* read_crypto_method, const void* key, size_t key_bits)
  : FileImpl (file->Mode () & ~(FileMode_Resize | FileMode_Write))
{
  try
  {
    impl = new Impl (file, buffer_size, read_crypto_method, read_crypto_method, key, key_bits);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::CryptoFileImpl");
    throw;
  }
}

CryptoFileImpl::~CryptoFileImpl ()
{
  try
  {
    Flush ();
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Получение пути к файлу
*/

const char* CryptoFileImpl::GetPath ()
{
  return impl->source_file->GetPath ();
}

/*
    Управление файловым указателем
*/

filepos_t CryptoFileImpl::Tell ()
{
  return impl->file_pos;
}

filepos_t CryptoFileImpl::Seek (filepos_t new_pos)
{
  if ((filesize_t)new_pos > impl->file_size)
    new_pos = impl->file_size;

  return impl->file_pos = new_pos;
}

void CryptoFileImpl::Rewind ()
{
  impl->file_pos = 0;
}

/*
    Управление размером файла
*/

filesize_t CryptoFileImpl::Size ()
{
  return impl->file_size;
}

void CryptoFileImpl::Resize (filesize_t new_size)
{
  try
  {
    impl->file_size = new_size;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Resize");
    throw;
  }
}

bool CryptoFileImpl::Eof ()
{
  return impl->file_pos == (filepos_t)impl->file_size;
}

/*
    Размер внутреннего буфера
*/

size_t CryptoFileImpl::GetBufferSize ()
{
  return impl->data_buffer.size ();
}

/*
    Чтение / запись
*/

size_t CryptoFileImpl::Read (void* buf, size_t size)
{
  try
  {
      //усечение файла при чтении

    if ((size_t)(impl->file_size - impl->file_pos) < size)
      size = (size_t)(impl->file_size - impl->file_pos);

    if (!size)
      return 0;

    filepos_t pos = impl->file_pos;
    char*     dst = (char*)buf;

      //последовательное чтение блоков данных

    while (size)
    {
        //подготовка буфера данных

      impl->PrepareData (pos);

        //копирование данных

      filesize_t  offset         = pos - impl->data_start_pos;
      filesize_t  available_size = impl->data_end_pos - pos;
      filesize_t  read_size      = size < available_size ? size : available_size;
      const char* src            = impl->data_buffer.data () + offset;

        //проверка возможности чтения

      if (!read_size)
        break; //end of file

      memcpy (dst, src, (size_t)read_size);

        //переход к следующему блоку

      size -= (size_t)read_size;
      dst  += read_size;
      pos  += read_size;
    }

      //обновление файлового указателя

    filesize_t result = pos - impl->file_pos;

    impl->file_pos = pos;

    return (size_t)result;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Read");
    throw;
  }
}

size_t CryptoFileImpl::Write (const void* buf, size_t size)
{
  try
  {
    if ((filesize_t)(impl->file_size - impl->file_pos) < (filesize_t)size)
    {
      if (Mode () & FileMode_Resize) Resize (impl->file_pos + size);
      else                           size = (size_t)(impl->file_size - impl->file_pos);
    }

    filepos_t   pos = impl->file_pos;
    const char* src = (const char*)buf;

      //последовательная запись блоков данных

    while (size)
    {
        //подготовка буфера данных

      impl->PrepareData (pos);

        //копирование данных

      filesize_t offset         = pos - impl->data_start_pos;
      filesize_t available_size = impl->data_end_pos - pos;
      size_t     write_size     = size < available_size ? size : (size_t)available_size;
      char*      dst            = impl->data_buffer.data () + offset;

      if (!write_size)
        break;

      memcpy (dst, src, write_size);

      if (impl->dirty_start_pos == impl->dirty_end_pos)
      {
        impl->dirty_start_pos = pos;
        impl->dirty_end_pos   = pos + write_size;
      }
      else
      {
        if (pos < impl->dirty_start_pos)                         impl->dirty_start_pos = pos;
        if ((filepos_t)(pos + write_size) > impl->dirty_end_pos) impl->dirty_end_pos   = pos + write_size;
      }

        //переход к следующему блоку

      src  += write_size;
      pos  += write_size;
      size -= write_size;
    }

      //обновление файлового указателя

    filesize_t result = pos - impl->file_pos;

    impl->file_pos = pos;

    return (size_t)result;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Write");
    throw;
  }
}

void CryptoFileImpl::Flush ()
{
  try
  {
    if (impl->file_size > impl->source_file->Size ())
      impl->source_file->Resize (impl->AdjustBlockSizeHigh ((filesize_t)impl->file_size));

      //запись буферов в исходный файл

    impl->FlushBuffer ();

      //сброс файловых буферов исходного файла

    impl->source_file->Flush ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Flush");
    throw;
  }
}
