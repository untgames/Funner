#include <stdio.h>
#include <windows.h>
#include <errno.h>
#include "dds.h"

#define DDS_SIG 0x20534444  //'DDS '

struct my_header9
{
//   char sig[4];
   DWORD  dwMagic;
   DDS_HEADER header;
};

my_header9 header;

int main(int varc,char *vars[])
{
   int r;
   printf("sizeof(my_header9) - %d\n",sizeof(my_header9));
   if(varc<1)
      return 0;
   printf("%s\n",vars[1]);
   FILE *f=fopen(vars[1],"rb");
   if(f==NULL)
      return 0;
   r=fread((void*)&header,sizeof(header),1,f);
   if(r<1)
      return 0;
   if(header.dwMagic!=DDS_SIG)
      return 0;

   putchar('\n');
   printf("Size:\t%dx%d\n",header.header.dwWidth,header.header.dwHeight);
   printf("Depth:\t%d\n",header.header.dwDepth); 

   printf("Pixel Format:\n");
   if(header.header.ddspf.dwFlags&DDS_FOURCC)
   {
      printf("\tCompression:\t");
      for(int i=0;i<4;i++)
         putchar(((char*)&(header.header.ddspf.dwFourCC))[i]);
      putchar('\n');
   }
   if(header.header.ddspf.dwFlags&DDS_RGBA)
      printf("\tColor data masks:\n");
   if(header.header.ddspf.dwFlags&DDS_RGB)
      printf("\t\tR:\t%08X\n\t\tG:\t%08X\n\t\tB:\t%08X\n",
         header.header.ddspf.dwRBitMask,header.header.ddspf.dwGBitMask,header.header.ddspf.dwBBitMask);
   if(header.header.ddspf.dwFlags&DDS_RGBA)
      printf("\t\tA:\t%08X\n",header.header.ddspf.dwABitMask);
   fclose(f);
   return 0;
}