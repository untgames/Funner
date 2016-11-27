#include <xtl/function.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выравнивание
///////////////////////////////////////////////////////////////////////////////////////////////////
#define ALIGN_SIZE 8

#pragma pack(8)

#ifdef _MSC_VER
#define ALIGN_STRUCT __declspec(align(ALIGN_SIZE))
#else
#define ALIGN_STRUCT __attribute__((aligned (ALIGN_SIZE)))
#endif


namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Основные константы
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t MIN_GRANULARITY               = 4096;             //минимальный размер памяти заказываемой у системы
const size_t SMALL_BLOCK_POOL_SIZE         = 256/ALIGN_SIZE;   //размер пула свободных блоков
const size_t MEDIUM_MIN_BLOCK_SIZE         = SMALL_BLOCK_POOL_SIZE*ALIGN_SIZE-1;//минимальный размер блока среднего размера
const size_t DEFAULT_SMALL_GRANULARITY     = MIN_GRANULARITY;  //размер страницы блоков малого размера (по умолчанию)
const size_t DEFAULT_MEDIUM_GRANULARITY    = MIN_GRANULARITY;  //минимальный размер страницы блоков малого размера (по умолчанию)
const size_t DEFAULT_MEDIUM_MAX_BLOCK_SIZE = 256*1024;         //максимальный размер блока среднего размера (по умолчанию)
const size_t DEFAULT_MIN_RESERVE_SIZE      = 1024*1024;        //рекомендуемый минимальный размер резерва (по умолчанию)
const size_t DEFAULT_MAX_RESERVE_SIZE      = 8192*1024;        //рекомендуемый максимальный размер резерва (по умолчанию)
const size_t NODE_HASH_TABLE_SIZE          = 256;              //размер хэш-таблицы узлов распределения
const char   DEFAULT_NODE_NAME []          = "default";        //имя узла распределния по умолчанию
const size_t MEDIUM_TREE_INDEX_SHIFT       = 8;                //смещение размера для получения индекса дерева
const size_t MEDIUM_TREE_COUNT             = 32;               //количество деревьев блоков среднего размера

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы маркера блока
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char BlockTag;

