#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

#include <wchar.h>
#include <stl/string_fwd>

namespace common
{

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
