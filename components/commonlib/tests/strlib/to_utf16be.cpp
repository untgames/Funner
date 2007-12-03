#include <common/utf_converter.h>
#include <stdio.h>
#include <string.h>
#include <common/file.h>
#include <common/streams.h>

using namespace common;

void dump(void *ptr,int l)
{
   char *str=(char*)ptr;
   for(int i=0;i<l;i++)
      printf("%02X ",str[i]);
   printf("\n");
}

char in[128];
char out[128];

char *names[4]={"data\\ansi.txt",
                 "data\\utf8.txt",
                 "data\\utf16le.txt",
                 "data\\utf16be.txt"};


Encoding en[4]={
  Encoding_Ascii7,
  Encoding_Utf8,
  Encoding_Utf16LE,
  Encoding_Utf16BE};

int main()
{
  InputFile *file;
  for(int i=0;i<4;i++)
  {
     memset(in,0,sizeof(in));
     file=new InputFile(names[i]);
     file->Read(in,127);
     memset(out,0,sizeof(out));
     utf_decode(in,64,en[i],(char*)out,120);
     memset(in,0,sizeof(in));
     utf_encode(out,64,(char*)in,120,Encoding_Utf16LE);
//     printf("%s\n",in);
     wprintf(L"%s\n",(char*)in);
     delete file;
  }
  return 0;
}
