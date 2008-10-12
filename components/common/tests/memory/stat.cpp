#include <common/heap.h>
#include <stdio.h>
#include <stdlib.h>

const size_t TOTAL    = 100000;
const size_t MIN_SIZE = 15;
const size_t MAX_SIZE = 2000;

using namespace common;

int main ()
{
  printf ("Results of stat_test:\n");

  Heap heap;

  for (size_t i=0;i<TOTAL;i++)
//    heap.Allocate (i&1023);
    heap.Allocate (rand () % (MAX_SIZE-MIN_SIZE)+MIN_SIZE);

  HeapStat stat;

  heap.GetStatistics (stat);

  printf ("Statistics:\n");
  printf ("\tAlloc from system:       %.2fM\n",float (stat.sys_allocate_size)/1000000.0f);
  printf ("\tDeallocate to system:    %.2fM\n",float (stat.sys_deallocate_size)/1000000.0f);
  printf ("\tSystem alloc calls:      %lu\n",stat.sys_allocate_count);
  printf ("\tSystem free calls:       %lu\n",stat.sys_deallocate_count);
  printf ("\tReserve size:            %.2fM\n",float (stat.reserve_size)/1000000.0f);
  printf ("\tAllocate count:          %lu\n",stat.allocate_count);
  printf ("\tDeallocate count:        %lu\n",stat.deallocate_count);
  printf ("\tAllocate size:           %.2fM\n",float (stat.allocate_size)/1000000.0f);
  printf ("\tDeallocate size:         %.2fM\n",float (stat.deallocate_size)/1000000.0f);
  printf ("\tMedium search:           %lu\n",stat.medium_search_count);
  printf ("\tMedium top use count:    %lu\n",stat.medium_top_use_count);
  printf ("\tMedium search steps:     %lu\n",stat.medium_search_steps_count);
  printf ("\tMedium max search steps: %lu\n",stat.medium_max_search_steps_count);
  printf ("\tAvg medium steps:        %.2f\n",float (stat.medium_search_steps_count)/float (stat.medium_search_count));
  printf ("\tMin block size:          %lu\n",stat.min_block_size);
  printf ("\tMax block size:          %lu\n",stat.max_block_size);
  printf ("Range statistics:\n");
  printf ("\t\t\t\t Blocks\tTotal size\n");
  printf ("--------------------------------------------------------------------------------\n");

  for (size_t i=0;i<stat.ranges_count;i++)
  {
    const HeapStat::Range& range = stat.ranges [i];

    printf ("\t[%8lu;%8lu]: %8lu\t%8lu\n",range.min_size,range.max_size,range.allocate_count-range.deallocate_count,
            range.allocate_size-range.deallocate_size);
  }

  return 0;
}
