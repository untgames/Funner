#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;
using namespace render::low_level;

namespace
{

/*
    Переопределения типов
*/

typedef xtl::intrusive_ptr<RenderTarget> RenderTargetPtr;
typedef xtl::com_ptr<ISwapChain>         SwapChainPtr;
typedef xtl::com_ptr<ITexture>           TexturePtr;
typedef xtl::com_ptr<IView>              ViewPtr;

}

/*
    Буфер кадра
*/

struct BasicRenderer::FrameBuffer
{
  SwapChainPtr    swap_chain;           //цепочка обмена
  RenderTargetPtr color_buffer;         //буфер цвета
  RenderTargetPtr depth_stencil_buffer; //буфер попиксельного отсечения

  BasicRenderer::FrameBuffer::FrameBuffer (IDevice& device, ISwapChain& in_swap_chain) : swap_chain (&in_swap_chain)
  {
    try
    {
        //получение дескриптора цепочки обмена

      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);
      
      TexturePtr texture;
      ViewPtr    view;
      ViewDesc   view_desc;

      memset (&view_desc, 0, sizeof view_desc);
      
        //создание буфера цвета

      texture      = TexturePtr (device.CreateRenderTargetTexture (swap_chain.get (), swap_chain_desc.buffers_count ? 1 : 0), false);
      view         = ViewPtr (device.CreateView (texture.get (), view_desc), false);
      color_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_Color), false);
      
        //создание буфера попиксельного отсечения

      texture              = TexturePtr (device.CreateDepthStencilTexture (swap_chain.get ()), false);
      view                 = ViewPtr (device.CreateView (texture.get (), view_desc), false);
      depth_stencil_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_DepthStencil), false);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::mid_level::low_level_driver::BasicRenderer::BasicRenderer::FrameBuffer");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

BasicRenderer::BasicRenderer (IDevice* in_device, size_t swap_chains_count, ISwapChain** swap_chains)
  : device (in_device),
    frame_position (frames.end ()),
    frames_count (0)
{
  try
  {
      //проверка аргументов

    if (!in_device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!swap_chains_count)
      throw xtl::make_null_argument_exception ("", "swap_chains_count");

    if (!swap_chains)
      throw xtl::make_null_argument_exception ("", "swap_chains");

      //создание буферов кадра

    frame_buffers.reserve (swap_chains_count);

    for (size_t i=0; i<swap_chains_count; i++)
    {
      ISwapChain* swap_chain = swap_chains [i];

      if (!swap_chain)
        throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument 'swap_chains[%u]'", i);

      frame_buffers.push_back (FrameBuffer (*device, *swap_chain));
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::BasicRenderer::BasicRenderer");
    throw;
  }
}

BasicRenderer::~BasicRenderer ()
{
}

/*
   Описание
*/

const char* BasicRenderer::GetDescription ()
{
  return "render::mid_level::low_level_driver::BasicRenderer";
}

/*
    Количество буферов кадра
*/

size_t BasicRenderer::GetFrameBuffersCount ()
{
  return frame_buffers.size ();
}

/*
    Получение буфера цвета и буфера попиксельного отсечения
*/

IRenderTarget* BasicRenderer::GetColorBuffer (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::BasicRenderer::GetColorBuffer", "index", index, frame_buffers.size ());

  return frame_buffers [index].color_buffer.get ();
}

IRenderTarget* BasicRenderer::GetDepthStencilBuffer (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::BasicRenderer::GetDepthStencilBuffer", "index", index, frame_buffers.size ());

  return frame_buffers [index].depth_stencil_buffer.get ();
}

/*
    Создание ресурсов
*/

namespace
{

//создание целевого буфера рендеринга
RenderTarget* create_render_buffer (IDevice& device, size_t width, size_t height, RenderTargetType type)
{
    //создание текстуры

  TextureDesc texture_desc;
  
  memset (&texture_desc, 0, sizeof texture_desc);
  
  texture_desc.dimension  = TextureDimension_2D;
  texture_desc.width      = width;
  texture_desc.height     = height;
  texture_desc.layers     = 1;
  texture_desc.usage_mode = UsageMode_Default;

  switch (type)
  {
    default:
    case RenderTargetType_Color:
      texture_desc.format     = PixelFormat_RGBA8;
      texture_desc.bind_flags = BindFlag_RenderTarget;

      break;
    case RenderTargetType_DepthStencil:
      texture_desc.format     = PixelFormat_D24S8;
      texture_desc.bind_flags = BindFlag_DepthStencil;

      break;
  }

  TexturePtr texture (device.CreateTexture (texture_desc), false);

    //создание отображения

  ViewDesc view_desc;

  memset (&view_desc, 0, sizeof view_desc);

  ViewPtr view (device.CreateView (texture.get (), view_desc), false);

    //создание целевого буфера рендеринга

  return new RenderTarget (view.get (), type);
}

}

IRenderTarget* BasicRenderer::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return create_render_buffer (*device, width, height, RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer (size_t width, size_t height)
{
  return create_render_buffer (*device, width, height, RenderTargetType_Color);
}

IClearFrame* BasicRenderer::CreateClearFrame ()
{
  return new ClearFrame;
}

/*
    Количество кадров / позиция вставки следующего кадра
*/

size_t BasicRenderer::FramesCount ()
{
  return frames_count;
}

void BasicRenderer::SetFramePosition (size_t position)
{
  if (position > frames_count)
    position = frames_count;

  frame_position = frames.begin ();
  
  advance (frame_position, position);
}

size_t BasicRenderer::GetFramePosition ()
{
  return size_t (distance (frames.begin (), frame_position));
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
      
  frames.insert (frame_position, frame);  

  frames_count++;  
}

/*
    Конец отрисовки / сброс отрисовки
*/

void BasicRenderer::DrawFrames ()
{
  if (frames.empty ())
    return;
    
    //отрисовка кадров    

  for (FrameList::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    (*iter)->Draw (device.get ());
  }

    //очистка списка кадров

  CancelFrames ();

    //вывод сформированной картинки
    
  for (FrameBufferArray::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
    iter->swap_chain->Present ();
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
  
  frame_position = frames.end ();
  frames_count   = 0;
}
