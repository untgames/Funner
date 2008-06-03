#include <common/heap.h>
#include <common/strlib.h>
#include <platform/platform.h>
#include <memory.h>
#include <xtl/function.h>

#include "shared.h"

using namespace common;

/*
    Утилиты
*/

inline size_t AlignSize (size_t size,size_t align)
{
  return (size+align-1)&~(align-1);
}

inline void* AlignPtr (void* p,size_t align)
{
  return (void*)(((size_t)p+align-1)&~(align-1));
}

inline void* AlignPtr (void* p,size_t align,size_t offset)
{
  return (char*)(((size_t)p+offset+align-1)&~(align-1))-offset;
}

//взятие индекса дерева по размеру блока (реализация этой функции взята из dlmalloc)
inline size_t GetTreeIndex (size_t size) 
{
  size_t X = size >> MEDIUM_TREE_INDEX_SHIFT;
  
  if (!X)
    return 0;
        
  if (X > 0xFFFF)
    return MEDIUM_TREE_COUNT-1;

  size_t Y = X,
         N = ((Y - 0x100) >> 16) & 8,
         K = (((Y <<= N) - 0x1000) >> 16) & 4;         
  
  N += K;
  N += K = (((Y <<= K) - 0x4000) >> 16) & 2;
  K = 14 - N + ((Y <<= K) >> 15);
  
  return (K << 1) + ((size >> (K + (MEDIUM_TREE_INDEX_SHIFT-1)) & 1));
}

/*
    Работа со статистикой
*/

void HeapInternalStat::Init ()
{
  memset (this,0,sizeof (*this));
  
  min_block_size = (size_t)-1;
}

/*
    Инициализация / завершение
*/

Heap::Impl::Impl (Heap* heap,ICustomAllocator* allocator)
{
  custom_allocator          = allocator;
  medium_top                = NULL;
  first_page                = NULL;
  first_free_page           = NULL;
  last_free_page            = NULL;
  small_page_pos            = NULL;
  small_page_end            = NULL;
  sys_allocate_count        = 0;
  sys_deallocate_count      = 0;
  sys_allocate_size         = 0;
  sys_deallocate_size       = 0;  
  max_heap_size             = (size_t)-1; //размер не ограничен
  small_granularity         = DEFAULT_SMALL_GRANULARITY;
  medium_granularity        = DEFAULT_MEDIUM_GRANULARITY;
  medium_max_block_size     = DEFAULT_MEDIUM_MAX_BLOCK_SIZE;
  min_reserve_size          = DEFAULT_MIN_RESERVE_SIZE;
  max_reserve_size          = DEFAULT_MAX_RESERVE_SIZE;
  reserve_size              = 0;
  cur_node                  = &default_node;
  default_node.ref_count    = 1;
  default_node.heap         = heap;
  default_node.node_size    = sizeof (AllocNode) + sizeof (DEFAULT_NODE_NAME);
  default_node.hash         = strihash (DEFAULT_NODE_NAME);  
  default_node.prev         = NULL;
  default_node.next         = NULL;  
  default_node.parent       = NULL;  
  default_node.first_child  = NULL;

  memset (small_first_free,0,sizeof (small_first_free));
  memset (node_hashtable,0,sizeof (node_hashtable));
  strcpy (default_node.name,DEFAULT_NODE_NAME);
  
  default_node.stat.Init ();

  InsertAllocNode (&default_node);
}

Heap::Impl::~Impl ()
{
  while (first_page)
    FreePage (first_page);
}

/*
    Распределение страниц памяти    
*/

