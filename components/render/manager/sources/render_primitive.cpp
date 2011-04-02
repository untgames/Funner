#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации примитива
*/

namespace
{

struct MeshPrimitive: public xtl::reference_counter, public CacheHolder
{
  render::low_level::PrimitiveType type;             //тип примитива
  VertexBufferPtr                  vertex_buffer;    //вершинный буфер
  LowLevelInputLayoutPtr           layout;           //лэйаут примитива
  size_t                           first;            //индекс первой вершины/индекса
  size_t                           count;            //количество примитивов
  MaterialProxy                    material;         //материал
  bool                             indexed;          //индексирован ли примитив
  RendererPrimitive                cached_primitive; //примитив закэшированный для рендеринга  
  
  MeshPrimitive (CacheHolder& parent_holder, const MaterialProxy& in_material, bool in_indexed)
    : type (PrimitiveType_PointList)
    , first (0)
    , count (0)    
    , material (in_material)
    , indexed (in_indexed)
  {
    parent_holder.AttachCacheSource (*this);
    
    material.AttachCacheHolder (*this);
  }
  
  void ResetCacheCore ()
  {   
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      memset (&cached_primitive, 0, sizeof (cached_primitive));
      
      cached_primitive.material_state_block = material.Resource ()->StateBlock ().get ();
      cached_primitive.indexed              = indexed;      
      cached_primitive.type                 = type;
      cached_primitive.first                = first;
      cached_primitive.count                = count;
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

struct Mesh: public xtl::reference_counter, public CacheHolder
{
  LowLevelBufferPtr      index_buffer;
  MeshPrimitiveArray     primitives;
  RendererPrimitiveArray cached_primitives;
  RendererPrimitiveGroup cached_group;
  
  Mesh (CacheHolder& parent_holder)
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
    MeshPtr mesh (new Mesh (*this), false);
    
      //конвертация вершинных буферов
             
    stl::vector<VertexBufferPtr> vertex_buffers;
    
    vertex_buffers.reserve (source.VertexBuffersCount ());
    
    for (size_t i=0, count=source.VertexBuffersCount (); i<count; i++)
      vertex_buffers.push_back (impl->buffers->CreateVertexBuffer (source.VertexBuffer (i), vb_usage));

      //конвертация индексного буфера (если он есть)
      
    InputDataType index_type = (InputDataType)0;
    
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
      bool                              is_indexed    = mesh->index_buffer;
      
      MeshPrimitivePtr dst_primitive (new MeshPrimitive (*mesh, impl->material_manager->GetMaterialProxy (src_primitive.material), is_indexed), false);

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
    
    Invalidate ();
    
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

  Invalidate ();  
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  impl->meshes.clear ();
  
  Invalidate ();  
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
