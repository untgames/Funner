#include <common/utf_converter.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char in[128]={0xFF,0xFE,0x7A,0x00,0x78,0x00,0x63,0x00,0x39,0x04,0x46,0x04,0x43,0x04};
  wint_t out[128];
  FILE *f=fopen("zzz.txt","w+");
  memset(out,0,sizeof(out));
  common::utf_decode(in,16,common::Encoding::Encoding_Utf16LE,(char*)out,128);
  wprintf(L"%‘\n",out);
  printf("---------------utf_encode-----------------\n");
  common::utf_encode(out,32,(char*)in,128,common::Encoding::Encoding_Utf8);
  for(int i=0;i<32;i++)
     putc(in[i],f);
  fclose(f);
  return 0;
}
