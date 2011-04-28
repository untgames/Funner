#include "shared.h"

using namespace render;
using namespace render::low_level;

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

class EntityLodCommonData: public CacheHolder
{
  public:
///Конструктор
    EntityLodCommonData (EntityImpl& owner, const TextureManagerPtr& in_texture_manager, const DeviceManagerPtr& in_device_manager)
      : entity (owner)
      , device_manager (in_device_manager)
      , texture_manager (in_texture_manager)
      , properties (in_device_manager)
    {
      StateBlockMask mask;
      
      mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
      
      default_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);
    }
    
///Обратная ссылка на объекта
    EntityImpl& Entity () { return entity; }
    
///Менеджер устройства отрисовки
    DeviceManagerPtr DeviceManager () { return device_manager; }
    
///Менеджер текстур
    TextureManagerPtr& TextureManager () { return texture_manager; }
    
///Свойства объекта
    PropertyBuffer& Properties () { return properties; }
    
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
        return ProgramParametersLayoutPtr ();
        
      MaterialState& state = *iter->second;

      state.UpdateCache ();      

      return state.parameters_layout;
    }

  protected:
    void FlushUnusedMaterials ()
    {
      for (StateMap::iterator iter=states.begin (), end=states.end (); iter!=end; ++iter)
        if (iter->second->use_count () == 1)
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
    }
    
    void UpdateCacheCore ()
    {
      try
      {
        FlushUnusedMaterials ();

        dynamic_textures.FlushUnusedTextures ();
        
        device_manager->Device ().SSSetConstantBuffer (ProgramParametersSlot_Entity, properties.Buffer ().get ());
        
        default_state_block->Capture ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::EntityLodCommonData::Impl::UpdateCacheCore");
        throw;
      }
    }    
    
  private:
    struct MaterialState: public xtl::reference_counter, public CacheHolder
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
        , parameters_layout (new ProgramParametersLayout (&common_data.DeviceManager ()->Device ()), false)
        , state_block_mask_hash (0)
        , dynamic_textures (common_data.TextureManager (), in_material, common_data.Entity ())
      {
        if (!material)
          throw xtl::make_null_argument_exception ("render::EntityLodCommonData::MaterialState::MaterialState", "material");
          
        common_data.AttachCacheSource (*this);
        
        AttachCacheSource (common_data.properties);
        AttachCacheSource (dynamic_textures);        
      }
      
      using CacheHolder::UpdateCache;
      
      void ResetCacheCore ()
      {
      }
      
      void UpdateCacheCore ()
      {
        try
        {         
          StateBlockMask mask;
          
          mask.ss_constant_buffers [ProgramParametersSlot_Entity] = true;
          
          dynamic_textures.UpdateMask (mask);
          
          render::low_level::IDevice& device = common_data.DeviceManager ()->Device ();
          
          size_t mask_hash = mask.Hash ();
          
          if (!state_block || state_block_mask_hash != mask_hash)
          {
            state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
            state_block_mask_hash = mask_hash;
          }
            
          dynamic_textures.Apply (device);
          
          device.SSSetConstantBuffer (ProgramParametersSlot_Entity, common_data.Properties ().Buffer ().get ());
          
          state_block->Capture ();
          
          parameters_layout->DetachAll ();
          parameters_layout->Attach (*material->ParametersLayout ());
          parameters_layout->SetSlot (ProgramParametersSlot_Entity, common_data.Properties ().Properties ().Layout ());
        }
        catch (xtl::exception& e)
        {
          state_block = LowLevelStateBlockPtr ();
          e.touch ("render::EntityLodCommonData::MaterialState::UpdateCacheCore");
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
    EntityImpl&                 entity;              //обратная ссылка на объект
    DeviceManagerPtr            device_manager;      //менеджер устройства отрисовки
    TextureManagerPtr           texture_manager;     //менеджер текстур
    PropertyBuffer              properties;          //свойства рендеринга
    StateMap                    states;              //карта состояний
    DynamicTextureEntityStorage dynamic_textures;    //хранилище динамических текстур объекта рендеринга    
    LowLevelStateBlockPtr       default_state_block; //блок состояний по умолчанию
};

/*
    Описание уровня детализации
*/

typedef stl::vector<RendererOperation> RendererOperationArray;

struct EntityLod: public xtl::reference_counter, public CacheHolder
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
  }
  
///Обработчики событий кэша
  void ResetCacheCore ()
  {
    cached_primitive = PrimitivePtr ();
    
    cached_operations.clear ();
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));
  }  

  void UpdateCacheCore ()
  {
    try
    {
      cached_primitive = primitive.Resource ();

      if (!cached_primitive)
        throw xtl::format_operation_exception ("", "Primitive '%s' not found", primitive.Name ());
        
        //получение групп

      size_t groups_count = cached_primitive->RendererPrimitiveGroupsCount (), operations_count = 0;
      
      const RendererPrimitiveGroup* groups = cached_primitive->RendererPrimitiveGroups ();
      
        //резервирование операций рендеринга

      for (size_t i=0; i<groups_count; i++)
        operations_count += groups [i].primitives_count;
        
      cached_operations.reserve (operations_count);
      
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

          operation.state_block       = common_data.GetStateBlock (material).get ();
          operation.parameters_layout = common_data.GetProgramParametersLayout (material).get ();

          cached_operations.push_back (operation);
        }
      }
        
      cached_operation_list.operations_count = cached_operations.size ();
      cached_operation_list.operations       = cached_operations.empty () ? (RendererOperation*)0 : &cached_operations [0];
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::EntityLod::UpdateCacheCore");
      throw;
    }
  }  
  
  using CacheHolder::UpdateCache;
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
  math::mat4f         transformation;    //матрица преобразований
  EntityLodArray      lods;              //уровни детализации
  bool                need_resort;       //уровни детализации требуют пересортировки
  PrimitiveManagerPtr primitive_manager; //менеджер примитивов

///Конструктор
  Impl (EntityImpl& owner, const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const PrimitiveManagerPtr& in_primitive_manager)
    : EntityLodCommonData (owner, texture_manager, device_manager)
    , need_resort (false)
    , primitive_manager (in_primitive_manager)
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
    e.touch ("render::EntityImpl::EntityImpl");
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
}

const common::PropertyMap& EntityImpl::Properties ()
{
  return impl->Properties ().Properties ();
}

/*
    Матрица преобразований
*/

void EntityImpl::SetTransformation (const math::mat4f& tm)
{
  impl->transformation = tm;
}

const math::mat4f& EntityImpl::Transformation ()
{
  return impl->transformation;
}

/*
    Работа с костями (для скиннинга)
      преобразования умножаются на матрицу EntityImpl::Transformation в случае если она не единична    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::EntityImpl::JointTransformation");
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
  static const char* METHOD_NAME = "render::EntityImpl::Primitive";

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
  static const char* METHOD_NAME = "render::EntityImpl::PrimitiveName";

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
    e.touch ("render::EntityImpl::SetPrimitive(const PrimitivePtr&,size_t)");
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
    e.touch ("render::EntityImpl::SetPrimitive(const char*,size_t)");
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
    Получение операций рендеринга
*/

const RendererOperationList& EntityImpl::RendererOperations (size_t level_of_detail)
{
  try
  {    
    EntityLodPtr lod = impl->FindLod (level_of_detail);

    if (!lod)
      throw xtl::make_argument_exception ("", "level_of_detail", level_of_detail, "Lod primitive is not set");
      
    lod->UpdateCache ();

    return lod->cached_operation_list;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EntityImpl::RendererOperations");
    throw;
  }
}
