#include <wchar.h>
#include <common/utf_converter.h>

namespace common
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: неверная кодировка Unicode
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct UtfExceptionTag;

//typedef DerivedException<Exception, UtfExceptionTag> UtfException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка Utf -> wint_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_decode (const void* source_buffer,            //буфер-источник с utf-строкой
                           size_t      source_buffer_size,       //размер буфера-источника в байтах
                           Encoding    source_buffer_encoding,   //кодировка буфера-источника
                           void*       destination_buffer,       //буфер-приёмник для декодируемой строки (wint_t)
                           size_t      destination_buffer_size)  //размер буфера-приёмника в байтах
{
   EncodingResult res;
   if (source_buffer_size>=1)
   {
      uint8_t first = *src++;
      if ( 0 == (first & 0x80) )
      {
         // ok, single byte ASCII-7 (US-ASCII) code
         cp = first;
      }
      else
      {
         // multibyte character code
         // read remaining byte count and 
         // most signifigant bits of the character code
         int bytes = 1;
         uint8_t ByteCountMask = 0x40;
         Char32 codeMask = 0x3F;
         while ( first & ByteCountMask )
         {
            ++bytes;
            ByteCountMask >>= 1;
            codeMask >>= 1;
         }
         if ( bytes < 2 || bytes > 4 )
         {
            // ERROR: Invalid number of following bytes
            err = 1;
         }
         else
         {
            if ( srcSize < bytes )
      {
               // ERROR: Not enough encoded bytes available
               err = 4;
            }
            else
            {
               // read remaining bytes of the character code
               cp = first & codeMask;
               for ( int i = 1 ; i < bytes ; ++i )
               {
                  cp <<= 6;
                  cp |= ( 0x3F & (Char32)*src++ );
               }
            }
         }
      }
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = 4;
   }

   if ( !err )
      *buffer = cp;
      *srcBytes = src-src0;
      return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка wint_t -> Utf
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //буфер-источник с wint_t - строкой
                           size_t      source_buffer_size,           //размер буфера-источника в байтах
                           void*       destination_buffer,           //буфер-приёмник с кодируемой строки (utf)
                           size_t      destination_buffer_size,      //размер буфера-приёмника в байтах
                           Encoding    destination_buffer_encoding) //кодировка буфера-приёмника
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка wchar_t -> utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf8 (const wchar_t* source_buffer,            //буфер-источник
                                size_t         source_buffer_size,       //размера буфера-источника
                                char*          destination_buffer,       //буфер-приёмник
                                size_t         destination_buffer_size) //размер буфера-приёмника
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка utf8 -> wchar_t
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf16 (const char* source_buffer,            //буфер-источник
                                 size_t      source_buffer_size,       //размера буфера-источника
                                 wchar_t*    destination_buffer,       //буфер-приёмник
                                 size_t      destination_buffer_size) //размер буфера-приёмника
{
}

}
