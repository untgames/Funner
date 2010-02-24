#include "shared.h"

struct GfxManager::Impl
{
  Test& test;

  Impl (Test& in_test) : test (in_test) {}

  void PerformShot (const scene_graph::Node& gun, const math::vec4f& color, float distance)
  {
    ShotGfx::Pointer shot_gfx (ShotGfx::Create (color, distance));

    shot_gfx->SetShotDirection (gun.WorldOrtZ ());
    shot_gfx->SetPosition      (gun.WorldPosition () + gun.WorldOrtZ () * distance / 2.f);

    shot_gfx->BindToScene (test.scene_manager.Scene (), NodeBindMode_AddRef);
  }
};

/*
   Конструктор/деструктор
*/

GfxManager::GfxManager (Test& test)
  : impl (new Impl (test))
  {}

GfxManager::~GfxManager ()
{
  delete impl;
}

/*
   Выстрел из пушки
*/

void GfxManager::PerformShot (const scene_graph::Node& gun, const math::vec4f& color, float distance)
{
  impl->PerformShot (gun, color, distance);
}
