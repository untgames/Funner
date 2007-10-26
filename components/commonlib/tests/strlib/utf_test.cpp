#include <common/utf_converter.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char in[128],out[128];
  gets(in);
  common::utf_decode(in,16,common::Encoding::Encoding_Ascii7,out,128);
  for(int i=0;i<10;i++)
     putchar(out[i]);
  return 0;
}
