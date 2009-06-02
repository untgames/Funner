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
  File          source_file;          //исходный файл
  CryptoContext read_crypto_context;  //контекст шифрования
  CryptoContext write_crypto_context; //контекст шифрования  
  Buffer        read_write_buffer;    //вспомогательный буфер для чтения / записи данных из исходного файла
  Buffer        data_buffer;          //кэш с данными после шифрования
  filepos_t     file_pos;             //файловая позиция
  filepos_t     data_start_pos;       //позиция начала данных
  bool          data_need_write;      //флаг необходимости записи данных в исходный файл

///Конструктор
  Impl (const File& in_source_file, const char* read_crypto_method, const char* write_crypto_method, const void* key, size_t key_bits)
    : source_file (in_source_file)
    , read_crypto_context (read_crypto_method, key, key_bits)
    , write_crypto_context (write_crypto_method, key, key_bits)
    , file_pos (0)
    , data_start_pos (0)
    , data_need_write (false)
  {
    size_t block_size = read_crypto_context.BlockSize ();

    if (!block_size)
      throw xtl::format_not_supported_exception ("", "Sequential crypting contexts not supported (method='%s')", read_crypto_method);
      
    if (block_size != write_crypto_context.BlockSize ())
      throw xtl::format_not_supported_exception ("", "Read crypto context '%s' block size %u defferes from write crypto context '%s' block size %u",
        read_crypto_method, read_crypto_context.BlockSize (), write_crypto_method, write_crypto_context.BlockSize ());
      
    size_t buffer_size = DEFAULT_BUFFER_SIZE;
    
    if (buffer_size > block_size) buffer_size = buffer_size / block_size * block_size;
    else                          buffer_size = block_size;

    data_buffer.reserve (buffer_size);
    read_write_buffer.reserve (buffer_size);
  }
  
///Корректировка размеров файла
  size_t AdjustBlockSizeHigh (size_t size)
  {
    size_t block_size = write_crypto_context.BlockSize ();
    
    if (!(size % block_size))
      return size;
      
    return size + block_size - size % block_size;
  }
  
  size_t AdjustBlockSizeLow (size_t size)
  {
    size_t block_size = read_crypto_context.BlockSize ();

    return size / block_size * block_size;
  }  
  
