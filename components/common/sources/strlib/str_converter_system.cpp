#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/bind.h>

#include <common/singleton.h>
#include <common/utf_converter.h>
#include <common/strwrap.h>
#include <common/strconv.h>
#include <common/exception.h>

using namespace stl;
using namespace common;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Utf-конвертер
///////////////////////////////////////////////////////////////////////////////////////////////////
class UtfConverter: public IStringConverter
{
  public:
    void AddRef  () {}
    void Release () {}

    void Convert (const void*& source, size_t& source_size, void*& destination, size_t& destination_size)
    {
      convert_encoding (source_encoding, source, source_size, destination_encoding, destination, destination_size);
    }
    
    static IStringConverter* Create (Encoding source_encoding, Encoding destination_encoding)
    {
      static bool         init = false;
      static UtfConverter converters [Encoding_Num][Encoding_Num];

      if (!init)
      {
        for (int source=Encoding_ASCII7; source<Encoding_Num; source++)
          for (int destination=Encoding_ASCII7; destination<Encoding_Num; destination++)
          {
            UtfConverter& conv = converters [source][destination];
            
            conv.source_encoding      = (Encoding)source;
            conv.destination_encoding = (Encoding)destination;
          }

        init = true;
      }

      return &converters [source_encoding][destination_encoding];
    }
    
  private:
    UtfConverter () {}

  private:
    Encoding source_encoding, destination_encoding;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон менеджера конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverterSystemImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StringConverterSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool RegisterConverter       (const char* source_encoding, const char* destination_encoding, const StringConverterSystem::ConverterFn&);
    void UnregisterConverter     (const char* source_encoding, const char* destination_encoding);
    void UnregisterAllConverters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка зарегистрированности конвертера
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsConverterRegistered (const char* source_encoding, const char* destination_encoding) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение конвертера
///////////////////////////////////////////////////////////////////////////////////////////////////
    IStringConverter* CreateConverter (const char* source_encoding, const char* destination_encoding) const;
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение хэша имён конвертера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetConverterHash (const char* source_encoding, const char* destination_encoding);

  private:
    typedef stl::hash_map<size_t, StringConverterSystem::ConverterFn> StringConverterMap;

  private:
    StringConverterMap converters;
};

typedef common::Singleton<StringConverterSystemImpl> StringConverterSystemImplSingleton;

/*
===================================================================================================
    StringConverterSystemImpl
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

namespace
{

const char* get_encoding_name (Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_ASCII7:  return "ASCII7";
    case Encoding_UTF8:    return "UTF8";
    case Encoding_UTF16LE: return "UTF16LE";
    case Encoding_UTF16BE: return "UTF16BE";
    default:               return "Unknown";
  }
}

}

StringConverterSystemImpl::StringConverterSystemImpl()
{ 
  for (int source=Encoding_ASCII7; source<Encoding_Num; source++)
    for (int destination=Encoding_ASCII7; destination<Encoding_Num; destination++)
      RegisterConverter (get_encoding_name ((Encoding)source), get_encoding_name((Encoding)destination),
                         xtl::bind (&UtfConverter::Create, (Encoding)source, (Encoding)destination));
}

/*
    Получение хэша имён конвертера
*/

size_t StringConverterSystemImpl::GetConverterHash (const char* source_encoding, const char* destination_encoding)
{
  return strihash (destination_encoding, strihash (source_encoding));
}

