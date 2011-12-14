#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;
using namespace render::mid_level::window_driver;

/*
    Конструктор / деструктор
*/

LowLevelFrame::LowLevelFrame ()
  : draw_callback (0)
{
}

LowLevelFrame::~LowLevelFrame ()
{
}

/*
    Установка интерфейса обратного вызова
*/

void LowLevelFrame::SetCallback (IDrawCallback* new_draw_callback)
{
  draw_callback = new_draw_callback;
}

/*
    Реализация визуализации
*/

void LowLevelFrame::DrawCore (render::low_level::IDevice* device)
{
  if (!draw_callback)
    return; //виузализация не требуется

    //установка области вывода
    
  int viewport_offset_x = 0, viewport_offset_y = 0;

  BasicFrame::BindViewport (device, viewport_offset_x, viewport_offset_y);

    //визуализация

  draw_callback->Draw (*device);
}
