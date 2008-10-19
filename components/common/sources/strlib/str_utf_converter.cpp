#include <wchar.h>
#include <string.h>

#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/string.h>

#include <common/utf_converter.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчики отдельных символов
///////////////////////////////////////////////////////////////////////////////////////////////////
template<Encoding encoding>
bool decoder (const void*&, size_t&, void*&, size_t&)
{
  return true;
}

template<Encoding encoding>
bool encoder (const void*&, size_t&, void*&, size_t&)
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Декодирование
///////////////////////////////////////////////////////////////////////////////////////////////////
template<>
bool decoder<Encoding_ASCII7> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const unsigned char* src_ptr = (const unsigned char*)src_char;
  size_t* dst_ptr = (size_t*)dst_char;

  if (src_size < sizeof(char))
    return false;

  (*dst_ptr) = (*src_ptr > 0x7F) ? 0x3F : (*src_ptr);

  src_ptr++;
  src_size -= sizeof(char);

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF8> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const unsigned char* src_ptr = (const unsigned char*)src_char;
  size_t* dst_ptr = (size_t*)dst_char;

  if (src_size < sizeof(char))
    return false;

  if (*src_ptr >= 0xF0)
  {
    if (src_size < sizeof(char)*4)
      return false;

    *dst_ptr =   (*(src_ptr+3) & 0x3F) +
               ( (*(src_ptr+2) & 0x3F) << 6 ) +
               ( (*(src_ptr+1) & 0x3F) << 12 ) +
               ( (*(src_ptr) & 0x07) << 18 );

    src_ptr += 4;
    src_size -= sizeof(char) * 4;
  }
  else if (*src_ptr >= 0xE0)
  {
    if (src_size < sizeof(char)*3)
      return false;

    *dst_ptr =   (*(src_ptr+2) & 0x3F) +
               ( (*(src_ptr+1) & 0x3F) << 6 ) +
               ( (*(src_ptr) & 0x0F) << 12 );

    src_ptr += 3;
    src_size -= sizeof(char) * 3;
  }
  else if (*src_ptr >= 0xC0)
  {
    if (src_size < sizeof(char)*2)
      return false;

    *dst_ptr =   (*(src_ptr+1) & 0x3F) +
               ( (*(src_ptr) & 0x1F) << 6 );

    src_ptr += 2;
    src_size -= sizeof(char) * 2;
  }
  else
  {
    *dst_ptr = (size_t)(*src_ptr > 0x7F ? 0x3F : *src_ptr);

    src_ptr++;
    src_size -= sizeof(char);
  }

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF16LE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const wchar_t* src_ptr = (const wchar_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (src_size < sizeof(wchar_t))
    return false;

  if ( (*src_ptr < 0xD800) || (*src_ptr > 0xDFFF) )
  {
    *dst_ptr = *src_ptr;
    src_ptr++;
    src_size -= sizeof(wchar_t);
  }
  else
  {
    if (src_size < sizeof(wchar_t)*2)
      return false;

    if ( (*src_ptr > 0xDBFF ) || (*(src_ptr+1) < 0xDC00) || (*(src_ptr+1) > 0xDFFF) )
      *dst_ptr = 0x3F;
    else
      *dst_ptr = (*src_ptr & 0x03FF) + ( (*(src_ptr+1) & 0x03FF) << 10) + 0x10000;

    src_ptr += 2;
    src_size += sizeof(wchar_t)*2;
  }

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF16BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const wchar_t* src_ptr = (const wchar_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;
  wchar_t tmp0, tmp1;

  if (src_size < sizeof(wchar_t))
    return false;

  tmp0 = ((*src_ptr >> 8) & 0xFF) + ((*src_ptr & 0xFF) << 8);

  if ( (tmp0 < 0xD800) || (tmp0 > 0xDFFF) )
  {
    *dst_ptr = tmp0;
    src_ptr++;
    src_size -= sizeof(wchar_t);
  }
  else
  {
    if (src_size < sizeof(wchar_t)*2)
      return false;

    tmp1 = ((*(src_ptr+1) >> 8) & 0xFF) + ((*(src_ptr+1) & 0xFF) << 8);

    if ( (tmp0 > 0xDBFF ) || (tmp1 < 0xDC00) || (tmp1 > 0xDFFF) )
      *dst_ptr = 0x3F;
    else
      *dst_ptr = ( tmp0 & 0x03FF) + ( ( tmp1 & 0x03FF) << 10) + 0x10000;

    src_ptr += 2;
    src_size -= sizeof(wchar_t)*2;
  }

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF32LE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (src_size < sizeof(size_t))
    return false;

  *dst_ptr = *src_ptr;
  src_ptr++;
  src_size -= sizeof(size_t);

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF32BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (src_size < sizeof(size_t))
    return false;

  *dst_ptr = ((*src_ptr & 0xFF) << 24) +
             ((*src_ptr & 0xFF00) << 8) +
             ((*src_ptr & 0xFF0000) >> 8) +
             ((*src_ptr & 0xFF000000) >> 24);

  src_ptr++;
  src_size -= sizeof(size_t);

  src_char = src_ptr;
  return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///Кодирование
///////////////////////////////////////////////////////////////////////////////////////////////////
template<>
bool encoder<Encoding_ASCII7> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  unsigned char* dst_ptr = (unsigned char*) dst_char;

  if (dst_size < sizeof(char))
    return false;

  *dst_ptr = (char)((*src_ptr) > 0x7F ? 0x3F : (*src_ptr));

  dst_ptr++;
  dst_size -= sizeof(char);

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF8> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  unsigned char* dst_ptr = (unsigned char*) dst_char;

  if (*src_ptr > 0xFFFF)
  {
    if (dst_size < sizeof(char)*4)
      return false;

    *(dst_ptr+3) = ((*src_ptr) & 0x3F) | 0x80;
    *(dst_ptr+2) = ( ( (*src_ptr) >> 6) & 0x3F) | 0x80;
    *(dst_ptr+1) = ( ( (*src_ptr) >> 12) & 0x3F) | 0x80;
    *(dst_ptr) = ( ( (*src_ptr) >> 18) & 0x07) | 0xF0;

    dst_ptr += 4;
    dst_size -= sizeof(char)*4;
  }
  else if (*src_ptr > 0x7FF)
  {
    if (dst_size < sizeof(char)*3)
      return false;

    *(dst_ptr+2) = ((*src_ptr) & 0x3F) | 0x80;
    *(dst_ptr+1) = ( ( (*src_ptr) >> 6) & 0x3F) | 0x80;
    *(dst_ptr) = ( ( (*src_ptr) >> 12) & 0x0F) | 0xE0;

    dst_ptr += 3;
    dst_size -= sizeof(char)*3;
  }
  else if (*src_ptr > 0x7F)
  {
    if (dst_size < sizeof(char)*2)
      return false;

    *(dst_ptr+1) = ((*src_ptr) & 0x3F) | 0x80;
    *(dst_ptr) = ( ( (*src_ptr) >> 6) & 0x1F) | 0xC0;

    dst_ptr += 2;
    dst_size -= sizeof(char)*2;
  }
  else
  {
    if (dst_size < sizeof(char))
      return false;

    *(dst_ptr) = (*src_ptr) & 0x7F;

    dst_ptr ++;
    dst_size -= sizeof(char);
  }

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF16LE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  wchar_t* dst_ptr = (wchar_t*) dst_char;
  size_t tmp;

  if (*src_ptr > 0x10000)
  {
    if (dst_size < sizeof(wchar_t)*2)
      return false;

    tmp = *src_ptr - 0x10000;
    *dst_ptr = (wchar_t) ( ( (tmp >> 10) & 0x03FF ) + 0xD800);
    *(dst_ptr+1) = (wchar_t) ( ( tmp & 0x03FF ) + 0xDC00);

    dst_ptr +=2;
    dst_size -= sizeof(wchar_t)*2;
  }
  else
  {
    if (dst_size < sizeof(wchar_t))
      return false;

    *dst_ptr = (wchar_t)*src_ptr;

    dst_ptr++;
    dst_size -= sizeof(wchar_t);
  }

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF16BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  wchar_t* dst_ptr = (wchar_t*) dst_char;
  wchar_t tmp0, tmp1;

  if (*src_ptr > 0x10000)
  {
    if (dst_size < sizeof(wchar_t)*2)
      return false;

    tmp0 = (wchar_t) ( (((*src_ptr - 0x10000) >> 10) & 0x3FF) + 0xD800 );
    tmp1 = (wchar_t) ( ((*src_ptr - 0x10000) & 0x3FF) + 0xDC00 );
    *dst_ptr = (wchar_t) ( ((tmp0 & 0xFF) << 8) + ((tmp0 >> 8) & 0xFF));
    *(dst_ptr+1) = (wchar_t) ( ((tmp1 & 0xFF) << 8) + ((tmp1 >> 8) & 0xFF));

    dst_ptr +=2;
    dst_size -= sizeof(wchar_t)*2;
  }
  else
  {
    if (dst_size < sizeof(wchar_t))
      return false;

    *dst_ptr = (wchar_t) ( ((*src_ptr & 0xFF) << 8) + ((*src_ptr >> 8) & 0xFF));

    dst_ptr++;
    dst_size -= sizeof(wchar_t);
  }

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF32LE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (dst_size < sizeof(size_t))
    return false;

  *dst_ptr = *src_ptr;

  dst_ptr++;
  dst_size -= sizeof(size_t);

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF32BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  const size_t* src_ptr = (const size_t*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (dst_size < sizeof(size_t))
    return false;

  *dst_ptr = ((*src_ptr & 0xFF) << 24) +
             ((*src_ptr & 0xFF00) << 8) +
             ((*src_ptr & 0xFF0000) >> 8) +
             ((*src_ptr & 0xFF000000) >> 24);

  dst_ptr++;
  dst_size -= sizeof(size_t);

  dst_char = dst_ptr;
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчеры
///////////////////////////////////////////////////////////////////////////////////////////////////
template<Encoding source_encoding, Encoding destination_encoding>
void convert_dispatcher2 (const void*&   source,
                          size_t&        source_size,
                          void*&         destination,
                          size_t&        destination_size)
{
  size_t buffer;
  void* buffer_ptr = &buffer;
  const void* buffer_cptr = &buffer;
  size_t buffer_size = 4;

  for (; source_size && destination_size;)
  {
    if (!decoder<source_encoding>(source, source_size, buffer_ptr, buffer_size))
      return;

    if (!encoder<destination_encoding>(buffer_cptr, buffer_size, destination, destination_size))
      return;
  }
}

template<Encoding source_encoding>
void convert_dispatcher1(const void*&   source,
                        size_t&        source_size,
                        Encoding       destination_encoding,
                        void*&         destination,
                        size_t&        destination_size)
{
  static const char* METHOD_NAME = "common::convert_dispatcher1";

  switch (destination_encoding)
  {
  case Encoding_ASCII7:
    convert_dispatcher2<source_encoding, Encoding_ASCII7> (source, source_size, destination, destination_size);
    break;
  case Encoding_UTF8:
    convert_dispatcher2<source_encoding, Encoding_UTF8> (source, source_size, destination, destination_size);
    break;
  case Encoding_UTF16LE:
    convert_dispatcher2<source_encoding, Encoding_UTF16LE> (source, source_size, destination, destination_size);
    break;
  case Encoding_UTF16BE:
    convert_dispatcher2<source_encoding, Encoding_UTF16BE> (source, source_size, destination, destination_size);
    break;
  case Encoding_UTF32LE:
    convert_dispatcher2<source_encoding, Encoding_UTF32LE> (source, source_size, destination, destination_size);
    break;
  case Encoding_UTF32BE:
    convert_dispatcher2<source_encoding, Encoding_UTF32BE> (source, source_size, destination, destination_size);
    break;
  default:
    throw xtl::make_argument_exception(METHOD_NAME, "destination_encoding");
  }
}

void convert_dispatcher0(Encoding       source_encoding,
                        const void*&   source,
                        size_t&        source_size,
                        Encoding       destination_encoding,
                        void*&         destination,
                        size_t&        destination_size)
{
  static const char* METHOD_NAME = "common::convert_dispatcher0";

  switch (source_encoding)
  {
  case Encoding_ASCII7:
    convert_dispatcher1<Encoding_ASCII7> (source, source_size, destination_encoding, destination, destination_size);
    break;
  case Encoding_UTF8:
    convert_dispatcher1<Encoding_UTF8> (source, source_size, destination_encoding, destination, destination_size);
    break;
  case Encoding_UTF16LE:
    convert_dispatcher1<Encoding_UTF16LE> (source, source_size, destination_encoding, destination, destination_size);
    break;
  case Encoding_UTF16BE:
    convert_dispatcher1<Encoding_UTF16BE> (source, source_size, destination_encoding, destination, destination_size);
    break;
  case Encoding_UTF32LE:
    convert_dispatcher1<Encoding_UTF32LE> (source, source_size, destination_encoding, destination, destination_size);
    break;
  case Encoding_UTF32BE:
    convert_dispatcher1<Encoding_UTF32BE> (source, source_size, destination_encoding, destination, destination_size);
    break;
  default:
    throw xtl::make_argument_exception(METHOD_NAME, "source_encoding");
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование кодировок
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert_encoding(Encoding       source_encoding,
                      const void*&   source,
                      size_t&        source_size,
                      Encoding       destination_encoding,
                      void*&         destination,
                      size_t&        destination_size)
{
  static const char* METHOD_NAME = "common::convert_encoding (...)";

  if (!source_size || !destination_size)
    return;

  if (!source)
    throw xtl::make_null_argument_exception(METHOD_NAME, "source");

  if (!destination)
    throw xtl::make_null_argument_exception(METHOD_NAME, "destination");

  convert_dispatcher0(source_encoding, source, source_size, destination_encoding, destination, destination_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование строк
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::wstring towstring (const char* string, int length)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::towstring", "string");    

  if (length == -1)
    length = strlen (string);        

  stl::wstring result;

  result.fast_resize (length);

  int result_size = mbstowcs (&result [0], string, length);

  if (result_size < 0)
    return L"(common::towstring error)";   

  result.fast_resize (result_size);
  
  return result;
}

stl::wstring towstring (const char* string)
{
  if (!string)
     throw xtl::make_null_argument_exception ("common::towstring", "string");

  return towstring (string, -1);
}

stl::wstring towstring (const stl::string& string)
{
  if (string.empty ())
    return L"";

  return towstring (&string [0], string.size ());
}

stl::string tostring (const wchar_t* string, int length)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::tostring", "string");

  if (length == -1)
    length = wcslen (string);

  stl::string result;  
  
  result.fast_resize (length * 4);

  int result_size = wcstombs (&result [0], string, length);
  
  if (result_size < 0)
    return "(common::tostring error)";

  result.fast_resize (result_size);

  return result;
}

stl::string tostring (const wchar_t* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::tostring", "string");

  return tostring (string, -1);
}

stl::string tostring (const stl::wstring& string)
{
  if (string.empty ())
    return "";

  return tostring (&string [0], string.size ());
}

/*
   Сжатие UTF16 строк
*/

size_t utf16_decompress (const wchar_t* source, size_t source_length, char* destination, size_t max_destination_length)
{
  if (!source)
  {
    if (source_length)
      throw xtl::make_null_argument_exception ("common::utf16_decompress", "source");

    return 0;
  }

  if (!destination)
  {
    if (max_destination_length)
      throw xtl::make_null_argument_exception ("common::utf16_decompress", "destination");

    return 0; 
  }

  size_t count = source_length * 6 + 1;
  
  if (count > max_destination_length)
    count = max_destination_length;

  if (!count)
    return 0;

  char* dst = destination;

  count = (count - 1) / 6;

  for (const char* src = reinterpret_cast<const char*> (source); count; src += 2, dst += 6, count--)
    xtl::xsnprintf (dst, 7, "%%%02X%%%02X", src[0], src[1]);

  *dst = 0;

  return (dst - destination) / 6;
}

size_t utf16_decompress (const wchar_t* source, char* destination, size_t max_destination_length)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::utf16_decompress (const wchar_t*, char*, size_t)", "source");

  return utf16_decompress (source, xtl::xstrlen (source), destination, max_destination_length);
}

size_t utf16_compress (const char* source, size_t source_length, wchar_t* destination, size_t max_destination_length)
{
  if (!source)
  {
    if (source_length)
      throw xtl::make_null_argument_exception ("common::utf16_compress", "source");

    return 0;
  }

  if (!destination)
  {
    if (max_destination_length)
      throw xtl::make_null_argument_exception ("common::utf16_compress", "destination");

    return 0; 
  }

  size_t count = source_length / 6 + 1;
  
  if (count > max_destination_length)
    count = max_destination_length;

  if (!count)
    return 0;

  count--;

  wchar_t* dst = destination;

  char* dummy_ptr;

  char decompressed_buffer[5] = {0, 0, 0, 0, 0};

  for (; count; source += 6, dst++, count--)
  {
    if ((source[0] != '%') || (source[3] != '%'))
      break;

    decompressed_buffer[0] = source[4];
    decompressed_buffer[1] = source[5];
    decompressed_buffer[2] = source[1];
    decompressed_buffer[3] = source[2];

    *dst = (wchar_t)strtoul (decompressed_buffer, &dummy_ptr, 16);
  }

  *dst = 0;

  return (dst - destination) * 6;
}

size_t utf16_compress (const char* source, wchar_t* destination, size_t max_destination_length)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::utf16_compress (const char*, wchar_t*, size_t)", "source");

  return utf16_compress (source, xtl::xstrlen (source), destination, max_destination_length);
}

stl::string utf16_decompress (const wchar_t* source, size_t source_length)
{
  stl::string result;

  result.fast_resize (source_length * 6);

  result.resize (utf16_decompress (source, source_length, &result[0], result.length () + 1) * 6);

  return result;
}

stl::string utf16_decompress (const wchar_t* source)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::utf16_decompress (const wchar_t*)", "source");

  return utf16_decompress (source, xtl::xstrlen (source));
}

stl::string utf16_decompress (const stl::wstring& source)
{
  return utf16_decompress (source.c_str (), source.length ());
}

stl::wstring utf16_compress (const char* source, size_t source_length)
{
  stl::wstring result;

  result.fast_resize (source_length / 6);

  result.resize (utf16_compress (source, source_length, &result[0], result.length () + 1) / 6);

  return result;
}

stl::wstring utf16_compress (const char* source)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::utf16_compress (const char*)", "source");

  return utf16_compress (source, xtl::xstrlen (source));
}

stl::wstring utf16_compress (const stl::string& source)
{
  return utf16_compress (source.c_str (), source.length ());
}

} //namespace common
