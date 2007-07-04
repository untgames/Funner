#include <common/parser.h>
#include <common/file.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>

using namespace common;

const char* file_name = "data/test3.wxf";

void* mem_alloc (size_t size,const char* oper)
{
  void* p = malloc (size);
  printf ("alloc %u bytes (%s) %s\n",size,oper,p?"successfull":"failed");
  return p;
}

void mem_free (void* p,const char* oper)
{
  if (!p)
    return;

  printf ("free %u bytes (%s)\n",_msize (p),oper);
  free (p);
}

void* operator new (size_t size)
{
  return mem_alloc (size,"operator new");
}

void* operator new [] (size_t size)
{
  return mem_alloc (size,"operator new []");
}

void operator delete (void* p)
{
  mem_free (p,"operator delete");
}

void operator delete [] (void* p)
{
  mem_free (p,"operator delete []");
}

int main ()
{
  printf ("Results of test_wxf-speed:\n");
  
  Parser::Log log;

  clock_t start = clock ();

  Parser parser (log,file_name);

  clock_t end = clock ();  

  if (log.HasErrors () || log.HasWarnings ()) 
  {
    printf ("Parse log:\n");

    for (size_t i=0;i<log.MessagesCount ();i++)
      printf ("%s\n",log.Message (i));
  }
  else printf ("No errors and warnings detected\n");
    
  size_t file_size = FileSystem::GetFileSize (file_name);
    
  printf ("File size: %.1fM\n",float (file_size)/1.0e6f);
  printf ("Speed:     %.2fM/sec\n",float (file_size)/float (end-start)*float (CLOCKS_PER_SEC)/1.0e6f);
  
  return 0;  
}
