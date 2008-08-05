#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    Константы
*/

namespace
{

const size_t FRAME_ARRAY_RESERVE_SIZE = 128; //резервируемое число кадров

}

/*
    Конструктор
*/

BasicRenderer::BasicRenderer (render::low_level::IDevice* in_device, render::low_level::ISwapChain* in_swap_chain)
  : device (in_device), swap_chain (in_swap_chain)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::BasicRenderer::BasicRenderer";

  if (!in_device)
    throw xtl::make_null_argument_exception (METHOD_NAME, "device");

  if (!in_swap_chain)
    throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

  static size_t free_pool_id = 1;

  resource_pool_id = free_pool_id++;

  if (!free_pool_id)
  {
    --free_pool_id;
    throw xtl::format_operation_exception (METHOD_NAME, "No free resource pool");
  }
  
  color_buffer         = RenderTargetPtr (CreateRenderBuffer (), false);
  depth_stencil_buffer = RenderTargetPtr (CreateDepthStencilBuffer (), false);  

  frames.reserve (FRAME_ARRAY_RESERVE_SIZE);
}

/*
   Описание
*/

const char* BasicRenderer::GetDescription ()
{
  return "render::mid_level::low_level_driver::BasicRenderer";
}

/*
    Внутренний идентификатор пула ресурсов
      (необходим для совместного использования ресурсов, созданных на разных IBasicRenderer)    
*/

size_t BasicRenderer::GetResourcePoolId ()
{
  return resource_pool_id;
}

/*
    Получение буфера цвета и буфера попиксельного отсечения
*/

IRenderTarget* BasicRenderer::GetColorBuffer ()
{
  return color_buffer.get ();
}

IRenderTarget* BasicRenderer::GetDepthStencilBuffer ()
{
  return depth_stencil_buffer.get ();
}

/*
    Создание ресурсов
*/

IRenderTarget* BasicRenderer::CreateDepthStencilBuffer ()
{
  using namespace render::low_level;

  xtl::com_ptr<ITexture> depth_stencil_texture (device->CreateDepthStencilTexture (swap_chain.get ()), false);

  ViewDesc depth_stencil_view_desc;

  memset (&depth_stencil_view_desc, 0, sizeof (depth_stencil_view_desc));

  depth_stencil_view_desc.layer     = 0;
  depth_stencil_view_desc.mip_level = 0;

  xtl::com_ptr<IView> depth_stencil_view (device->CreateView (depth_stencil_texture.get (), depth_stencil_view_desc), false);
  
  return new RenderTarget (depth_stencil_view.get (), RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer ()
{
  using namespace render::low_level;

  xtl::com_ptr<ITexture> render_target_texture (device->CreateRenderTargetTexture (swap_chain.get (), 1), false);

  ViewDesc render_target_view_desc;

  memset (&render_target_view_desc, 0, sizeof (render_target_view_desc));

  render_target_view_desc.layer     = 0;
  render_target_view_desc.mip_level = 0;

  xtl::com_ptr<IView> render_target_view (device->CreateView (render_target_texture.get (), render_target_view_desc), false);
  
  return new RenderTarget (render_target_view.get (), RenderTargetType_Color);
}

IClearFrame* BasicRenderer::CreateClearFrame ()
{
  return new ClearFrame;
}

/*
   Добавление кадра в список отрисовки
*/

void BasicRenderer::AddFrame (IFrame* in_frame)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::BasicRenderer::AddFrame";

  if (!in_frame)
    throw xtl::make_null_argument_exception (METHOD_NAME, "frame");
    
  BasicFrame* frame = dynamic_cast<BasicFrame*> (in_frame);
  
  if (!frame)  
    throw xtl::make_argument_exception (METHOD_NAME, "frame", typeid (in_frame).name (),
      "Frame type incompatible with render::mid_level::low_level_driver::BasicFrame");
      
  frames.push_back (frame);
}

/*
    Конец отрисовки / сброс отрисовки
*/

void BasicRenderer::DrawFrames ()
{
  if (frames.empty ())
    return;
    
    //отрисовка кадров

  for (FrameArray::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    (*iter)->Draw (device.get ());
  }

    //очистка списка кадров

  frames.clear ();

    //вывод сформированной картинки    

  swap_chain->Present ();
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
}
