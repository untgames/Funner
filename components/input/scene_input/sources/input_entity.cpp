#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    Конструктор / деструктор
*/

InputEntity::InputEntity (const scene_graph::InputZoneModel& in_zone)
  : zone (in_zone)
{
}

InputEntity::~InputEntity ()
{
}

/*
    Обработка события нажатия
*/

void InputEntity::OnTouch (const TouchEvent& event, const math::vec3f& touch_world_position, size_t zone_index, const math::vec2f& touch_local_position)
{
  printf ("zone='%s', touch=%d, button=%d, state=%d, position=[%.3f, %.3f], world_position=[%.3f, %.3f, %.3f], zone_index=%d, local_position=[%.3f, %.3f]\n",
    zone.Name (), event.touch, event.button, event.state, event.position.x, event.position.y, touch_world_position.x, touch_world_position.y,
    touch_world_position.z, zone_index, touch_local_position.x, touch_local_position.y);
}
