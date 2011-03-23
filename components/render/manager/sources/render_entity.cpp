#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: use UpdateCache

namespace
{

/*
    Константы
*/

const size_t LODS_RESERVE = 4; //резервируемое количество уровней детализации

/*
    Описание данных объекта
*/

struct EntityLodCommonData: public CacheHolder
{
  PropertyBuffer properties; //свойства рендеринга
  
///Конструктор
  EntityLodCommonData (const DeviceManagerPtr& device_manager)
    : properties (device_manager)
  {
    AttachCacheSource (properties);
  }
};

/*
    Описание уровня детализации
*/

struct EntityLod: public xtl::reference_counter, public CacheHolder
{
  EntityLodCommonData& common_data;      //общие данные для всех уровней детализации
  size_t               level_of_detail;  //номер уровня детализации
  PrimitiveProxy       primitive;        //примитив

///Конструктор
  EntityLod (EntityLodCommonData& in_common_data, size_t in_level_of_detail, const PrimitiveProxy& in_primitive)
    : common_data (in_common_data)
    , level_of_detail (in_level_of_detail)
    , primitive (in_primitive)
  {
    AttachCacheSource (common_data);
    
    primitive.AttachCacheHolder (*this);
  }
  
///Обработчики событий кэша
  void UpdateCacheCore ()
  {
  }
  
  void ResetCacheCore ()
  {
  }  
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
  math::mat4f           transformation;    //матрица преобразований
  EntityLodArray        lods;              //уровни детализации
  bool                  need_resort;       //уровни детализации требуют пересортировки
  PrimitiveManagerPtr   primitive_manager; //менеджер примитивов
  EntityTextureStorage  textures;          //хранилище текстур объекта рендеринга
  
///Конструктор
  Impl (EntityImpl& owner, const DeviceManagerPtr& device_manager, const PrimitiveManagerPtr& in_primitive_manager)
    : EntityLodCommonData (device_manager)
    , need_resort (false)
    , primitive_manager (in_primitive_manager)
    , textures (owner)
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
  
///Работа с кэшем
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
    Конструктор / деструктор
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
    Свойства рендеринга
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  impl->properties.SetProperties (properties);
}

const common::PropertyMap& EntityImpl::Properties ()
{
  return impl->properties.Properties ();
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
