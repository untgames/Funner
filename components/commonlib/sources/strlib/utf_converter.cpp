#include <wchar.h>
#include <string.h>
#include <common/utf_converter.h>
#include <common/exception.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчики отдельных символов
///////////////////////////////////////////////////////////////////////////////////////////////////

class CharConverter
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Декодирование
///////////////////////////////////////////////////////////////////////////////////////////////////
  static bool decode_ASCII7 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const char* src_ptr = (const char*)src_char;
    size_t* dst_ptr = (size_t*)dst_char;
    
    if (src_size < sizeof(char))
      return false;
      
    *dst_ptr = (size_t)(*src_ptr > 0x7F ? '?' : *src_ptr);
    
    src_ptr++;
    src_size -= sizeof(char);
    src_char = src_ptr;
    
    return true;
  }

  static bool decode_UTF8 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const char* src_ptr = (const char*)src_char;
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
      src_char = src_ptr;
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
      src_char = src_ptr;
    }
    else if (*src_ptr >= 0xC0)
    {
      if (src_size < sizeof(char)*2)
        return false;

      *dst_ptr =   (*(src_ptr+1) & 0x3F) +
                 ( (*(src_ptr) & 0x1F) << 6 );
      
      src_ptr += 2;
      src_size -= sizeof(char) * 2;
      src_char = src_ptr;
    }
    else
    {
      *dst_ptr = (size_t)(*src_ptr > 0x7F ? '?' : *src_ptr);

      src_ptr++;
      src_size -= sizeof(char);
      src_char = src_ptr;
    }
    
    return true;
  }

  static bool decode_UTF16LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
      src_char = src_ptr;
    }
    else
    {
      if ( (src_size < sizeof(wchar_t)*2) || (*src_ptr > 0xDBFF ) )
        return false;
      if ( (*(src_ptr+1) < 0xDC00) || (*(src_ptr+1) > 0xDFFF) )
        return false;
      
      *dst_ptr = (*src_ptr & 0x03FF) + ( (*(src_ptr+1) & 0x03FF) << 10) + 0x10000;
      src_ptr += 2;
      src_size += sizeof(wchar_t)*2;
      src_char = src_ptr;
    }
    return true;
  }

  static bool decode_UTF16BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
      src_char = src_ptr;
    }
    else
    {
      if ( (src_size < sizeof(wchar_t)*2) || (tmp0 > 0xDBFF ) )
        return false;
      
      tmp1 = ((*(src_ptr+1) >> 8) & 0xFF) + ((*(src_ptr+1) & 0xFF) << 8);
      
      if ( (tmp1 < 0xDC00) || (tmp1 > 0xDFFF) )
        return false;
      
      *dst_ptr = ( tmp0 & 0x03FF) + ( ( tmp1 & 0x03FF) << 10) + 0x10000;
      src_ptr += 2;
      src_size -= sizeof(wchar_t)*2;
      src_char = src_ptr;
    }
    return true;
  }

  static bool decode_UTF32LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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

  static bool decode_UTF32BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
  static bool encode_ASCII7 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    char* dst_ptr = (char*) dst_char;
    
    if (dst_size < sizeof(char))
      return false;
      
    *dst_ptr = (char)(*src_ptr > 0x7F ? '?' : *src_ptr);
    
    dst_ptr++;
    dst_size -= sizeof(char);
    dst_char = dst_ptr;
    
    return true;
  }

  static bool encode_UTF8 (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
  {
    const size_t* src_ptr = (const size_t*) src_char;
    char* dst_ptr = (char*) dst_char;
    
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
      dst_char = dst_ptr;
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
      dst_char = dst_ptr;
    }
    else if (*src_ptr > 0x7F)
    {
      if (dst_size < sizeof(char)*2)
        return false;

      *(dst_ptr+1) = ((*src_ptr) & 0x3F) | 0x80;
      *(dst_ptr) = ( ( (*src_ptr) >> 6) & 0x1F) | 0xC0;
      
      dst_ptr += 2;
      dst_size -= sizeof(char)*2;
      dst_char = dst_ptr;
    }
    else
    {
      if (dst_size < sizeof(char))
        return false;

      *(dst_ptr) = (*src_ptr) & 0x7F;
      
      dst_ptr ++;
      dst_size -= sizeof(char);
      dst_char = dst_ptr;
    }
    
    return true;
  }

  static bool encode_UTF16LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
      dst_char = dst_ptr;
    }
    else
    {
      if (dst_size < sizeof(wchar_t))
        return false;
      
      *dst_ptr = (wchar_t)*src_ptr;
      
      dst_ptr++;
      dst_size -= sizeof(wchar_t);
      dst_char = dst_ptr;
    }
    
    return true;
  }

  static bool encode_UTF16BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
      dst_char = dst_ptr;
    }
    else
    {
      if (dst_size < sizeof(wchar_t))
        return false;
      
      *dst_ptr = (wchar_t) ( ((*src_ptr & 0xFF) << 8) + ((*src_ptr >> 8) & 0xFF));
      
      dst_ptr++;
      dst_size -= sizeof(wchar_t);
      dst_char = dst_ptr;
    }
    
    return true;
  }

  static bool encode_UTF32LE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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

  static bool encode_UTF32BE (const void*& src_char, size_t& src_size, void*& dst_char, size_t& dst_size)
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
};

