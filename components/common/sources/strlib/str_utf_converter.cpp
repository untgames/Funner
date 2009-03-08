#include <wchar.h>
#include <string.h>

#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/string.h>
#include <xtl/utility>

#include <common/utf_converter.h>

namespace
{

const size_t DECOMPRESSED_WCHAR_SIZE = 4;

template <class T=wchar_t, size_t wchar_size = sizeof (T)> struct selector {};

template <class T>
size_t wchar_decompress_impl (const unsigned char* src, size_t src_count, char*& dst, size_t dst_count, selector<T, 2>)
{
  size_t i = 0;

  for (; (i < src_count) && dst_count; src += sizeof (T), dst += DECOMPRESSED_WCHAR_SIZE, i++, dst_count--)
    xtl::xsnprintf (dst, DECOMPRESSED_WCHAR_SIZE + 1, "%02X%02X", src [0], src [1]);

  return i;
}

template <class T>
size_t wchar_decompress_impl (const unsigned char* src, size_t src_count, char*& dst, size_t dst_count, selector<T, 4>)
{
  size_t i = 0;    

  for (; src_count && dst_count; src += 4, dst += DECOMPRESSED_WCHAR_SIZE, src_count--, dst_count--, i++)
  {
    T* wchar_src = (T*)src;

    if ((*wchar_src > 0xD7FF) && (*wchar_src < 0xE000))                                    //некорректный символ
      xtl::xsnprintf (dst, DECOMPRESSED_WCHAR_SIZE + 1, "%02X00", '?');
    else if (*wchar_src < 0x10000)                                                         //16-битный символ
      xtl::xsnprintf (dst, DECOMPRESSED_WCHAR_SIZE + 1, "%02X%02X", src[0], src[1]);
    else if (*wchar_src < 0x110000)                                                        //32-битный символ
    {
      if (dst_count == 1)   //не хватает места для записи двойного символа
        return i;

      size_t subtract         = *wchar_src - 0x10000,
             first_surrogate  = (subtract >> 10) | 0xD800,
             second_surrogate = (subtract & 0x3FF) | 0xDC00;

      xtl::xsnprintf (dst, (DECOMPRESSED_WCHAR_SIZE * 2) + 1, "%02X%02X%02X%02X", first_surrogate & 0xFF, first_surrogate >> 8,
                      second_surrogate & 0xFF, second_surrogate >> 8);

      dst += DECOMPRESSED_WCHAR_SIZE;
      dst_count--;

      if (src_count > 1)
        src_count--;
    }
    else                                                                                   // некорректный символ
    {
      xtl::xsnprintf (dst, DECOMPRESSED_WCHAR_SIZE + 1, "%02X00", '?');
    }
  }

  return i;
}

template <class T>
size_t wchar_compress_impl (const char* src, size_t count, wchar_t*& dst, size_t dst_count, selector<T, 2>)
{
  char* dummy_ptr;

  char decompressed_buffer [5] = {0, 0, 0, 0, 0};

  size_t i = 0;

  for (; (i < count) && dst_count; src += DECOMPRESSED_WCHAR_SIZE, dst++, i++, dst_count--)
  {
    decompressed_buffer [0] = src [2];
    decompressed_buffer [1] = src [3];
    decompressed_buffer [2] = src [0];
    decompressed_buffer [3] = src [1];

    *dst = (wchar_t)strtoul (decompressed_buffer, &dummy_ptr, 16);
  }

  return i * DECOMPRESSED_WCHAR_SIZE;
}

template <class T>
size_t wchar_compress_impl (const char* src, size_t count, wchar_t*& dst, size_t dst_count, selector<T, 4>)
{
  char* dummy_ptr;

  char decompressed_buffer [5] = {0, 0, 0, 0, 0};

  size_t i = 0;

  for (; count && dst_count; src += DECOMPRESSED_WCHAR_SIZE, dst++, count--, i++, dst_count--)
  {
    decompressed_buffer [0] = src [2];
    decompressed_buffer [1] = src [3];
    decompressed_buffer [2] = src [0];
    decompressed_buffer [3] = src [1];

    T utf_code = (T)strtoul (decompressed_buffer, &dummy_ptr, 16);

    if ((utf_code < 0xD800) || (utf_code > 0xDFFF)) //16-битный символ
      *dst = utf_code;
    else if (utf_code < 0xDC00)                     //32-битный символ
    {
      if (count == 1)                               //нет необходимого последующего символа
      {
        *dst = '?';
        continue;
      }

      i++;
      count--;

      decompressed_buffer [0] = src [6];
      decompressed_buffer [1] = src [7];
      decompressed_buffer [2] = src [4];
      decompressed_buffer [3] = src [5];

      T second_surrogate_code = (T)strtoul (decompressed_buffer, &dummy_ptr, 16);

      if ((second_surrogate_code < 0xDC00) || (second_surrogate_code > 0xDFFF)) //некорректный символ
        *dst = '?';

      *dst = (((0x3FF & utf_code) << 10) | (0x3FF & second_surrogate_code)) + 0x10000;
    }
    else                                            //некорректный символ
    {
      *dst = '?';
    }
  }

  return i * DECOMPRESSED_WCHAR_SIZE;
}

}

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
  xtl::compile_time_assert<sizeof (unsigned short) == 2> ();

  const unsigned short* src_ptr = (const unsigned short*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;

  if (src_size < 2)
    return false;

  if ( (*src_ptr < 0xD800) || (*src_ptr > 0xDFFF) )
  {
    *dst_ptr = *src_ptr;
    src_ptr++;
    src_size -= 2;
  }
  else
  {
    if (src_size < 4)
      return false;

    if ( (*src_ptr > 0xDBFF ) || (*(src_ptr+1) < 0xDC00) || (*(src_ptr+1) > 0xDFFF) )
      *dst_ptr = '?';
    else
      *dst_ptr = ((*src_ptr & 0x3FF) << 10) + (*(src_ptr+1) & 0x3FF) + 0x10000;

    src_ptr += 2;
    src_size -= 4;
  }

  src_char = src_ptr;
  return true;
}

