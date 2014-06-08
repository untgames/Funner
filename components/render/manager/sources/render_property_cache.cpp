#include "shared.h"

using namespace render::manager;

namespace
{

/*
    Константы
*/

const size_t RESERVED_BUFFER_POOL_SIZE = 256;
const size_t MIN_BUFER_SIZE            = 64;

/*
    Вспомогательные структуры
*/

///Элемент пула буферов
struct BufferPoolEntry
{
  LowLevelBufferPtr buffer;          //буфер данных
  FrameTime         last_use_time;   //последнее время использования
  FrameId           last_use_frame;  //последний кадр использования
};

typedef stl::vector<BufferPoolEntry> BufferArray;

///Пул буферов свойств
struct BufferPool: public xtl::reference_counter, private Cache
{
  BufferArray                   buffers;               //все доступные буферы
  size_t                        first_available_index; //индекс первого доступого буфера
  render::low_level::BufferDesc buffer_desc;           //описатель буфера
  SettingsPtr                   settings;              //настройки
  Log                           log;                   //поток отладочного вывода

///Конструктор
  BufferPool (size_t buffer_size, const DeviceManagerPtr& device_manager)
    : Cache (&device_manager->CacheManager ())
    , first_available_index (0)
    , settings (&device_manager->Settings ())
  {
    if (!settings)
      throw xtl::make_null_argument_exception ("render::manager::BufferPool::BufferPool", "settings");
    
    buffers.reserve (RESERVED_BUFFER_POOL_SIZE);
    
    memset (&buffer_desc, 0, sizeof (buffer_desc));
    
    buffer_desc.size         = buffer_size;
    buffer_desc.usage_mode   = render::low_level::UsageMode_Default;
    buffer_desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
    buffer_desc.access_flags = render::low_level::AccessFlag_ReadWrite;
    
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer pool created for buffer size %u", buffer_size);
  }
  
///Деструктор
  ~BufferPool ()
  {
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer pool destroyed for buffer size %u", buffer_desc.size);
  }
  
///Сброс
  void Reset ()
  {
    first_available_index = 0;
  }
  
///Выделение буфера
  LowLevelBufferPtr Allocate (render::low_level::IDevice& device)
  {
    if (first_available_index < buffers.size ())
    {
      BufferPoolEntry& entry = buffers [first_available_index++];

      entry.last_use_time  = CurrentTime ();
      entry.last_use_frame = CurrentFrame ();

      return entry.buffer;
    }
      
    LowLevelBufferPtr result (device.CreateBuffer (buffer_desc), false);
    
    BufferPoolEntry entry;
    
    entry.buffer         = result;
    entry.last_use_time  = CurrentTime ();
    entry.last_use_frame = CurrentFrame ();
    
    buffers.push_back (entry);
    
    first_available_index++;
    
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer allocated with size %u", buffer_desc.size);
    
    return result;
  }
  
///Сброс закэшированных элементов
  void FlushCache ()
  {
    if (buffers.empty ())
      return;
    
    FrameTime current_time  = CurrentTime (), time_delay = TimeDelay ();
    FrameId   current_frame = CurrentFrame (), frame_delay = FrameDelay ();

    for (BufferArray::iterator iter=buffers.end ()-1, end=buffers.begin ()-1; iter!=end; --iter)
      if (current_time - iter->last_use_time <= time_delay && current_frame - iter->last_use_frame <= frame_delay)
      {
        size_t buffers_count = buffers.end () - iter - 1;
        
        if (buffers_count)
        {
          if (settings->HasDebugLog ())
            log.Printf ("Property cache %u buffers destroyed with size %u", buffers_count, buffer_desc.size);

          buffers.erase (iter+1, buffers.end ());
        }

        return;
      }
  }
};

typedef xtl::intrusive_ptr<BufferPool> BufferPoolPtr;

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k) 
{
  if (!k)
    return 1;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
    k |= k >> i;

  return k + 1;
}

}

/*
    Описание реализации кэша
*/

typedef CacheMap<size_t, BufferPoolPtr> PoolMap;

struct PropertyCache::Impl
{
  DeviceManagerPtr           device_manager;   //менеджер устройства отрисовки
  PoolMap                    pools;            //пулы буферов свойств
  size_t                     last_hash;        //последний запрошенный хэш
  size_t                     last_buffer_size; //последний запрошенный размер буфера
  ProgramParametersLayoutPtr last_layout;      //последний запрошенный лэйаут
  BufferPoolPtr              last_pool;        //последний запрошенный пул
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , pools (&device_manager->CacheManager ())
    , last_hash (0)
    , last_buffer_size (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

PropertyCache::PropertyCache (const DeviceManagerPtr& device_manager)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    impl = new Impl (device_manager); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyCache::PropertyCache");
    throw;
  }
}

PropertyCache::~PropertyCache ()
{
}

/*
    Получение объекта параметров
*/

void PropertyCache::Convert (const common::PropertyMap& source_map, LowLevelBufferPtr& result_buffer, ProgramParametersLayoutPtr& result_layout)
{
  try
  {
    const common::PropertyLayout& layout = source_map.Layout ();
    
    size_t hash                  = layout.Hash (),
           buffer_size           = layout.BufferSize (),
           corrected_buffer_size = stl::max (get_next_higher_power_of_two (buffer_size), MIN_BUFER_SIZE);
           
      //поиск лэйаута
      
    if (impl->last_hash != hash)
    {
      impl->last_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_FrameEntity, layout, true);
      impl->last_hash   = hash;
    }    
    
      //поиск пула буферов
      
    if (impl->last_buffer_size != corrected_buffer_size)
    {
      if (BufferPoolPtr* pool = impl->pools.Find (corrected_buffer_size))
      {
        impl->last_pool = *pool;
      }
      else
      {
        BufferPoolPtr new_pool (new BufferPool (corrected_buffer_size, impl->device_manager), false);
        
        impl->pools.Add (corrected_buffer_size, new_pool);

        impl->last_pool = &*new_pool;
      }

      impl->last_buffer_size = corrected_buffer_size;
    }
    
      //выделение буфера      

    LowLevelBufferPtr buffer = impl->last_pool->Allocate (impl->device_manager->Device ());

      //установка данных

    buffer->SetData (0, buffer_size, source_map.BufferData ());

      //сохранение результата
    
    result_buffer = buffer;
    result_layout = impl->last_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyCache::Convert");
    
    throw;
  }
}

/*
    Сброс кэша
*/

namespace
{

struct PoolReseter
{
  void operator () (size_t, const BufferPoolPtr& pool) const
  {
    pool->Reset ();
  }
};

}

void PropertyCache::Reset ()
{
  impl->pools.ForEach (PoolReseter ());
    
  impl->last_hash        = 0;
  impl->last_buffer_size = 0;
  impl->last_pool        = BufferPoolPtr ();
  impl->last_layout      = ProgramParametersLayoutPtr ();
}
