#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

//TODO: расчет вершинного буфера для спрайтов для каждого кадра
//TODO: реакция отдельных спрайтов на события материала

/*
    Описание реализации примитива
*/

namespace
{

///Общие данные мешей
struct MeshCommonData
{
  DeviceManagerPtr   device_manager; //менеджер устройства отрисовки
  LowLevelBufferPtr  index_buffer;   //индексный буфер
  stl::string        name;           //имя мэша  
  
  MeshCommonData (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("render::manager::MeshCommonData::MeshCommonData", "device_manager");
  }
};

///Примитив меша
struct MeshPrimitive: public xtl::reference_counter, public CacheHolder, public DebugIdHolder
{
  MeshCommonData&                  common_data;                  //общие данные для примитива
  size_t                           primitive_index;              //индекс примитива в меше
  render::low_level::PrimitiveType type;                         //тип примитива
  VertexBufferPtr                  vertex_buffer;                //вершинный буфер
  LowLevelInputLayoutPtr           layout;                       //лэйаут примитива
  size_t                           first;                        //индекс первой вершины/индекса
  size_t                           count;                        //количество примитивов
  size_t                           base_vertex;                  //индекс базовой вершины
  MaterialProxy                    material;                     //материал
  Log                              log;                          //поток протоколирования
  MaterialPtr                      cached_material;              //закэшированный материал
  RendererPrimitive                cached_primitive;             //примитив закэшированный для рендеринга
  size_t                           cached_state_block_mask_hash; //хэш маски закэшированного блока состояний
  LowLevelStateBlockPtr            cached_state_block;           //закэшированный блок состояний  
  
///Конструктор
  MeshPrimitive (CacheHolder& parent_holder, size_t in_primitive_index, const MaterialProxy& in_material, MeshCommonData& in_common_data)
    : common_data (in_common_data)
    , primitive_index (in_primitive_index)
    , type (PrimitiveType_PointList)
    , first (0)
    , count (0)    
    , base_vertex (0)
    , material (in_material)
    , cached_state_block_mask_hash (0)
  {
    parent_holder.AttachCacheSource (*this);
    
    material.AttachCacheHolder (*this);
    
    memset (&cached_primitive, 0, sizeof (cached_primitive));    
    
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' primitive #%u created (id=%u)", common_data.name.c_str (), primitive_index, Id ());
  }
  
///Деструктор
  ~MeshPrimitive ()
  {
      //предварительная очистка коллекция для возможности отслеживать порядок удаления объектов до и после удаления данного    
    
    cached_material              = MaterialPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
    
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' primitive #%u destroyed (id=%u)", common_data.name.c_str (), primitive_index, Id ());    
  }
  
///Сброс кэша
  void ResetCacheCore ()
  {
    if (common_data.device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset mesh '%s' primitive #%u cache (id=%u)", common_data.name.c_str (), primitive_index, Id ());        
    
    cached_material              = MaterialPtr ();   
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
  }
  
///Обновление кэша
  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = common_data.device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update mesh '%s' primitive #%u cache (id=%u)", common_data.name.c_str (), primitive_index, Id ());
      
      memset (&cached_primitive, 0, sizeof (cached_primitive));
      
        //запрос состояния материала
      
      cached_material = material.Resource ();
      
      LowLevelStateBlockPtr material_state_block = cached_material ? cached_material->StateBlock () : LowLevelStateBlockPtr ();
      
      render::low_level::IDevice&        device  = common_data.device_manager->Device ();
      render::low_level::IDeviceContext& context = common_data.device_manager->ImmediateContext ();
        
      render::low_level::StateBlockMask mask;      

      if (material_state_block)
      {
        material_state_block->Apply (&context);
        material_state_block->GetMask (mask);
      }
      
        //установка вершинных/индексного буфера
        
      mask.is_index_buffer = true; //0 index buffer is need to be set
      mask.is_layout       = true;
      
      const LowLevelBufferPtr* streams = vertex_buffer->Streams ();

      for (size_t i=0, streams_count=vertex_buffer->StreamsCount (); i<streams_count; i++)
      {        
        context.ISSetVertexBuffer (i, streams [i].get ());
        
        mask.is_vertex_buffers [i] = true;
      }

      context.ISSetInputLayout  (layout.get ());
      context.ISSetIndexBuffer  (common_data.index_buffer.get ());
      
        //обновление блока состояний примитива
      
      size_t mask_hash = mask.Hash ();
      
      if (cached_state_block_mask_hash != mask_hash)
      {
        if (has_debug_log)
          log.Printf ("...create state block for mesh primitive");
        
        cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
        cached_state_block_mask_hash = mask_hash;
      }
      
      cached_state_block->Capture (&context);
      
        //кэширование параметров примитива для отрисовки
      
      cached_primitive.material        = cached_material.get ();
      cached_primitive.state_block     = cached_state_block.get ();
      cached_primitive.indexed         = common_data.index_buffer != LowLevelBufferPtr ();
      cached_primitive.type            = type;
      cached_primitive.first           = first;
      cached_primitive.count           = count;
      cached_primitive.base_vertex     = base_vertex;
      cached_primitive.tags_count      = cached_material ? cached_material->TagsCount () : 0;
      cached_primitive.tags            = cached_material ? cached_material->Tags () : (const size_t*)0;
      cached_primitive.dynamic_indices = 0;
      
        //обновление зависимостей всегда, поскольку любые изменения материала/примитива должны быть отображены на зависимые кэши
        
      InvalidateCacheDependencies ();
      
      if (has_debug_log)
        log.Printf ("...mesh primitive cache updated");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::MeshPrimitive::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
  using CacheHolder::ResetCache;
};

typedef xtl::intrusive_ptr<MeshPrimitive> MeshPrimitivePtr;
typedef stl::vector<MeshPrimitivePtr>     MeshPrimitiveArray;
typedef stl::vector<RendererPrimitive>    RendererPrimitiveArray;

///Мэш
struct Mesh: public xtl::reference_counter, public MeshCommonData, public CacheHolder, public DebugIdHolder
{
  MeshPrimitiveArray     primitives;         //список примитивов мэша
  RendererPrimitiveArray cached_primitives;  //закэшированные примитивы
  RendererPrimitiveGroup cached_group;       //группы закэшированных примитивов
  Log                    log;                //поток протоколирования
  
///Конструктор
  Mesh (CacheHolder& parent_holder, const char* in_name, const DeviceManagerPtr& device_manager)
    : MeshCommonData (device_manager)    
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("render::manager::Mesh::Mesh", "name");

