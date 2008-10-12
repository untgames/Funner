#include <common/heap.h>
#include <stdio.h>

using namespace common;

struct TestSample
{
  size_t      size;
  size_t      align;
  size_t      offset;
  const char* context;
};

TestSample test_samples [] = {
  {0},   {15}, {255}, {4095}, {1024*1024}, {256*1024},
  {0,1}, {4,8,3}, {13,256,11}, {1024*1024,8192,15},
  {0,4,3,"test"}, {255,64,13,"another_test"}, {4096,128,121,"test"}, {1024*1024,4096,153,"another_test"}
};

int main ()
{
  printf ("Results of size_test:\n");

  Heap heap;

  for (size_t i=0;i<sizeof (test_samples)/sizeof (TestSample);i++)
  {
    TestSample& sample = test_samples [i];

    heap.SetCurrentContext (sample.context ? heap.GetContext (sample.context) : heap.GetDefaultContext ());

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

    printf ("size=%lu align=%lu offset=%lu context=%s real_size=%lu\n",
            sample.size,sample.align,sample.offset,sample.context?sample.context:"default",heap.Size (p));

    heap.Deallocate (p);
  }

  return 0;
}
