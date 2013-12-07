#include "shared.h"

using namespace render;
using namespace render::manager;
using namespace render::low_level;

//TODO: если материал не содержит динамических текстур - возвращаеть StateBlock от Entity
//TODO: Обработчик оповещения об обновлении примитивов

namespace
{

/*
    Константы
*/

const size_t LODS_RESERVE = 4; //резервируемое количество уровней детализации

/*
    Описание данных объекта
*/

class EntityPrimitiveStateMap;

class EntityLodCommonData: public CacheHolder, public DebugIdHolder
{
  public:
///Конструктор
    EntityLodCommonData (EntityImpl& owner, const TextureManagerPtr& in_texture_manager, const DeviceManagerPtr& in_device_manager)
      : entity (owner)
      , device_manager (in_device_manager)
      , texture_manager (in_texture_manager)
      , properties (in_device_manager)
      , entity_parameters_layout (&in_device_manager->Device (), &in_device_manager->Settings ())
      , shader_options_cache (&in_device_manager->CacheManager ())
      , scissor_state (false)
    {
      StateBlockMask mask;
      
      mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
      
      default_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);            
      
      entity_parameters_layout.AttachSlot (ProgramParametersSlot_Entity, properties.Properties ());
      
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Entity created (id=%u)", Id ());
    }
    
///Деструктор
    ~EntityLodCommonData ()
    {
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Entity destroyed (id=%u)", Id ());      
    }

///Обратная ссылка на объекта
    EntityImpl& Entity () { return entity; }
    
///Менеджер устройства отрисовки
    DeviceManagerPtr DeviceManager () { return device_manager; }
    
///Менеджер текстур
    TextureManagerPtr& TextureManager () { return texture_manager; }
    
///Свойства объекта
    PropertyBuffer& Properties () { return properties; }
    
///Расположение свойств
    ProgramParametersLayout& ParametersLayout () { return entity_parameters_layout; }

///Динамические текстуры
    DynamicTextureEntityStorage& DynamicTextures () { return dynamic_textures; }
    
///Поиск состояния
    LowLevelStateBlockPtr FindStateBlock (MaterialImpl* material)
    {
      StateMap::iterator iter = states.find (material);
      
      if (iter == states.end ())
        return LowLevelStateBlockPtr ();
        
      MaterialState& state = *iter->second;
        
      state.UpdateCache ();
        
      return state.state_block;
    }
    
    LowLevelStateBlockPtr GetStateBlock (MaterialImpl* material)
    {
      if (!material)
        return LowLevelStateBlockPtr ();        
      
      LowLevelStateBlockPtr result = FindStateBlock (material);
      
      if (result)
        return result;

      if (!material->HasDynamicTextures ())
      {        
        if (properties.Properties ().Size () == 0)
          return LowLevelStateBlockPtr ();
          
        return default_state_block;
      }
        
      StatePtr state (new MaterialState (*this, material), false);
      
      states.insert_pair (material, state);
      
      state->UpdateCache ();
      
      return state->state_block;
    }
    
    ProgramParametersLayoutPtr GetProgramParametersLayout (MaterialImpl* material)
    {
      if (!material)
        return ProgramParametersLayoutPtr ();
        
      StateMap::iterator iter = states.find (material);
      
      if (iter == states.end ())
      {
        if (properties.Properties ().Size () == 0)
          return material->ParametersLayout ();
        
        StatePtr state (new MaterialState (*this, material), false);
        
        states.insert_pair (material, state);
        
        state->UpdateCache ();

        return state->parameters_layout;        
      }
        
      MaterialState& state = *iter->second;

      state.UpdateCache ();      

      return state.parameters_layout;
    }
    
///Кэш опций шейдера
    render::manager::ShaderOptionsCache& ShaderOptionsCache () { return shader_options_cache; }
    
///Режим отсечения
    void SetScissorState (bool state)
    {
      if (state == scissor_state)
        return;
      
      scissor_state = state;
      
      InvalidateCache (false);
    }
    
