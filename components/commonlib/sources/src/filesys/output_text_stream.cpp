#include <common/streams.h>
#include <stdarg.h>

using namespace common;
using namespace stl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации OutputTextStream
///////////////////////////////////////////////////////////////////////////////////////////////////
struct OutputTextStream::Impl
{
  File  file;     //файл ассоциированный с потоком
  char* buffer;   //буфер потока
  char* position; //позиция в буфере
  char* finish;   //указатель на конец буфера
  char  internal_buffer [OutputTextStream::DEFAULT_BUFFER_SIZE]; //внутренний буфер

  Impl  ();
  ~Impl ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление буфером
///////////////////////////////////////////////////////////////////////////////////////////////////  
  void   SetBufferSize (size_t new_buffer_size);
  size_t GetBufferSize () const { return (size_t)(finish - buffer); }
  void   FlushToFile   ();  

///////////////////////////////////////////////////////////////////////////////////////////////////
///Печать
///////////////////////////////////////////////////////////////////////////////////////////////////
  void VPrintf (const char* format,va_list list);
  void Print   (const char* string,size_t string_length);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с файлом
///////////////////////////////////////////////////////////////////////////////////////////////////
  void BindFile (const File&);
};

/*
    OutputTextStream::Impl
*/

OutputTextStream::Impl::Impl ()
{
  buffer   = internal_buffer;
  position = buffer;
  finish   = buffer + sizeof (internal_buffer);
}

OutputTextStream::Impl::~Impl ()
{
  FlushToFile ();
  
  if (buffer != internal_buffer)
    ::operator delete (buffer);
}

inline void OutputTextStream::Impl::SetBufferSize (size_t new_size)
{
  if (new_size <= GetBufferSize ())
    return;    

  FlushToFile ();

  if (buffer != internal_buffer)
    ::operator delete (buffer);

  try
  {
    buffer   = (char*)::operator new (new_size);
    position = buffer;
    finish   = buffer + new_size;    
  }
  catch (...)
  {
    buffer   = internal_buffer;
    position = buffer;
    finish   = buffer + sizeof (internal_buffer);

    throw;
  }
}

inline void OutputTextStream::Impl::FlushToFile ()
{
  if (position - buffer)
    file.Write (buffer,position-buffer);

  position = buffer;
}

inline void OutputTextStream::Impl::VPrintf (const char* format,va_list list)
{
  int size = vsnprintf (position,finish-position,format,list);
  
  if (size != -1)
  {
    position += size;
    return;
  }
    
  if (position != buffer)
  {
    FlushToFile ();

    size = vsnprintf (position,finish-position,format,list);
    
    if (size != -1)
    {
      position += size;
      return;
    }
  }

  string tmp = common::format (format,list);

  file.Write (tmp.c_str (),tmp.size ());

  position += tmp.size ();
}

inline void OutputTextStream::Impl::Print (const char* string,size_t size)
{
  if ((size_t)(finish - position) >= size)
  {
    memcpy (position,string,size);
    position += size;
  }
  else
  {
    FlushToFile ();

    file.Write (string,size);
  }
}

inline void OutputTextStream::Impl::BindFile (const File& new_file)
{
  if (file == new_file)
    return;

  FlushToFile ();

  file = new_file;
}

/*
    OutputTextStream
*/

OutputTextStream::OutputTextStream ()
{
  impl = new Impl;
}

OutputTextStream::OutputTextStream (size_t buffer_size)
{
  impl = new Impl;
  
  try
  {
    impl->SetBufferSize (buffer_size);
  }
  catch (...)
  {
    throw;
  }
}

OutputTextStream::OutputTextStream (const File& file,size_t buffer_size)
{ 
  impl = new Impl;
  
  try
  {
    impl->SetBufferSize (buffer_size);

    BindFile (file);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

OutputTextStream::OutputTextStream (const char* file_name,size_t buffer_size)
{
  impl = new Impl;
  
  try
  {
    impl->SetBufferSize (buffer_size);

    BindFile (file_name);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

OutputTextStream::~OutputTextStream ()
{
  delete impl;
}

void OutputTextStream::Printf (const char* format,...)
{
  if (!format)
    RaiseNullArgument ("OutputTextStream::Printf","format");
    
  if (!*format)
    return;

  va_list list;

  va_start (list,format);
  
  impl->VPrintf (format,list);
}

void OutputTextStream::VPrintf (const char* format,va_list list)
{     
  if (!format)
    RaiseNullArgument ("OutputTextStream::VPrintf","format");

  if (!*format)
    return;

  impl->VPrintf (format,list);    
}

void OutputTextStream::Print (const char* string)
{
  if (!string)
    RaiseNullArgument ("OutputTextStream::Print","string");

  impl->Print (string,strlen (string));
}

void OutputTextStream::Print (const char* string,size_t size)
{
  if (!string && size)
    RaiseNullArgument ("OutputTextStream::Print","string");

  impl->Print (string,size);
}

void OutputTextStream::SetBufferSize (size_t new_buffer_size)
{
  impl->SetBufferSize (new_buffer_size);
}

size_t OutputTextStream::GetBufferSize () const
{
  return impl->GetBufferSize ();
}

void OutputTextStream::Flush ()
{
  impl->FlushToFile ();
  impl->file.Flush ();
}

void OutputTextStream::BindFile (const File& file)
{
  impl->BindFile (file);  
}

void OutputTextStream::BindFile (const char* file_name)
{
  impl->BindFile (OutputFile (file_name));
}

void OutputTextStream::UnbindFile ()
{
  impl->BindFile (File ());
}