template<>
bool decoder<Encoding_UTF16BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  xtl::compile_time_assert<sizeof (unsigned short) == 2> ();

  const unsigned short* src_ptr = (const unsigned short*) src_char;
  size_t* dst_ptr = (size_t*) dst_char;
  unsigned short tmp0, tmp1;

  if (src_size < 2)
    return false;

  tmp0 = (*src_ptr >> 8) | (*src_ptr << 8);

  if ((tmp0 < 0xD800) || (tmp0 > 0xDFFF))
  {
    *dst_ptr = tmp0;
    src_ptr++;
    src_size -= 2;
  }
  else
  {
    if (src_size < 4)
      return false;

    tmp1 = (*(src_ptr+1) >> 8) | (*(src_ptr+1) << 8);

    if ((tmp0 > 0xDBFF ) || (tmp1 < 0xDC00) || (tmp1 > 0xDFFF))
      *dst_ptr = '?';
    else
      *dst_ptr = ((tmp0 & 0x3FF) << 10) + (tmp1 & 0x3FF) + 0x10000;

    src_ptr += 2;
    src_size -= 4;
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
  xtl::compile_time_assert<sizeof (unsigned short) == 2> ();

  const size_t* src_ptr = (const size_t*) src_char;
  unsigned short* dst_ptr = (unsigned short*) dst_char;
  size_t tmp;

  if (*src_ptr > 0x10000)
  {
    if (dst_size < 4)
      return false;

    if (*src_ptr > 0x10FFFF)
      return false;

    tmp = *src_ptr - 0x10000;
    *dst_ptr = (unsigned short) ((tmp >> 10) | 0xD800);
    *(dst_ptr+1) = (unsigned short) ((tmp & 0x3FF) | 0xDC00);

    dst_ptr +=2;
    dst_size -= 4;
  }
  else
  {
    if (dst_size < 2)
      return false;

    *dst_ptr = (unsigned short)*src_ptr;

    dst_ptr++;
    dst_size -= 2;
  }

  dst_char = dst_ptr;
  return true;
}