    bool ScissorState () { return scissor_state; }
    
///Область отсечения
    void SetScissor (const RectArea& rect)
    {
      scissor_rect = rect;
      
      InvalidateCache (false);
    }
    
    const RectArea& Scissor () { return scissor_rect; }

///Управление кэшированием
    using CacheHolder::UpdateCache;
    using CacheHolder::ResetCache;

  protected:
    void FlushUnusedMaterials ()
    {
      for (StateMap::iterator iter=states.begin (), end=states.end (); iter!=end;)
        if (iter->second->material->use_count () == 1)
        {
          StateMap::iterator next = iter;
          
          ++next;
          
          states.erase (iter);
          
          iter = next;
        }
        else ++iter;
    }
    
///Работа с кэшем
    void ResetCacheCore ()
    {
      if (device_manager->Settings ().HasDebugLog ())
        Log ().Printf ("Reset entity cache (id=%u)", Id ());      
    }
    
    void UpdateCacheCore ()
    {
      try
      {
        if (device_manager->Settings ().HasDebugLog ())
          Log ().Printf ("Update entity cache (id=%u)", Id ());

        FlushUnusedMaterials ();

        dynamic_textures.FlushUnusedTextures ();

        low_level::IDeviceContext& context = device_manager->ImmediateContext ();

        context.SSSetConstantBuffer (ProgramParametersSlot_Entity, properties.Buffer ().get ());

        default_state_block->Capture (&context);
        
        InvalidateCacheDependencies (); //TODO: проверить необходимость
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::EntityLodCommonData::Impl::UpdateCacheCore");
        throw;
      }
    }    
    
  private:
    struct MaterialState: public xtl::reference_counter, public CacheHolder, public DebugIdHolder
    {
      EntityLodCommonData&          common_data;
      MaterialPtr                   material;
      ProgramParametersLayoutPtr    parameters_layout;
      size_t                        state_block_mask_hash;
      LowLevelStateBlockPtr         state_block;
      DynamicTextureMaterialStorage dynamic_textures;
      
      MaterialState (EntityLodCommonData& in_common_data, MaterialImpl* in_material)
        : common_data (in_common_data)
        , material (in_material)
        , parameters_layout (new ProgramParametersLayout (&common_data.DeviceManager ()->Device (), &common_data.DeviceManager ()->Settings ()), false)
        , state_block_mask_hash (0)
        , dynamic_textures (common_data.TextureManager (), in_material, common_data.Entity ())
      {
        if (!material)
          throw xtl::make_null_argument_exception ("render::manager::EntityLodCommonData::MaterialState::MaterialState", "material");
          
        common_data.AttachCacheSource (*this);
        
        AttachCacheSource (common_data.properties);
        AttachCacheSource (dynamic_textures);        
        
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Entity material instance created (entity_id=%u, id=%u)", common_data.Id (), Id ());
      }
      
      ~MaterialState ()
      {
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Entity material instance destroyed (entity_id=%u, id=%u)", common_data.Id (), Id ());        
      }
      
      using CacheHolder::UpdateCache;
      
      void ResetCacheCore ()
      {
        if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
          Log ().Printf ("Reset entity material instance cache (entity_id=%u, id=%u)", common_data.Id (), Id ());        
      }
      
