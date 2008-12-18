#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

#include <wchar.h>
#include <stl/string_fwd>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Допустимые кодировок
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Encoding
{
  Encoding_ASCII7,
  Encoding_UTF8,
  Encoding_UTF16LE,
  Encoding_UTF16BE,
  Encoding_UTF32LE,
  Encoding_UTF32BE,

  Encoding_Num
};

void convert_encoding (Encoding       source_encoding,
                       const void*&   source,
                       size_t&        source_size,
                       Encoding       destination_encoding,
                       void*&         destination,
                       size_t&        destination_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка char <-> wchar_t
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::string  tostring  (const wchar_t* string, int length);
stl::string  tostring  (const wchar_t* string);
stl::string  tostring  (const stl::wstring&);
stl::wstring towstring (const char* string, int length);
stl::wstring towstring (const char* string);
stl::wstring towstring (const stl::string&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сжатие wchar_t строк
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t       wchar_decompress (const wchar_t* source, size_t source_length, char* destination, size_t max_destination_length);
size_t       wchar_decompress (const wchar_t* source, char* destination, size_t max_destination_length);
size_t       wchar_compress   (const char* source, size_t source_length, wchar_t* destination, size_t max_destination_length);
size_t       wchar_compress   (const char* source, wchar_t* destination, size_t max_destination_length);
stl::string  wchar_decompress (const wchar_t* source, size_t source_length);
stl::string  wchar_decompress (const wchar_t* source);
stl::string  wchar_decompress (const stl::wstring& source);
stl::wstring wchar_compress   (const char* source, size_t source_length);
stl::wstring wchar_compress   (const char* source);
stl::wstring wchar_compress   (const stl::string& source);

};

#endif
