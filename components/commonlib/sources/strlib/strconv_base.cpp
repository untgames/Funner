#include <common/strconv.h>
#include <common/utf_converter.h>
#include "strconv_base.h"
#include <common/exception.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///¬нутренн€€ реализаци€ фабрики конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StringConverterObject::Impl
{
  Impl(Encoding, Encoding);
  ~Impl();
  
  void Convert (const void*& source,
                size_t&      source_size,
                void*&       destination,
                size_t&      destination_size);
private:
  Encoding source_encoding;
  Encoding destination_encoding;
};

StringConverterObject::Impl::Impl(Encoding source, Encoding destination):
  source_encoding(source), destination_encoding(destination)
{
  static const char* METHOD_NAME = "common::StringConverterObject::Impl::Impl";

  if (source >= Encoding_Count)
    RaiseInvalidArgument(METHOD_NAME, "source_encoding");
  if (destination >= Encoding_Count)
    RaiseInvalidArgument(METHOD_NAME, "destination_encoding");
}

StringConverterObject::Impl::~Impl()
{
  static const char* METHOD_NAME = "common::StringConverterObject::Impl::~Impl";
  try
  {
  }
  catch (...)
  {
  }
}

void StringConverterObject::Impl::Convert (const void*& source,
                                           size_t&      source_size,
                                           void*&       destination,
                                           size_t&      destination_size)
{
  static const char* METHOD_NAME = "common::StringConverterObject::Impl::Convert";
  try
  {
    convert_encoding(source_encoding, source, source_size, destination_encoding, destination, destination_size);
  }
  catch(common::Exception& exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}
                                            
///////////////////////////////////////////////////////////////////////////////////////////////////
///ћетоды фабрики конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
StringConverterObject::StringConverterObject (const StringConverterObject& obj)
  :impl (obj.impl)
{
  static const char* METHOD_NAME = "common::StringConverterObject::StringConverterObject";
  try
  {
  }
  catch(common::Exception& exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

StringConverterObject::StringConverterObject (Encoding source_encoding,
                                              Encoding destination_encoding)
  :impl (new Impl(source_encoding, destination_encoding) )
{
  static const char* METHOD_NAME = "common::StringConverterObject::StringConverterObject";
  try
  {
  }
  catch(common::Exception& exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}

StringConverterObject::~StringConverterObject ()
{
  static const char* METHOD_NAME = "common::StringConverterObject::~StringConverterObject";
  try
  {
  }
  catch(...)
  {
  }
}

void StringConverterObject::AddRef  ()
{
  static const char* METHOD_NAME = "common::StringConverterObject::AddRef";
  try
  {
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
  }
}

void StringConverterObject::Release ()
{
  static const char* METHOD_NAME = "common::StringConverterObject::AddRef";
  try
  {
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
  }
}

void StringConverterObject::Convert (const void*& source,
                                     size_t&      source_size,
                                     void*&       destination,
                                     size_t&      destination_size)
{
  static const char* METHOD_NAME = "common::StringConverterObject::Convert";
  try
  {
    if (!impl)
      RaiseInvalidOperation(METHOD_NAME, "Possibly using destroyed interface");

    impl->Convert(source, source_size, destination, destination_size);
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
  }
}

IStringConverter* StringConverterObject::operator () ()
{
  static const char* METHOD_NAME = "common::StringConverterObject::operator ()";
  try
  {
    return new StringConverterObject(*this);
  }
  catch (common::Exception exception)
  {
    exception.Touch(METHOD_NAME);
  }
  
}

}// namespace common