MemPage* Heap::Impl::AllocPage (BlockTag tag,size_t min_size,size_t recommended_size,size_t align,size_t offset)
{  
  min_size         += sizeof (MemPage) + align + offset;
  recommended_size += sizeof (MemPage) + align + offset;  
  
  size_t allocated_size = sys_allocate_size - sys_deallocate_size;
  
  if (max_heap_size < min_size || allocated_size > max_heap_size - min_size)
    return NULL;
    
  if (max_heap_size < recommended_size || allocated_size > max_heap_size - recommended_size)
    recommended_size = max_heap_size - allocated_size;

  void*  buf;
  size_t size = recommended_size;        
  
  for (;;)
  {    
    buf = custom_allocator->Allocate (size);

    if (buf)
      break;

    if      (size - min_size >= min_size)  size /= 2;
    else if (size != min_size)             size  = min_size;
    else                                   return NULL;
  }
  
  MemPage* page = (MemPage*)AlignPtr (buf,align,sizeof (MemPage)+offset);

  page->tag           = tag;
  page->base          = buf;
  page->size          = size - sizeof (MemPage) - align;
  page->reserved_size = size;
  page->prev          = NULL;
  page->next          = first_page;
  page->prev_free     = NULL;
  page->next_free     = NULL;
  first_page          = page;

  if (page->next)
    page->next->prev = page;

  sys_allocate_count++;
  sys_allocate_size += size;

  return page;
}

void Heap::Impl::FreePage (MemPage* page)
{
  if (page->next_free || page->prev_free)
    RemoveFreePage (page);

  sys_deallocate_count++;  
  sys_deallocate_size += page->reserved_size;

  if (page->next) page->next->prev = page->prev;
  if (page->prev) page->prev->next = page->next;
  else            first_page       = page->next;

  custom_allocator->Deallocate (page->base);
}

bool Heap::Impl::InsertFreePage (MemPage* page)
{
  if (page->prev_free || page->next_free)
    return true;

  if (max_reserve_size - reserve_size < page->reserved_size)
  {
    if (page->reserved_size > max_reserve_size)
    {     
      FreePage (page);
      return false;
    }

    TrimFreePages (page->reserved_size < min_reserve_size ? min_reserve_size - page->reserved_size : 0);
  }      

  page->next_free = first_free_page;
  page->prev_free = NULL;
  
  if (first_free_page) first_free_page->prev_free = page;
  else                 last_free_page             = page;
  
  first_free_page = page;
    
  reserve_size += page->reserved_size;
  
  return true;
}

void Heap::Impl::RemoveFreePage (MemPage* page)
{
  reserve_size -= page->reserved_size;    
  
  if (page->prev_free) page->prev_free->next_free = page->next_free;
  else                 first_free_page            = page->next_free;
  if (page->next_free) page->next_free->prev_free = page->prev_free;
  else                 last_free_page             = page->prev_free;

  page->prev_free = page->next_free = NULL;
}

void Heap::Impl::TrimFreePages (size_t limit)
{
  while (last_free_page && reserve_size > limit)
  {        
    MemPage* page = last_free_page;
    
    if (page->tag == MEDIUM_BLOCK_ID)
    {
      char* pos = (char*)page + sizeof (MemPage);

      if (medium_top && (char*)medium_top >= pos && (char*)medium_top < pos + page->size)
        medium_top = NULL;
      else
        RemoveMediumBlock ((MediumBlock*)pos);
    }
    
    FreePage (last_free_page);
  }
}

/*
    Распределение блоков малого размера
*/

void* Heap::Impl::AllocSmallBlock (size_t size,AllocNode* node)
{
  size_t pool_index = (size + sizeof (unsigned char)) / ALIGN_SIZE;  
    
  size = (pool_index + 1) * ALIGN_SIZE;  
  
  unsigned char* block = small_first_free [pool_index];  
  
  if (block)
  {
    small_first_free [pool_index] = *(unsigned char**)block;
    *block                        = pool_index;
  }
  else if (small_page_pos + size <= small_page_end)
  {
    block           = small_page_pos;
    small_page_pos += size;
    *block          = pool_index;        
  }
  else
  {
    MemPage* page = AllocPage (SMALL_BLOCK_ID,DEFAULT_SMALL_GRANULARITY,small_granularity);
    
    if (!page)
    {
      //try to alloc medium block
      return NULL;
    }

    block          = (unsigned char*)page + sizeof (MemPage);
    small_page_end = block + page->size;
    small_page_pos = block + size;
    *block         = pool_index;
  }
  
  HeapInternalStat& stat = node->stat;

  stat.small_allocate_count [pool_index]++;  
  
  return block + sizeof (unsigned char);
}

