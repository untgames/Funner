#include <wchar.h>
#include <string.h>
#include <common/utf_converter.h>
#include <common/exception.h>

/*void dump32(void *ptr,int l)
{
   FILE *file=fopen("dump.txt","a");
   char *str=(char*)ptr;
   for(int i=0;i<l;i++)
      fputc(str[i],file);
//       fprintf(file,"%p|",str[i]);
   fclose(file);
} */

namespace common
{

#define Char unsigned __int16

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
EncodingResult utf_decode (const void* source_buffer,            //буфер-источник с utf-строкой
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

int decode_ASCII7( const unsigned char* src, int srcSize, int* srcBytes, char32* buffer )
{
   const unsigned char* src0=src;
   int err=0;
   if (*src0>=(unsigned char)128)
      *buffer=0x3F;    //Заменить на '?'
   else
      *buffer = *src;

/*   if ( srcSize >= 1 )
   {
      cp = *src++;
      //if ( cp >= (unsigned char)128 )
      //err = 1;        // ERROR: Out-of-range ASCII-7 code
   }
   else
   {
      // ERROR: Not enough encoded bytes available
      err = 4;
   }
*/
//   if ( !err )
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
                   cp |= ( 0x3F & (char32)*src++ );
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
      err = 4;
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
   return err;
}

int encode_UTF16( unsigned char* dst, int dstSize, int* dstBytes, char32 cp, bool bigEndian)
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
   return err;
}

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

enum
{
   UtfError_OutOfRange=1,
   UtfError_InvalidScalarValue=2,
   UtfError_NotEnoughEncoded=4,
   UtfError_NotEnoughBufferSpace=5
};

void ReturnException(int ex,char *func,int i)
{
   char ErrorBuf[128];
   switch(ex)
   {
      case UtfError_OutOfRange:
         Raise<ArgumentException>(ErrorBuf,"Out-of-range ASCII-7 code (%d)",i);
//         throw ArgumentException(func,ErrorBuf);
         break;
      case UtfError_InvalidScalarValue:
         Raise<ArgumentException>(ErrorBuf,"Invalid Unicode scalar value (%d)",i);
//         throw ArgumentException(func,ErrorBuf);
         break;
      case UtfError_NotEnoughEncoded:
         Raise<ArgumentException>(ErrorBuf,"Not enough encoded bytes available (%d)",i);
//         throw ArgumentException(func,ErrorBuf);
         break;
      case UtfError_NotEnoughBufferSpace:
         Raise<ArgumentException>(ErrorBuf,"Not enough buffer space (%d)",i);
//         throw ArgumentException(func,ErrorBuf);
         break;
      default:
         throw ArgumentException(func,"Unknown error");
         break;
   }
}
}
