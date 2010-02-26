#include "shared.h"

struct GfxManager::Impl
{
  Test&            test;
  ShotGfx::Pointer shot_sprites;

  Impl (Test& in_test)
    : test (in_test)
  {
    shot_sprites = ShotGfx::Create ();

    shot_sprites->BindToScene (test.scene_manager.Scene (), NodeBindMode_AddRef);
  }

  void PerformShot (const scene_graph::Node& gun, const math::vec4f& color, float distance)
  {
    shot_sprites->AddShot (gun, color, distance);
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
