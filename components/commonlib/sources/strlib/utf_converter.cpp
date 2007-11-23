#include <wchar.h>
#include <common/utf_converter.h>

namespace common
{

#define Char unsigned __int16

bool decode_ASCII7(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer);
bool decode_UTF8(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer);
bool decode_UTF16(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer,bool bigEndian);
bool decode_UTF32(const unsigned char* src, int srcSize, int* srcBytes, wint_t* buffer,bool bigEndian);

bool encode_ASCII7(unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp);
bool encode_UTF8(unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp);
bool encode_UTF16(unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp, bool bigEndian);
bool encode_UTF32(unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp, bool bigEndian);
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

   printf("---source_buffer=%p\n",source_buffer);
   printf("---source_buffer_size=%d\n",source_buffer_size);
   printf("---bsrc=%p\n",bsrc);

   while((unsigned char*)source_buffer+source_buffer_size>bsrc)
   {
      printf("source_buffer=%p\n",source_buffer);
      printf("source_buffer_size=%d\n",source_buffer_size);
      printf("bsrc=%p\n",bsrc);
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
      printf("srcBytes=%d\n",srcBytes);
      printf("dst=%p\n",dst);
      printf("dst=%p\n",*dst);
      dst++;
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
   EncodingResult res={0,0};
   wint_t *bsrc=(wint_t*)source_buffer;
   wint_t *dst=(wint_t*)destination_buffer;
   wint_t *old_bsrc=(wint_t*)source_buffer;
   wint_t *old_dst=(wint_t*)destination_buffer;
   int srcSize;
   int srcBytes;
   bool r;

   printf("---source_buffer=%p\n",source_buffer);
   printf("---source_buffer_size=%d\n",source_buffer_size);
   printf("---bsrc=%p\n",bsrc);

   while((wint_t*)source_buffer+source_buffer_size>bsrc)
   {
printf("start ------------------\n");
//      printf("source_buffer=%p\n",source_buffer);
//      printf("source_buffer_size=%d\n",source_buffer_size);
//      printf("bsrc=%p\n",bsrc);
      srcSize=source_buffer_size-res.source_buffer_processed_size;
      switch (destination_buffer_encoding)
      {
         case Encoding_Ascii7:
//bool encode_ASCII7(unsigned char* dst,int dstSize,int* dstBytes,unsigned int cp);
            r=encode_ASCII7((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc);
            break;
         case Encoding_Utf8:
            r=encode_UTF8((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc);
            break;
         case Encoding_Utf16BE:	
            r=encode_UTF16((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc,true);
            break;
         case Encoding_Utf16LE:	
            r=encode_UTF16((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc,false);
            break;
         case Encoding_Utf32BE:
            r=encode_UTF32((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc,true);
            break;
         case Encoding_Utf32LE:
            r=encode_UTF32((unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc,false);
            break;
      }
//      printf("srcBytes=%d\n",srcBytes);
//      printf("dst=%p\n",dst);
    printf("r=%s\n",r==true?"true":"false");
    printf("srcBytes=%d\n",srcBytes);
//      printf("bsrc=%p\n",bsrc);
//(unsigned char *)dst,destination_buffer_size,&srcBytes,*bsrc
      dst=(wint_t*)((char*)dst+srcBytes);
      res.destination_buffer_processed_size+=srcBytes;
      (char*)bsrc++;
//      printf("---\nbsrc=%p\n",bsrc);
//      printf("dst=%p\n",dst);
      printf("bsrc-old=%p\n",(char*)bsrc-(char*)old_bsrc);
      printf("dst-old=%p\n",(char*)dst-(char*)old_dst);
old_bsrc=bsrc;
old_dst=dst;
printf("end --------------------\n");
   }
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


bool encode_ASCII7( unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp)
{
   const unsigned char *dst0=dst;
   int err=0;

   if ( dstSize >= 1 )
   {
      if(cp>=128)
      {
         // ERROR: Out-of-range ASCII-7 code
         err = 1;
      }
      else
      {
         *dst++=(unsigned char)cp;
      }
   }
   else
   {
      // ERROR: Not enough buffer space
      err = 5;
      cp = unsigned int(-1);
   }

   *dstBytes = dst-dst0;
   return !err;
}

bool encode_UTF8(unsigned char* dst,int dstSize, int* dstBytes, unsigned int cp)
{
   const unsigned char *dst0=dst;
   int err=0;

   if(cp<0x80) 
   {
      if ( dstSize < 1 )
      {
         // ERROR: Not enough buffer space.
         err = 5;
      }
      else
      {
         *dst++ = (unsigned char)cp;
      }
   }
   else if (cp < 0x800) 
   {
      if ( dstSize < 2 )
      {
         // ERROR: Not enough buffer space.
         err = 5;
      }
      else
      {
         *dst++ = (unsigned char)( 0xC0 | (cp>>6) );
         *dst++ = (unsigned char)( 0x80 | (cp&0x3F) );
      }
   }
   else if (cp < 0x10000) 
   {
      if ( dstSize < 3 )
      {
         // ERROR: Not enough buffer space.
         err = 5;
      }
      else
      {
         *dst++ = (unsigned char)( 0xE0 | (cp>>12) );
         *dst++ = (unsigned char)( 0x80 | ( (cp>>6) &0x3F) );
         *dst++ = (unsigned char)( 0x80 | (cp&0x3F) );
      }
   }
   else if (cp < 0x200000) 
   {
      if ( dstSize < 4 )
      {
         // ERROR: Not enough buffer space.
         err = 5;
      }
      else
      {
         *dst++ = (unsigned char)( 0xF0 | (cp>>18) );
         *dst++ = (unsigned char)( 0x80 | ( (cp>>12) &0x3F) );
         *dst++ = (unsigned char)( 0x80 | ( (cp>>6) &0x3F) );
         *dst++ = (unsigned char)( 0x80 | (cp&0x3F) );
      }
   }
   else
   {
      // ERROR: Invalid Unicode scalar value
      err = 2;
   }
   *dstBytes = dst-dst0;
   return !err;
}

bool encode_UTF16( unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp, bool bigEndian)
{
   const unsigned char*dst0= dst;
   int err= 0;

   // encode
   Char codes[2];
   int codeCount = 0;
   if ( cp >= 0x10000 )
   {
      codes[codeCount++] = Char( ((cp-0x10000)>>10) + 0xD800 );
      codes[codeCount++] = Char( ((cp-0x10000)&1023) + 0xDC00 );
   }
   else
   {
      codes[codeCount++] = Char( cp );
   }
   // write
   int codeSize = unsigned(codeCount) * 2U;
   if ( dstSize < codeSize )
   {
      // Error: Not enough buffer space
      err = 5;
   }
   else
   {
      for ( int i = 0 ; i < codeCount ; ++i )
      {
         Char code = codes[i];
         if ( bigEndian )
         {
            *dst++ = unsigned char(code >> 8);
            *dst++ = unsigned char(code);
         }
         else
         {
            *dst++ = unsigned char(code);
            *dst++ = unsigned char(code >> 8);
         }
      }
   }

   *dstBytes = dst-dst0;
   return !err;
}

bool encode_UTF32( unsigned char* dst, int dstSize, int* dstBytes, unsigned int cp, bool bigEndian )
{
   const unsigned char *dst0= dst;
   int err=0;

   // write
   int codeCount = 1;
   int codeSize = unsigned(codeCount) * 4U;
   if ( dstSize < codeSize )
   {
      // Error: Not enough buffer space
      err = 5;
   }
   else
   {
      unsigned int code = cp;
      if ( bigEndian )
      {
         *dst++ = unsigned char(code>>24);
         *dst++ = unsigned char(code>>16);
         *dst++ = unsigned char(code>>8);
         *dst++ = unsigned char(code);
      }
      else
      {
         *dst++ = unsigned char(code);
         *dst++ = unsigned char(code>>8);
         *dst++ = unsigned char(code>>16);
         *dst++ = unsigned char(code>>24);
      }
   }
   *dstBytes = dst-dst0;
   return !err;
}

}
