#include "shared.h"

using namespace scene_graph;
using namespace input;

namespace
{

const int RESERVE_TOUCHES_COUNT = 5; //������������� ����� ������������� ������� ��� ���������

}

/*
    ����������� / ����������
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
    ������� ������� �������
*/

void InputEntity::AddTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
      return;
      
  touches.push_back (TouchDesc (touch, button));
  
  wait_for_release_event = true;
  
  UpdateBroadcasts ();
}

bool InputEntity::HasTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
      return true;
      
  return false;
}

void InputEntity::RemoveTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
    {
      touches.erase (iter);
      
      wait_for_release_event = !touches.empty ();
      
      UpdateBroadcasts ();
      
      return;
    }
}

/*
    ��������� �������
*/

void InputEntity::OnTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position, size_t zone_index, const math::vec2f& touch_local_position)
{
  try
  {
      //������������ ��������� �������
      
    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    context.touch_local_position = touch_local_position;
    
      //��������: ������������� �� ������� �� ����

    bool has_touch = HasTouch (event.touch, event.button);    
    
    if (has_touch)
    {
        //��������� ������ ����� ������� �� ����
      
      switch (event.state)
      {
        case TouchState_Moving:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchMove, context);      
          break;
        case TouchState_Pressed:
          //ignored
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpInside, context);
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchClick, context);
          
          RemoveTouch (event.touch, event.button);

          break;
        default:
          break;        
      }
    }
    else
    {
        //��������� ������ ��� ������� �� ����        
        
      AddTouch (event.touch, event.button); ////???hover?????

      switch (event.state)
      {
        case TouchState_Moving:          
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchEnter, context);
          break;
        case TouchState_Pressed:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchDown, context);
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpInside, context);
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
      
      //���������� ��������� ����������

    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    
      //�������� ����������

    switch (event.state)
    {
      case TouchState_Moving:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchMove, context);
        break;
      case TouchState_Pressed:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchDown, context);
        break;
      case TouchState_Released:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchUp, context);
        break;
      default:
        break;
    }
    
      //��������� ����������
      
    if (HasTouch (event.touch, event.button))
    {
      switch (event.state)
      {
        case TouchState_Moving:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchLeave, context);
          ///????????remove????????????
          break;
        case TouchState_Pressed:
          //ignored
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpOutside, context);
          RemoveTouch (event.touch, event.button);
          break;
        default:
          break;
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
    ���������� ������ ����������
*/

void InputEntity::UpdateNotifications ()
{
  try
  {
      //�������� ������� �������� ������������
    
    has_screen_handlers  = false;
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchDown);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchUp);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchMove);

      //���������� �������� �� ����������
      
    UpdateBroadcasts ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::UpdateNotifications");
    throw;
  }
}
