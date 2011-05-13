#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации примитива
*/

namespace
{

struct MeshCommonData
{
  DeviceManagerPtr   device_manager; //менеджер устройства отрисовки
  LowLevelBufferPtr  index_buffer;   //индексный буфер
  
  MeshCommonData (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("render::MeshCommonData::MeshCommonData", "device_manager");
  }
};

struct MeshPrimitive: public xtl::reference_counter, public CacheHolder
{
  MeshCommonData&                  common_data;                  //общие данные для примитива
  render::low_level::PrimitiveType type;                         //тип примитива
  VertexBufferPtr                  vertex_buffer;                //вершинный буфер
  LowLevelInputLayoutPtr           layout;                       //лэйаут примитива
  size_t                           first;                        //индекс первой вершины/индекса
  size_t                           count;                        //количество примитивов
  MaterialProxy                    material;                     //материал
  MaterialPtr                      cached_material;              //закэшированный материал
  RendererPrimitive                cached_primitive;             //примитив закэшированный для рендеринга
  size_t                           cached_state_block_mask_hash; //хэш маски закэшированного блока состояний
  LowLevelStateBlockPtr            cached_state_block;           //закэшированный блок состояний  
  
  MeshPrimitive (CacheHolder& parent_holder, const MaterialProxy& in_material, MeshCommonData& in_common_data)
    : common_data (in_common_data)
    , type (PrimitiveType_PointList)
    , first (0)
    , count (0)    
    , material (in_material)
    , cached_state_block_mask_hash (0)
  {
    parent_holder.AttachCacheSource (*this);
    
    material.AttachCacheHolder (*this);
    
    memset (&cached_primitive, 0, sizeof (cached_primitive));    
  }
  
  void ResetCacheCore ()
  {
    cached_material = MaterialPtr ();   
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      memset (&cached_primitive, 0, sizeof (cached_primitive));
      
      cached_material = material.Resource ();
      
      LowLevelStateBlockPtr material_state_block = cached_material ? cached_material->StateBlock ().get () : (render::low_level::IStateBlock*)0;
      
      render::low_level::IDevice& device = common_data.device_manager->Device ();
        
      render::low_level::StateBlockMask mask;      

      if (material_state_block)
      {
        material_state_block->Apply ();                
        material_state_block->GetMask (mask);
      }
        
      mask.is_index_buffer = true; //0 index buffer is need to be set
      mask.is_layout       = true;
      
      const LowLevelBufferPtr* streams = vertex_buffer->Streams ();

      for (size_t i=0, streams_count=vertex_buffer->StreamsCount (); i<streams_count; i++)
      {        
        device.ISSetVertexBuffer (i, streams [i].get ());
        
        mask.is_vertex_buffers [i] = true;
      }

      device.ISSetInputLayout  (layout.get ());
      device.ISSetIndexBuffer  (common_data.index_buffer.get ());
      
      size_t mask_hash = mask.Hash ();
      
      if (cached_state_block_mask_hash != mask_hash)
      {
        cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
        cached_state_block_mask_hash = mask_hash;
      }
      
      cached_state_block->Capture ();
      
      cached_primitive.material    = cached_material.get ();
      cached_primitive.state_block = cached_state_block.get ();
      cached_primitive.indexed     = common_data.index_buffer != LowLevelBufferPtr (); 
      cached_primitive.type        = type;
      cached_primitive.first       = first;
      cached_primitive.count       = count;
      cached_primitive.tags_count  = cached_material ? cached_material->TagsCount () : 0;
      cached_primitive.tags        = cached_material ? cached_material->Tags () : (const size_t*)0;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MeshPrimitive::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
};

typedef xtl::intrusive_ptr<MeshPrimitive> MeshPrimitivePtr;
typedef stl::vector<MeshPrimitivePtr>     MeshPrimitiveArray;
typedef stl::vector<RendererPrimitive>    RendererPrimitiveArray;

struct Mesh: public xtl::reference_counter, public MeshCommonData, public CacheHolder
{
  MeshPrimitiveArray     primitives;
  RendererPrimitiveArray cached_primitives;
  RendererPrimitiveGroup cached_group;
  
  Mesh (CacheHolder& parent_holder, const DeviceManagerPtr& device_manager)
    : MeshCommonData (device_manager)
  {
    parent_holder.AttachCacheSource (*this);
  }
  
