#include "shared.h"

using namespace scene_graph;

namespace
{

const char*  SHOT_TRAIL_MATERIAL = "shot_trail";
const size_t SHOT_TRAIL_DURATION = 1000;
const float  SHOT_TRAIL_SIZE     = 1.f;

const size_t SHOTS_RESERVE_COUNT = 100;

}

//Эффект выстрела

///Создание системы частиц
ShotGfx::Pointer ShotGfx::Create ()
{
  return Pointer (new ShotGfx (), false);
}

///Конструктор / деструктор
ShotGfx::ShotGfx ()
  : time (0)
{
  update_connection = this->AttachController (xtl::bind (&ShotGfx::UpdateEffect, this, _1));

  shots.reserve (SHOTS_RESERVE_COUNT);
  Reserve       (SHOTS_RESERVE_COUNT);
  SetMaterial   (SHOT_TRAIL_MATERIAL);
}

ShotGfx::~ShotGfx ()
{
}

///Добавление выстрела
void ShotGfx::AddShot (const scene_graph::Node& gun, const math::vec4f& color, float distance)
{
  ShotDescPtr shot_desc (new ShotDesc, false);

  shot_desc->position  = gun.WorldPosition () + gun.WorldOrtZ () * distance / 2.f;
  shot_desc->direction = gun.WorldOrtZ ();
  shot_desc->color     = color;
  shot_desc->distance  = distance;
  shot_desc->born_time = time;

  shots.push_back (shot_desc);
}

///Получение направления выстрела
const math::vec3f& ShotGfx::ShotDirection (size_t sprite) const
{
  if (sprite > shots.size ())
    throw xtl::make_range_exception ("ShotGfx::ShotDirection", "sprite", sprite, 0u, shots.size ());

  return shots [sprite]->direction;
}

///Метод, вызываемый при посещении данного объекта
void ShotGfx::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    SpriteList::AcceptCore (visitor);
}

///Обновление эффекта
void ShotGfx::UpdateEffect (float dt)
{
  time += dt * 1000.f;

  for (int i = shots.size () - 1; i >= 0; i--)
  {
    if (time - shots [i]->born_time > SHOT_TRAIL_DURATION)
      shots.erase (shots.begin () + i);
  }

  Resize (shots.size ());

  scene_graph::SpriteModel::SpriteDesc *sprite_desc = Sprites ();

  for (ShotsArray::iterator iter = shots.begin (), end = shots.end (); iter != end; ++iter, sprite_desc++)
  {
    size_t shot_time = time - (*iter)->born_time;

    sprite_desc->position = (*iter)->position;
    sprite_desc->size     = math::vec2f ((*iter)->distance, SHOT_TRAIL_SIZE);
    sprite_desc->color    = (*iter)->color;
    sprite_desc->color.w  = (*iter)->color.w * (SHOT_TRAIL_DURATION - shot_time) / SHOT_TRAIL_DURATION;
  }
}

//Биллбоард

///Создание
BillboardSpriteList::Pointer BillboardSpriteList::Create ()
{
  return Pointer (new BillboardSpriteList (), false);
}

///Конструктор / деструктор
BillboardSpriteList::BillboardSpriteList ()
{
}

BillboardSpriteList::~BillboardSpriteList ()
{
}

///Метод, вызываемый при посещении данного объекта
void BillboardSpriteList::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    SpriteList::AcceptCore (visitor);
}
