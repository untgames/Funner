#include "shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t RESERVED_BUFFER_POOL_SIZE = 256;

/*
    Вспомогательные структуры
*/

typedef stl::vector<LowLevelBufferPtr> BufferArray;

///Пул буферов свойств
struct BufferPool: public xtl::reference_counter
{
  BufferArray                   buffers;               //все доступные буферы
  size_t                        first_available_index; //индекс первого доступого буфера
  render::low_level::BufferDesc buffer_desc;           //описатель буфера

///Конструктор
  BufferPool (size_t buffer_size) : first_available_index (0)
  {
    buffers.reserve (RESERVED_BUFFER_POOL_SIZE);
    
    memset (&buffer_desc, 0, sizeof (buffer_desc));
    
    buffer_desc.size         = buffer_size;
    buffer_desc.usage_mode   = render::low_level::UsageMode_Default;
    buffer_desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
    buffer_desc.access_flags = render::low_level::AccessFlag_ReadWrite;    
  }
  
///Сброс
  void Flush ()
  {
    first_available_index = 0;
  }
  
///Выделение буфера
  LowLevelBufferPtr Allocate (render::low_level::IDevice& device)
  {
    if (first_available_index < buffers.size ())
      return buffers [first_available_index++];
      
    LowLevelBufferPtr result (device.CreateBuffer (buffer_desc), false);
    
    buffers.push_back (result);
    
    first_available_index++;
    
    return result;
  }
};

typedef xtl::intrusive_ptr<BufferPool> BufferPoolPtr;

}

/*
    Описание реализации кэша
*/

typedef stl::map<size_t, BufferPoolPtr> PoolMap;

struct PropertyCache::Impl
{
  DeviceManagerPtr           device_manager;   //менеджер устройства отрисовки
  PoolMap                    pools;            //пулы буферов свойств
  size_t                     last_hash;        //последний запрошенный хэш
  size_t                     last_buffer_size; //последний запрошенный размер буфера
  ProgramParametersLayoutPtr last_layout;      //последний запрошенный лэйаут
  BufferPool*                last_pool;        //последний запрошенный пул
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , last_hash (0)
    , last_buffer_size (0)
    , last_pool (0)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("render::PropertyCache::Impl::Impl", "device_manager");          
  }
};

/*
    Конструктор / деструктор
*/

PropertyCache::PropertyCache (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
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
    
    size_t hash        = layout.Hash (),
           buffer_size = layout.BufferSize ();
           
      //поиск лэйаута
      
    if (impl->last_hash != hash)
    {
      impl->last_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_FrameEntity, layout);
      impl->last_hash   = hash;
    }    
    
      //поиск буфера
      
    if (impl->last_buffer_size != buffer_size)
    {
      PoolMap::iterator iter = impl->pools.upper_bound (buffer_size);
      
      if (iter != impl->pools.end ())
      {
        impl->last_pool = &*iter->second;
      }
      else
      {
        stl::pair<PoolMap::iterator, bool> p = impl->pools.insert_pair (buffer_size, BufferPoolPtr (new BufferPool (buffer_size), false));

        impl->last_pool = &*p.first->second;
      }

      impl->last_buffer_size = buffer_size;      
    }
    
    result_buffer = impl->last_pool->Allocate (impl->device_manager->Device ());
    result_layout = impl->last_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PropertyCache::Convert");
    
    throw;
  }
}

/*
    Сброс кэша
*/

void PropertyCache::Flush ()
{
  for (PoolMap::iterator iter=impl->pools.begin (), end=impl->pools.end (); iter!=end; ++iter)
    iter->second->Flush ();
    
  impl->last_hash        = 0;
  impl->last_buffer_size = 0;
  impl->last_pool        = 0;
  impl->last_layout      = ProgramParametersLayoutPtr ();
}
