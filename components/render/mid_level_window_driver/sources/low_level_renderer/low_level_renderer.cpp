#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    Описание реализации низкоуровневой системы визуализации
*/

struct LowLevelRenderer::Impl
{
};

/*
    Конструктор / деструктор
*/

LowLevelRenderer::LowLevelRenderer (render::low_level::IDevice*)
  : impl (new Impl)
{
}

LowLevelRenderer::~LowLevelRenderer ()
{
}

/*
    Создание ресурсов
*/

render::mid_level::ILowLevelFrame* LowLevelRenderer::CreateFrame ()
{
  return new LowLevelFrame;
}
