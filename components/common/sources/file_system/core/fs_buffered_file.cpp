#include "shared.h"

using namespace common;
using namespace stl;

typedef xtl::uninitialized_storage<char> Buffer;

/*
    Описание реализации буферизированного файла
*/

struct BufferedFileImpl::Impl
{
  FileImplPtr base_file;            //базовый файл
  Buffer      buffer;               //файловый буфер
  filesize_t  file_size;            //размер файла
  filepos_t   file_pos;             //файловая позиция
  filepos_t   data_start_pos;       //позиция начала данных
  filepos_t   data_end_pos;         //позиция конца данных для чтения
  filepos_t   data_dirty_start_pos; //файловая позиция начала блока данных, требующего выгрузки на диск
  filepos_t   data_dirty_end_pos;   //файловая позиция конца блока данных, требующего выгрузки на диск
  filepos_t   data_tail_start_pos;  //файловая позиция начала блока незаписанных данных
  
///Конструктор
  Impl (FileImplPtr in_base_file, size_t buffer_size)
    : base_file (in_base_file)  
    , file_size (base_file->Size ())
    , file_pos ()
    , data_start_pos ()
    , data_end_pos ()
    , data_dirty_start_pos ()
    , data_dirty_end_pos ()
    , data_tail_start_pos (base_file->Size ())
  {
    buffer.resize (buffer_size);    
  }
  
///Сброс буфера
  void FlushBuffer ()
  {
    try
    {
      if (data_dirty_start_pos == data_dirty_end_pos)
        return;

      if ((filesize_t)data_dirty_end_pos > base_file->Size ())
        base_file->Resize ((filesize_t)data_dirty_end_pos);
        
      if (base_file->Seek (data_dirty_start_pos) != data_dirty_start_pos)
        throw xtl::format_operation_exception ("", "Can't seek file");      
        
      char*  data       = buffer.data () + (data_dirty_start_pos - data_start_pos);
      size_t size       = data_dirty_end_pos - data_dirty_start_pos;
      size_t write_size = base_file->Write (data, size);

      if (write_size != size)
        throw xtl::format_operation_exception ("", "Can't write file");

      if (data_dirty_end_pos > data_tail_start_pos)
        data_tail_start_pos = data_dirty_end_pos;

      data_dirty_start_pos = 0;
      data_dirty_end_pos   = 0;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("common::BufferedFileImpl::Impl::FlushBuffer");
      throw;
    }
  }  

///Подготовка данных
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
        data_start_pos = pos;
        
