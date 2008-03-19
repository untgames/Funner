#include "shared.h"
#include <common/exception.h>
#include "strconv_base.h"

using namespace stl;

namespace common
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор синглтона, регистрация конвертеров по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////

StringConverterManager::StringConverterManager()
{
  /// Исходная кодировка - ascii7
  RegisterConverter("ascii7",  "ascii7",   StringConverterObject(Encoding_ASCII7,  Encoding_ASCII7));
  RegisterConverter("ascii7",  "utf8",     StringConverterObject(Encoding_ASCII7,  Encoding_UTF8));
  RegisterConverter("ascii7",  "utf16le",  StringConverterObject(Encoding_ASCII7,  Encoding_UTF16LE));
  RegisterConverter("ascii7",  "utf16be",  StringConverterObject(Encoding_ASCII7,  Encoding_UTF16BE));
  RegisterConverter("ascii7",  "utf32le",  StringConverterObject(Encoding_ASCII7,  Encoding_UTF32LE));
  RegisterConverter("ascii7",  "utf32be",  StringConverterObject(Encoding_ASCII7,  Encoding_UTF32BE));
  /// Исходная кодировка - utf8
  RegisterConverter("utf8",    "ascii7",   StringConverterObject(Encoding_UTF8,    Encoding_ASCII7));
  RegisterConverter("utf8",    "utf8",     StringConverterObject(Encoding_UTF8,    Encoding_UTF8));
  RegisterConverter("utf8",    "utf16le",  StringConverterObject(Encoding_UTF8,    Encoding_UTF16LE));
  RegisterConverter("utf8",    "utf16be",  StringConverterObject(Encoding_UTF8,    Encoding_UTF16BE));
  RegisterConverter("utf8",    "utf32le",  StringConverterObject(Encoding_UTF8,    Encoding_UTF32LE));
  RegisterConverter("utf8",    "utf32be",  StringConverterObject(Encoding_UTF8,    Encoding_UTF32BE));
  /// Исходная кодировка - utf16le
  RegisterConverter("utf16le", "ascii7",   StringConverterObject(Encoding_UTF16LE, Encoding_ASCII7));
  RegisterConverter("utf16le", "utf8",     StringConverterObject(Encoding_UTF16LE, Encoding_UTF8));
  RegisterConverter("utf16le", "utf16le",  StringConverterObject(Encoding_UTF16LE, Encoding_UTF16LE));
  RegisterConverter("utf16le", "utf16be",  StringConverterObject(Encoding_UTF16LE, Encoding_UTF16BE));
  RegisterConverter("utf16le", "utf32le",  StringConverterObject(Encoding_UTF16LE, Encoding_UTF32LE));
  RegisterConverter("utf16le", "utf32be",  StringConverterObject(Encoding_UTF16LE, Encoding_UTF32BE));
  /// Исходная кодировка - utf16be
  RegisterConverter("utf16be", "ascii7",   StringConverterObject(Encoding_UTF16BE, Encoding_ASCII7));
  RegisterConverter("utf16be", "utf8",     StringConverterObject(Encoding_UTF16BE, Encoding_UTF8));
  RegisterConverter("utf16be", "utf16le",  StringConverterObject(Encoding_UTF16BE, Encoding_UTF16LE));
  RegisterConverter("utf16be", "utf16be",  StringConverterObject(Encoding_UTF16BE, Encoding_UTF16BE));
  RegisterConverter("utf16be", "utf32le",  StringConverterObject(Encoding_UTF16BE, Encoding_UTF32LE));
  RegisterConverter("utf16be", "utf32be",  StringConverterObject(Encoding_UTF16BE, Encoding_UTF32BE));
  /// Исходная кодировка - utf32le
  RegisterConverter("utf32le", "ascii7",   StringConverterObject(Encoding_UTF32LE, Encoding_ASCII7));
  RegisterConverter("utf32le", "utf8",     StringConverterObject(Encoding_UTF32LE, Encoding_UTF8));
  RegisterConverter("utf32le", "utf16le",  StringConverterObject(Encoding_UTF32LE, Encoding_UTF16LE));
  RegisterConverter("utf32le", "utf16be",  StringConverterObject(Encoding_UTF32LE, Encoding_UTF16BE));
  RegisterConverter("utf32le", "utf32le",  StringConverterObject(Encoding_UTF32LE, Encoding_UTF32LE));
  RegisterConverter("utf32le", "utf32be",  StringConverterObject(Encoding_UTF32LE, Encoding_UTF32BE));
  /// Исходная кодировка - utf32be
  RegisterConverter("utf32be", "ascii7",   StringConverterObject(Encoding_UTF32BE, Encoding_ASCII7));
  RegisterConverter("utf32be", "utf8",     StringConverterObject(Encoding_UTF32BE, Encoding_UTF8));
  RegisterConverter("utf32be", "utf16le",  StringConverterObject(Encoding_UTF32BE, Encoding_UTF16LE));
  RegisterConverter("utf32be", "utf16be",  StringConverterObject(Encoding_UTF32BE, Encoding_UTF16BE));
  RegisterConverter("utf32be", "utf32le",  StringConverterObject(Encoding_UTF32BE, Encoding_UTF32LE));
  RegisterConverter("utf32be", "utf32be",  StringConverterObject(Encoding_UTF32BE, Encoding_UTF32BE));
}

bool StringConverterManager::RegisterConverter (const char* source_encoding,
                                                const char* destination_encoding,
                                                const StringConverterSystem::ConverterFn& converter)
{
  static const char* METHOD_NAME = "common::StringConverterManager::RegisterConverter";
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "source_encoding");
      return false;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "destination_encoding");
      return false;
    }

    pair<StringConverterMap::iterator, bool> result = string_converter_map.insert_pair( strihash( destination_encoding, strihash(source_encoding) ), converter);
    if (!result.second)
    {
      //RaiseInvalidOperation(METHOD_NAME, "A converter functor for \'%s-to-%s\' already registered", source_encoding, destination_encoding);
      return false;
    }
    
    return true;
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

void StringConverterManager::UnregisterConverter (const char* source_encoding,
                                                  const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterManager::UnregisterConverter";
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "source_encoding");
      return;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "destination_encoding");
      return;
    }

    string_converter_map.erase( strihash( destination_encoding, strihash(source_encoding) ) );
  
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

void StringConverterManager::UnregisterAllConverters ()
{
  static const char* METHOD_NAME = "common::StringConverterManager::UnregisterAllConverters";
  try
  {
    string_converter_map.clear();
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

bool StringConverterManager::IsConverterRegistered(const char* source_encoding,
                                                  const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterManager::IsConverterRegistered";
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "source_encoding");
      return false;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "destination_encoding");
      return false;
    }

    StringConverterMap::iterator conv = string_converter_map.find( strihash( destination_encoding, strihash(source_encoding) ) );
    
    if (conv == string_converter_map.end())
      return false;
    
    return true;
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }  
}

IStringConverter* StringConverterManager::GetConverter(const char* source_encoding,
                                                       const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterManager::GetConverter";
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "source_encoding");
      return NULL;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument(METHOD_NAME, "destination_encoding");
      return NULL;
    }

    StringConverterMap::iterator conv = string_converter_map.find( strihash( destination_encoding, strihash(source_encoding) ) );
    
    if (conv == string_converter_map.end())
    {
      RaiseInvalidOperation(METHOD_NAME, "A converter functor for \'%s-to-%s\' is not registered", source_encoding, destination_encoding);
      return NULL;
    }
    
    return (conv->second)();
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

} // namespace common