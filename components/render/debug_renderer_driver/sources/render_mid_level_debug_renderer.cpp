#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

Renderer::Renderer ()  
{
  color_buffer         = RenderTargetPtr (CreateRenderBuffer (), false);
  depth_stencil_buffer = RenderTargetPtr (CreateDepthStencilBuffer (), false);
}

/*
   Описание
*/

const char* Renderer::GetDescription ()
{
  return "render::mid_level::debug::Renderer";
}

/*
    Внутренний идентификатор пула ресурсов
      (необходим для совместного использования ресурсов, созданных на разных IRenderer)    
*/

size_t Renderer::GetResourcePoolId ()
{
  return 0;
}

/*
    Получение буфера цвета и буфера попиксельного отсечения
*/

IRenderTarget* Renderer::GetColorBuffer ()
{
  return color_buffer.get ();
}

IRenderTarget* Renderer::GetDepthStencilBuffer ()
{
  return depth_stencil_buffer.get ();
}

/*
    Создание ресурсов
*/

IRenderTarget* Renderer::CreateDepthStencilBuffer ()
{
  return new RenderTarget (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_DepthStencil);
}

IRenderTarget* Renderer::CreateRenderBuffer ()
{
  return new RenderTarget (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_Color);
}
