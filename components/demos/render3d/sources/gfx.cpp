#include "shared.h"

using namespace scene_graph;

namespace
{

const char* SHOT_TRAIL_MATERIAL = "shot_trail";
const float SHOT_TRAIL_DURATION = 4.f;
const float SHOT_TRAIL_SIZE     = 1.f;

}

//Эффект выстрела

///Создание системы частиц
ShotGfx::Pointer ShotGfx::Create (const math::vec4f& color, float distance)
{
  return Pointer (new ShotGfx (color, distance), false);
}

///Конструктор / деструктор
ShotGfx::ShotGfx (const math::vec4f& in_color, float distance)
  : color (in_color), life_time (0.f)
{
  update_connection = this->AttachController (xtl::bind (&ShotGfx::UpdateEffect, this, _1));

  SetMaterial (SHOT_TRAIL_MATERIAL);

  Resize (1);

  scene_graph::SpriteModel::SpriteDesc *sprite_desc = Sprites ();

  sprite_desc->position = 0.f;
  sprite_desc->size     = math::vec2f (distance, SHOT_TRAIL_SIZE);
  sprite_desc->color    = color;
}

ShotGfx::~ShotGfx ()
{
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
  life_time += dt;

  if (life_time > SHOT_TRAIL_DURATION)
    Unbind ();

  scene_graph::SpriteModel::SpriteDesc *sprite_desc = Sprites ();

  sprite_desc->color.w = color.w * (SHOT_TRAIL_DURATION - life_time) / SHOT_TRAIL_DURATION;
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