      void UpdateCacheCore ()
      {
        try
        {
          if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
            Log ().Printf ("Update entity material instance cache (entity_id=%u, id=%u)", common_data.Id (), Id ());
          
          StateBlockMask mask;
          
          mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
          
          dynamic_textures.UpdateMask (mask);
          
          render::low_level::IDevice&        device  = common_data.DeviceManager ()->Device ();
          render::low_level::IDeviceContext& context = common_data.DeviceManager ()->ImmediateContext ();
          
          size_t mask_hash = mask.Hash ();
          
          if (!state_block || state_block_mask_hash != mask_hash)
          {
            state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
            state_block_mask_hash = mask_hash;
          }
            
          dynamic_textures.Apply (context);

          context.SSSetConstantBuffer (ProgramParametersSlot_Entity, common_data.Properties ().Buffer ().get ());
          
          state_block->Capture (&context);
          
          parameters_layout->DetachAll ();
          parameters_layout->Attach (*material->ParametersLayout ());
          parameters_layout->Attach (common_data.entity_parameters_layout);
        }
        catch (xtl::exception& e)
        {
          state_block = LowLevelStateBlockPtr ();
          e.touch ("render::manager::EntityLodCommonData::MaterialState::UpdateCacheCore");
          throw;
        }
        catch (...)
        {
          state_block = LowLevelStateBlockPtr ();          
          throw;
        }
      }
    };
    
    typedef xtl::intrusive_ptr<MaterialState>      StatePtr;
    typedef stl::hash_map<MaterialImpl*, StatePtr> StateMap;

  private:  
    EntityImpl&                          entity;                   //обратная ссылка на объект
    DeviceManagerPtr                     device_manager;           //менеджер устройства отрисовки
    TextureManagerPtr                    texture_manager;          //менеджер текстур
    PropertyBuffer                       properties;               //свойства рендеринга
    ProgramParametersLayout              entity_parameters_layout; //объект расположения свойств
    StateMap                             states;                   //карта состояний
    DynamicTextureEntityStorage          dynamic_textures;         //хранилище динамических текстур объекта рендеринга
    render::manager::ShaderOptionsCache  shader_options_cache;     //кэш опций шейдера
    bool                                 scissor_state;            //состояние режима отсечения
    RectArea                             scissor_rect;             //область отсечения
    LowLevelStateBlockPtr                default_state_block;      //блок состояний по умолчанию
};

/*
    Описание уровня детализации
*/

typedef stl::vector<RendererOperation> RendererOperationArray;

struct EntityLod: public xtl::reference_counter, public CacheHolder, public DebugIdHolder, public IPrimitiveUpdateListener
{
  EntityLodCommonData&   common_data;           //общие данные для всех уровней детализации
  size_t                 level_of_detail;       //номер уровня детализации
  PrimitiveProxy         primitive;             //примитив
  PrimitivePtr           cached_primitive;      //закэшированный примитив
  RendererOperationArray cached_operations;     //закэшированные операции рендеринга
  RendererOperationList  cached_operation_list; //закэшированные операции рендеринга

///Конструктор
  EntityLod (EntityLodCommonData& in_common_data, size_t in_level_of_detail, const PrimitiveProxy& in_primitive)
    : common_data (in_common_data)
    , level_of_detail (in_level_of_detail)
    , primitive (in_primitive)
  {
    AttachCacheSource (common_data);
    
    primitive.AttachCacheHolder (*this);
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));

    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Entity lod created (entity_id=%u, id=%u)", common_data.Id (), Id ());
  }
  
///Деструктор
  ~EntityLod ()
  {
    if (cached_primitive)
      cached_primitive->DetachListener (this);

    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Entity lod destroyed (entity_id=%u, id=%u)", common_data.Id (), Id ());
  }
  
