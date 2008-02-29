#ifndef COMMONLIB_COMMON_STRCONV_HEADER
#define COMMONLIB_COMMON_STRCONV_HEADER

#include <stddef.h>

namespace common
{

class StringConverter
{
public:
  StringConverter(const char* source_encoding, const char* dest_encoding);
  StringConverter(const Converter& converter);
  ~StringConverter();
  
  StringConverter& operator =(const Converter&);
  
  void Convert(size_t& source_size, const void*& source, size_t& dest_size, const void*& dest);
  void operator()(size_t& source_size, const void*& source, size_t& dest_size, const void*& dest);
  
private:
  struct Impl;
  Impl* impl;
};

} //namespace common

#endif