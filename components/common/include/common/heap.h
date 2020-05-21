#ifndef COMMONLIB_HEAP_HEADER
#define COMMONLIB_HEAP_HEADER

#include <cstddef>
#include <new>

#include <xtl/functional_fwd>

namespace common
{

//implementation forwards
class  Heap;
struct AllocNode;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Статистика менеджера памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
struct HeapStat
{
  struct Range {                        //интервал блоков
    size_t min_size;                    //минимальный размер блоков в интервале (не меньше)
    size_t max_size;                    //максимальный размер блоков в интервале (не больше)
    size_t allocate_count;              //количество выделений
    size_t deallocate_count;            //количество освобождений
    size_t allocate_size;               //объём заказанной памяти
    size_t deallocate_size;             //объём освобождённой памяти
  };

  enum { MAX_RANGES = 64 };             //максимальное количество интервалов

  size_t sys_allocate_count;            //количество заказов памяти у системного распределителя
  size_t sys_deallocate_count;          //количество освобождений памяти системному распределителю
  size_t sys_allocate_size;             //объём памяти заказанной у системного распределителя
  size_t sys_deallocate_size;           //объём памяти возвращённой системному распределителю
  size_t allocate_count;                //количество заказов памяти
  size_t deallocate_count;              //количество освобождений памяти
  size_t allocate_size;                 //объём заказанной памяти
  size_t deallocate_size;               //объём освобождённой памяти
  size_t medium_search_count;           //количество поисков блоков среднего размера
  size_t medium_search_steps_count;     //общее количество итераций поиска блока среднего размера
  size_t medium_max_search_steps_count; //максимальное количество итераций поиска блока среднего размера
  size_t medium_top_use_count;          //количество использований "верхнего" блока вместо найденного
  size_t min_block_size;                //минимальный размер заказанного блока (за всё время работы)
  size_t max_block_size;                //максимальный размер заказанного блока (за всё время работы)
  size_t reserve_size;                  //размеры резерва
  size_t ranges_count;                  //количество интервалов блоков
  Range  ranges [MAX_RANGES];           //интервалы блоков
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение возникающее в случае невозможности создания контекста распределения
///////////////////////////////////////////////////////////////////////////////////////////////////
class AllocationContextException: public std::exception
{
  public:
    const char* what () const throw ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст распределения
///////////////////////////////////////////////////////////////////////////////////////////////////
class AllocationContext
{
  friend class Heap;
  public:
    AllocationContext  (const AllocationContext&);
    ~AllocationContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
   AllocationContext& operator = (const AllocationContext&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
   const char* GetName () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Куча связанная с контекстом
///////////////////////////////////////////////////////////////////////////////////////////////////
   Heap& GetHeap () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Расстановка связей между контекстами
///////////////////////////////////////////////////////////////////////////////////////////////////
   bool              SetParent (const AllocationContext&);
   AllocationContext GetParent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение статистики
///////////////////////////////////////////////////////////////////////////////////////////////////
   void GetStatistics     (HeapStat&) const; //добавить кэширование!!
   void GetSelfStatistics (HeapStat&) const;

  private:
    AllocationContext (AllocNode*);

  private:
    AllocNode* node;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс пользовательского распределителя памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomAllocator
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение / освобождение
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* Allocate   (size_t block_size) = 0;
    virtual void  Deallocate (void*) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ICustomAllocator () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Куча
///////////////////////////////////////////////////////////////////////////////////////////////////
class Heap
{
  friend struct AllocNode;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы и деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Heap  ();
    Heap  (ICustomAllocator* allocator);
//    Heap  (void* buffer,size_t buffer_size);
    ~Heap ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение / освобождение блока памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Allocate    (size_t size);
    void* Allocate    (size_t size,size_t align,size_t offset=0);
    void* TryAllocate (size_t size);
    void* TryAllocate (size_t size,size_t align,size_t offset=0);
    void  Deallocate  (void* ptr);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер выделенного блока
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size (void* ptr) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка минимального размера страницы размещаемой системным распределителем
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetGranularity       (size_t small_granularity,size_t medium_granularity);
    size_t GetSmallGranularity  () const; //для блоков размером < 256 байт
    size_t GetMediumGranularity () const; //для блоков размером >= 256 байт

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление резервом свободных страниц (зарезервированных, но не используемых)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetReserveSize    (size_t min_reserve_size,size_t max_reserve_size);
    size_t GetMinReserveSize () const;
    size_t GetMaxReserveSize () const;
    void   TrimReserve       (size_t min_reserve_size,size_t max_reserve_size);
    void   TrimReserve       (); //используются GetMinReserveSize/GetMaxReserveSize
    void   FlushReserve      (); //освобождение всех неиспользуемых страниц

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка пользовательского обработчика ситуации отстутствия заказываемой памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<bool (size_t need_size)> FailHandler;

    void               SetFailHandler (const FailHandler&);
    const FailHandler& GetFailHandler () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка пользовательского отладочного обработчика
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char*)> DebugHandler;

    void                SetDebugHandler (const DebugHandler&);
    const DebugHandler& GetDebugHandler () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с контекстами распределения
///////////////////////////////////////////////////////////////////////////////////////////////////
    AllocationContext GetDefaultContext () const; //контекст распределения по умолчанию

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение статистики
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetStatistics (HeapStat&);

  private:
    Heap (const Heap&);
    void operator = (const Heap&);

    void Init (ICustomAllocator*);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Распределитель памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemoryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка того, инициализирован ли менеджер
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsInitialized ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Врапперы для упрощения выделения памяти из основного менеджера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void* Allocate   (size_t size);
    static void* Allocate   (size_t size, size_t align, size_t offset = 0);
    static void  Deallocate (void* p);
};

}

#endif
