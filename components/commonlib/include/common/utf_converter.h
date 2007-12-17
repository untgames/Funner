#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

#include <wchar.h>
#include <stl/string_fwd>

namespace common
{

typedef unsigned int char32;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unicode-кодировки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Encoding
{
  Encoding_Ascii7,
  Encoding_Utf8,
  Encoding_Utf16BE,
  Encoding_Utf16LE,
  Encoding_Utf32BE,
  Encoding_Utf32LE
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: неверная кодировка Unicode
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct UtfExceptionTag;

//typedef DerivedException<Exception, UtfExceptionTag> UtfException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Результат перекодировки
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EncodingResult
{
  size_t source_buffer_processed_size;      //количество успешно обработанных байтов буфера-источника
  size_t destination_buffer_processed_size; //количество успешно обработанных байтов буфера-приёмника
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка Utf -> wint_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_decode (const void* source_buffer,            //буфер-источник с utf-строкой
                           size_t      source_buffer_size,       //размер буфера-источника в байтах
                           Encoding    source_buffer_encoding,   //кодировка буфера-источника
                           void*       destination_buffer,       //буфер-приёмник для декодируемой строки (wint_t)
                           size_t      destination_buffer_size); //размер буфера-приёмника в байтах

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка wint_t -> Utf
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //буфер-источник с wint_t - строкой
                           size_t      source_buffer_size,           //размер буфера-источника в байтах
                           void*       destination_buffer,           //буфер-приёмник с кодируемой строки (utf)
                           size_t      destination_buffer_size,      //размер буфера-приёмника в байтах
                           Encoding    destination_buffer_encoding); //кодировка буфера-приёмника

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
