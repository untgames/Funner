#include <wchar.h>
#include <common/utf_converter.h>

namespace common
{

bool decode_ASCII7(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer);
bool decode_UTF8(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer);
bool decode_UTF16(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer,bool bigEndian);
bool decode_UTF32(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer,bool bigEndian);

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
   EncodingResult res={0,0};
   unsigned char *bsrc=(unsigned char*)source_buffer;
   wint_t *dst=(wint_t*)destination_buffer;
   int srcSize;
   int srcBytes;
   bool r;

   while((unsigned char*)source_buffer+source_buffer_size>bsrc)
   {
      srcSize=source_buffer_size-res.source_buffer_processed_size;
      switch (source_buffer_encoding)
      {
         case Encoding_Ascii7:
            r=decode_ASCII7(bsrc,srcSize,&srcBytes,dst);
            break;
         case Encoding_Utf8:
            r=decode_UTF8(bsrc,srcSize,&srcBytes,dst);
            break;
         case Encoding_Utf16BE:
            r=decode_UTF16(bsrc,srcSize,&srcBytes,dst,true);
            break;
         case Encoding_Utf16LE:
            r=decode_UTF16(bsrc,srcSize,&srcBytes,dst,false);
            break;
         case Encoding_Utf32BE:
            r=decode_UTF32(bsrc,srcSize,&srcBytes,dst,true);
            break;
         case Encoding_Utf32LE:
            r=decode_UTF32(bsrc,srcSize,&srcBytes,dst,false);
            break;
      }
      res.destination_buffer_processed_size+=srcBytes;
      bsrc+=srcBytes;
      printf("%s\n",dst);
   }
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

bool decode_ASCII7( const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer )
{
   const unsigned char* src0=src;
   int err=0;
   wint_t cp=wint_t(-1);

   if ( srcSize >= 1 )
   {
      cp = *src++;
      //if ( cp >= (unsigned char)128 )
      //err = 1;				// ERROR: Out-of-range ASCII-7 code
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = 4;
   }

   if ( !err )
      *buffer = cp;
   *srcBytes = src-src0;
   return !err;
}

bool decode_UTF8( const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer )
{
   const unsigned char*src0=src;
   int err=0;
   wint_t cp=wint_t(-1);

   if ( srcSize >= 1 )
   {
      unsigned char first = *src++;
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
          unsigned char ByteCountMask = 0x40;
          wint_t codeMask = 0x3F;
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
                   cp |= ( 0x3F & (wint_t)*src++ );
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
   return !err;
}

bool decode_UTF16( const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer, bool bigEndian )
{
   const unsigned char* src0=src;
   int err=0;
   wint_t cp = wint_t(-1);

   if ( srcSize >= 2 )
   {
      if ( bigEndian )
         cp = (wint_t(src[0])<<8) + wint_t(src[1]);
      else
         cp = wint_t(src[0]) + (wint_t(src[1])<<8);
      src += 2;
      if ( 0xD800 == (cp&0xFFFFFC00) )
      {
         if ( srcSize >= 4 )
         {
            wint_t ch2;
            if ( bigEndian )
               ch2 = (wint_t(src[0])<<8) + wint_t(src[1]);
            else
               ch2 = wint_t(src[0]) + (wint_t(src[1])<<8);
            src += 2;
            cp = (cp<<10) + ch2 - ((0xd800<<10UL)+0xdc00-0x10000);
         }
         else
         {
            // ERROR: Not enough encoded bytes available
            err = 4;
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
   return !err;
}

bool decode_UTF32( const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer, bool bigEndian )
{
   const unsigned char* src0=src;
   int err=0;
   wint_t cp= wint_t(-1);

   if ( srcSize >= 4 )
   {
      if ( bigEndian )
      {
         cp = 0;
         for ( int i = 0 ; i < 4 ; ++i )
         {
            cp <<= 8;
            cp += src[i];
         }
      }
      else // little endian
      {
         cp = 0;
         for ( int i = 4 ; i > 0 ; )
         {
            --i;
            cp <<= 8;
            cp += src[i];
         }
      }
      src += 4;
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = 4;
   }

   if ( !err )
      *buffer = cp;
   *srcBytes = src-src0;
   return !err;
}

}