  void ResetCacheCore ()
  {
    cached_primitives.clear ();   
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      cached_primitives.reserve (primitives.size ());
      
      for (MeshPrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
      {
        MeshPrimitive& src_primitive = **iter;
        
        src_primitive.UpdateCache ();
        
        cached_primitives.push_back (src_primitive.cached_primitive);
      }
      
      memset (&cached_group, 0, sizeof (cached_group));
      
      cached_group.primitives_count = cached_primitives.size ();
      
      if (!cached_primitives.empty ())
        cached_group.primitives = &cached_primitives [0];
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::Mesh::UpdateCacheCore");
      throw;
    }
  }  
  
  using CacheHolder::UpdateCache;    
};

typedef xtl::intrusive_ptr<Mesh>            MeshPtr;
typedef stl::vector<MeshPtr>                MeshArray;
typedef stl::vector<RendererPrimitiveGroup> RenderPrimitiveGroupsArray;

}

struct PrimitiveImpl::Impl
{
  DeviceManagerPtr           device_manager;   //менеджер устройства
  MaterialManagerPtr         material_manager; //менеджер материалов
  BuffersPtr                 buffers;          //буферы примитива
  MeshArray                  meshes;           //меши
  RenderPrimitiveGroupsArray render_groups;    //группы

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const MaterialManagerPtr& in_material_manager, const BuffersPtr& in_buffers)
    : device_manager (in_device_manager)
    , material_manager (in_material_manager)
    , buffers (in_buffers)
  {
    static const char* METHOD_NAME = "render::PrimitiveImpl::Impl::Impl";
    
    if (!device_manager)
      throw xtl::format_operation_exception (METHOD_NAME, "No DeviceManager binded");

    if (!buffers)
      throw xtl::make_null_argument_exception (METHOD_NAME, "buffers");
  }
};

/*
    Конструктор / деструктор
*/

PrimitiveImpl::PrimitiveImpl (const DeviceManagerPtr& device_manager, const MaterialManagerPtr& material_manager, const BuffersPtr& buffers)
{
  try
  {
    impl = new Impl (device_manager, material_manager, buffers);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveImpl::PrimitiveImpl");
    throw;
  }
}

PrimitiveImpl::~PrimitiveImpl ()
{
}

/*
    Буферы примитива
*/

const PrimitiveImpl::BuffersPtr& PrimitiveImpl::Buffers ()
{
  return impl->buffers;
}

/*
    Работа с мешами
*/

size_t PrimitiveImpl::MeshesCount ()
{
  return impl->meshes.size ();
}

size_t PrimitiveImpl::AddMesh (const media::geometry::Mesh& source, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  try
  {
    MeshPtr mesh (new Mesh (*this, impl->device_manager), false);
    
      //конвертация вершинных буферов
             
    stl::vector<VertexBufferPtr> vertex_buffers;
    
    vertex_buffers.reserve (source.VertexBuffersCount ());
    
    for (size_t i=0, count=source.VertexBuffersCount (); i<count; i++)
      vertex_buffers.push_back (impl->buffers->CreateVertexBuffer (source.VertexBuffer (i), vb_usage));

      //конвертация индексного буфера (если он есть)
      
    InputDataType index_type = InputDataType_UShort;
    
    if (source.IndexBuffer ().Size ())
    {
      mesh->index_buffer = impl->buffers->CreateIndexBuffer (source.IndexBuffer (), ib_usage);
      
      switch (source.IndexBuffer ().DataType ())
      {
        case media::geometry::IndexType_UInt32:
          index_type = InputDataType_UInt;
          break;
        case media::geometry::IndexType_UInt16:
          index_type = InputDataType_UShort;
          break;
        case media::geometry::IndexType_UInt8:
          index_type = InputDataType_UByte;
          break;          
        default:
          throw xtl::format_operation_exception ("", "Unexpected index buffer data type %s", get_index_type_name (source.IndexBuffer ().DataType ()));
      }
    }

      //конвертация примитивов
    
    mesh->primitives.reserve (source.PrimitivesCount ());
    
    for (size_t i=0, count=source.PrimitivesCount (); i<count; i++)
    {
      const media::geometry::Primitive& src_primitive = source.Primitive (i);
      
      MeshPrimitivePtr dst_primitive (new MeshPrimitive (*mesh, impl->material_manager->GetMaterialProxy (src_primitive.material), *mesh), false);

      switch (src_primitive.type)
      {
        case media::geometry::PrimitiveType_LineList:
          dst_primitive->type  = PrimitiveType_LineList;
          dst_primitive->count = src_primitive.count * 2;
          break;
        case media::geometry::PrimitiveType_LineStrip:
          dst_primitive->type  = PrimitiveType_LineStrip;
          dst_primitive->count = src_primitive.count + 1;
          break;
        case media::geometry::PrimitiveType_TriangleList:
          dst_primitive->type  = PrimitiveType_TriangleList;
          dst_primitive->count = src_primitive.count * 3;
          break;
        case media::geometry::PrimitiveType_TriangleStrip:
          dst_primitive->type  = PrimitiveType_TriangleStrip;
          dst_primitive->count = src_primitive.count + 2;
          break;
        case media::geometry::PrimitiveType_TriangleFan:
          dst_primitive->type  = PrimitiveType_TriangleFan;
          dst_primitive->count = src_primitive.count + 2;
          break;
        default:
          throw xtl::format_operation_exception ("", "Bad primitive #%u type %s", i, get_type_name (src_primitive.type));
      }

      if (src_primitive.vertex_buffer >= vertex_buffers.size ())
        throw xtl::format_operation_exception ("", "Bad primitive #%u vertex buffer index %u (vertex buffers count is %u)", i, src_primitive.vertex_buffer,
          vertex_buffers.size ());          

      dst_primitive->vertex_buffer = vertex_buffers [src_primitive.vertex_buffer];
      dst_primitive->first         = src_primitive.first;
      dst_primitive->layout        = dst_primitive->vertex_buffer->CreateInputLayout (impl->device_manager->InputLayoutManager (), index_type);
      
      mesh->primitives.push_back (dst_primitive);
    }
    
      //добавление меша      
      
    impl->meshes.push_back (mesh);
    
    ResetCache ();
    
    return impl->meshes.size () - 1;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveImpl::AddMesh");
    throw;
  }
}

