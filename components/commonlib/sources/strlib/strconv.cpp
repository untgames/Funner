#include <common/strconv.h>
#include <common/strlib.h>
#include <common/exception.h>
#include <stl/hash_map>
#include <xtl/function.h>

using namespace stl;

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления конвертацией строк
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef hash_map<size_t, StringConverterSystem::ConverterFn> StringConverterMap;
StringConverterMap string_converter_map;

void StringConverterSystem::RegisterConverter (const char* source_encoding,
                                               const char* destination_encoding,
                                               const ConverterFn& converter)
{
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument("common::StringConverterSystem::RegisterConverter", "source_encoding");
      return;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument("common::StringConverterSystem::RegisterConverter", "destination_encoding");
      return;
    }

    pair<StringConverterMap::iterator, bool> result = string_converter_map.insert_pair( strhash( destination_encoding, strhash(source_encoding) ), converter);
    if (!result.second)
    {
      RaiseInvalidOperation("common::common::StringConverterSystem::RegisterConverter",
                            "A converter functor for \'%s-to-%s\' already registered", source_encoding, destination_encoding);
      return;
    }
    //RaiseNotImplemented("common::StringConverterSystem::RegisterConverter (...)");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverterSystem::RegisterConverter (...)");
    throw;
  }
}

void StringConverterSystem::UnregisterConverter (const char* source_encoding,
                                                 const char* destination_encoding)
{
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument("common::StringConverterSystem::UnregisterConverter", "source_encoding");
      return;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument("common::StringConverterSystem::UnregisterConverter", "destination_encoding");
      return;
    }

    string_converter_map.erase( strhash( destination_encoding, strhash(source_encoding) ) );
  
    //RaiseNotImplemented("common::StringConverterSystem::UnregisterConverter (...)");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverterSystem::UnregisterConverter (...)");
    throw;
  }
}

void StringConverterSystem::UnregisterAllConverters ()
{
  try
  {
    string_converter_map.clear();    
    //RaiseNotImplemented("common::StringConverterSystem::UnregisterAllConverters (...)");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverterSystem::UnregisterAllConverters (...)");
    throw;
  }
}

IStringConverter* GetConverter(const char* source_encoding,
                               const char* destination_encoding)
{
  try
  {
    if (!source_encoding)
    {
      RaiseNullArgument("common::GetConverter", "source_encoding");
      return NULL;
    }

    if (!destination_encoding)
    {
      RaiseNullArgument("common::GetConverter", "destination_encoding");
      return NULL;
    }

    StringConverterMap::iterator conv = string_converter_map.find( strhash( destination_encoding, strhash(source_encoding) ) );
    
    if (conv == string_converter_map.end())
    {
      RaiseInvalidOperation("common::GetConverter",
                            "A converter functor for \'%s-to-%s\' is not registered", source_encoding, destination_encoding);
      return NULL;
    }
    
    return (conv->second)();
    //RaiseNotImplemented("common::StringConverterSystem::UnregisterConverter (...)");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::GetConverter (...)");
    throw;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конвертер строк
///////////////////////////////////////////////////////////////////////////////////////////////////
StringConverter::StringConverter (const char* source_encoding, const char* destination_encoding)
{
  try
  {
    converter = GetConverter(source_encoding, destination_encoding);
  }
  catch (...)
  {
  }
}

StringConverter::StringConverter (const StringConverter& conv)
{
  try
  {
    converter = conv.converter;
    converter->AddRef();
  }
  catch (...)
  {
  }
}

StringConverter::~StringConverter ()
{
  try
  {
    converter->Release();
  }
  catch (...)
  {
  }
}

StringConverter& StringConverter::operator = (const StringConverter& conv)
{
  try
  {
    converter->Release();
    converter = conv.converter;
    converter->AddRef();
    //RaiseNotImplemented("common::StringConverter::operator =");
    return *this;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverter::operator =");
    throw;
  }
}
  
void StringConverter::Convert (size_t& source_buffer_size, const void*& source_buffer_ptr,
                               size_t& destination_buffer_size, void*& destination_buffer_ptr) const
{
  try
  {
    converter->Convert(source_buffer_size, source_buffer_ptr, destination_buffer_size, destination_buffer_ptr);
    //RaiseNotImplemented("common::StringConverter::Convert ()");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverter::Convert ()");
    throw;
  }
}

void StringConverter::operator () (size_t& source_buffer_size, const void*& source_buffer_ptr,
                                   size_t& destination_buffer_size, void*& destination_buffer_ptr) const
{
  try
  {
    converter->Convert(source_buffer_size, source_buffer_ptr, destination_buffer_size, destination_buffer_ptr);
    //RaiseNotImplemented("common::StringConverter::operator ()");
    //return;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverter::operator ()");
    throw;
  }
}

} //namespace common
