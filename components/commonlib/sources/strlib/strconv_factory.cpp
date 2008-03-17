#include <common/utf_converter.h>
#include <common/strconv.h>
#include <common/exception.h>

using namespace common;
///////////////////////////////////////////////////////////////////////////////////////////////////
///Скрытая реализация
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SubImpl
{
public:
  SubImpl(Encoding source, Encoding destination):
    source_encoding(source), destination_encoding(destination)
  { }
  
  void Convert(size_t&      source_size,
               const void*& source,
               size_t&      destination_size,
               void*&       destination)
  {
    convert_encoding(source_encoding, source, source_size,
                     destination_encoding, destination, destination_size);
  }
  
  void AddRef()
  {
    references_count++;
  }
  
  void Release()
  {
    references_count--;
  }
  
private: 
  Encoding source_encoding;
  Encoding destination_encoding;
  size_t references_count;
};

struct UtfConverter::Impl
{
public:
  Impl(Encoding source, Encoding destination):
    impl(source, destination)
  { }

  SubImpl impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализованный интерфейс преобразователя
///////////////////////////////////////////////////////////////////////////////////////////////////
class UtfStringConverter: public IStringConverter
{
public:
  UtfStringConverter(SubImpl* i):
    impl(i)
  {
    AddRef();
  }
  
  ~UtfStringConverter()
  {
    Release();
    impl = NULL;
  }
  
  void Convert(size_t&      source_size,
               const void*& source,
               size_t&      destination_size,
               void*&       destination)
  {
    if (!impl)
      RaiseInvalidOperation("common::UtfStringConverter::Convert()", "Trying to access object from invalid interface");
    
    impl->Convert(source_size, source, destination_size, destination);
  }  
  
  void AddRef()
  {
    if (!impl)
      RaiseInvalidOperation("common::UtfStringConverter::AddRef()", "Trying to access object from invalid interface");
      
    impl->AddRef();
  }
  void Release()
  {
    if (!impl)
      RaiseInvalidOperation("common::UtfStringConverter::Release()", "Trying to access object from invalid interface");
    
    impl->Release();
  }
  
private:
  SubImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Методы фабрики
///////////////////////////////////////////////////////////////////////////////////////////////////
UtfConverter::UtfConverter(Encoding source, Encoding destination)
{
  try
  {
    impl = new Impl(source, destination);
  }
  catch (...)
  {
  }
}

UtfConverter::~UtfConverter()
{
  try
  {
    delete impl;
  }
  catch (...)
  {
  }  
}

IStringConverter* UtfConverter::operator() ()
{
  static const char* METHOD_NAME = "common::UtfConverter::operator()";
  try
  {
    return new UtfStringConverter(&(impl->impl));
  }
  catch (Exception& exception)
  {
    exception.Touch(METHOD_NAME);
    throw;
  }
}