void PrimitiveImpl::RemoveMeshes (size_t first_mesh, size_t meshes_count)
{
  if (first_mesh >= impl->meshes.size ())
    return;
    
  if (first_mesh + meshes_count > impl->meshes.size ())
    meshes_count = impl->meshes.size () - first_mesh;        

  impl->meshes.erase (impl->meshes.begin () + first_mesh, impl->meshes.begin () + first_mesh + meshes_count);

  ResetCache ();  
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  impl->meshes.clear ();
  
  ResetCache ();  
}

/*
    Работа с линиями
*/

size_t PrimitiveImpl::LinesCount ()
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::LinesCount");
}

size_t PrimitiveImpl::AddLines (size_t lines_count, const Line* lines, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::AddLines");
}

void PrimitiveImpl::UpdateLines (size_t first_lines, size_t lines_count, const Line* Lines)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::UpdateLines");
}

void PrimitiveImpl::SetLinesMaterial (size_t first_lines, size_t lines_count, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::SetLinesMaterial");
}

void PrimitiveImpl::RemoveLines (size_t first_lines, size_t lines_count)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::ReserveLines");
}

void PrimitiveImpl::RemoveAllLines ()
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::ReserveAllLines");
}

void PrimitiveImpl::ReserveLines (size_t lines_count)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::ReserveLines");
}

/*
    Работа со спрайтами
*/

size_t PrimitiveImpl::SpritesCount ()
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::SpritesCount");
}

size_t PrimitiveImpl::AddSprites (size_t sprites_count, const Sprite* sprites, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::AddSprites");
}

void PrimitiveImpl::UpdateSprites (size_t first_sprite, size_t sprites_count, const Sprite* sprites)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::UpdateSprites");
}

void PrimitiveImpl::SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::SetSpritesMaterial");
}

void PrimitiveImpl::RemoveSprites (size_t first_sprite, size_t sprites_count)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::RemoveSprites");
}

void PrimitiveImpl::RemoveAllSprites ()
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::RemoveAllSprites");
}

void PrimitiveImpl::ReserveSprites (size_t sprites_count)
{
  throw xtl::make_not_implemented_exception ("render::PrimitiveImpl::ReserveSprites");
}

/*
    Обновление кэша примитива
*/

void PrimitiveImpl::UpdateCacheCore ()
{
  try
  {
    impl->render_groups.reserve (impl->meshes.size ());
    
    for (MeshArray::iterator iter=impl->meshes.begin (), end=impl->meshes.end (); iter!=end; ++iter)
    {
      Mesh& mesh = **iter;
      
      mesh.UpdateCache ();
      
      impl->render_groups.push_back (mesh.cached_group);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveImpl::UpdateCacheCore");
    throw;
  }
}

void PrimitiveImpl::ResetCacheCore ()
{
  impl->render_groups.clear ();
}

/*
    Группы римитивов рендеринга
*/

size_t PrimitiveImpl::RendererPrimitiveGroupsCount ()
{
  UpdateCache ();
  
  return impl->render_groups.size ();
}

RendererPrimitiveGroup* PrimitiveImpl::RendererPrimitiveGroups ()
{
  UpdateCache ();
  
  if (impl->render_groups.empty ())
    return 0;
  
  return &impl->render_groups [0];
}
