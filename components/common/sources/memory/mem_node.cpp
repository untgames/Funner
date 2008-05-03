#include <common/heap.h>
#include "shared.h"

using namespace common;

static const size_t tree_minmax [32][2] = {
  {MEDIUM_MIN_BLOCK_SIZE,383}, {384,511}, {512,767}, {768,1023}, {1024,1535}, {1536,2047}, {2048,3071}, 
  {3072,4095}, {4096,6143}, {6144,8191}, {8192,12287}, {12288,16383}, {16384,24575}, {24576,32767}, {32768,49151},
  {49152,65535}, {65536,98303}, {98304,131071}, {131072,196607}, {196608,262143}, {262144,393215},
  {393216,524287}, {524288,786431}, {786432,1048575}, {1048576,1572863}, {1572864,2097151},
  {2097152,3145727}, {3145728,4194303}, {4194304,6291455}, {6291456,8388607}, {8388608,12582911},
  {12582912,(size_t)-1}
};

bool AllocNode::SetParent (AllocNode* new_parent)
{
  if (!new_parent || new_parent->heap != heap)
    return false;

  for (AllocNode* node=new_parent;node;node=node->parent)
    if (node == this)
      return false; //new_parent является потомком данного узла
      
  if (next)        next->prev          = prev;
  if (prev)        prev->next          = next;
  else if (parent) parent->first_child = next;
  
  parent                  = new_parent;
  prev                    = NULL;
  next                    = new_parent->first_child;
  new_parent->first_child = this;
  
  if (next) next->prev = this;

  return true;        
}

bool AllocNode::Unlink ()
{
  if (!parent)
    return false;

  while (first_child)
    first_child->SetParent (parent);

  if (next)        next->prev          = prev;
  if (prev)        prev->next          = next;
  else if (parent) parent->first_child = next;

  next = prev = parent = NULL;

  return true;
}

void AllocNode::ComputeStatistics (HeapStat& res,const HeapInternalStat& stat,const Heap::Impl& heap_impl)
{
  res.sys_allocate_count            = heap_impl.sys_allocate_count;
  res.sys_deallocate_count          = heap_impl.sys_deallocate_count;
  res.sys_allocate_size             = heap_impl.sys_allocate_size;
  res.sys_deallocate_size           = heap_impl.sys_deallocate_size;
  res.reserve_size                  = heap_impl.reserve_size;
  res.medium_top_use_count          = stat.medium_top_use_count;
  res.medium_search_count           = stat.medium_search_count;
  res.medium_search_steps_count     = stat.medium_search_steps_count;
  res.medium_max_search_steps_count = stat.medium_max_search_steps_count;
  res.min_block_size                = stat.min_block_size;
  res.max_block_size                = stat.max_block_size;

  HeapStat::Range* range            = res.ranges;
  size_t           allocate_count   = 0, 
                   deallocate_count = 0,
                   allocate_size    = 0,
                   deallocate_size  = 0;

  for (size_t i=0;i<SMALL_BLOCK_POOL_SIZE;i++)
    if (stat.small_allocate_count [i])
    {
      size_t block_size = (i + 1) * ALIGN_SIZE;

      range->min_size          = block_size - ALIGN_SIZE;
      range->max_size          = block_size - 1;
      range->allocate_count    = stat.small_allocate_count [i];
      range->allocate_size     = stat.small_allocate_count [i] * block_size;
      range->deallocate_count  = stat.small_deallocate_count [i];
      range->deallocate_size   = stat.small_deallocate_count [i] * block_size;
      allocate_count          += range->allocate_count;
      allocate_size           += range->allocate_size;
      deallocate_count        += range->deallocate_count;
      deallocate_size         += range->deallocate_size;

      if (block_size > res.max_block_size) res.max_block_size = block_size;
      if (block_size < res.min_block_size) res.min_block_size = block_size;

      range++;
    }
  
  for (size_t i=0;i<MEDIUM_TREE_COUNT;i++)
    if (stat.medium_allocate_count [i])
    {
      range->min_size          = tree_minmax [i][0];
      range->max_size          = tree_minmax [i][1];
      range->allocate_count    = stat.medium_allocate_count [i];
      range->allocate_size     = stat.medium_allocate_size [i];
      range->deallocate_count  = stat.medium_deallocate_count [i];
      range->deallocate_size   = stat.medium_deallocate_size [i];
      allocate_count          += range->allocate_count;
      allocate_size           += range->allocate_size;
      deallocate_count        += range->deallocate_count;
      deallocate_size         += range->deallocate_size;      
      
      range++;  
    }

  res.allocate_count   = allocate_count;
  res.deallocate_count = deallocate_count;
  res.allocate_size    = allocate_size;
  res.deallocate_size  = deallocate_size;
  res.ranges_count     = range - res.ranges;
}

void AllocNode::GetSelfStatistics (HeapStat& res)
{
  ComputeStatistics (res,stat,*heap->impl);
}

void AllocNode::AccumulateStatistics (HeapInternalStat& res)
{
  res.medium_top_use_count      += stat.medium_top_use_count;
  res.medium_search_count       += stat.medium_search_count;
  res.medium_search_steps_count += stat.medium_search_steps_count;  
    
  if (stat.medium_max_search_steps_count > res.medium_max_search_steps_count)
    res.medium_max_search_steps_count = stat.medium_max_search_steps_count;
    
  if (stat.min_block_size < res.min_block_size) res.min_block_size = stat.min_block_size;
  if (stat.max_block_size > res.max_block_size) res.max_block_size = stat.max_block_size;
  
  for (size_t i=0;i<SMALL_BLOCK_POOL_SIZE;i++)
  {
    res.small_allocate_count [i]   += stat.small_allocate_count [i];
    res.small_deallocate_count [i] += stat.small_deallocate_count [i];
  }
  
  for (size_t i=0;i<MEDIUM_TREE_COUNT;i++)
  {
    res.medium_allocate_count [i]   += stat.medium_allocate_count [i];
    res.medium_allocate_size [i]    += stat.medium_allocate_size [i];
    res.medium_deallocate_count [i] += stat.medium_deallocate_count [i];
    res.medium_deallocate_size [i]  += stat.medium_deallocate_size [i];
  }
  
  for (AllocNode* node=first_child;node;node=node->next)
    node->AccumulateStatistics (res);
}

void AllocNode::GetStatistics (HeapStat& res)
{
  total_stat.Init ();
  
  AccumulateStatistics (total_stat);    
  ComputeStatistics    (res,total_stat,*heap->impl);  
}
