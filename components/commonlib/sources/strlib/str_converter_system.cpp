#include <stl/hash_map>

#include <xtl/function.h>

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
template <Encoding source_encoding, Encoding destination_encoding>
struct UtfConverter: public IStringConverter
{
  void AddRef  () {}
  void Release () {}

  void Convert (const void*& source, size_t& source_size, void*& destination, size_t& destination_size)
  {
    convert_encoding (source_encoding, source, source_size, destination_encoding, destination, destination_size);
  }
  
  static IStringConverter* Create ()
  {
    UtfConverter converter;

    return &converter;
  }
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

StringConverterSystemImpl::StringConverterSystemImpl()
{
    //исходная кодировка - ascii7

  RegisterConverter ("ascii7", "ascii7",  &UtfConverter<Encoding_ASCII7, Encoding_ASCII7>::Create);
  RegisterConverter ("ascii7", "utf8",    &UtfConverter<Encoding_ASCII7, Encoding_UTF8>::Create);
  RegisterConverter ("ascii7", "utf16le", &UtfConverter<Encoding_ASCII7, Encoding_UTF16LE>::Create);
  RegisterConverter ("ascii7", "utf16be", &UtfConverter<Encoding_ASCII7, Encoding_UTF16BE>::Create);
  RegisterConverter ("ascii7", "utf32le", &UtfConverter<Encoding_ASCII7, Encoding_UTF32LE>::Create);
  RegisterConverter ("ascii7", "utf32be", &UtfConverter<Encoding_ASCII7, Encoding_UTF32BE>::Create);

    //исходная кодировка - utf8

  RegisterConverter ("utf8", "ascii7",  &UtfConverter<Encoding_UTF8, Encoding_ASCII7>::Create);
  RegisterConverter ("utf8", "utf8",    &UtfConverter<Encoding_UTF8, Encoding_UTF8>::Create);
  RegisterConverter ("utf8", "utf16le", &UtfConverter<Encoding_UTF8, Encoding_UTF16LE>::Create);
  RegisterConverter ("utf8", "utf16be", &UtfConverter<Encoding_UTF8, Encoding_UTF16BE>::Create);
  RegisterConverter ("utf8", "utf32le", &UtfConverter<Encoding_UTF8, Encoding_UTF32LE>::Create);
  RegisterConverter ("utf8", "utf32be", &UtfConverter<Encoding_UTF8, Encoding_UTF32BE>::Create);

    //исходная кодировка - utf16le

  RegisterConverter ("utf16le", "ascii7",  &UtfConverter<Encoding_UTF16LE, Encoding_ASCII7>::Create);
  RegisterConverter ("utf16le", "utf8",    &UtfConverter<Encoding_UTF16LE, Encoding_UTF8>::Create);
  RegisterConverter ("utf16le", "utf16le", &UtfConverter<Encoding_UTF16LE, Encoding_UTF16LE>::Create);
  RegisterConverter ("utf16le", "utf16be", &UtfConverter<Encoding_UTF16LE, Encoding_UTF16BE>::Create);
  RegisterConverter ("utf16le", "utf32le", &UtfConverter<Encoding_UTF16LE, Encoding_UTF32LE>::Create);
  RegisterConverter ("utf16le", "utf32be", &UtfConverter<Encoding_UTF16LE, Encoding_UTF32BE>::Create);

    //исходная кодировка - utf16be

  RegisterConverter ("utf16be", "ascii7",  &UtfConverter<Encoding_UTF16BE, Encoding_ASCII7>::Create);
  RegisterConverter ("utf16be", "utf8",    &UtfConverter<Encoding_UTF16BE, Encoding_UTF8>::Create);
  RegisterConverter ("utf16be", "utf16le", &UtfConverter<Encoding_UTF16BE, Encoding_UTF16LE>::Create);
  RegisterConverter ("utf16be", "utf16be", &UtfConverter<Encoding_UTF16BE, Encoding_UTF16BE>::Create);
  RegisterConverter ("utf16be", "utf32le", &UtfConverter<Encoding_UTF16BE, Encoding_UTF32LE>::Create);
  RegisterConverter ("utf16be", "utf32be", &UtfConverter<Encoding_UTF16BE, Encoding_UTF32BE>::Create);

    //исходная кодировка - utf32le

  RegisterConverter ("utf32le", "ascii7",  &UtfConverter<Encoding_UTF32LE, Encoding_ASCII7>::Create);
  RegisterConverter ("utf32le", "utf8",    &UtfConverter<Encoding_UTF32LE, Encoding_UTF8>::Create);
  RegisterConverter ("utf32le", "utf16le", &UtfConverter<Encoding_UTF32LE, Encoding_UTF16LE>::Create);
  RegisterConverter ("utf32le", "utf16be", &UtfConverter<Encoding_UTF32LE, Encoding_UTF16BE>::Create);
  RegisterConverter ("utf32le", "utf32le", &UtfConverter<Encoding_UTF32LE, Encoding_UTF32LE>::Create);
  RegisterConverter ("utf32le", "utf32be", &UtfConverter<Encoding_UTF32LE, Encoding_UTF32BE>::Create);

    //исходная кодировка - utf32be

  RegisterConverter ("utf32be", "ascii7",  &UtfConverter<Encoding_UTF32BE, Encoding_ASCII7>::Create);
  RegisterConverter ("utf32be", "utf8",    &UtfConverter<Encoding_UTF32BE, Encoding_UTF8>::Create);
  RegisterConverter ("utf32be", "utf16le", &UtfConverter<Encoding_UTF32BE, Encoding_UTF16LE>::Create);
  RegisterConverter ("utf32be", "utf16be", &UtfConverter<Encoding_UTF32BE, Encoding_UTF16BE>::Create);
  RegisterConverter ("utf32be", "utf32le", &UtfConverter<Encoding_UTF32BE, Encoding_UTF32LE>::Create);
  RegisterConverter ("utf32be", "utf32be", &UtfConverter<Encoding_UTF32BE, Encoding_UTF32BE>::Create);
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
      RaiseNullArgument ("", "source_encoding");

    if (!destination_encoding)
      RaiseNullArgument ("", "destination_encoding");
      
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
      RaiseNullArgument ("", "source_encoding");

    if (!destination_encoding)
      RaiseNullArgument ("", "destination_encoding");

    StringConverterMap::const_iterator iter = converters.find (GetConverterHash (source_encoding, destination_encoding));
    
    if (iter == converters.end ())
      RaiseInvalidOperation ("", "A converter functor for \'%s-to-%s\' is not registered", source_encoding, destination_encoding);
   
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
