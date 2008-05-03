#include <xtl/function.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t ALIGN_SIZE                    = 8;                //������������ �� ���������
const size_t MIN_GRANULARITY               = 4096;             //����������� ������ ������ ������������ � �������
const size_t SMALL_BLOCK_POOL_SIZE         = 256/ALIGN_SIZE;   //������ ���� ��������� ������
const size_t MEDIUM_MIN_BLOCK_SIZE         = SMALL_BLOCK_POOL_SIZE*ALIGN_SIZE-1;//����������� ������ ����� �������� �������
const size_t DEFAULT_SMALL_GRANULARITY     = MIN_GRANULARITY;  //������ �������� ������ ������ ������� (�� ���������)
const size_t DEFAULT_MEDIUM_GRANULARITY    = MIN_GRANULARITY;  //����������� ������ �������� ������ ������ ������� (�� ���������)
const size_t DEFAULT_MEDIUM_MAX_BLOCK_SIZE = 256*1024;         //������������ ������ ����� �������� ������� (�� ���������)
const size_t DEFAULT_MIN_RESERVE_SIZE      = 1024*1024;        //������������� ����������� ������ ������� (�� ���������)
const size_t DEFAULT_MAX_RESERVE_SIZE      = 8192*1024;        //������������� ������������ ������ ������� (�� ���������)
const size_t NODE_HASH_TABLE_SIZE          = 256;              //������ ���-������� ����� �������������
const char   DEFAULT_NODE_NAME []          = "default";        //��� ���� ������������ �� ���������
const size_t MEDIUM_TREE_INDEX_SHIFT       = 8;                //�������� ������� ��� ��������� ������� ������
const size_t MEDIUM_TREE_COUNT             = 32;               //���������� �������� ������ �������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char BlockTag;

const BlockTag BLOCK_ID_MASK    = 3; //����� �������������� ���� �������������� �����
const BlockTag SMALL_BLOCK_ID   = 1; //������������� ����� ������ �������
const BlockTag MEDIUM_BLOCK_ID  = 2; //������������� ����� �������� �������
const BlockTag LARGE_BLOCK_ID   = 3; //������������� ����� �������� �������
const BlockTag ALIGNED_BLOCK_ID = 4; //���� �������� ���������� � ������������
const BlockTag CONTEXT_BLOCK_ID = 8; //���� �������� ������ �� �������� �������������

