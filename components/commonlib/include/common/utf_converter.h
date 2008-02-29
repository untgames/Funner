#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

#include <wchar.h>
#include <stl/string_fwd>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unicode-кодировки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Encoding
{
  Encoding_Ascii7,
  Encoding_Utf8,
  Encoding_Utf16LE,
  Encoding_Utf16BE,
  Encoding_Utf32LE,
  Encoding_Utf32BE,

  Encoding_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка char <-> wchar_t
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::string  tostring  (const wchar_t* string, int length);
stl::string  tostring  (const wchar_t* string);
stl::string  tostring  (const stl::wstring&);
stl::wstring towstring (const char* string, int length);
stl::wstring towstring (const char* string);
stl::wstring towstring (const stl::string&);

};

#endif