const BlockTag BLOCK_ID_MASK    = 3; //маска идентификатора типа распределителя блока
const BlockTag SMALL_BLOCK_ID   = 1; //идентификатор блока малого размера
const BlockTag MEDIUM_BLOCK_ID  = 2; //идентификатор блока среднего размера
const BlockTag LARGE_BLOCK_ID   = 3; //идентификатор блока большого размера
const BlockTag ALIGNED_BLOCK_ID = 4; //блок содержит информацию о выравнивании

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание страницы памяти, выделенной системным распределителем
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ALIGN_STRUCT MemPage
{
  void*    base;          //базовый адрес страницы
  size_t   size;          //размер страницы доступный пользователю
  size_t   reserved_size; //зарезервированный размер страницы
  BlockTag tag;           //блоки какого размера расположены на странице
  MemPage* prev;          //предыдущая страница
  MemPage* next;          //следующая страница
  MemPage* prev_free;     //предыдущая страница резерва
  MemPage* next_free;     //следующая страница резерва
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделенный блок среднего размера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ALIGN_STRUCT MediumAllocBlock
{
  size_t            size;       //размер блока
  MediumAllocBlock* prev;       //предыдущий блок (по расположению на странице)
  MediumAllocBlock* next;       //следующий блок (по расположению на странице)
  MemPage*          page;       //страница, которой принадлежит блок  
  bool              is_free;    //флаг, устанавливаемый если блок свободен
  char              filler [3]; //дополнение до границы двойного слова (в данном поле может быть размещен BlockTag)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет узла дерева
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TreeColor
{
  RED,
  BLACK
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Свободный блок среднего размера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ALIGN_STRUCT MediumBlock: public MediumAllocBlock
{
  MediumBlock* left;      //указатель на левое поддерево
  MediumBlock* right;     //указатель на правое поддерево  
  MediumBlock* parent;    //указатель на родительский узел
  MediumBlock* prev_free; //предыдущий свободный блок такого же размера
  MediumBlock* next_free; //следующий свободный блок такого же размера
  TreeColor    color;     //цвет узла дерева   
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блок большого размера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ALIGN_STRUCT LargeAllocBlock
{
  char filler [ALIGN_SIZE]; //дополнение до границы двойного слова (в данном поле может быть размещен BlockTag)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренняя статистика распределений отслеживаемая контекстом
///////////////////////////////////////////////////////////////////////////////////////////////////
struct HeapInternalStat
{
  size_t medium_top_use_count;                            //количество раз использования "верхнего" блока
  size_t medium_search_count;                             //количество поисков блока среднего размера
  size_t medium_search_steps_count;                       //количество итераций поиска блока среднего размера
  size_t medium_max_search_steps_count;                   //максимальное количество итераций поиска блока среднего размера
  size_t min_block_size;                                  //минимальный размер выделенного блока
  size_t max_block_size;                                  //максимальный размер выделенного блока
  size_t small_allocate_count [SMALL_BLOCK_POOL_SIZE];    //количество выделений блоков малого размера
  size_t small_deallocate_count [SMALL_BLOCK_POOL_SIZE];  //количество освобождений блоков малого размера  
  size_t medium_allocate_count [MEDIUM_TREE_COUNT];       //количество выделений блоков среднего размера  
  size_t medium_deallocate_count [MEDIUM_TREE_COUNT];     //количество освобождений блоков среднего размера    
  size_t medium_allocate_size [MEDIUM_TREE_COUNT];        //объём памяти заказанной блокам среднего размера
  size_t medium_deallocate_size [MEDIUM_TREE_COUNT];      //объём памяти освобождённой блоками среднего размера  

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация структуры
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Init ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации контекста распределения
///////////////////////////////////////////////////////////////////////////////////////////////////
struct AllocNode
{
  Heap*            heap;        //куча, которой принадлежит узел
  size_t           hash;        //хэш имени узла
  size_t           ref_count;   //счётчик ссылок на узел  
  size_t           node_size;   //память выделенная под сам узел
  AllocNode*       parent;      //родительский узел
  AllocNode*       first_child; //первый дочерний узел
  AllocNode*       prev;        //предыдущий узел
  AllocNode*       next;        //следующий узел
  HeapInternalStat stat;        //статистика распределений
  HeapInternalStat total_stat;  //статистика распределений для данного узла и всех вложенных
  char             name [1];    //буфер имени контекста

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок узла
///////////////////////////////////////////////////////////////////////////////////////////////////
  void AddRef  () { ref_count++; }
  void Release ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Расстановка связей между узлами
///////////////////////////////////////////////////////////////////////////////////////////////////  
  bool SetParent (AllocNode*);
  bool Unlink    (); //удаление связи с родителем (return false только для parent == NULL)

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение статистики
///////////////////////////////////////////////////////////////////////////////////////////////////
  void GetSelfStatistics    (HeapStat&);
  void GetStatistics        (HeapStat&);
  void AccumulateStatistics (HeapInternalStat&);
  
  static void ComputeStatistics (HeapStat& result,const HeapInternalStat& stat,const Heap::Impl& heap_impl);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дерево свободных блоков
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MediumBlockTree
{
  MediumBlock* root;           //корень дерева свободных блоков
  size_t       min_block_size; //минимальный размер блока 
  size_t       max_block_size; //максимальный размер блока
  size_t       nodes_count;    //количество узлов
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация / завершение
///////////////////////////////////////////////////////////////////////////////////////////////////  
  MediumBlockTree ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск первого блока рзмером не ниже size
///////////////////////////////////////////////////////////////////////////////////////////////////
  MediumBlock* FindSmallestBlock (size_t size,AllocNode*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление блоков из дерева
///////////////////////////////////////////////////////////////////////////////////////////////////
  void InsertBlock (MediumBlock*);
  void RemoveBlock (MediumBlock*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вращения
///////////////////////////////////////////////////////////////////////////////////////////////////  
  void RotateLeft  (MediumBlock*);
  void RotateRight (MediumBlock*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации кучи
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Heap::Impl
{
  MemPage*          first_page;              //первая выделенная страница
  MemPage*          first_free_page;         //первая страница резерва
  MemPage*          last_free_page;          //последняя свободная страница
  unsigned char*    small_page_pos;          //указатель на следующий доступный для выделения блок на странице малых блоков
  unsigned char*    small_page_end;          //указатель на конец страницы малых блоков
  unsigned char*    small_first_free [SMALL_BLOCK_POOL_SIZE]; //указатели на первые свободные блоки малого размера
  MediumBlock*      medium_top;              //"верхний" блок на последней заказанной странице для блоков среднего размера  
  MediumBlockTree   medium_tree [MEDIUM_TREE_COUNT]; //массив деревьев свободных блоков
  size_t            max_heap_size;           //максимальный объём памяти занимаемой кучей
  size_t            small_granularity;       //размер страницы для блоков малого размера
  size_t            medium_granularity;      //размер страницы для блоков среднего размера
  size_t            medium_max_block_size;   //максимальный допустимый размер блоков среднего размера
  size_t            min_reserve_size;        //рекомендуемый минимальный размер резерва  
  size_t            max_reserve_size;        //рекомендуемый максимальный размер резерва
  size_t            reserve_size;            //текущий размер резерва
  size_t            sys_allocate_count;      //количество выделений памяти у системного распределителя
  size_t            sys_deallocate_count;    //количество освобождений памяти системному распределителю
  size_t            sys_allocate_size;       //суммарный объём памяти выделенной у системного распределителя
  size_t            sys_deallocate_size;     //суммарный объём памяти освобождённой системному распределителю
  ICustomAllocator* custom_allocator;        //пользовательский распределитель страничной памяти
  FailHandler       fail_handler;            //пользовательский обработчик отказа выделения памяти
  DebugHandler      debug_handler;           //пользовательский отладочный обработчик
  AllocNode         default_node;            //узел распределения по умолчанию
  char              default_node_name_buf [sizeof (DEFAULT_NODE_NAME)]; //буфер для имени узла по умолчанию

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация / завершение
///////////////////////////////////////////////////////////////////////////////////////////////////    
  Impl  (Heap* owner,ICustomAllocator* allocator);
  ~Impl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Распределение страниц памяти
///////////////////////////////////////////////////////////////////////////////////////////////////  
  MemPage* AllocPage (BlockTag tag,                  //тэг размера блока
                      size_t   min_size,             //минимальный необходимый размер 
                      size_t   recommended_size);    //размер рекомендуемый к выделению (>min_size)
  void FreePage       (MemPage*);  
  bool InsertFreePage (MemPage*); //return - возможно ли дальнейшее использование страницы 
  void RemoveFreePage (MemPage*);
  void TrimFreePages  (size_t limit);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение блоков малого размера
///////////////////////////////////////////////////////////////////////////////////////////////////      
  void* AllocSmallBlock (size_t size);
  void  FreeSmallBlock  (void*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с деревями свободных блоков среднего размера
///////////////////////////////////////////////////////////////////////////////////////////////////    
  void InsertMediumBlock (MediumBlock*);
  void RemoveMediumBlock (MediumBlock*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение блоков среднего размера
///////////////////////////////////////////////////////////////////////////////////////////////////
  void* AllocMediumBlock (size_t size);
  void  FreeMediumBlock  (void*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение блоков большого размера
///////////////////////////////////////////////////////////////////////////////////////////////////
  void* AllocLargeBlock (size_t size);
  void  FreeLargeBlock  (void*);  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Inlines
///////////////////////////////////////////////////////////////////////////////////////////////////
inline void AllocNode::Release ()
{
  if (--ref_count)
    return;

  Unlink ();  

  heap->Deallocate (this);
}

}