void Heap::Impl::FreeSmallBlock (void* p,AllocNode* node)
{
  unsigned char* block      = (unsigned char*)p - sizeof (unsigned char);
  size_t         pool_index = *block;

  *(unsigned char**)block       = small_first_free [pool_index];
  small_first_free [pool_index] = block;
    
  node->stat.small_deallocate_count [pool_index]++;
}

/*
    Управление выделением памяти средних блоков
*/

inline void Heap::Impl::InsertMediumBlock (MediumBlock* block)
{
  medium_tree [GetTreeIndex (block->size)].InsertBlock (block);
}

inline void Heap::Impl::RemoveMediumBlock (MediumBlock* block)
{
  medium_tree [GetTreeIndex (block->size)].RemoveBlock (block);
}

void* Heap::Impl::AllocMediumBlock (size_t size,AllocNode* node)
{
  size = AlignSize (size+sizeof (MediumAllocBlock),ALIGN_SIZE);  
        
  MediumBlockTree& tree  = medium_tree [GetTreeIndex (size)]; 
  MediumBlock*     block = NULL;
  
  //имеет смысл упростить логику условных выражений!!
  
  if (size <= tree.max_block_size)
  {
    block = tree.FindSmallestBlock (size,node);    

    if (!medium_top || block->size < medium_top->size)
    {      
      tree.RemoveBlock (block);

      if (block->size == block->page->size)
        RemoveFreePage (block->page);
          
      if (block->size - size >= MEDIUM_MIN_BLOCK_SIZE + sizeof (MediumAllocBlock)) //разделяем блок на два
      { 
        MediumBlock* reserved_block = (MediumBlock*)((char*)block+size);
        
        reserved_block->size    = block->size - size;
        reserved_block->prev    = block;
        reserved_block->next    = block->next;
        reserved_block->page    = block->page;
        reserved_block->is_free = true;
        block->next             = reserved_block;
        block->size             = size;
        
        if (reserved_block->next)
          reserved_block->next->prev = reserved_block;

        InsertMediumBlock (reserved_block);
      }
    }
    else block = NULL;
  }

  if (!block && medium_top && medium_top->size >= size) //размещаем блок на доступной вершине
  {    
    node->stat.medium_top_use_count++;
    
    block = medium_top;    
    
    if (medium_top->page->size == medium_top->size)
      RemoveFreePage (medium_top->page);
    
    if (block->size - size >= MEDIUM_MIN_BLOCK_SIZE + sizeof (MediumAllocBlock)) //разрезаем блок
    {
      MediumBlock* new_top = (MediumBlock*)((char*)block+size);
    
      new_top->size    = block->size - size;
      new_top->page    = block->page;
      new_top->prev    = block;
      new_top->next    = NULL;
      new_top->is_free = true;
      block->next      = new_top;
      block->size      = size;
      medium_top       = new_top;      
    }
    else medium_top = NULL;    
  }
  
  if (!block)
  {        
    MemPage* page = AllocPage (MEDIUM_BLOCK_ID,size,size < medium_granularity ? medium_granularity : size);   

    if (!page)
      return NULL;      
      
    block = (MediumBlock*)((char*)page + sizeof (MemPage));
      
    if (page->size - size < MEDIUM_MIN_BLOCK_SIZE + sizeof (MediumAllocBlock))
    {
      block->size = page->size;
      block->page = page;
      block->prev = NULL;
      block->next = NULL;
      
      if (medium_top)
      {
        InsertMediumBlock (medium_top);
        medium_top = NULL;
      }
    }
    else
    {    
      MediumBlock* reserved_block = (MediumBlock*)((char*)block + size);

      reserved_block->size    = page->size-size;
      reserved_block->page    = page;
      reserved_block->prev    = block;
      reserved_block->next    = NULL;
      reserved_block->is_free = true;
      block->size             = size;
      block->page             = page;
      block->prev             = NULL;    
      block->next             = reserved_block;

      if (!medium_top || reserved_block->size > medium_top->size)
      {
        if (medium_top)
          InsertMediumBlock (medium_top);
          
        medium_top = reserved_block;
      }
      else InsertMediumBlock (reserved_block);
    }
  }
  
  block->is_free = false;
  
  HeapInternalStat& stat       = node->stat;
  size_t            tree_index = GetTreeIndex (block->size);
  
  size = block->size;
  
  stat.medium_allocate_count [tree_index]++;    
  stat.medium_allocate_size [tree_index] += size;

  if (size > stat.max_block_size) stat.max_block_size = size;
  if (size < stat.min_block_size) stat.min_block_size = size;  
    
  return (char*)block + sizeof (MediumAllocBlock);
}