bool StringConverterSystemImpl::RegisterConverter
 (const char*                               source_encoding,
  const char*                               destination_encoding,
  const StringConverterSystem::ConverterFn& converter)
{
  static const char* METHOD_NAME = "common::StringConverterSystemImpl::RegisterConverter";

  try
  {
    if (!source_encoding)
      raise_null_argument ("", "source_encoding");

    if (!destination_encoding)
      raise_null_argument ("", "destination_encoding");
      
    size_t hash = GetConverterHash (source_encoding, destination_encoding);
      
    StringConverterMap::iterator iter = converters.find (hash);

    if (iter != converters.end ())
      return false;

    converters.insert_pair (hash, converter);

    return true;
  }
  catch (common::Exception& exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

void StringConverterSystemImpl::UnregisterConverter
 (const char* source_encoding,
  const char* destination_encoding)
{
  if (!source_encoding)
    return;

  if (!destination_encoding)
    return;

  converters.erase (GetConverterHash (source_encoding, destination_encoding));
}

void StringConverterSystemImpl::UnregisterAllConverters ()
{
  converters.clear ();
}

bool StringConverterSystemImpl::IsConverterRegistered
  (const char* source_encoding,
   const char* destination_encoding) const
{
  if (!source_encoding)
    return false;

  if (!destination_encoding)
    return false;

  StringConverterMap::const_iterator iter = converters.find (GetConverterHash (source_encoding, destination_encoding));

  return iter != converters.end ();
}

IStringConverter* StringConverterSystemImpl::CreateConverter
 (const char* source_encoding,
  const char* destination_encoding) const
{
  static const char* METHOD_NAME = "common::StringConverterSystemImpl::CreateConverter";

  try
  {
    if (!source_encoding)
      raise_null_argument ("", "source_encoding");

    if (!destination_encoding)
      raise_null_argument ("", "destination_encoding");

    StringConverterMap::const_iterator iter = converters.find (GetConverterHash (source_encoding, destination_encoding));
    
    if (iter == converters.end ())
      raise_invalid_operation ("", "A converter functor for \'%s-to-%s\' is not registered", source_encoding, destination_encoding);
   
    return (iter->second)();
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);
    throw;
  }
}

/*
===================================================================================================
    StringConverterSystem
===================================================================================================
*/

bool StringConverterSystem::RegisterConverter
 (const char*        source_encoding,
  const char*        destination_encoding,
  const ConverterFn& converter)
{
  return StringConverterSystemImplSingleton::Instance ().RegisterConverter (source_encoding, destination_encoding, converter);
}

void StringConverterSystem::UnregisterConverter (const char* source_encoding, const char* destination_encoding)
{
  StringConverterSystemImplSingleton::Instance().UnregisterConverter(source_encoding, destination_encoding);
}

void StringConverterSystem::UnregisterAllConverters ()
{
  StringConverterSystemImplSingleton::Instance ().UnregisterAllConverters ();
}

bool StringConverterSystem::IsConverterRegistered (const char* source_encoding, const char* destination_encoding)
{
  return StringConverterSystemImplSingleton::Instance ().IsConverterRegistered (source_encoding, destination_encoding);
}

/*
===================================================================================================
    Конвертер строк
===================================================================================================
*/

StringConverter::StringConverter (const char* source_encoding, const char* destination_encoding)
{
  converter = StringConverterSystemImplSingleton::Instance ().CreateConverter (source_encoding, destination_encoding); 
}

StringConverter::StringConverter (const StringConverter& conv)
  : converter (conv.converter)
{
  converter->AddRef ();
}

StringConverter::~StringConverter ()
{
  try
  {
    converter->Release ();
  }
  catch (...)
  {
  }
}

StringConverter& StringConverter::operator = (const StringConverter& conv)
{
  StringConverter (conv).Swap (*this);

  return *this;
}
  
void StringConverter::Convert
 (const void*& source_buffer_ptr,
  size_t&      source_buffer_size,
  void*&       destination_buffer_ptr,
  size_t&      destination_buffer_size) const
{
  try
  {
    converter->Convert (source_buffer_ptr, source_buffer_size, destination_buffer_ptr, destination_buffer_size);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("common::StringConverter::Convert");
    throw;
  }
}

/*
    Обмен
*/

void StringConverter::Swap (StringConverter& conv)
{
  IStringConverter* tmp = converter;
  converter             = conv.converter;
  conv.converter        = tmp;
}

namespace common
{

void swap (StringConverter& conv1, StringConverter& conv2)
{
  conv1.Swap (conv2);
}

} //namespace common
