#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    BatchStateBlock
===================================================================================================
*/

/*
    �������� ���������� ����� ��������� ������
*/

struct BatchStateBlock::Impl
{
  BatchingManager&      batching_manager;             //������ �� �������� ��������
  MaterialImpl&         material;                     //������ �� ��������
  LowLevelStateBlockPtr cached_state_block;           //��������� ����������
  size_t                cached_state_block_mask_hash; //��� ����� ��������� ����������

/// �����������
  Impl (BatchingManager& in_batching_manager, MaterialImpl& in_material)
    : batching_manager (in_batching_manager)
    , material (in_material)
    , cached_state_block_mask_hash ()
  {
  }
};

/*
    ����������� / ����������
*/

BatchStateBlock::BatchStateBlock (BatchingManager& batching_manager, MaterialImpl& material)
  : impl (new Impl (batching_manager, material))
{
  AttachCacheSource (batching_manager);
  AttachCacheSource (material);
}

BatchStateBlock::~BatchStateBlock ()
{
}

/*
    ���� ���������
*/

LowLevelStateBlockPtr BatchStateBlock::StateBlock ()
{
  UpdateCache ();

  return impl->cached_state_block;
}

/*
    ���������� ������������
*/

void BatchStateBlock::UpdateCacheCore ()
{
  try
  {
    BatchingManager&                   batching_manager = impl->batching_manager;
    render::manager::DeviceManager&    device_manager   = batching_manager.DeviceManager ();
    render::low_level::IDevice&        device           = device_manager.Device ();
    render::low_level::IDeviceContext& context          = device_manager.ImmediateContext ();

    impl->cached_state_block = LowLevelStateBlockPtr ();

    LowLevelStateBlockPtr material_state_block = impl->material.StateBlock ();
      
    render::low_level::StateBlockMask mask;

    if (material_state_block)
    {
      material_state_block->Apply (&context);
      material_state_block->GetMask (mask);
    }
    
      //��������� ���������/���������� ������
      
    mask.is_index_buffer       = true;
    mask.is_layout             = true;
    mask.is_vertex_buffers [0] = true;

    context.ISSetVertexBuffer (0, batching_manager.DynamicVertexBuffer ().LowLevelBuffer ().get ());
    context.ISSetInputLayout  (device_manager.InputLayoutManager ().DynamicPrimitivesInputLayout ().get ());
    context.ISSetIndexBuffer  (batching_manager.DynamicIndexBuffer ().LowLevelBuffer ().get ());
    
      //���������� ����� ��������� ���������
    
    size_t mask_hash = mask.Hash ();
    
    if (impl->cached_state_block_mask_hash != mask_hash)
    {
      impl->cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
      impl->cached_state_block_mask_hash = mask_hash;
    }

    impl->cached_state_block->Capture (&context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::BatchStateBlock::UpdateCacheCore");
    throw;
  }
}

void BatchStateBlock::ResetCacheCore ()
{
  impl->cached_state_block           = LowLevelStateBlockPtr ();
  impl->cached_state_block_mask_hash = 0;
}

/*
===================================================================================================
    BatchingManager
===================================================================================================
*/

/*
    �������� ���������� ��������� �������������
*/

namespace
{

/// ��� ��� ��������� ������������ ������ / ��������
template <class T> class Pool: public xtl::noncopyable
{
  public:
/// �����������
    Pool () : start (), pos (), end () {}

/// ���������
    T* Allocate (size_t count)
    {
      pos += count;

      if (pos <= end)
        return pos;

      pos -= count;

      return 0;
    }    

/// �����
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
  DeviceManagerPtr                      device_manager;       //�������� ���������� ���������
  render::manager::DynamicVertexBuffer  dynamic_vb;           //������������ ��������� �����
  render::manager::DynamicIndexBuffer   dynamic_ib;           //������������ ��������� �����
  DynamicVertexPool                     dynamic_vertex_pool;  //��� ������������ ������
  DynamicIndexPool                      dynamic_index_pool;   //��� ������������ ��������

/// �����������
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , dynamic_vb (render::low_level::UsageMode_Stream, render::low_level::BindFlag_VertexBuffer)
    , dynamic_ib (render::low_level::UsageMode_Stream, render::low_level::BindFlag_IndexBuffer)
  {
  }

/// �������������� ������������ �������
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
    ����������� / ����������
*/

BatchingManager::BatchingManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

BatchingManager::~BatchingManager ()
{
}

/*
    ������ �� �������� ����������
*/

render::manager::DeviceManager& BatchingManager::DeviceManager ()
{
  return *impl->device_manager;
}

/*
    �������������� ������� ��� ������������ ����������
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
    ������������ ������
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
    ��������� ������ � ��������
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
    ��������� ����� ��������� �� ���������
*/

BatchStateBlockPtr BatchingManager::GetStateBlock (MaterialImpl* material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