void Heap::Impl::FreeMediumBlock (void* p,AllocNode* node)
{
  MediumBlock *block = (MediumBlock*)((char*)p-sizeof (MediumAllocBlock)),
              *prev  = (MediumBlock*)block->prev,
              *next  = (MediumBlock*)block->next;                            
              
  HeapInternalStat& stat       = node->stat;
  size_t            tree_index = GetTreeIndex (block->size);
                
  stat.medium_deallocate_count [tree_index]++;  
  stat.medium_deallocate_size [tree_index] += block->size;

  if (prev && prev->is_free) //объединяем данный блок с предыдущим
  {      
    RemoveMediumBlock (prev);

    prev->size += block->size;
    prev->next  = next;

    if (next)
      next->prev = prev;

    block = prev;    
  }

  if (next && next->is_free) //объединяем данный блок с следующим
  {
    if (next != medium_top)
      RemoveMediumBlock (next);
    else 
      medium_top = block;

    block->size += next->size;
    block->next  = next->next;

    if (block->next)
      block->next->prev = block;
  }

  block->is_free = true;    
  
  MemPage* page = block->page;    
  
  if (block->size == page->size && !InsertFreePage (page))
    return;

  if (block != medium_top)
    InsertMediumBlock (block);    
}

/*
    Выделение блоков большого размера
*/

void* Heap::Impl::AllocLargeBlock (size_t size,AllocNode* node,size_t align,size_t offset)
{
  MemPage* page = AllocPage (LARGE_BLOCK_ID,size,size,align,offset);
  
  if (!page)
    return NULL;

  HeapInternalStat& stat       = node->stat;
  size_t            tree_index = GetTreeIndex (page->reserved_size);  

  stat.medium_allocate_count [tree_index]++;  
  stat.medium_allocate_size [tree_index] += page->reserved_size;  

  if (page->size > stat.max_block_size) stat.max_block_size = page->reserved_size;
  if (page->size < stat.min_block_size) stat.min_block_size = page->reserved_size;

  return (unsigned char*)page+sizeof (MemPage);
}

void Heap::Impl::FreeLargeBlock (void* p,AllocNode* node)
{
  MemPage* page = (MemPage*)((unsigned char*)p-sizeof (MemPage));  

  HeapInternalStat& stat       = node->stat;  
  size_t            tree_index = GetTreeIndex (page->reserved_size);

  stat.medium_deallocate_count [tree_index]++;
  stat.medium_deallocate_size [tree_index] += page->reserved_size;

  FreePage (page);
}

/*
    Управление узлами распределения
*/

void Heap::Impl::InsertAllocNode (AllocNode* node)
{
  AllocNode*& first = node_hashtable [node->hash&(NODE_HASH_TABLE_SIZE-1)];
  
  node->prev_hash = NULL;
  node->next_hash = first;
  
  if (first)
    first->prev_hash = node;
  
  first = node;
}

void Heap::Impl::RemoveAllocNode (AllocNode* node)
{
  AllocNode*& first = node_hashtable [node->hash&(NODE_HASH_TABLE_SIZE-1)];
  
  if (node->next_hash) node->next_hash->prev_hash = node->prev_hash;
  if (node->prev_hash) node->prev_hash->next_hash = node->next_hash;
  else                 first                      = node->next_hash;
}

/*
    Инициализация / завершение работы менеджера памяти
*/

Heap::Heap ()
{
  Init (Platform::GetSystemAllocator ());
}

Heap::Heap (ICustomAllocator* allocator)
{
  Init (allocator);
}

