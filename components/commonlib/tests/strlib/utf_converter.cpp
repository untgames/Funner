#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <stl/string>

#include <common/utf_converter.h>
#include <common/file.h>
#include <common/streams.h>

using namespace common;

void dump32(void *ptr,int l)
{
   FILE *file=fopen("dump.txt","a");
   char *str=(char*)ptr;
   for(int i=0;i<l;i++)
      fputc(str[i],file);
//       fprintf(file,"%p|",str[i]);
   fclose(file);
}

void dump (const void* buffer, size_t length, Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_Ascii7:
    {
      const char* s = (const char*)buffer;
  
      for (size_t i=0; i<length; i++)
        printf ("%c", s [i]);

      break;
    }
    case Encoding_Utf8:
    {
      stl::wstring result (' ', length);
      
      result.resize (mbstowcs (&result [0], (const char*)buffer, length));

      printf ("%S", result.c_str ());

      break;
    }
    case Encoding_Utf16LE:
    {
      const wchar_t* s = (const wchar_t*)buffer;
  
      for (size_t i=0; i<length/2; i++)
        printf ("%C", s [i]);

      break;    
    }
    case Encoding_Utf16BE:
    {
      const wchar_t* s = (const wchar_t*)buffer;
  
      for (size_t i=0; i<length/2; i++)
        printf ("%C", (s [i]>>8)|((s [i]<<8)&0xFF00));

      break;    
    }    
  }
}

const char* get_encoding_name (Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_Ascii7:  return "ascii7";
    case Encoding_Utf8:    return "utf8";
    case Encoding_Utf16LE: return "utf16le";
    case Encoding_Utf16BE: return "utf16be";
    default:               return "unknown";
  }
}

void test_utf_converter (const char* file_name, Encoding source_encoding)
{
  Encoding encoding [4] = {Encoding_Ascii7, Encoding_Utf8, Encoding_Utf16LE, Encoding_Utf16BE};
  
  for (size_t i=0; i<4; i++)
  {
    printf ("test encoding chain: %s->utf32le->%s: '", get_encoding_name (source_encoding), get_encoding_name (encoding [i]));
  
    char buffer [2][128];

    InputFile file (file_name);

    memset(buffer [0],0,sizeof(buffer [0]));
    
    size_t source_length = file.Read (buffer [0], sizeof (buffer [0]));
//    printf("source_length = %d\n",source_length);
    EncodingResult decode_result = utf_decode (buffer [0], source_length, source_encoding, buffer [1], sizeof (buffer [1]));
//    printf ("decode = %d\n",decode_result.destination_buffer_processed_size);
//    dump32(buffer[1],128);
    memset(buffer [0],0,sizeof(0));
    EncodingResult encode_result = utf_encode (buffer [1], decode_result.destination_buffer_processed_size, buffer [0], sizeof (buffer [0]), encoding [i]);
//    printf ("encode = %d\n",encode_result.destination_buffer_processed_size);

    dump (buffer [0], encode_result.destination_buffer_processed_size, encoding [i]);

    printf ("'\n");
//    swap();
  }
}

int main()
{
  printf ("Results of utf_converter_test:\n");
  
  if (!setlocale (LC_ALL, ".1251"))
  {
    printf ("Error at set locale\n");
    return 0;
  }

  const char* source_file_name [4] = {"data\\ansi.txt", "data\\utf8.txt", "data\\utf16le.txt","data\\utf16be.txt"};
  Encoding    source_encoding [4]  = {Encoding_Ascii7, Encoding_Utf8, Encoding_Utf16LE,Encoding_Utf16BE};

  for (size_t i=0; i<4; i++)
    test_utf_converter (source_file_name [i], source_encoding [i]);

  return 0;
}
