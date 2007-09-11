#include <common/heap.h>
#include "shared.h"

using namespace common;

/*
    AllocationContextException
*/

const char* AllocationContextException::what () const throw ()
{
  return "No enough memory for create allocation context";
}

/*
    AllocationContext
*/

AllocationContext::AllocationContext (AllocNode* _node)
{
  node = _node;  
  node->AddRef ();
}

AllocationContext::AllocationContext (Heap& heap,const char* name)
{
  node = NULL;
  (*this) = heap.GetContext (name);
}

AllocationContext::AllocationContext (const AllocationContext& context)
{
  node = context.node;
  node->AddRef ();
}

AllocationContext::~AllocationContext ()
{
  node->Release ();
}

AllocationContext& AllocationContext::operator = (const AllocationContext& context)
{
  if (&context == this)
    return *this;
    
  node->Release ();
    
  node = context.node;
  
  node->AddRef (); 
  
  return *this;
}

const char* AllocationContext::GetName () const
{
  return node->name;
}

Heap& AllocationContext::GetHeap () const
{
  return *node->heap;
}

void AllocationContext::GetSelfStatistics (HeapStat& stat) const
{
  node->GetSelfStatistics (stat);
}

void AllocationContext::GetStatistics (HeapStat& stat) const
{
  node->GetStatistics (stat);
}

bool AllocationContext::SetParent (const AllocationContext& context)
{
  return node->SetParent (context.node);
}

AllocationContext AllocationContext::GetParent () const
{
  return node->parent ? node->parent : node;
}