    name = in_name;
    
    parent_holder.AttachCacheSource (*this);
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' created (id=%u)", name.c_str (), Id ());
  }
  
///Деструктор
  ~Mesh ()
  {
      //предварительная очистка коллекция для возможности отслеживать порядок удаления объектов до и после удаления данного
      
    memset (&cached_group, 0, sizeof (cached_group));      

    cached_primitives.clear ();
    primitives.clear ();
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Mesh '%s' destroyed (id=%u)", name.c_str (), Id ());    
  }
  
///Сброс кэша
  void ResetCacheCore ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset mesh '%s' cache (id=%u)", name.c_str (), Id ());        
        
    cached_primitives.clear ();   
  }
  
///Обновление кэша
  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update mesh '%s' cache (id=%u)", name.c_str (), Id ());
      
      cached_primitives.clear ();
      cached_primitives.reserve (primitives.size ());
      
      for (MeshPrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
      {
        MeshPrimitive& src_primitive = **iter;

        if (!src_primitive.IsValid ())
          continue;        

        cached_primitives.push_back (src_primitive.cached_primitive);
      }
      
      memset (&cached_group, 0, sizeof (cached_group));
      
      cached_group.primitives_count = cached_primitives.size ();
      
      if (!cached_primitives.empty ())
        cached_group.primitives = &cached_primitives [0];
        
        //обновление зависимостей всегда, поскольку любые изменения материала/примитива должны быть отображены на зависимые кэши
        
      InvalidateCacheDependencies ();        
        
      if (has_debug_log)
        log.Printf ("...mesh cache updated");        
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::Mesh::UpdateCacheCore");
      throw;
    }
  }  
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

typedef xtl::intrusive_ptr<Mesh>            MeshPtr;
typedef stl::vector<MeshPtr>                MeshArray;
typedef stl::vector<RendererPrimitiveGroup> RenderPrimitiveGroupsArray;

}

struct PrimitiveImpl::Impl: public DebugIdHolder
{
  DeviceManagerPtr           device_manager;    //менеджер устройства
  MaterialManagerPtr         material_manager;  //менеджер материалов
  BuffersPtr                 buffers;           //буферы примитива
  MeshArray                  meshes;            //меши
  stl::string                name;              //имя примитива
  RenderPrimitiveGroupsArray render_groups;     //группы
  Log                        log;               //поток протоколирования
  PrimitiveUpdateNotifier    update_notifier;   //оповещатель обновлений примитива

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const MaterialManagerPtr& in_material_manager, const BuffersPtr& in_buffers, const char* in_name)
    : device_manager (in_device_manager)
    , material_manager (in_material_manager)
    , buffers (in_buffers)
  {
    static const char* METHOD_NAME = "render::manager::PrimitiveImpl::Impl::Impl";
    
    if (!device_manager)
      throw xtl::format_operation_exception (METHOD_NAME, "No DeviceManager binded");

    if (!buffers)
      throw xtl::make_null_argument_exception (METHOD_NAME, "buffers");
      
    if (!in_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
    name = in_name;
      
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Primitive '%s' created (id=%u)", name.c_str (), Id ());
  }

///Деструктор
  ~Impl ()
  {
      //предварительная очистка коллекция для возможности отслеживать порядок удаления объектов до и после удаления данного    
    
    meshes.clear ();
    render_groups.clear ();
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Primitive '%s' destroyed (id=%u)", name.c_str (), Id ());
  }
};