///Обработчики событий кэша
  void ResetCacheCore ()
  {
    if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
      Log ().Printf ("Reset entity lod cache (entity_id=%u, id=%u)", common_data.Id (), Id ());

    if (cached_primitive)
      cached_primitive->DetachListener (this);
        
    cached_primitive = PrimitivePtr ();
    
    cached_operations.clear ();
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));
  }  

  void UpdateCacheCore ()
  {
    try
    {
      if (common_data.DeviceManager ()->Settings ().HasDebugLog ())
        Log ().Printf ("Update entity lod cache (entity_id=%u, id=%u)", common_data.Id (), Id ());
            
      cached_primitive = primitive.Resource ();

      if (!cached_primitive)
        throw xtl::format_operation_exception ("", "Primitive '%s' not found", primitive.Name ());

        //регистрация слушателя
      
      cached_primitive->AttachListener (this);
        
        //получение групп

      size_t groups_count = cached_primitive->RendererPrimitiveGroupsCount (), operations_count = 0;
      
      const RendererPrimitiveGroup* groups = cached_primitive->RendererPrimitiveGroups ();
      
        //резервирование операций рендеринга

      for (size_t i=0; i<groups_count; i++)
        operations_count += groups [i].primitives_count;
        
      cached_operations.clear ();
      cached_operations.reserve (operations_count);
      
      const RectAreaImpl* scissor = common_data.ScissorState () ? Wrappers::Unwrap<RectAreaImpl> (common_data.Scissor ()).get () : (const RectAreaImpl*)0;
      
        //построение списка операций
        
      for (size_t i=0; i<groups_count; i++)
      {
        const RendererPrimitiveGroup& group            = groups [i];
        const RendererPrimitive*      primitives       = group.primitives;        
        size_t                        primitives_count = group.primitives_count;
        
        for (size_t j=0; j<primitives_count; j++)
        {
          const RendererPrimitive& renderer_primitive = primitives [j];
          
          RendererOperation operation;
          
          memset (&operation, 0, sizeof (operation));
          
          operation.primitive = &renderer_primitive;
          operation.entity    = &common_data.Entity ();

          MaterialImpl* material = renderer_primitive.material;

          operation.state_block              = common_data.GetStateBlock (material).get ();
          operation.entity_parameters_layout = common_data.GetProgramParametersLayout (material).get ();
          operation.shader_options_cache     = &common_data.ShaderOptionsCache ();
          operation.scissor                  = scissor;

          cached_operations.push_back (operation);
        }
      }
        
      cached_operation_list.operations_count = cached_operations.size ();
      cached_operation_list.operations       = cached_operations.empty () ? (RendererOperation*)0 : &cached_operations [0];
      
      InvalidateCacheDependencies ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::EntityLod::UpdateCacheCore");
      throw;
    }
  }  

/// Обработчик оповещения об обновлении примитивов
  void UpdateRendererPrimitives (size_t group_index, size_t first_primitive_index, size_t primitives_count)
  {
//TODO: обработчик оповещения
  }
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;  
  using CacheHolder::InvalidateCache;
};

typedef xtl::intrusive_ptr<EntityLod> EntityLodPtr;
typedef stl::vector<EntityLodPtr>     EntityLodArray;

/*
    Функтор сравнения лодов
*/

struct EntityLodLess
{
  bool operator () (const EntityLodPtr& lod1, const EntityLodPtr& lod2) const
  {
    return lod1->level_of_detail < lod2->level_of_detail;
  }
  
  bool operator () (const EntityLodPtr& lod, size_t level_of_detail) const
  {
    return lod->level_of_detail < level_of_detail;
  }    
  
  bool operator () (size_t level_of_detail, const EntityLodPtr& lod) const
  {
    return level_of_detail < lod->level_of_detail;
  }  
};

}

/*
    Описание реализации объекта рендеринга
*/