        if (base_file->Mode () & FileMode_Read)
        {

            //изменение размеров буферов

          size_t available_size = base_file->Size () - data_start_pos;

          if (available_size > buffer.size ())
            available_size = buffer.size ();

          if (data_start_pos > data_tail_start_pos)
            available_size = 0;

          if ((filepos_t)(data_start_pos + available_size) > data_tail_start_pos)
            available_size = data_tail_start_pos - data_start_pos;

            //чтение данных
        
          if (available_size)
          {
            if (base_file->Seek (data_start_pos) != data_start_pos)
              throw xtl::format_operation_exception ("", "Can't seek file");

            size_t result = base_file->Read (buffer.data (), available_size);        

            data_end_pos = data_start_pos + result;
          }
          else data_end_pos = data_start_pos;

          if (size_t (data_end_pos - data_start_pos) < buffer.size () && available_size < buffer.size ())
            data_end_pos = data_start_pos + buffer.size ();
        }
        else
        {
          data_end_pos = data_start_pos + buffer.size ();
        }
      }
      catch (...)
      {
        data_start_pos = 0;
        data_end_pos   = 0;

        throw;
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("common::BufferedFileImpl::PrepareData");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

BufferedFileImpl::BufferedFileImpl (FileImplPtr in_base_file, size_t buffer_size)
  : FileImpl (in_base_file->Mode ())
  , impl (new Impl (in_base_file, buffer_size))
{
}

BufferedFileImpl::~BufferedFileImpl ()
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

const char* BufferedFileImpl::GetPath ()
{
  return impl->base_file->GetPath ();
}

/*
    Размер буфера
*/

size_t BufferedFileImpl::GetBufferSize ()
{
  return impl->buffer.capacity ();
}

/*
    Управление файловым указателем
*/

filepos_t BufferedFileImpl::Tell ()
{
  return (filepos_t)impl->file_pos;
}

filepos_t BufferedFileImpl::Seek (filepos_t new_pos)
{
  if ((filesize_t)new_pos > impl->file_size)
    new_pos = impl->file_size;
    
  impl->file_pos = new_pos;

  return impl->file_pos;
}

void BufferedFileImpl::Rewind ()
{
  impl->file_pos = 0;
}

/*
    Размер файла
*/


filesize_t BufferedFileImpl::Size ()
{
  return impl->file_size;
}

void BufferedFileImpl::Resize (filesize_t new_size)
{
  try
  {
    impl->file_size = new_size;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BufferedFileImpl::Resize");
    throw;
  }
}

bool BufferedFileImpl::Eof ()
{
  return (filesize_t)impl->file_pos == impl->file_size;
}

/*
    Чтение / запись
*/

size_t BufferedFileImpl::Read (void* buf, size_t size)
{
  try
  {
      //усечение файла при чтении

    if (impl->file_size - impl->file_pos < size)
      size = impl->file_size - impl->file_pos;

    if (size > impl->buffer.capacity ())
    {      
        //чтение в обход кэша
        
      impl->FlushBuffer ();

      if (impl->base_file->Seek (impl->file_pos) != impl->file_pos)
        throw xtl::format_operation_exception ("", "Can't seek file");

      size_t read_size = impl->base_file->Read (buf, size);

      if (read_size != size)
        throw xtl::format_operation_exception ("", "Can't read file");

      impl->file_pos += read_size;

      return read_size;
    }    

      //последовательное чтение блоков данных

    filepos_t pos = impl->file_pos;
    char*     dst = (char*)buf;      

    while (size)
    {
        //подготовка буфера данных

      impl->PrepareData (pos);

        //копирование данных

      size_t      offset         = pos - impl->data_start_pos;
      size_t      available_size = impl->data_end_pos - pos;
      size_t      read_size      = size < available_size ? size : available_size;
      const char* src            = impl->buffer.data () + offset;

        //проверка возможности чтения

      if (!read_size)
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

size_t BufferedFileImpl::Write (const void* buf,size_t size)
{
  try
  {
    if ((filesize_t)(impl->file_size - impl->file_pos) < (filesize_t)size)
    {
      if (Mode () & FileMode_Resize) Resize (impl->file_pos + size);
      else                           size = impl->file_size - impl->file_pos;
    }

    if (!size)
      return 0;

    if (size > impl->buffer.capacity ())
    {
      //запись в обход кэша

      impl->FlushBuffer ();

      if ((filesize_t)(impl->file_pos + size) > impl->base_file->Size ())
        impl->base_file->Resize ((filesize_t)(impl->file_pos + size));

      if (impl->base_file->Seek (impl->file_pos) != impl->file_pos)
        throw xtl::format_operation_exception ("", "Can't seek file");

      size_t write_size = impl->base_file->Write (buf, size);
      
      if (write_size != size)
        throw xtl::format_operation_exception ("", "Can't write file");        
        
      impl->file_pos += write_size;
        
      return write_size;
    }    

      //последовательная запись блоков данных

    filepos_t   pos = impl->file_pos;
    const char* src = (const char*)buf;

    while (size)
    {
        //подготовка буфера данных

      impl->PrepareData (pos);

        //копирование данных

      size_t offset         = pos - impl->data_start_pos;
      size_t available_size = impl->data_end_pos - pos;
      size_t write_size     = size < available_size ? size : available_size;
      char*  dst            = impl->buffer.data () + offset;

      if (!write_size)
        break;
      
      memcpy (dst, src, write_size);
      
      if (impl->data_dirty_start_pos == impl->data_dirty_end_pos)
      {
        impl->data_dirty_start_pos = pos;
        impl->data_dirty_end_pos   = pos + write_size;
      }
      else
      {
        if (pos < impl->data_dirty_start_pos)                         impl->data_dirty_start_pos = pos;
        if ((filepos_t)(pos + write_size) > impl->data_dirty_end_pos) impl->data_dirty_end_pos   = pos + write_size;
      }

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
    exception.touch ("common::BufferedFileImpl::Write");
    throw;
  }  
}

void BufferedFileImpl::Flush ()
{
  try
  {
    impl->FlushBuffer ();

    impl->base_file->Flush  ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BufferedFileImpl::Flush");
    throw;
  }
}
