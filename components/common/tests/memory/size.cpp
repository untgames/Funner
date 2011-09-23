#include <common/heap.h>
#include <stdio.h>

using namespace common;

struct TestSample
{
  size_t size;
  size_t align;
  size_t offset;
};

TestSample test_samples [] = {
  {0},   {15}, {255}, {4095}, {1024*1024}, {256*1024},
  {0,1}, {4,8,3}, {13,256,11}, {1024*1024,8192,15},
  {0,4,3}, {255,64,13}, {4096,128,121}, {1024*1024,4096,153}
};

int main ()
{
  printf ("Results of size_test:\n");

  Heap heap;

  for (size_t i=0;i<sizeof (test_samples)/sizeof (TestSample);i++)
  {
    TestSample& sample = test_samples [i];

    void* p;

    if (sample.align || sample.offset) p = heap.Allocate (sample.size,sample.align,sample.offset);
    else                               p = heap.Allocate (sample.size);

    if (!p)
    {
      printf ("allocate failed at size=%lu align=%lu offset=%lu\n",sample.size,sample.align,sample.offset);
      continue;
    }

    if (heap.Size (p) < sample.size)
    {
      printf ("error: allocated_size = %lu real_size = %lu\n",sample.size,heap.Size (p));
      return 0;
    }

    printf ("size=%lu align=%lu offset=%lu real_size=%lu\n",
            sample.size,sample.align,sample.offset,heap.Size (p));

    heap.Deallocate (p);
  }

  return 0;
}
