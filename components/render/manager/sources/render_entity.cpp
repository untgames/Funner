#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: use UpdateCache

namespace
{

/*
    ���������
*/

const size_t LODS_RESERVE = 4; //������������� ���������� ������� �����������

/*
    �������� ������ �������
*/

class EntityPrimitiveStateMap;

class EntityLodCommonData: public CacheHolder
{
  public:
///�����������
  EntityLodCommonData (EntityImpl& owner, const DeviceManagerPtr& device_manager)
    : entity (owner)
    , properties (device_manager)
  {
  }
  
///�������� ������ �� �������
  EntityImpl& Entity () { return entity; }
  
///�������� �������
  PropertyBuffer& Properties () { return properties; }
  
///����� ���������
  LowLevelStateBlockPtr FindStateBlock (MaterialImpl* material)
  {
    StateMap::iterator iter = states.find (material);
    
    if (iter == states.end ())
      return LowLevelStateBlockPtr ();
      
    return iter->second->state_block;
  }
    
  private:
    //TODO: cache processing
    
  private:
    struct PrimitiveState: public xtl::reference_counter, public CacheHolder
    {
      EntityLodCommonData&  common_data;
      MaterialPtr           material;      
      LowLevelStateBlockPtr state_block;
      
      PrimitiveState (EntityLodCommonData& in_common_data, MaterialImpl* in_material)
        : common_data (in_common_data)
        , material (in_material)
      {
        if (!material)
          throw xtl::make_null_argument_exception ("render::EntityLodCommonData::PrimitiveState::PrimitiveState", "material");
        
        AttachCacheSource (common_data.properties);
        AttachCacheSource (*material);
      }
      
      void ResetCacheCore ()
      {
        //TODO: implement
      }
      
      void UpdateCacheCore ()
      {
        //TODO: implement
      }
    };
    
    typedef xtl::intrusive_ptr<PrimitiveState>     StatePtr;
    typedef stl::hash_map<MaterialImpl*, StatePtr> StateMap;

  private:  
    EntityImpl&    entity;     //�������� ������ �� ������
    PropertyBuffer properties; //�������� ����������  
    StateMap       states;     //����� ���������
};

/*
    �������� ������ �����������
*/

typedef stl::vector<RendererOperation> RendererOperationArray;

struct EntityLod: public xtl::reference_counter, public CacheHolder
{
  EntityLodCommonData&   common_data;           //����� ������ ��� ���� ������� �����������
  size_t                 level_of_detail;       //����� ������ �����������
  PrimitiveProxy         primitive;             //��������
  PrimitivePtr           cached_primitive;      //�������������� ��������
  RendererOperationArray cached_operations;     //�������������� �������� ����������
  RendererOperationList  cached_operation_list; //�������������� �������� ����������

///�����������
  EntityLod (EntityLodCommonData& in_common_data, size_t in_level_of_detail, const PrimitiveProxy& in_primitive)
    : common_data (in_common_data)
    , level_of_detail (in_level_of_detail)
    , primitive (in_primitive)
  {
    AttachCacheSource (common_data);
    
    primitive.AttachCacheHolder (*this);
    
    memset (&cached_operation_list, 0, sizeof (cached_operation_list));
  }
  
///����������� ������� ����
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
        
        //��������� �����

      size_t groups_count = cached_primitive->RendererPrimitiveGroupsCount (), operations_count = 0;
      
      const RendererPrimitiveGroup* groups = cached_primitive->RendererPrimitiveGroups ();
      
        //�������������� �������� ����������

      for (size_t i=0; i<groups_count; i++)
        operations_count += groups [i].primitives_count;
        
      cached_operations.reserve (operations_count);
      
        //���������� ������ ��������
        
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
          
          operation.entity_state_block = common_data.FindStateBlock (material).get ();

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
    ������� ��������� �����
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
    �������� ���������� ������� ����������
*/

struct EntityImpl::Impl: public EntityLodCommonData
{
  math::mat4f           transformation;    //������� ��������������
  EntityLodArray        lods;              //������ �����������
  bool                  need_resort;       //������ ����������� ������� ��������������
  PrimitiveManagerPtr   primitive_manager; //�������� ����������
  EntityTextureStorage  textures;          //��������� ������� ������� ����������
  
///�����������
  Impl (EntityImpl& owner, const DeviceManagerPtr& device_manager, const PrimitiveManagerPtr& in_primitive_manager)
    : EntityLodCommonData (owner, device_manager)
    , need_resort (false)
    , primitive_manager (in_primitive_manager)
    , textures (owner)
  {
    lods.reserve (LODS_RESERVE);
  }
  
///��������� ������ �����������
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
  
///������ � �����
  void ResetCacheCore ()
  {
  }
  
  void UpdateCacheCore ()
  {
    try
    {
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::EntityImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;
};

/*
    ����������� / ����������
*/

EntityImpl::EntityImpl (const DeviceManagerPtr& device_manager, const PrimitiveManagerPtr& primitive_manager)
{
  try
  {
    impl = new Impl (*this, device_manager, primitive_manager);
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
    �������� ����������
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
    ������� ��������������
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
    ������ � ������� (��� ���������)
      �������������� ���������� �� ������� EntityImpl::Transformation � ������ ���� ��� �� ��������    
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
    ������ � �������� �����������
*/

size_t EntityImpl::LodsCount ()
{
  return impl->lods.size ();
}

/*
    ������ � ����������
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
    ��������� �������� ����������
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
