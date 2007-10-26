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
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка wint_t -> Utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //буфер-источник с wint_t - строкой
                           size_t      source_buffer_size,           //размер буфера-источника в байтах
                           void*       destination_buffer,           //буфер-приёмник с кодируемой строки (utf)
                           size_t      destination_buffer_size,      //размер буфера-приёмника в байтах
                           Encoding    destination_buffer_encoding) //кодировка буфера-приёмника
{
   EncodingResult res;
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка wchar_t -> utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf8 (const wchar_t* source_buffer,            //буфер-источник
                                size_t         source_buffer_size,       //размера буфера-источника
                                char*          destination_buffer,       //буфер-приёмник
                                size_t         destination_buffer_size) //размер буфера-приёмника
{
   EncodingResult res;
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка utf8 -> utf16
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult convert_to_utf16 (const char* source_buffer,            //буфер-источник
                                 size_t      source_buffer_size,       //размера буфера-источника
                                 wchar_t*    destination_buffer,       //буфер-приёмник
                                 size_t      destination_buffer_size) //размер буфера-приёмника
{
   unsigned int *cp=(unsigned int*)destination_buffer;
   memset(cp,0,destination_buffer_size);
   char *src=(char*)source_buffer;
   EncodingResult res;
   unsigned int i;
   for(i=0;i<source_buffer_size;)
   {
      char first = *src++;
      if ( 0 == (first & 0x80) )
      {
         // ok, single byte ASCII-7 (US-ASCII) code
         *cp = first;
         i++;
         cp++;
      }
      else
      {
         // multibyte character code
         // read remaining byte count and 
         // most signifigant bits of the character code
         int bytes = 1;
         char ByteCountMask = 0x40;
         unsigned int codeMask = 0x3F;
         while ( first & ByteCountMask )
         {
            ++bytes;
            ByteCountMask >>= 1;
            codeMask >>= 1;
         }
/*         if (!(bytes<2 || bytes>4))
         {
            if (!(srcSize<bytes))
            {
               // read remaining bytes of the character code
               cp = first & codeMask;
               for ( int i = 1 ; i < bytes ; ++i )
               {
                  cp <<= 6;
                  cp |= ( 0x3F & (unsigned int)*src++ );
               }
            }
         }*/
      i+=bytes;
      }
   }
   res.source_buffer_processed_size=i;
   res.destination_buffer_processed_size=0;
   return res;
}

}
