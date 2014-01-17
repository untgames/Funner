#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    BatchStateBlock
===================================================================================================
*/

/*
    Описание реализации блока состояний пакета
*/

struct BatchStateBlock::Impl
{
};

/*
    Конструктор / деструктор
*/

BatchStateBlock::BatchStateBlock (BatchingManager& batching_manager, MaterialImpl& material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

BatchStateBlock::~BatchStateBlock ()
{
}

/*
    Блок состояний
*/

LowLevelStateBlockPtr BatchStateBlock::StateBlock ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление кэшированием
*/

void BatchStateBlock::UpdateCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchStateBlock::ResetCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    BatchingManager
===================================================================================================
*/

/*
    Описание реализации менеджера пакетирования
*/

namespace
{

/// Пул для выделения динамических вершин / индексов
template <class T> class Pool: public xtl::noncopyable
{
  public:
/// Конструктор
    Pool () : start (), pos (), end () {}

/// Выделение
    T* Allocate (size_t count)
    {
      pos += count;

      if (pos <= end)
        return pos;

      pos -= count;

      return 0;
    }    

/// Сброс
    void Reset (T* new_start, size_t size, bool save_relative_position)
    {
      size_t offset = pos - start;

      start = new_start;
      end   = new_start + size;
      pos   = save_relative_position ? start + offset : start;
    }

  private:
    T* start;
    T* pos;
    T* end;  
};

typedef Pool<DynamicPrimitiveVertex> DynamicVertexPool;
typedef Pool<DynamicPrimitiveIndex>  DynamicIndexPool;

}

struct BatchingManager::Impl
{
  DeviceManagerPtr                      device_manager;       //менеджер устройства отрисовки
  render::manager::DynamicVertexBuffer  dynamic_vb;           //динамический вершинный буфер
  render::manager::DynamicIndexBuffer   dynamic_ib;           //динамический индексный буфер
  DynamicVertexPool                     dynamic_vertex_pool;  //пул динамических вершин
  DynamicIndexPool                      dynamic_index_pool;   //пул динамических индексов

/// Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , dynamic_vb (render::low_level::UsageMode_Stream, render::low_level::BindFlag_VertexBuffer)
    , dynamic_ib (render::low_level::UsageMode_Stream, render::low_level::BindFlag_IndexBuffer)
  {
  }

/// Резервирование динамических буферов
  void ReserveDynamicBuffers (size_t vertices_count, size_t indices_count)
  {
    try
    {
      dynamic_vb.Reserve (vertices_count);
      dynamic_ib.Reserve (indices_count);

      dynamic_vb.SyncBuffers (device_manager->Device ());
      dynamic_ib.SyncBuffers (device_manager->Device ());

      dynamic_vertex_pool.Reset (dynamic_vb.Data (), vertices_count, true);
      dynamic_index_pool.Reset (dynamic_ib.Data (), indices_count, true);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::BatchingManager::ReserveDynamicBuffers");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

BatchingManager::BatchingManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

BatchingManager::~BatchingManager ()
{
}

/*
    Ссылка на менеджер устройства
*/

render::manager::DeviceManager& BatchingManager::DeviceManager ()
{
  return *impl->device_manager;
}

/*
    Резервирование буферов для динамических примитивов
*/

void BatchingManager::ReserveDynamicBuffers (size_t vertices_count, size_t indices_count)
{
  impl->ReserveDynamicBuffers (vertices_count, indices_count);
}

size_t BatchingManager::DynamicVerticesCount () const
{
  return impl->dynamic_vb.Capacity ();
}

size_t BatchingManager::DynamicIndicesCount () const
{
  return impl->dynamic_ib.Capacity ();
}

/*
    Динамические буферы
*/

render::manager::DynamicVertexBuffer& BatchingManager::DynamicVertexBuffer ()
{
  return impl->dynamic_vb;
}

render::manager::DynamicIndexBuffer& BatchingManager::DynamicIndexBuffer ()
{
  return impl->dynamic_ib;
}

/*
    Выделение вершин и индексов
*/

DynamicPrimitiveVertex* BatchingManager::AllocateDynamicVertices (size_t count, size_t* out_base_vertex_index)
{
  DynamicPrimitiveVertex* result = impl->dynamic_vertex_pool.Allocate (count);

  if (result)
  {
    if (out_base_vertex_index)
      *out_base_vertex_index = result - impl->dynamic_vb.Data ();

    return result;
  }

  impl->dynamic_vb.Reserve (impl->dynamic_vb.Capacity () * 2);

  impl->dynamic_vertex_pool.Reset (impl->dynamic_vb.Data (), impl->dynamic_vb.Size (), true);

  result = impl->dynamic_vertex_pool.Allocate (count);

  if (!result)
    return 0;

  if (out_base_vertex_index)
    *out_base_vertex_index = result - impl->dynamic_vb.Data ();

  return result;
}

DynamicPrimitiveIndex* BatchingManager::AllocateDynamicIndices (size_t count)
{
  DynamicPrimitiveIndex* result = impl->dynamic_index_pool.Allocate (count);

  if (result)
    return result;

  impl->dynamic_ib.Reserve (impl->dynamic_ib.Capacity () * 2);

  impl->dynamic_index_pool.Reset (impl->dynamic_ib.Data (), impl->dynamic_ib.Size (), true);

  result = impl->dynamic_index_pool.Allocate (count);

  if (!result)
    return 0;

  return result;
}

void BatchingManager::ResetDynamicBuffers ()
{
  impl->dynamic_vb.Clear ();
  impl->dynamic_ib.Clear ();

  impl->dynamic_vertex_pool.Reset (impl->dynamic_vb.Data (), impl->dynamic_vb.Size (), false);
  impl->dynamic_index_pool.Reset (impl->dynamic_ib.Data (), impl->dynamic_ib.Size (), false);
}

/*
    Получение блока состояний по материалу
*/

BatchStateBlockPtr BatchingManager::GetStateBlock (MaterialImpl* material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
