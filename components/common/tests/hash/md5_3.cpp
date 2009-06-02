// Тест хеширования файла блоками произвольного размера с использованием MD5Context
//                      (c) Gred

#include <common/hash.h>
#include <common/file.h>
#include <stdio.h>

using namespace common;

const char*  sample_file_name  = "data/sample1.txt";
const size_t READ_BUFFER_SIZE = 128;

int main ()
{
  printf ("Results of md5_3_test:\n");
  
  try
  {
    InputFile     file (sample_file_name);    
    Md5Context    context;
    unsigned char buffer [READ_BUFFER_SIZE], digest [16];    
    
    for (size_t len;(len=file.Read (buffer,READ_BUFFER_SIZE));)
      context.Update (buffer,len);
      
    context.Finish (digest);      
    
    printf ("hash = ");

    for (int i=0;i<16;i++)
      printf ("%02x",digest [i]);

    printf ("\n");
  }
  catch (std::exception& exception)
  {
    printf ("FileException: %s\n",exception.what ());
  }

  return 0;
}
