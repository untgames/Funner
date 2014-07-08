#include "shared.h"

using namespace scene_graph;
using namespace input;

namespace
{

const int RESERVE_TOUCHES_COUNT = 5; //резервируемое число одновременных нажатий для обработки

}

/*
    Конструктор / деструктор
*/

InputEntity::InputEntity (const scene_graph::InputZoneModel& in_zone, InputScene& in_scene)
  : zone (in_zone)
  , scene (in_scene)
  , has_screen_handlers (false)
  , wait_for_release_event (false)
{
  touches.reserve (RESERVE_TOUCHES_COUNT);
  
  on_notifications_changed_connection = zone.RegisterEventHandler (InputZoneEvent_AfterNotificationAdded, xtl::bind (&InputEntity::UpdateNotifications, this));
  
  UpdateNotifications ();
}

InputEntity::~InputEntity ()
{
}

/*
    Ведение таблицы нажатий
*/

InputEntity::TouchDesc* InputEntity::AddTouch (InputPort& port, touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->port == &port && iter->touch == touch && iter->button == button)
      return &*iter;
      
  touches.push_back (TouchDesc (port, touch, button));
  
  wait_for_release_event = true;
  
  UpdateBroadcasts ();
  
  return &touches.back ();
}

InputEntity::TouchDesc* InputEntity::FindTouch (InputPort& port, touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->port == &port && iter->touch == touch && iter->button == button)
      return &*iter;
      
  return 0;
}

void InputEntity::RemoveTouch (InputPort& port, touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->port == &port && iter->touch == touch && iter->button == button)
    {
      touches.erase (iter);
      
      wait_for_release_event = !touches.empty ();
      
      UpdateBroadcasts ();
      
      return;
    }
}

/*
    Удаление всех нажатий, связанных с указанной областью ввода
*/

void InputEntity::RemoveAllTouches (InputPort& port)
{
  for (TouchDescArray::iterator iter=touches.begin (); iter!=touches.end ();)
    if (iter->port == &port) touches.erase (iter);
    else                     ++iter;
}

/*
    Оповещение
*/

void InputEntity::Notify (InputPort& input_port, InputZoneNotification notification, const InputZoneNotificationContext& context)
{
  if (!zone.Scene ())
    return;
    
  zone.Notify (input_port.AttachedViewport (), notification, context);

  if (context.button == 0)
  {
    switch (notification)
    {
      case InputZoneNotification_OnTouchMove:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnPressMove, context);
        break;
      case InputZoneNotification_OnTouchDown:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnPress, context);
        break;        
      case InputZoneNotification_OnTouchClick:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnClick, context);
        break;        
      case InputZoneNotification_OnTouchLeave:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnPressLeave, context);
        break;
      case InputZoneNotification_OnTouchEnter:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnPressEnter, context);
        break;        
      default:
        break;
    }
  }
}

/*
    Обработка события
*/

void InputEntity::OnTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position, size_t zone_index, const math::vec2f& touch_local_position)
{
  try
  {
    InputEntityPtr self_lock (this);

    xtl::com_ptr<const scene_graph::InputZoneModel> zone_lock (&zone);

      //формирование контекста события
      
    const InputZoneModel::ZoneDesc& desc = zone.ZoneDescs ()[zone_index];
    
    if (zone_index >= zone.ZoneDescsCount ())
      throw xtl::make_range_exception ("", "zone_index", zone_index, zone.ZoneDescsCount ());          
      
    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    context.touch_local_position = desc.position + touch_local_position.x * desc.axis_x + touch_local_position.y * desc.axis_y;

      //проверка: зафиксировано ли нажатие на зону

    TouchDesc* touch_desc = FindTouch (input_port, event.touch, event.button);    
    
    if (touch_desc)
    {     
      touch_desc->on_touch_method_entered = true;
      
      switch (event.state)
      {
        case TouchState_Moving:
          if (touch_desc->is_inside) Notify (input_port, InputZoneNotification_OnTouchMove, context);
          else                       Notify (input_port, InputZoneNotification_OnTouchEnter, context);
          
          touch_desc->is_inside = true;
          
          break;
        case TouchState_Pressed:
          //ignored
          break;
        case TouchState_Released:
          Notify (input_port, InputZoneNotification_OnTouchUpInside, context);
          
          if (touch_desc->touch_check)          
            Notify (input_port, InputZoneNotification_OnTouchClick, context);          

          Notify (input_port, InputZoneNotification_OnTouchLeave, context);
          
          RemoveTouch (input_port, event.touch, event.button);

          break;
        default:
          break;        
      }
    }
    else
    {      
      switch (event.state)
      {
        case TouchState_Moving:
          AddTouch (input_port, event.touch, event.button);              
          Notify   (input_port, InputZoneNotification_OnTouchEnter, context);                              
          break;
        case TouchState_Pressed:
          touch_desc = AddTouch (input_port, event.touch, event.button);              
          
          touch_desc->touch_check = true;

          Notify (input_port, InputZoneNotification_OnTouchEnter, context);          
          Notify (input_port, InputZoneNotification_OnTouchDown, context);
                    
          break;
        case TouchState_Released:
          Notify (input_port, InputZoneNotification_OnTouchUpInside, context);
          break;
        default:
          break;        
      }        
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::OnTouch");
    throw;
  }
}

void InputEntity::OnBroadcastTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position)
{
  try
  {    
    if (!has_screen_handlers && !wait_for_release_event)
      return;

    InputEntityPtr self_lock (this);
      
    xtl::com_ptr<const scene_graph::InputZoneModel> zone_lock (&zone);
      
      //подготовка контекста оповещения

    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    
      //экранные оповещения

    switch (event.state)
    {
      case TouchState_Moving:
        Notify (input_port, InputZoneNotification_OnScreenTouchMove, context);
        break;
      case TouchState_Pressed:
        Notify (input_port, InputZoneNotification_OnScreenTouchDown, context);
        break;
      case TouchState_Released:
        Notify (input_port, InputZoneNotification_OnScreenTouchUp, context);
        break;
      default:
        break;
    }
    
      //зависимые оповещения
      
    TouchDesc* touch_desc = FindTouch (input_port, event.touch, event.button);
      
    if (touch_desc)
    {      
      if (!touch_desc->on_touch_method_entered)
      {
        switch (event.state)
        {
          case TouchState_Moving:
            if (touch_desc->is_inside)
            {
              Notify (input_port, InputZoneNotification_OnTouchLeave, context);
              
              touch_desc->is_inside = false;
            }
            
            break;
          case TouchState_Pressed:
            //ignored
            break;
          case TouchState_Released:
            if (touch_desc->touch_check)
              Notify (input_port, InputZoneNotification_OnTouchUpOutside, context);

            RemoveTouch (input_port, event.touch, event.button);
            break;
          default:
            break;
        }
      }
      else
      {
        touch_desc->on_touch_method_entered = false;
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::OnBroadcastTouch");
    throw;
  }
}

void InputEntity::UpdateBroadcasts ()
{
  if (IsAttached ())
  {
    if (!(wait_for_release_event || has_screen_handlers))
      Detach ();
  }
  else
  {
    if (wait_for_release_event || has_screen_handlers)
      scene.RegisterBroadcastListener (*this);
  }
}

/*
    Обновление списка оповещений
*/

void InputEntity::UpdateNotifications ()
{
  try
  {
      //проверка наличия экранных обработчиков
    
    has_screen_handlers  = false;
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchDown);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchUp);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchMove);

      //обновление подписки на броадкасты
      
    UpdateBroadcasts ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::UpdateNotifications");
    throw;
  }
}