template<>
bool encoder<Encoding_UTF16BE> (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
{
  xtl::compile_time_assert<sizeof (unsigned short) == 2> ();

  const size_t* src_ptr = (const size_t*) src_char;
  unsigned short* dst_ptr = (unsigned short*) dst_char;
  unsigned short tmp0, tmp1;

  if (*src_ptr > 0x10000)
  {
    if (dst_size < 4)
      return false;

    if (*src_ptr > 0x10FFFF)
      return false;

    size_t tmp = *src_ptr - 0x10000;

    tmp0 = (unsigned short) ((tmp >> 10) | 0xD800);
    tmp1 = (unsigned short) ((tmp & 0x3FF) | 0xDC00);
    *dst_ptr = (unsigned short) ((tmp0 << 8) | (tmp0 >> 8));
    *(dst_ptr+1) = (unsigned short) ((tmp1 << 8) | (tmp1 >> 8));

    dst_ptr +=2;
    dst_size -= 4;
  }
  else
  {
    if (dst_size < 2)
      return false;

    *dst_ptr = (unsigned short) (((*src_ptr & 0xFF) << 8) + ((*src_ptr >> 8) & 0xFF));

    dst_ptr++;
    dst_size -= 2;
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
   Сжатие wchar_t строк
*/

size_t wchar_decompress (const wchar_t* source, size_t source_length, char* destination, size_t max_destination_length)
{
  static const char* METHOD_NAME = "common::wchar_decompress (const wchar_t*, size_t, char*, size_t)";

  if (!source)
  {
    if (source_length)
      throw xtl::make_null_argument_exception (METHOD_NAME, "source");

    return 0;
  }

  if (!destination)
  {
    if (max_destination_length)
      throw xtl::make_null_argument_exception (METHOD_NAME, "destination");

    return 0;
  }

  if (!max_destination_length)
    return 0;

  size_t processed_characters = wchar_decompress_impl ((unsigned char*)source, source_length, destination,
    (max_destination_length - 1) / DECOMPRESSED_WCHAR_SIZE, selector<> ());

  *destination = 0;

  return processed_characters;
}

size_t wchar_decompress (const wchar_t* source, char* destination, size_t max_destination_length)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::wchar_decompress (const wchar_t*, char*, size_t)", "source");

  return wchar_decompress (source, xtl::xstrlen (source), destination, max_destination_length);
}

size_t wchar_compress (const char* source, size_t source_length, wchar_t* destination, size_t max_destination_length)
{
  static const char* METHOD_NAME = "common::wchar_compress (const char*, size_t, wchar_t*, size_t)";

  if (!source)
  {
    if (source_length)
      throw xtl::make_null_argument_exception (METHOD_NAME, "source");

    return 0;
  }

  if (!destination)
  {
    if (max_destination_length)
      throw xtl::make_null_argument_exception (METHOD_NAME, "destination");

    return 0;
  }

  if (!max_destination_length)
    return 0;

  size_t processed_characters = wchar_compress_impl (source, source_length / DECOMPRESSED_WCHAR_SIZE,
    destination, max_destination_length - 1, selector<> ());

  *destination = 0;

  return processed_characters;
}

size_t wchar_compress (const char* source, wchar_t* destination, size_t max_destination_length)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::wchar_compress (const char*, wchar_t*, size_t)", "source");

  return wchar_compress (source, xtl::xstrlen (source), destination, max_destination_length);
}

stl::string wchar_decompress (const wchar_t* source, size_t source_length)
{
  stl::string result;

  result.fast_resize (source_length * DECOMPRESSED_WCHAR_SIZE * 2);

  wchar_decompress (source, source_length, &result[0], result.length () + 1);

  result.resize (result.length ());

  return result;
}

stl::string wchar_decompress (const wchar_t* source)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::wchar_decompress (const wchar_t*)", "source");

  return wchar_decompress (source, xtl::xstrlen (source));
}

stl::string wchar_decompress (const stl::wstring& source)
{
  return wchar_decompress (source.c_str (), source.length ());
}

stl::wstring wchar_compress (const char* source, size_t source_length)
{
  stl::wstring result;

  result.fast_resize (source_length / DECOMPRESSED_WCHAR_SIZE);

  result.resize (wchar_compress (source, source_length, &result[0], result.length () + 1) / DECOMPRESSED_WCHAR_SIZE);

  return result;
}

stl::wstring wchar_compress (const char* source)
{
  if (!source)
    throw xtl::make_null_argument_exception ("common::wchar_compress (const char*)", "source");

  return wchar_compress (source, xtl::xstrlen (source));
}

stl::wstring wchar_compress (const stl::string& source)
{
  return wchar_compress (source.c_str (), source.length ());
}

} //namespace common
