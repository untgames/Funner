#include <common\parser.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

using namespace common;

const char* file_name = "data/test2.xml";

size_t filesize (const char* name)
{
  struct stat info;

  return stat (name,&info) ? (size_t)-1 : info.st_size;
}

int main ()
{
  ParseLog log;

  printf ("Results of test_xml-speed:\n");

  clock_t start = clock ();
  
  Parser p (log, file_name);
  
  clock_t end = clock ();  
    
  for (size_t i = 0; i < log.MessagesCount (); i++)
  {
    printf ("%s\n", log.Message(i));
  }
    
  size_t file_size = filesize (file_name);
    
  printf ("File size: %.1fM\n",float (file_size)/1.0e6f);
  printf ("Speed:     %.2fM/sec\n",float (file_size)/float (end-start)*float (CLOCKS_PER_SEC)/1.0e6f);
  
  return 0;  
}
