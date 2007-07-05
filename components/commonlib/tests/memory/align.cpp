#include <common/heap.h>
#include <stdio.h>

using namespace common;

inline bool check_align (void* p,size_t align,size_t offset)
{
  return (((size_t)p+offset) & (align-1)) == 0;
}

struct TestSample
{
  size_t size;
  size_t align;
  size_t offset;
};

TestSample test_samples [] = {
  {0,1,0}, {4,8,3}, {13,256,11}, {10,3,0}, {1024*1024,8192,15}
};

int main ()
{
  printf ("Results of align_test:\n");
  
  Heap heap;  
  
  for (size_t i=0;i<sizeof (test_samples)/sizeof (TestSample);i++)
  {
    TestSample& sample = test_samples [i];
    
    void* p = heap.Allocate (sample.size,sample.align,sample.offset);
    
    if (!p)
    {
      printf ("allocate failed at size=%u align=%u offset=%u\n",sample.size,sample.align,sample.offset);
      continue;
    }
    
    if (!check_align (p,sample.align,sample.offset))
      printf ("align failed at size=%u align=%u offset=%u\n",sample.size,sample.align,sample.offset);
    
    heap.Deallocate (p);
  }
  
  printf ("successfull\n");

  return 0;
}
