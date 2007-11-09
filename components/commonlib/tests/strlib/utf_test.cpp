#include <common/utf_converter.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char in[128]={0x7A,0x78,0x63,0xD0,0xB,0xD1,0x86,0xD1,0x83,0x0};
  wint_t out[128];
  FILE *f=fopen("zzz.txt","w+");
  memset(out,0,sizeof(out));
  common::utf_decode(in,16,common::Encoding::Encoding_Utf8,(char*)out,128);
  for(int i=0;i<32;i++)
     putwc(out[i],f);
  fclose(f);
  return 0;
}