void Heap::Init (ICustomAllocator* allocator)
{
  if (!allocator)
    throw std::bad_alloc ();

  impl = (Impl*)allocator->Allocate (sizeof (Impl));

  if (!impl)
    throw std::bad_alloc ();

  try
  {
    new (impl) Impl (this,allocator);
  }
  catch (...)
  {
    allocator->Deallocate (impl);
  }
  
  allocator->AddRef ();
}

Heap::~Heap ()
{
  ICustomAllocator* allocator = impl->custom_allocator;

  impl->~Impl ();

  allocator->Deallocate (impl);
  allocator->Release ();
}

/*
    Установка пользовательских обработчиков событий менеджера памяти
*/

void Heap::SetFailHandler (const FailHandler& handler)
{
  impl->fail_handler = handler;
}

const Heap::FailHandler& Heap::GetFailHandler () const
{
  return impl->fail_handler;
}

void Heap::SetDebugHandler (const DebugHandler& handler)
{
  impl->debug_handler = handler;
}

const Heap::DebugHandler& Heap::GetDebugHandler () const
{
  return impl->debug_handler;
}

/*
    Конфигурирование менеджера памяти
*/

void Heap::SetGranularity (size_t small_granularity,size_t medium_granularity)
{
  impl->small_granularity  = small_granularity > MIN_GRANULARITY ? small_granularity : MIN_GRANULARITY;
  impl->medium_granularity = medium_granularity > MIN_GRANULARITY ? medium_granularity : MIN_GRANULARITY;
}

size_t Heap::GetSmallGranularity () const
{
  return impl->small_granularity;
}

size_t Heap::GetMediumGranularity () const
{
  return impl->medium_granularity;
}

/*
    Заказ / освобождение памяти
*/

void* Heap::TryAllocate (size_t size)
{
  BlockTag       tag;
  unsigned char* p;
  AllocNode*     node = impl->cur_node;  

  size += sizeof (BlockTag);
  
  if (node != &impl->default_node)
    size += sizeof (AllocNode*);

  if (size < MEDIUM_MIN_BLOCK_SIZE)
  {
    p   = (unsigned char*)impl->AllocSmallBlock (size,node);
    tag = SMALL_BLOCK_ID;
  }
  else if (size <= impl->medium_max_block_size)
  {
    p   = (unsigned char*)impl->AllocMediumBlock (size,node);
    tag = MEDIUM_BLOCK_ID;
  }
  else
  {
    p   = (unsigned char*)impl->AllocLargeBlock (size,node);
    tag = LARGE_BLOCK_ID;
  }

  if (!p)
    return NULL;    
    
  if (node == &impl->default_node)
  {
    *(BlockTag*)p = tag;
    
    return p + sizeof (BlockTag);
  }  

  *(AllocNode**)p  = node;
  p               += sizeof (AllocNode*);
  *(BlockTag*)p    = tag|CONTEXT_BLOCK_ID;
  
  node->AddRef ();
  
  return p + sizeof (BlockTag);
}

void* Heap::TryAllocate (size_t size,size_t align,size_t offset)
{
  if (align & (align-1)) //проверка случая, когда align не является степенью двойки
    return NULL;    

  AllocNode* node = impl->cur_node;
  
  offset += sizeof (BlockTag);  
  
  if (node != &impl->default_node)
    offset += sizeof (AllocNode*);
    
  size_t full_size = size + align + offset;

  if (full_size > impl->medium_max_block_size)
  {
    unsigned char* p = (unsigned char*)impl->AllocLargeBlock (size,node,align,offset);
    
    if (!p)
      return NULL;
    
    if (node == &impl->default_node)
    {
      *(BlockTag*)p = LARGE_BLOCK_ID;
      
      return p + sizeof (BlockTag);       
    }
    
    *(AllocNode**)p  = node;
    p               += sizeof (AllocNode*);
    *(BlockTag*)p    = LARGE_BLOCK_ID|CONTEXT_BLOCK_ID;
    
    node->AddRef ();
 
    return p + sizeof (BlockTag);
  }
  
  offset += sizeof (void*);
  
  void*    buf;
  BlockTag tag;  

  if (full_size < MEDIUM_MIN_BLOCK_SIZE)
  {
    buf = impl->AllocSmallBlock (full_size,node);
    tag = SMALL_BLOCK_ID|ALIGNED_BLOCK_ID;
  }
  else
  {
    buf = impl->AllocMediumBlock (full_size,node);
    tag = MEDIUM_BLOCK_ID|ALIGNED_BLOCK_ID;
  }  
  
  if (!buf)
    return NULL;
        
  unsigned char* p = (unsigned char*)AlignPtr (buf,align,offset);
  
  if (node == &impl->default_node)
  {
    *(void**)p     = buf;
    p             += sizeof (void*);
    *(BlockTag*)p  = tag;

    return p + sizeof (BlockTag);
  }  
  
  *(void**)p       = buf;
  p               += sizeof (void*);
  *(AllocNode**)p  = node;
  p               += sizeof (AllocNode*);
  *(BlockTag*)p    = tag|CONTEXT_BLOCK_ID;
  
  node->AddRef ();

  return p + sizeof (BlockTag);
}

