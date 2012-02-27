#include "shared.h"

using namespace common;

/*
    Описание реализации парсера
*/

struct Parser::Impl
{
  ParseLog  log;  //протокол разбора
  ParseNode root; //корень дерева разбора
  
///Конструкторы
  Impl (ParseLog in_log, const char* file_name, const char* format)
    : log (in_log)
  {
      //проверка корректности аргументов

    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    if (!format)
      throw xtl::make_null_argument_exception ("", "format");
      
      //загрузка буфера

    stl::string parse_buffer;

    FileSystem::LoadTextFile (file_name, parse_buffer);    

    try
    {
        //разбор    

      root = parse (log, format, file_name, parse_buffer.size () + 1, &parse_buffer [0]);
    }
    catch (std::exception& exception)
    {
      log.FatalError ("exception: %s", exception.what ());
    }
    catch (...)
    {
      log.FatalError ("unknown exception");
    }
  }
  
  Impl (ParseLog in_log, const char* name, size_t buffer_size, const char* buffer, const char* format)
    : log (in_log)
  {
      //проверка корректности аргументов

    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!buffer && buffer_size)
      throw xtl::make_null_argument_exception ("", "buffer");

    if (!format)
      throw xtl::make_null_argument_exception ("", "format");

        //копирование буфера

    stl::string parse_buffer (buffer, buffer_size);    
      
    try
    {    
        //разбор

      root = parse (log, format, name, parse_buffer.size () + 1, &parse_buffer [0]);
    }
    catch (std::exception& exception)
    {
      log.FatalError ("exception: %s", exception.what ());
    }
    catch (...)
    {
      log.FatalError ("unknown exception");
    }    
  }  
};

/*
    Конструкторы / деструктор
*/

Parser::Parser (const char* file_name, const char* format)
{
  try
  {
    ParseLog log;
    
    impl = new Impl (log, file_name, format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Parser::Parser(const char*, const char*)");
    throw;
  }  
}

Parser::Parser (const char* name, size_t buffer_size, const char* buffer, const char* format)
{
  try
  {
    ParseLog log;
    
    impl = new Impl (log, name, buffer_size, buffer, format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Parser::Parser(const char*, size_t, const char*, const char*)");
    throw;
  }
}

Parser::Parser (ParseLog& log, const char* file_name, const char* format)
{
  try
  {
    impl = new Impl (log, file_name, format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Parser::Parser(ParseLog&, const char*, const char*)");
    throw;
  }  
}

Parser::Parser (ParseLog& log, const char* name, size_t buffer_size, const char* buffer, const char* format)
{
  try
  {
    impl = new Impl (log, name, buffer_size, buffer, format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Parser::Parser(ParseLog&, const char*, size_t, const char*, const char*)");
    throw;
  }
}

Parser::~Parser ()
{
}

/*
    Протокол разбора
*/

ParseLog& Parser::Log ()
{
  return impl->log;
}

const ParseLog& Parser::Log () const
{
  return impl->log;
}

/*
    Корень дерева разбора
*/

ParseNode Parser::Root () const
{
  return impl->root;
}

/*
    Обмен
*/

void Parser::Swap (Parser& parser)
{
  impl.swap (parser.impl);
}

namespace common
{

void swap (Parser& parser1, Parser& parser2)
{
  parser1.Swap (parser2);
}

}