///Сброс буфера данных
  void FlushBuffer ()
  {
    try
    {
      if (!data_need_write)
        return;

      if (source_file.Seek (data_start_pos) != data_start_pos)
        throw xtl::format_operation_exception ("", "Can't seek file");

      read_write_buffer.resize (data_buffer.size (), false);

      size_t result = write_crypto_context.Update (data_buffer.size (), data_buffer.data (), read_write_buffer.data ());
      
      if (result != read_write_buffer.size ())
        throw xtl::format_operation_exception ("", "Can't encrypt/decrypt data from file");

      result = source_file.Write (read_write_buffer.data (), read_write_buffer.size ());

      if (result != read_write_buffer.size ())
        throw xtl::format_operation_exception ("", "Can't write data to file");

      data_need_write = false;
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
    if (pos >= data_start_pos && size_t (pos - data_start_pos) < data_buffer.size ())
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

        size_t available_size = AdjustBlockSizeLow (source_file.Size ()) - data_start_pos;        

        data_buffer.resize (available_size < data_buffer.capacity () ? available_size : data_buffer.capacity (), false);
        
        read_write_buffer.resize (data_buffer.size (), false);
          
          //чтение и шифрование данных
          
        if (source_file.Seek (data_start_pos) != data_start_pos)
          throw xtl::format_operation_exception ("", "Can't seek file");

        size_t result = source_file.Read (read_write_buffer.data (), read_write_buffer.size ());

        if (result != read_write_buffer.size ())
          throw xtl::format_operation_exception ("", "Can't read data from file");

        result = read_crypto_context.Update (result, read_write_buffer.data (), data_buffer.data ());        

        if (result != read_write_buffer.size ())
          throw xtl::format_operation_exception ("", "Can't encrypt/decrypt data from file");
      }
      catch (...)
      {
        data_start_pos = 0;

        data_buffer.resize (0);
        
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

CryptoFileImpl::CryptoFileImpl (const File& file, const char* read_crypto_method, const char* write_crypto_method, const void* key, size_t key_bits)  
  : FileImpl (file.Mode ())
{
  try
  {
    impl = new Impl (file, read_crypto_method, write_crypto_method, key, key_bits);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::CryptoFileImpl");
    throw;
  }
}

CryptoFileImpl::CryptoFileImpl (const File& file, const char* read_crypto_method, const void* key, size_t key_bits)
  : FileImpl (file.Mode () & ~(FileMode_Resize | FileMode_Write))
{
  try
  {
    impl = new Impl (file, read_crypto_method, read_crypto_method, key, key_bits);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::CryptoFileImpl");
    throw;
  }
}

CryptoFileImpl::~CryptoFileImpl ()
{
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
  try
  {
    if ((filesize_t)new_pos > Size ())
      Resize (new_pos);

    return impl->file_pos = new_pos;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Seek");
    throw;
  }
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
  return impl->AdjustBlockSizeLow (impl->source_file.Size ());
}

void CryptoFileImpl::Resize (filesize_t new_size)
{
  try
  {
    impl->FlushBuffer ();
    
    impl->source_file.Resize (impl->AdjustBlockSizeHigh (new_size));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Resize");
    throw;
  }
}

bool CryptoFileImpl::Eof ()
{
  return impl->file_pos == Size ();
}

/*
    Размер внутреннего буфера
*/

size_t CryptoFileImpl::GetBufferSize ()
{
  return impl->data_buffer.capacity ();
}

/*
    Чтение / запись
*/

size_t CryptoFileImpl::Read (void* buf, size_t size)
{
  try
  {
    filepos_t pos = impl->file_pos;
    char*     dst = (char*)buf;
    
      //последовательное чтение блоков данных

    while (size)
    {
        //подготовка буфера данных
      
      impl->PrepareData (pos);      
      
        //копирование данных

      size_t      offset         = pos - impl->data_start_pos;
      size_t      available_size = impl->data_buffer.size () - offset;
      size_t      read_size      = size < available_size ? size : available_size;
      const char* src            = impl->data_buffer.data () + offset;
      
        //проверка возможности чтения
        
      if (!available_size)
        break; //end of file

      memcpy (dst, src, read_size);
      
        //переход к следующему блоку

      size -= read_size;
      dst  += read_size;
      pos  += read_size;
    }

      //обновление файлового указателя

    size_t result = pos - impl->file_pos;

    impl->file_pos = pos;

    return result;
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
    filepos_t   pos = impl->file_pos;
    const char* src = (const char*)buf;
    
      //последовательная запись блоков данных
    
    while (size)
    {
        //подготовка буфера данных

      impl->PrepareData (pos);

        //копирование данных
        
      size_t offset         = pos - impl->data_start_pos;
      size_t available_size = impl->data_buffer.size () - offset;
      size_t write_size     = size < available_size ? size : available_size;
      char*  dst            = impl->data_buffer.data () + offset;
      
      if (!available_size)
      {
        if (!(Mode () & FileMode_Resize))
          break; //end of file & no resize

        write_size = size < impl->data_buffer.capacity () ? size : impl->data_buffer.capacity ();

        impl->data_buffer.resize (impl->AdjustBlockSizeHigh (write_size));

        available_size = impl->data_buffer.size () - offset;
      }

      memcpy (dst, src, write_size);

      impl->data_need_write = true;

        //переход к следующему блоку

      src  += write_size;
      pos  += write_size;
      size -= write_size;
    }

      //обновление файлового указателя

    size_t result = pos - impl->file_pos;

    impl->file_pos = pos;

    return result;
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
      //запись буферов в исходный файл

    impl->FlushBuffer ();

      //сброс файловых буферов исходного файла

    impl->source_file.Flush ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoFileImpl::Flush");
    throw;
  }
}