/*
    Конструктор / деструктор
*/

PrimitiveImpl::PrimitiveImpl (const DeviceManagerPtr& device_manager, const MaterialManagerPtr& material_manager, const BuffersPtr& buffers, const char* name)
{
  try
  {
    impl = new Impl (device_manager, material_manager, buffers, name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::PrimitiveImpl");
    throw;
  }
}

PrimitiveImpl::~PrimitiveImpl ()
{
}

/*
    Имя
*/

const char* PrimitiveImpl::Name ()
{
  return impl->name.c_str ();
}

void PrimitiveImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::PrimitiveImpl::SetName", "name");
    
  if (impl->device_manager->Settings ().HasDebugLog ())
    impl->log.Printf ("Primitive '%s' name changed to '%s' (id=%u)", impl->name.c_str (), name, impl->Id ());
    
  impl->name = name;    
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
    MeshPtr mesh (new Mesh (*this, source.Name (), impl->device_manager), false);
    
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
      
      MeshPrimitivePtr dst_primitive (new MeshPrimitive (*mesh, i, impl->material_manager->GetMaterialProxy (src_primitive.material), *mesh), false);

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

      dst_primitive->base_vertex = src_primitive.base_vertex;

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
    
    InvalidateCache ();
    
    return impl->meshes.size () - 1;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::AddMesh");
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

  InvalidateCache ();  
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  impl->meshes.clear ();
  
  InvalidateCache ();  
}

/*
    Работа со спрайтами
*/

size_t PrimitiveImpl::SpriteListsCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveImpl::AddStandaloneSpriteList (const SpriteListPtr& list, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveImpl::AddBatchingSpriteList (const SpriteListPtr& list, SpriteMode sprite_mode)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveImpl::RemoveSpriteList (size_t index)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveImpl::RemoveAllSpriteLists ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Работа с линиями
*/

size_t PrimitiveImpl::LineListsCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveImpl::AddStandaloneLineList (const LineListPtr& list, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveImpl::AddBatchingLineList (const LineListPtr& list)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveImpl::RemoveLineList (size_t index)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveImpl::RemoveAllLineLists ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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

/*
    Регистрация слушателей обновления примитивов рендеринга
*/

void PrimitiveImpl::AttachListener (IPrimitiveUpdateListener* listener)
{
  impl->update_notifier.Attach (listener);
}

void PrimitiveImpl::DetachListener (IPrimitiveUpdateListener* listener)
{
  impl->update_notifier.Detach (listener);
}

/*
    Управление кэшированием
*/

void PrimitiveImpl::UpdateCacheCore ()
{
  try
  {
    bool has_debug_log = impl->device_manager->Settings ().HasDebugLog ();
     
    if (has_debug_log)
      impl->log.Printf ("Update primitive '%s' cache (id=%u)", impl->name.c_str (), impl->Id ());    
    
    impl->render_groups.clear ();
    impl->render_groups.reserve (impl->meshes.size ());
    
    for (MeshArray::iterator iter=impl->meshes.begin (), end=impl->meshes.end (); iter!=end; ++iter)
    {
      Mesh& mesh = **iter;
      
      if (!mesh.IsValid ())
        continue;

      impl->render_groups.push_back (mesh.cached_group);
    }
    
    InvalidateCacheDependencies ();
    
    if (has_debug_log)
      impl->log.Printf ("...primitive cache updated");
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveImpl::UpdateCacheCore");
    throw;
  }
}

void PrimitiveImpl::ResetCacheCore ()
{
  bool has_debug_log = impl->device_manager->Settings ().HasDebugLog ();
     
  if (has_debug_log)
    impl->log.Printf ("Reset primitive '%s' cache (id=%u)", impl->name.c_str (), impl->Id ());

  for (MeshArray::iterator iter=impl->meshes.begin (), end=impl->meshes.end (); iter!=end; ++iter)
  {
    Mesh& mesh = **iter;        
    
    mesh.ResetCache ();
    
    for (MeshPrimitiveArray::iterator iter=mesh.primitives.begin (), end=mesh.primitives.end (); iter!=end; ++iter)
      (*iter)->ResetCache ();
  }
}