struct EntityImpl::Impl: public EntityLodCommonData
{
  EntityLodArray      lods;              //уровни детализации
  bool                need_resort;       //уровни детализации требуют пересортировки
  PrimitiveManagerPtr primitive_manager; //менеджер примитивов
  math::vec3f         lod_point;         //точка расчёта lod-уровня
  void*               user_data;         //пользовательские данные

///Конструктор
  Impl (EntityImpl& owner, const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const PrimitiveManagerPtr& in_primitive_manager)
    : EntityLodCommonData (owner, texture_manager, device_manager)
    , need_resort (false)
    , primitive_manager (in_primitive_manager)
    , user_data ()
  {
    lods.reserve (LODS_RESERVE);
  }
  
///Получение уровня детализации
  int FindLodIndex (size_t index, bool find_nearest = false)
  {
    if (need_resort)
    {
      if (!lods.empty ())
        stl::sort (lods.begin (), lods.end (), EntityLodLess ());
      
      need_resort = false;
    }
    
    EntityLodArray::iterator iter = find_nearest ? stl::lower_bound (lods.begin (), lods.end (), index, EntityLodLess ()) : 
      stl::equal_range (lods.begin (), lods.end (), index, EntityLodLess ()).first;
    
    if (iter != lods.end ())
      return iter - lods.begin ();
      
    return -1;
  }
  
  EntityLodPtr FindLod (size_t level_of_detail, bool find_nearest = false)
  {
    int index = FindLodIndex (level_of_detail, find_nearest);
    
    return index != -1 ? lods [index] : EntityLodPtr ();
  }  
    
  using CacheHolder::UpdateCache;
};

/*
    Конструктор / деструктор
*/

EntityImpl::EntityImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const PrimitiveManagerPtr& primitive_manager)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture_manager)
      throw xtl::make_null_argument_exception ("", "texture_manager");
      
    if (!primitive_manager)
      throw xtl::make_null_argument_exception ("", "primitive_manager");
    
    impl = new Impl (*this, device_manager, texture_manager, primitive_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::EntityImpl");
    throw;
  }
}

EntityImpl::~EntityImpl ()
{
}

/*
    Хранилище динамических текстур
*/

DynamicTextureEntityStorage& EntityImpl::DynamicTextureStorage ()
{
  return impl->DynamicTextures ();
}

/*
    Свойства рендеринга
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  impl->Properties ().SetProperties (properties);

  impl->ParametersLayout ().AttachSlot (ProgramParametersSlot_Entity, properties);
}

const common::PropertyMap& EntityImpl::Properties ()
{
  return impl->Properties ().Properties ();
}

/*
    Макро-определения шейдера
*/

void EntityImpl::SetShaderOptions (const common::PropertyMap& properties)
{
  try
  {
    impl->ShaderOptionsCache ().SetProperties (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetShaderOptions");
    throw;
  }
}

const common::PropertyMap& EntityImpl::ShaderOptions ()
{
  return impl->ShaderOptionsCache ().Properties ();
}

/*
    Пользовательские данные
*/

void EntityImpl::SetUserData (void* data)
{
  impl->user_data = data;
}

void* EntityImpl::UserData ()
{
  return impl->user_data;
}

/*
    Работа с костями (для скиннинга)
      преобразования умножаются на матрицу EntityImpl::Transformation в случае если она не единична    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::manager::EntityImpl::JointTransformation");
}

/*
    Работа с уровнями детализации
*/

size_t EntityImpl::LodsCount ()
{
  return impl->lods.size ();
}

/*
    Работа с примитивом
*/

PrimitivePtr EntityImpl::Primitive (size_t level_of_detail)
{
  static const char* METHOD_NAME = "render::manager::EntityImpl::Primitive";

  EntityLodPtr lod = impl->FindLod (level_of_detail);

  if (lod)
  {
    PrimitivePtr result = lod->primitive.Resource ();
    
    if (result)
      return result;
      
    if (*lod->primitive.Name ())
      throw xtl::format_operation_exception (METHOD_NAME, "Lod #%u primitive '%s' not found", level_of_detail, lod->primitive.Name ());
  }

  throw xtl::make_argument_exception (METHOD_NAME, "level_of_detail", level_of_detail, "Lod primitive is not set");
}

const char* EntityImpl::PrimitiveName (size_t level_of_detail)
{
  static const char* METHOD_NAME = "render::manager::EntityImpl::PrimitiveName";

  EntityLodPtr lod = impl->FindLod (level_of_detail);

  if (lod)
    return lod->primitive.Name ();

  throw xtl::make_argument_exception (METHOD_NAME, "level_of_detail", level_of_detail, "Lod primitive is not set");
}

void EntityImpl::ResetPrimitive (size_t level_of_detail)
{
  int index = impl->FindLodIndex (level_of_detail);
  
  if (index == -1)
    return;
    
  impl->lods.erase (impl->lods.begin () + index);
}

void EntityImpl::SetPrimitive (const PrimitivePtr& primitive, size_t level_of_detail)
{
  try
  {
    PrimitiveProxy proxy = impl->primitive_manager->GetPrimitiveProxy ("");
    
    proxy.SetResource (primitive);
    
    EntityLodPtr lod (new EntityLod (*impl, level_of_detail, proxy), false);  
    
    int lod_index = impl->FindLodIndex (level_of_detail);
    
    if (lod_index == -1)
    {
      impl->lods.push_back (lod);
      impl->need_resort = true;
    }
    else impl->lods [lod_index] = lod;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetPrimitive(const PrimitivePtr&,size_t)");
    throw;
  }
}

void EntityImpl::SetPrimitive (const char* name, size_t level_of_detail)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!*name)
      throw xtl::make_argument_exception ("", "name", name, "Empty primitive name is not allowed as entity level of detail");
    
    EntityLodPtr lod (new EntityLod (*impl, level_of_detail, impl->primitive_manager->GetPrimitiveProxy (name)), false);
      
    int lod_index = impl->FindLodIndex (level_of_detail);
    
    if (lod_index == -1)
    {
      impl->lods.push_back (lod);
      impl->need_resort = true;
    }
    else impl->lods [lod_index] = lod;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::SetPrimitive(const char*,size_t)");
    throw;
  }  
}