#pragma pack(8) //������ ��������� � ALIGN_SIZE

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ������, ���������� ��������� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MemPage
{
  void*    base;          //������� ����� ��������
  size_t   size;          //������ �������� ��������� ������������
  size_t   reserved_size; //����������������� ������ ��������
  BlockTag tag;           //����� ������ ������� ����������� �� ��������
  MemPage* prev;          //���������� ��������
  MemPage* next;          //��������� ��������
  MemPage* prev_free;     //���������� �������� �������
  MemPage* next_free;     //��������� �������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MediumAllocBlock
{
  size_t            size;    //������ �����
  MediumAllocBlock* prev;    //���������� ���� (�� ������������ �� ��������)
  MediumAllocBlock* next;    //��������� ���� (�� ������������ �� ��������)
  MemPage*          page;    //��������, ������� ����������� ����  
  int               is_free; //����, ��������������� ���� ���� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TreeColor
{
  RED,
  BLACK
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MediumBlock: public MediumAllocBlock
{
  MediumBlock* left;      //��������� �� ����� ���������
  MediumBlock* right;     //��������� �� ������ ���������  
  MediumBlock* parent;    //��������� �� ������������ ����
  MediumBlock* prev_free; //���������� ��������� ���� ������ �� �������
  MediumBlock* next_free; //��������� ��������� ���� ������ �� �������
  TreeColor    color;     //���� ���� ������   
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ������������� ������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct HeapInternalStat
{
  size_t medium_top_use_count;                            //���������� ��� ������������� "��������" �����
  size_t medium_search_count;                             //���������� ������� ����� �������� �������
  size_t medium_search_steps_count;                       //���������� �������� ������ ����� �������� �������
  size_t medium_max_search_steps_count;                   //������������ ���������� �������� ������ ����� �������� �������
  size_t min_block_size;                                  //����������� ������ ����������� �����
  size_t max_block_size;                                  //������������ ������ ����������� �����
  size_t small_allocate_count [SMALL_BLOCK_POOL_SIZE];    //���������� ��������� ������ ������ �������
  size_t small_deallocate_count [SMALL_BLOCK_POOL_SIZE];  //���������� ������������ ������ ������ �������  
  size_t medium_allocate_count [MEDIUM_TREE_COUNT];       //���������� ��������� ������ �������� �������  
  size_t medium_deallocate_count [MEDIUM_TREE_COUNT];     //���������� ������������ ������ �������� �������    
  size_t medium_allocate_size [MEDIUM_TREE_COUNT];        //����� ������ ���������� ������ �������� �������
  size_t medium_deallocate_size [MEDIUM_TREE_COUNT];      //����� ������ ������������ ������� �������� �������  

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Init ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct AllocNode
{
  Heap*            heap;        //����, ������� ����������� ����
  size_t           hash;        //��� ����� ����
  size_t           ref_count;   //������� ������ �� ����  
  size_t           node_size;   //������ ���������� ��� ��� ����
  AllocNode*       parent;      //������������ ����
  AllocNode*       first_child; //������ �������� ����
  AllocNode*       prev;        //���������� ����
  AllocNode*       next;        //��������� ����
  AllocNode*       prev_hash;   //���������� ���� � ���-�������
  AllocNode*       next_hash;   //��������� ���� � ���-�������
  HeapInternalStat stat;        //���������� �������������
  HeapInternalStat total_stat;  //���������� ������������� ��� ������� ���� � ���� ���������
  char             name [1];    //����� ����� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
  void AddRef  () { ref_count++; }
  void Release ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////  
  bool SetParent (AllocNode*);
  bool Unlink    (); //�������� ����� � ��������� (return false ������ ��� parent == NULL)

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void GetSelfStatistics    (HeapStat&);
  void GetStatistics        (HeapStat&);
  void AccumulateStatistics (HeapInternalStat&);
  
  static void ComputeStatistics (HeapStat& result,const HeapInternalStat& stat,const Heap::Impl& heap_impl);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MediumBlockTree
{
  MediumBlock* root;           //������ ������ ��������� ������
  size_t       min_block_size; //����������� ������ ����� 
  size_t       max_block_size; //������������ ������ �����
  size_t       nodes_count;    //���������� �����
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////  
  MediumBlockTree ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ����� ������� �� ���� size
///////////////////////////////////////////////////////////////////////////////////////////////////
  MediumBlock* FindSmallestBlock (size_t size,AllocNode*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ������ �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void InsertBlock (MediumBlock*);
  void RemoveBlock (MediumBlock*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////  
  void RotateLeft  (MediumBlock*);
  void RotateRight (MediumBlock*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Heap::Impl
{
  MemPage*          first_page;              //������ ���������� ��������
  MemPage*          first_free_page;         //������ �������� �������
  MemPage*          last_free_page;          //��������� ��������� ��������
  unsigned char*    small_page_pos;          //��������� �� ��������� ��������� ��� ��������� ���� �� �������� ����� ������
  unsigned char*    small_page_end;          //��������� �� ����� �������� ����� ������
  unsigned char*    small_first_free [SMALL_BLOCK_POOL_SIZE]; //��������� �� ������ ��������� ����� ������ �������
  MediumBlock*      medium_top;              //"�������" ���� �� ��������� ���������� �������� ��� ������ �������� �������  
  MediumBlockTree   medium_tree [MEDIUM_TREE_COUNT]; //������ �������� ��������� ������
  size_t            max_heap_size;           //������������ ����� ������ ���������� �����
  size_t            small_granularity;       //������ �������� ��� ������ ������ �������
  size_t            medium_granularity;      //������ �������� ��� ������ �������� �������
  size_t            medium_max_block_size;   //������������ ���������� ������ ������ �������� �������
  size_t            min_reserve_size;        //������������� ����������� ������ �������  
  size_t            max_reserve_size;        //������������� ������������ ������ �������
  size_t            reserve_size;            //������� ������ �������
  size_t            sys_allocate_count;      //���������� ��������� ������ � ���������� ��������������
  size_t            sys_deallocate_count;    //���������� ������������ ������ ���������� ��������������
  size_t            sys_allocate_size;       //��������� ����� ������ ���������� � ���������� ��������������
  size_t            sys_deallocate_size;     //��������� ����� ������ ������������ ���������� ��������������
  ICustomAllocator* custom_allocator;        //���������������� �������������� ���������� ������
  FailHandler       fail_handler;            //���������������� ���������� ������ ��������� ������
  DebugHandler      debug_handler;           //���������������� ���������� ����������
  AllocNode*        node_hashtable [NODE_HASH_TABLE_SIZE]; //���-������� ����� �������������
  AllocNode*        cur_node;                //������� ���� �������������    
  AllocNode         default_node;            //���� ������������� �� ���������
  char              default_node_name_buf [sizeof (DEFAULT_NODE_NAME)]; //����� ��� ����� ���� �� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
  Impl  (Heap* owner,ICustomAllocator* allocator);
  ~Impl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////  
  MemPage* AllocPage (BlockTag tag,                  //��� ������� �����
                      size_t   min_size,             //����������� ����������� ������ 
                      size_t   recommended_size,     //������ ������������� � ��������� (>min_size)
                      size_t   align=ALIGN_SIZE,     //������������
                      size_t   offset=0);            //�������� �� ������������
  void FreePage       (MemPage*);  
  bool InsertFreePage (MemPage*); //return - �������� �� ���������� ������������� �������� 
  void RemoveFreePage (MemPage*);
  void TrimFreePages  (size_t limit);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////      
  void* AllocSmallBlock (size_t size,AllocNode*);
  void  FreeSmallBlock  (void*,AllocNode*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������� ������ �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////    
  void InsertMediumBlock (MediumBlock*);
  void RemoveMediumBlock (MediumBlock*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void* AllocMediumBlock (size_t size,AllocNode*);
  void  FreeMediumBlock  (void*,AllocNode*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void* AllocLargeBlock (size_t size,AllocNode*,size_t align=ALIGN_SIZE,size_t offset=0);
  void  FreeLargeBlock  (void*,AllocNode*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void InsertAllocNode (AllocNode*);
  void RemoveAllocNode (AllocNode*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Inlines
///////////////////////////////////////////////////////////////////////////////////////////////////
inline void AllocNode::Release ()
{
  if (--ref_count)
    return;

  Unlink ();  
  heap->impl->RemoveAllocNode (this);
  heap->Deallocate (this);
}

}
