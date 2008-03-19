#include "shared.h"
#include <common/strlib.h>
#include <common/exception.h>

using namespace stl;

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления конвертацией строк
///////////////////////////////////////////////////////////////////////////////////////////////////


bool StringConverterSystem::RegisterConverter (const char* source_encoding,
                                               const char* destination_encoding,
                                               const ConverterFn& converter)
{
  static const char* METHOD_NAME = "common::StringConverterSystem::RegisterConverter";
  try
  {
    return StringConverterManagerSingleton::Instance().RegisterConverter(source_encoding, destination_encoding, converter);
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

void StringConverterSystem::UnregisterConverter (const char* source_encoding,
                                                 const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterSystem::UnregisterConverter";
  try
  {
    StringConverterManagerSingleton::Instance().UnregisterConverter(source_encoding, destination_encoding);
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

void StringConverterSystem::UnregisterAllConverters ()
{
  static const char* METHOD_NAME = "common::StringConverterSystem::UnregisterAllConverters";
  try
  {
    StringConverterManagerSingleton::Instance().UnregisterAllConverters();    
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

bool StringConverterSystem::IsConverterRegistered (const char* source_encoding,
                                                   const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterSystem::IsConverterRegistered";
  try
  {
    return StringConverterManagerSingleton::Instance().IsConverterRegistered (source_encoding, destination_encoding);    
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
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
    converter = StringConverterManagerSingleton::Instance().GetConverter(source_encoding, destination_encoding);
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
    return *this;
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverter::operator =");
    throw;
  }
}
  
void StringConverter::Convert (const void*& source_buffer_ptr,
                               size_t&      source_buffer_size,
                               void*&       destination_buffer_ptr,
                               size_t&      destination_buffer_size) const
{
  try
  {
    converter->Convert(source_buffer_ptr, source_buffer_size, destination_buffer_ptr, destination_buffer_size);
  }
  catch (common::Exception exception)
  {
    exception.Touch("common::StringConverter::Convert ()");
    throw;
  }
}

} //namespace common