bool EntityImpl::HasPrimitive (size_t level_of_detail)
{
  return impl->FindLodIndex (level_of_detail) != -1;
}

void EntityImpl::ResetAllPrimitives ()
{
  impl->lods.clear ();

  impl->need_resort = false;
}

/*
    Точка в локальной системе координат объекта для расчёта удаленности от камеры
*/

void EntityImpl::SetLodPoint (const math::vec3f& p)
{
  impl->lod_point = p;
}

const math::vec3f& EntityImpl::LodPoint ()
{
  return impl->lod_point;
}

/*
    Управление областью отсечения объекта
*/

void EntityImpl::SetScissor (const RectArea& scissor)
{
  impl->SetScissor (scissor);
}

const RectArea& EntityImpl::Scissor ()
{
  return impl->Scissor ();
}

void EntityImpl::SetScissorState (bool state)
{
  impl->SetScissorState (state);
}

bool EntityImpl::ScissorState ()
{
  return impl->ScissorState ();
}

/*
    Получение операций рендеринга
*/

const RendererOperationList& EntityImpl::RendererOperations (size_t level_of_detail, bool find_nearest)
{
  try
  {    
    EntityLodPtr lod = impl->FindLod (level_of_detail, find_nearest);

    if (!lod)
      throw xtl::make_argument_exception ("", "level_of_detail", level_of_detail, "Lod primitive is not set");

    impl->Properties ().UpdateCache (); //TODO: убрать!!!

    lod->UpdateCache ();

    return lod->cached_operation_list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EntityImpl::RendererOperations");
    throw;
  }
}

/*
    Управление кэшированием
*/

void EntityImpl::UpdateCache ()
{
  impl->Properties ().UpdateCache ();

  impl->UpdateCache ();
  
  for (EntityLodArray::iterator iter=impl->lods.begin (), end=impl->lods.end (); iter!=end; ++iter)
    (*iter)->UpdateCache ();
}

void EntityImpl::ResetCache ()
{
  impl->Properties ().ResetCache ();

  impl->ResetCache ();
  
  for (EntityLodArray::iterator iter=impl->lods.begin (), end=impl->lods.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();  
}
