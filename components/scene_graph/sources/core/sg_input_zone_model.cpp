#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации зоны ввода
*/

namespace
{

typedef xtl::signal<void (InputZoneModel& sender, InputZoneEvent event_id)>                                                   EventSignal;
typedef xtl::signal<void (InputZoneModel& sender, const Viewport& viewport, const char* notification_id, const char* params)> NotificationSignal;

struct Notification
{
  stl::hash_key<const char*> name_hash;
  NotificationSignal         signal;
  
  Notification () : name_hash ("") {}
  
  Notification (const Notification&) : name_hash ("") {}
};

typedef stl::list<Notification> NotificationList;

struct ZoneImpl
{
  math::mat4f  basis;
  math::mat4f  inv_basis;
  math::planef plane;
  float        plane_normal_length;
};

typedef stl::vector<ZoneImpl> ZoneImplArray;

}

struct InputZoneModel::Impl
{
  NotificationList           notifications;                      //карта событий ввода
  NotificationList::iterator cached_notification;                //сохраненная последняя нотификация
  NotificationSignal         default_notification_signal;        //обработчик нотификаций по умолчанию
  EventSignal                event_signals [InputZoneEvent_Num]; //сигналы зоны
  bool                       is_active;                          //активна ли зона
  bool                       bounds_auto_update;                 //резжим автоматического обновления ограничивающего объема
  ZoneImplArray              zones_impl;                         //реализации зон
  math::mat4f                inv_world_tm;                       //инвертированная матрица мирового преобразования
  bool                       need_update_inv_world_tm;           //инвертированная матрица мирового преобразования требует обновления
  bool                       need_update_zones_internals;        //необходимо обновить внутреннее представление зон
  
///Конструктор
  Impl ()
    : cached_notification (notifications.end ())
    , is_active (true)
    , bounds_auto_update (true)
    , need_update_inv_world_tm (true)
    , need_update_zones_internals (true)
  {
  }
  
///Поиск нотификации
  Notification* FindNotification (const stl::hash_key<const char*>& name_hash)
  {
    if (cached_notification != notifications.end () && cached_notification->name_hash == name_hash)
    {
      return &*cached_notification;
    }
    else
    {
      for (NotificationList::iterator iter=notifications.begin (), end=notifications.end (); iter!=end;)
      {
        if (iter->signal.empty ())
        {          
          NotificationList::iterator next = iter;
          
          ++next;
          
          notifications.erase (iter);
          
          iter = next;
          
          continue;
        }
        
        if (iter->name_hash == name_hash)        
        {
          cached_notification = iter;
          return &*iter;
        }
        
        ++iter;
      }
      
      return 0;
    }        
  }

///Оповещение о событии
  void Notify (InputZoneModel& sender, InputZoneEvent event_id)
  {
    try
    {
      if (!event_signals [event_id])
        return;

      event_signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }  
};

/*
    Конструктор / деструктор
*/

InputZoneModel::InputZoneModel ()
  : impl (new Impl)
{
}

InputZoneModel::~InputZoneModel ()
{
}

/*
    Количество зон / получение массива зон
*/

size_t InputZoneModel::ZoneDescsCount () const
{
  return const_cast<InputZoneModel&> (*this).ZoneDescsCountCore ();
}

const InputZoneModel::ZoneDesc* InputZoneModel::ZoneDescs () const
{
  return const_cast<InputZoneModel&> (*this).ZoneDescsCore ();
}

/*
    Режим автоматического обновления ограничивающего объема
*/

void InputZoneModel::SetBoundsAutoUpdate (bool state)
{
  if (impl->bounds_auto_update == state)
    return;
    
  impl->bounds_auto_update = state;
  
  if (state)
    UpdateZoneBounds ();
}

bool InputZoneModel::BoundsAutoUpdate () const
{
  return impl->bounds_auto_update;
}

void InputZoneModel::UpdateZoneBounds ()
{
  const ZoneDesc* descs = ZoneDescsCore ();
  
  bound_volumes::aaboxf box;

  for (size_t i=0, count=ZoneDescsCountCore (); i<count; i++)
  {
    const ZoneDesc& desc = descs [i];

    box += desc.position + ( desc.axis_x + desc.axis_y) * 0.5f;
    box += desc.position + ( desc.axis_x - desc.axis_y) * 0.5f;
    box += desc.position + (-desc.axis_x + desc.axis_y) * 0.5f;
    box += desc.position + (-desc.axis_x - desc.axis_y) * 0.5f;
  }

  SetBoundBox (box);
}

/*
    Активность зоны
*/

bool InputZoneModel::IsActive () const
{
  return impl->is_active;
}

void InputZoneModel::SetActive (bool state)
{
  if (state == impl->is_active)
    return;
    
  impl->is_active = state;
  
  impl->Notify (*this, InputZoneEvent_OnActivityChanged);
  
  UpdateNotify ();
}

/*
    Подписка на события модели
*/

xtl::connection InputZoneModel::RegisterEventHandler (InputZoneEvent event_id, const EventHandler& event_handler) const
{
  switch (event_id)
  {
    case InputZoneEvent_AfterZoneDescsUpdate:
    case InputZoneEvent_OnActivityChanged:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::InputZoneModel::RegisterEventHandler", "event_id", event_id);
  }
  
  return impl->event_signals [event_id].connect (event_handler);
}

xtl::connection InputZoneModel::RegisterNotificationHandler (const char* notification_id, const NotificationHandler& handler) const
{
  if (!notification_id)
    throw xtl::make_null_argument_exception ("scene_graph::InputZoneModel::RegisterNotificationHandler", "notification_id");
    
  stl::hash_key<const char*> name_hash = notification_id;
    
  if (Notification* notification = impl->FindNotification (name_hash))
  {
    return notification->signal.connect (handler);
  }
  else
  {
    impl->notifications.push_back ();
    
    try
    {
      Notification& notification = impl->notifications.back ();
      
      notification.name_hash = name_hash;
      
      return notification.signal.connect (handler);
    }
    catch (...)
    {
      impl->notifications.pop_back ();
      throw;
    }
  }
}

xtl::connection InputZoneModel::RegisterNotificationHandler (const NotificationHandler& handler) const
{
  return impl->default_notification_signal.connect (handler);
}

/*
    Оповещение о возникновении события ввода
*/

void InputZoneModel::Notify (const Viewport& viewport, const char* notification_id, const char* notification_params) const
{
  try
  {
    if (!notification_id)
      throw xtl::make_null_argument_exception ("", "notification_id");

    if (!notification_params)
      throw xtl::make_null_argument_exception ("", "notification_params");            

    if (Notification* notification = impl->FindNotification (notification_id))
      notification->signal (const_cast<InputZoneModel&> (*this), viewport, notification_id, notification_params);

    impl->default_notification_signal (const_cast<InputZoneModel&> (*this), viewport, notification_id, notification_params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::InputZoneModel::Notify");
    throw;
  }
}

bool InputZoneModel::HasNotificationHandler (const char* notification_id) const
{    
  return notification_id && (!impl->default_notification_signal.empty () || impl->FindNotification (notification_id));
}

/*
    Динамическая диспетчеризация
*/

void InputZoneModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Оповещение об изменении данных зоны
*/

void InputZoneModel::UpdateZoneDescsNotify ()
{
  impl->need_update_zones_internals = true;

  impl->Notify (*this, InputZoneEvent_AfterZoneDescsUpdate);
  
  if (impl->bounds_auto_update)
    UpdateZoneBounds ();

  UpdateNotify ();
}

/*
    Связывание свойств узла с методами узла
*/

void InputZoneModel::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Active", xtl::bind (&InputZoneModel::IsActive, this), xtl::bind (&InputZoneModel::SetActive, this, _1));
}

/*
    Оповещения
*/

void InputZoneModel::AfterUpdateWorldTransformEvent ()
{
  impl->need_update_inv_world_tm = true;
}

/*
    Обновление дополнительной информации о зонах
*/

void InputZoneModel::UpdateZoneInternals ()
{
  try
  {
    impl->zones_impl.clear ();
    
    size_t zones_count = ZoneDescsCountCore ();
    
    impl->zones_impl.reserve (zones_count);
    
    const ZoneDesc* descs = ZoneDescsCore ();
    
    for (size_t i=0; i<zones_count; i++)
    {
      const ZoneDesc& desc = descs [i];
      
      ZoneImpl zone_impl;
      
      math::mat4f basis;
      
      math::vec3f normal = normalize (cross (desc.axis_x, desc.axis_y));
      
      basis [0] = math::vec4f (desc.axis_x, 0.0f);
      basis [1] = math::vec4f (desc.axis_y, 0.0f);
      basis [2] = math::vec4f (normal, 0.0f);
      basis [3] = math::vec4f (desc.position - (desc.axis_x + desc.axis_y) * 0.5f, 1.0f);
      basis     = transpose (basis);
      
      zone_impl.basis               = basis;
      zone_impl.inv_basis           = inverse (basis);
      zone_impl.plane               = math::planef (normal, -dot (normal, desc.position));
      zone_impl.plane_normal_length = length (zone_impl.plane.normal ());
      
      impl->zones_impl.push_back (zone_impl);
    }
    
    impl->need_update_zones_internals = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::InputZoneModel::UpdateZoneInternals");
    throw;
  }
}

/*
    Проверка пересечения луча с зонами
*/

bool InputZoneModel::IsIntersected
 (NodeTransformSpace transform_space,
  const math::vec3f& ray_from,
  const math::vec3f& ray_to,
  size_t&            out_zone_index,
  float&             out_ray_intersection_distance,
  float&             out_ray_to_zone_distance,
  math::vec2f&       out_zone_intersection_point) const
{
  try
  {
      //обновление внутреннего представления зон
    
    if (impl->need_update_zones_internals)
      const_cast<InputZoneModel&> (*this).UpdateZoneInternals ();
      
      //перевод луча в локальную систему координат
      
    math::vec3f local_ray_from, local_ray_to;
      
    switch (transform_space)
    {
      case NodeTransformSpace_Local:
        local_ray_from = ray_from;
        local_ray_to   = ray_to;
        break;
      case NodeTransformSpace_Parent:
      {
        math::mat4f inv_parent_tm = inverse (ParentTM ());

        local_ray_from = inv_parent_tm * ray_from;
        local_ray_to   = inv_parent_tm * ray_to;

        break;
      }
      case NodeTransformSpace_World:
        if (impl->need_update_inv_world_tm)
        {
          impl->inv_world_tm = inverse (WorldTM ());
          
          impl->need_update_inv_world_tm = false;
        }      
        
        local_ray_from = impl->inv_world_tm * ray_from;
        local_ray_to   = impl->inv_world_tm * ray_to;
      
        break;        
      default:
        throw xtl::make_argument_exception ("", "transform_space", transform_space);
    }
    
    static const float EPS = 1e-06f;
    
    math::vec3f local_ray_dir = local_ray_to - local_ray_from;
    float       eps           = EPS * length (local_ray_dir);
    
      //проверка пересечения
      
    bool intersected = false, quasi_intersected = false;

    out_ray_intersection_distance = 0.0f;
    out_ray_to_zone_distance      = 0.0f;
    out_zone_index                = 0;
    out_zone_intersection_point   = math::vec2f (0.0f);    
      
    for (ZoneImplArray::iterator iter=impl->zones_impl.begin (), end=impl->zones_impl.end (); iter!=end; ++iter)
    {
      const ZoneImpl& zone_impl = *iter;
      
      float distance = 0.0f;

      if (!intersect (zone_impl.plane, local_ray_from, local_ray_dir, distance, eps * zone_impl.plane_normal_length))
        continue;
        
      if (intersected && (distance < 0.0f || distance >= out_ray_intersection_distance))
        continue;        
        
      math::vec3f local_intersection = local_ray_from + local_ray_dir * distance,
                  zone_intersection  = zone_impl.inv_basis * local_intersection;            
                  
      if (intersected)
      {
        if (zone_intersection.x < 0.0f || zone_intersection.x > 1.0f)
          continue;
        
        if (zone_intersection.y < 0.0f || zone_intersection.y > 1.0f)
          continue;
      }
      else
      {
        bool is_inside_zone = true;        
        
        math::vec3f normalized_zone_intersection = zone_intersection;
        
        if      (normalized_zone_intersection.x < 0.0f) normalized_zone_intersection.x = 0.0f, is_inside_zone = false;
        else if (normalized_zone_intersection.x > 1.0f) normalized_zone_intersection.x = 1.0f, is_inside_zone = false;        
        if      (normalized_zone_intersection.y < 0.0f) normalized_zone_intersection.y = 0.0f, is_inside_zone = false;
        else if (normalized_zone_intersection.y > 1.0f) normalized_zone_intersection.y = 1.0f, is_inside_zone = false;        

        if (is_inside_zone)
        {
          intersected              = distance >= 0.0f && distance <= 1.0f;
          out_ray_to_zone_distance = 0.0f;
        }
        else
        {
          math::vec3f nearest_intersection = zone_impl.basis * normalized_zone_intersection;
          
          float ray_to_zone_distance = 0.0f;
                     
          switch (transform_space)
          {
            default:            
            case NodeTransformSpace_Local:
              ray_to_zone_distance = length (nearest_intersection - local_intersection);
              break;
            case NodeTransformSpace_Parent:
            {
              const math::mat4f& local_tm = LocalTM ();

              ray_to_zone_distance = length (local_tm * nearest_intersection - local_tm * local_intersection);

              break;
            }
            case NodeTransformSpace_World:
            {
              const math::mat4f& world_tm = LocalTM ();

              ray_to_zone_distance = length (world_tm * nearest_intersection - world_tm * local_intersection);

              break;
            }
          }
          
          if (quasi_intersected && ray_to_zone_distance > out_ray_to_zone_distance)
            continue;
            
          out_ray_to_zone_distance = ray_to_zone_distance;
          quasi_intersected        = true;
        }
      }

      out_ray_intersection_distance = distance;
      out_zone_intersection_point   = math::vec2f (zone_intersection) - math::vec2f (0.5f);
      out_zone_index                = iter - impl->zones_impl.begin ();
    }
    
    return intersected;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::InputZoneModel::IsIntersected");
    throw;
  }
}
