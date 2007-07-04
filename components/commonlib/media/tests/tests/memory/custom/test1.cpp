#include <common/heap.h>
#include <stdio.h>

using namespace common;

size_t size_table [] = {0,16,256,1024*1024};

const size_t TABLE_SIZE = sizeof (size_table) / sizeof (size_t);

void* buf_table [TABLE_SIZE] = {0};

int main ()
{
  Heap heap;
  
  for (size_t i=0;i<TABLE_SIZE;i++)
  {
    printf ("Allocate %u bytes\n",size_table [i]);
    buf_table [i] = heap.Allocate (size_table [i]);
  }

  for (size_t i=0;i<TABLE_SIZE;i++)
  {
    printf ("Deallocate %u bytes (reserved %u)\n",size_table [i],heap.Size (buf_table [i]));
    heap.Deallocate (buf_table [i]);
  }

  return 0;
}
