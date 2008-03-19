#ifndef COMMON_STRLIB_STRCONV_BASE_HEADER
#define COMMON_STRLIB_STRCONV_BASE_HEADER

#include <common/strconv.h>
#include <common/utf_converter.h>
#include <xtl/shared_ptr.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Стандартная фабрика конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverterObject: public IStringConverter
{
public:
  StringConverterObject(const StringConverterObject&);
  StringConverterObject(Encoding, Encoding);
  ~StringConverterObject();
  
  void AddRef();
  void Release();
  
  void Convert (const void*& source,
                size_t&      source_size,
                void*&       destination,
                size_t&      destination_size);

  IStringConverter* operator () ();
private:
  
  struct Impl;
  xtl::shared_ptr<Impl> impl;
};

}//namespace common

#endif