void* Heap::Allocate (size_t size)
{
  void* p = TryAllocate (size);
  
  if (p)
    return p;
    
  if (!impl->fail_handler)
    return NULL;
  
  while (!p && impl->fail_handler (size))
    p = TryAllocate (size);

  return p;
}

void* Heap::Allocate (size_t size,size_t align,size_t offset)
{
  void* p = TryAllocate (size,align,offset);
  
  if (p)
    return p;
    
  if (!impl->fail_handler)
    return NULL;
  
  while (!p && impl->fail_handler (size))
    p = TryAllocate (size,align,offset);

  return p;
}

void Heap::Deallocate (void* p)
{
  if (!p)
    return;

  unsigned char* block = (unsigned char*)p - sizeof (BlockTag);
  AllocNode*     node  = (AllocNode*)(block - sizeof (AllocNode*)); //переменная используется не во всех ветвях    
  
  switch (*(BlockTag*)block)
  {
    case SMALL_BLOCK_ID:
      impl->FreeSmallBlock (block,&impl->default_node);
      break;
    case MEDIUM_BLOCK_ID:
      impl->FreeMediumBlock (block,&impl->default_node);
      break;
    case LARGE_BLOCK_ID:
      impl->FreeLargeBlock (block,&impl->default_node);
      break;
    case SMALL_BLOCK_ID|ALIGNED_BLOCK_ID:
      impl->FreeSmallBlock (*(void**)(block - sizeof (void*)),&impl->default_node);
      break;
    case MEDIUM_BLOCK_ID|ALIGNED_BLOCK_ID:
      impl->FreeMediumBlock (*(void**)(block - sizeof (void*)),&impl->default_node);
      break;
    case SMALL_BLOCK_ID|CONTEXT_BLOCK_ID:      
      node->Release ();
      impl->FreeSmallBlock (block-sizeof (AllocNode*),node);
      break;
    case MEDIUM_BLOCK_ID|CONTEXT_BLOCK_ID:
      node->Release ();    
      impl->FreeMediumBlock (block-sizeof (AllocNode*),node);
      break;
    case LARGE_BLOCK_ID|CONTEXT_BLOCK_ID:
      node->Release ();    
      impl->FreeLargeBlock (block-sizeof (AllocNode*),node);
      break;
    case SMALL_BLOCK_ID|ALIGNED_BLOCK_ID|CONTEXT_BLOCK_ID:
      node->Release ();
      impl->FreeSmallBlock (*(void**)(block - sizeof (void*) - sizeof (AllocNode*)),node);
      break;
    case MEDIUM_BLOCK_ID|ALIGNED_BLOCK_ID|CONTEXT_BLOCK_ID:
      node->Release ();
      impl->FreeMediumBlock (*(void**)(block - sizeof (void*) - sizeof (AllocNode*)),node);
      break;
    default:
     //call user handler
     break;
  }
}