/*
#define Char unsigned __int16

enum
{
   UtfError_OutOfRange=1,
   UtfError_InvalidScalarValue=2,
   UtfError_NotEnoughEncoded=4,
   UtfError_NotEnoughBufferSpace=5
};


void ReturnException(int ex,char *func,int i);

int decode_ASCII7(const unsigned char* src, int srcSize, int* srcBytes, char32* buffer);
int decode_UTF8(const unsigned char* src, int srcSize, int* srcBytes, char32* buffer);
int decode_UTF16(const unsigned char* src, int srcSize, int* srcBytes, char32* buffer,bool bigEndian);
int decode_UTF32(const unsigned char* src, int srcSize, int* srcBytes, char32* buffer,bool bigEndian);

int encode_ASCII7(unsigned char* dst, int dstSize, int* dstBytes, char32 cp);
int encode_UTF8(unsigned char* dst, int dstSize, int* dstBytes, char32 cp);
int encode_UTF16(unsigned char* dst, int dstSize, int* dstBytes, char32 cp, bool bigEndian);
int encode_UTF32(unsigned char* dst, int dstSize, int* dstBytes, char32 cp, bool bigEndian);
///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: неверная кодировка Unicode
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct UtfExceptionTag;

//typedef DerivedException<Exception, UtfExceptionTag> UtfException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка Utf -> char32
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult  utf_decode (const void* source_buffer,            //буфер-источник с utf-строкой
                           size_t      source_buffer_size,       //размер буфера-источника в байтах
                           Encoding    source_buffer_encoding,   //кодировка буфера-источника
                           void*       destination_buffer,       //буфер-приёмник для декодируемой строки (char32)
                           size_t      destination_buffer_size)  //размер буфера-приёмника в байтах
{
   EncodingResult res={0,0};
   if(source_buffer_size==0||destination_buffer_size==0)
      return res;
   if(source_buffer==NULL)
      throw ArgumentNullException("common::utf_decode","source_buffer is null");
   if(destination_buffer==NULL)
      throw ArgumentNullException("common::utf_decode","destination_buffer is null");
   unsigned char *bsrc=(unsigned char*)source_buffer;
   char32 *dst=(char32*)destination_buffer;
   int srcSize;
   int srcBytes;
   int r;
   unsigned int i;

   i=0;
   res.destination_buffer_processed_size=0;
   while(i<source_buffer_size&&
         res.destination_buffer_processed_size<destination_buffer_size)
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
      if(r!=0)
         ReturnException(r,"common::utf_decode",i);
      i+=srcBytes;
      dst++;
      res.source_buffer_processed_size=i;
      res.destination_buffer_processed_size+=sizeof(char32);
      bsrc+=srcBytes;
   }
   return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перекодировка char32 -> Utf8
///////////////////////////////////////////////////////////////////////////////////////////////////
EncodingResult utf_encode (const void* source_buffer,                //буфер-источник с char32 - строкой
                           size_t      source_buffer_size,           //размер буфера-источника в байтах
                           void*       destination_buffer,           //буфер-приёмник с кодируемой строки (utf)
                           size_t      destination_buffer_size,      //размер буфера-приёмника в байтах
                           Encoding    destination_buffer_encoding) //кодировка буфера-приёмника
{
   EncodingResult res={0,0};
   if(source_buffer_size==0||destination_buffer_size==0)
      return res;
   if(source_buffer==NULL)
      throw ArgumentNullException("common::utf_encode","source_buffer is null");
   if(destination_buffer==NULL)
      throw ArgumentNullException("common::utf_encode","destination_buffer is null");
   char32 *bsrc=(char32*)source_buffer;
   char32 *dst=(char32*)destination_buffer;
   char32 *old_bsrc=(char32*)source_buffer;
   char32 *old_dst=(char32*)destination_buffer;
   int srcSize;
   int srcBytes;
   int r;

   while((char32*)source_buffer+source_buffer_size/sizeof(char32)>bsrc&&
          res.destination_buffer_processed_size<destination_buffer_size)
   {
      srcSize=source_buffer_size-res.source_buffer_processed_size;
      switch (destination_buffer_encoding)
      {
         case Encoding_Ascii7:
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
      if(r!=0)
         ReturnException(r,"common::utf_encode",res.destination_buffer_processed_size);
      dst=(char32*)((char*)dst+srcBytes);
      res.destination_buffer_processed_size+=srcBytes;
      (char*)bsrc++;
      old_bsrc=bsrc;
      old_dst=dst;
   }
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
      i+=bytes;
      }
   }
   res.source_buffer_processed_size=i;
   res.destination_buffer_processed_size=0;
   return res;
}

int decode_ASCII7( const unsigned char* src, int srcSize, int* srcBytes, char32* buffer )
{
   const unsigned char* src0=src;
   int err=0;
   if (*src0>=(unsigned char)128)
      *buffer=0x3F;    //Заменить на '?'
   else
      *buffer = *src;

   *src++;
   *srcBytes = src-src0;
   return err;
}

int decode_UTF8( const unsigned char* src, int srcSize, int* srcBytes, char32* buffer )
{
   const unsigned char*src0=src;
   int err=0;
   char32 cp=char32(-1);

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
          char32 codeMask = 0x3F;
          while ( first & ByteCountMask )
          {
             ++bytes;
             ByteCountMask >>= 1;
             codeMask >>= 1;
          }
          if ( bytes < 2 || bytes > 4 )
          {
             // ERROR: Invalid number of following bytes
             err = UtfError_OutOfRange;
          }
          else
          {
             if ( srcSize < bytes )
             {
                // ERROR: Not enough encoded bytes available
                err = UtfError_NotEnoughEncoded;
             }
             else
             {
                // read remaining bytes of the character code
                cp = first & codeMask;
                for ( int i = 1 ; i < bytes ; ++i )
                {
                   cp <<= 6;
                   cp |= ( 0x3F & (char32)*src++ );
                }
             }
          }
      }
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = UtfError_NotEnoughEncoded;
   }
   if ( !err )
      *buffer = cp;
   *srcBytes = src-src0;
   return err;
}

int decode_UTF16( const unsigned char* src, int srcSize, int* srcBytes, char32* buffer, bool bigEndian )
{
   const unsigned char* src0=src;
   int err=0;
   char32 cp = char32(-1);

   if ( srcSize >= 2 )
   {
      if ( bigEndian )
         cp = (char32(src[0])<<8) + char32(src[1]);
      else
         cp = char32(src[0]) + (char32(src[1])<<8);
      src += 2;
      if ( 0xD800 == (cp&0xFFFFFC00) )
      {
         if ( srcSize >= 4 )
         {
            char32 ch2;
            if ( bigEndian )
               ch2 = (char32(src[0])<<8) + char32(src[1]);
            else
               ch2 = char32(src[0]) + (char32(src[1])<<8);
            src += 2;
            cp = (cp<<10) + ch2 - ((0xd800<<10UL)+0xdc00-0x10000);
         }
         else
         {
            // ERROR: Not enough encoded bytes available
            err = UtfError_NotEnoughEncoded;
         }
      }
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = UtfError_NotEnoughEncoded;
   }

   if ( !err )
      *buffer = cp;
   else
      *buffer = 0x3F;
   *srcBytes = src-src0;
   return err;
}

int decode_UTF32( const unsigned char* src, int srcSize, int* srcBytes, char32* buffer, bool bigEndian )
{
   const unsigned char* src0=src;
   int err=0;
   char32 cp= char32(-1);

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
      err = UtfError_NotEnoughEncoded;
   }

   if ( !err )
      *buffer = cp;
   *srcBytes = src-src0;
   return err;
}


int encode_ASCII7( unsigned char* dst, int dstSize, int* dstBytes, char32 cp)
{
   const unsigned char *dst0=dst;
   int err=0;

   if(cp>=128)
      *dst++='?';  //Заменить на '?'
   else
      *dst++=(unsigned char)cp;

   *dstBytes = dst-dst0;
   return err;
}

int encode_UTF8(unsigned char* dst,int dstSize, int* dstBytes, char32 cp)
{
   const unsigned char *dst0=dst;
   int err=0;

   if(cp<0x80) 
   {
      if ( dstSize < 1 )
      {
         // ERROR: Not enough buffer space.
         err = UtfError_NotEnoughBufferSpace;
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
         err = UtfError_NotEnoughBufferSpace;
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
         err = UtfError_NotEnoughBufferSpace;
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
         err = UtfError_NotEnoughBufferSpace;
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
      err = UtfError_InvalidScalarValue;
   }
   *dstBytes = dst-dst0;
   return err;
}

int encode_UTF16( unsigned char* dst, int dstSize, int* dstBytes, char32 cp, bool bigEndian)
{
   const unsigned char*dst0= dst;
   int err=0;

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
      err = UtfError_NotEnoughBufferSpace;
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
   return err;
}

int encode_UTF32( unsigned char* dst, int dstSize, int* dstBytes,char32 cp, bool bigEndian )
{
   const unsigned char *dst0= dst;
   int err=0;

   // write
   int codeCount = 1;
   int codeSize = unsigned(codeCount) * 4U;
   if ( dstSize < codeSize )
   {
      // Error: Not enough buffer space
      err = UtfError_NotEnoughBufferSpace;
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
   return err;
}

*/
///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование строк
///////////////////////////////////////////////////////////////////////////////////////////////////
stl::wstring towstring (const char* string, int length)
{
   if(string==NULL)
      throw ArgumentNullException("towstring","string is null");
  if (!string)
    return L"";

  if (length == -1)
    length = strlen (string);

  stl::wstring result;
  
  int result_size = (int)mbstowcs (0, string, length);

  if (result_size == -1)
    return L"(common::towstring error)";

  if (!result_size)
    return L"";

  result.fast_resize (result_size);
  
  mbstowcs (&result [0], string, length);

  return result;
}

stl::wstring towstring (const char* string)
{
   if(string==NULL)
      throw ArgumentNullException("towstring","string is null");
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
   if(string==NULL)
      throw ArgumentNullException("tostring","string is null");
  if (!string)
    return "";

  if (length == -1)
    length = wcslen (string);

  stl::string result;
  
  int result_size = (int)wcstombs (0, string, length);
  
  if (result_size == -1)
    return "(common::tostring error)";
    
  if (!result_size)
    return "";
  
  result.fast_resize (result_size);
  
  wcstombs (&result [0], string, length);
  
  return result;
}

stl::string tostring (const wchar_t* string)
{
   if(string==NULL)
      throw ArgumentNullException("tostring","string is null");
  return tostring (string, -1);
}

stl::string tostring (const stl::wstring& string)
{
  if (string.empty ())
    return "";

  return tostring (&string [0], string.size ());
}

} //namespace common