size_t Heap::Size (void* p) const
{
  if (!p)
    return 0;
    
  unsigned char* block       = (unsigned char*)p - sizeof (BlockTag);
  size_t         header_size = sizeof (BlockTag);
  BlockTag       tag         = (*(BlockTag*)block) & BLOCK_ID_MASK;
  BlockTag       info_tag    = (*(BlockTag*)block) & ~BLOCK_ID_MASK;
    
  switch (info_tag)
  {
    case ALIGNED_BLOCK_ID:
      block        = *(unsigned char**)(block - sizeof (void*));
      header_size += sizeof (void*);    
      break;    
    case CONTEXT_BLOCK_ID:
      block       -= sizeof (AllocNode*);
      header_size += sizeof (AllocNode*);
      break;
    case ALIGNED_BLOCK_ID|CONTEXT_BLOCK_ID:
      block        = *(unsigned char**)(block - sizeof (void*) - sizeof (AllocNode*));
      header_size += sizeof (void*) + sizeof (AllocNode*);    
      break;
    case 0: //no info
      break;
    default:
      //call user handler
      return 0;      
  }

  switch (tag)
  {
    case SMALL_BLOCK_ID:
      return size_t (block [-1]+1) * ALIGN_SIZE - sizeof (unsigned char) - header_size;
    case MEDIUM_BLOCK_ID:
      return ((MediumAllocBlock*)(block - sizeof (MediumAllocBlock)))->size - sizeof (MediumAllocBlock) - header_size;
    case LARGE_BLOCK_ID:
      return ((MemPage*)(block - sizeof (MemPage)))->size - header_size;
    default:
     //call user handler
     return 0;
  }
}

/*
    Работа с резервом неисользуемых страниц
*/

void Heap::SetReserveSize (size_t min_size,size_t max_size)
{
  if (max_size < min_size)
    max_size = min_size;

  impl->min_reserve_size = min_size;
  impl->max_reserve_size = max_size;
}

size_t Heap::GetMinReserveSize () const
{
  return impl->min_reserve_size;
}

size_t Heap::GetMaxReserveSize () const
{
  return impl->max_reserve_size;
}

void Heap::TrimReserve (size_t min_size,size_t max_size)
{
  if (max_size < min_size)
    max_size = min_size;

  if (impl->reserve_size > max_size)
    impl->TrimFreePages (min_size);
}

void Heap::TrimReserve ()
{
  if (impl->reserve_size > impl->max_reserve_size)
    impl->TrimFreePages (impl->min_reserve_size);
}

void Heap::FlushReserve ()
{
  impl->TrimFreePages (0);
}

/*
    Работа с контекстами распределения
*/

AllocationContext Heap::GetContext (const char* name)
{
  if (!name)
    return NULL;
    
  size_t     hash = strihash (name);
  AllocNode* node = impl->node_hashtable [hash&(NODE_HASH_TABLE_SIZE-1)];
  
  for (;node;node=node->next_hash)
    if (node->hash == hash)
      return node;

    //узел не найден, необходимо его создать
    
  size_t size = sizeof (AllocNode) + strlen (name); //+1 байт на завершение строки учтён в структуре AllocNode  

  node = (AllocNode*)Allocate (size);
  
  if (!node)
    throw AllocationContextException ();
    
  node->ref_count   = 0;
  node->node_size   = size;
  node->heap        = this;
  node->hash        = hash;
  node->parent      = &impl->default_node;
  node->prev        = NULL;
  node->next        = impl->default_node.first_child;
  node->first_child = NULL;
  
  impl->default_node.first_child = node;
  
  if (node->next)
    node->next->prev = node;
  
  strcpy (node->name,name);
  
  impl->InsertAllocNode (node);
  node->stat.Init ();

  return node;
}

void Heap::SetCurrentContext (const AllocationContext& context)
{
  impl->cur_node->Release ();
    
  impl->cur_node = context.node;

  impl->cur_node->AddRef ();
}

AllocationContext Heap::GetCurrentContext () const
{
  return impl->cur_node;
}

AllocationContext Heap::GetDefaultContext () const
{
  return &impl->default_node;
}

/*
    Получение статистики
*/

void Heap::GetStatistics (HeapStat& stat)
{
  impl->default_node.GetStatistics (stat);
}
