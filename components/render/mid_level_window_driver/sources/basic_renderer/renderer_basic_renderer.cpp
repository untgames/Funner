#include "shared.h"

using namespace common;
using namespace render::mid_level;
using namespace render::mid_level::window_driver;
using namespace render::low_level;

namespace
{

/*
    Переопределения типов
*/

typedef xtl::com_ptr<ITexture> TexturePtr;
typedef xtl::com_ptr<IView>    ViewPtr;

}

/*
    Конструктор / деструктор
*/

RendererDispatch::RendererDispatch (render::low_level::IDevice& in_device)
  : device (&in_device),
    frame_position (frames.end ()),
    frames_count (0)
{
  try
  {
    renderer2d = Renderer2DPtr (new Renderer2D (device.get ()));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::RendererDispatch::RendererDispatch");
    throw;
  }
}

RendererDispatch::~RendererDispatch ()
{
}

/*
    Получение устройства
*/

IDevice& RendererDispatch::GetDevice ()
{
  return *device;
}

/*
   Регистрация/удаление окна
*/

void RendererDispatch::AddFrameBuffer (FrameBuffer* frame_buffer)
{
  try
  {
    if (!frame_buffer)
      throw xtl::make_null_argument_exception ("", "frame_buffer");

    frame_buffers.push_back (frame_buffer);

    FrameBufferCreateNotify (frame_buffers.back ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::RendererDispatch::AddFrameBuffer");
    throw;
  }
}

void RendererDispatch::RemoveFrameBuffer (FrameBuffer* frame_buffer)
{
  for (FrameBufferArray::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
  {
    if (*iter == frame_buffer)
    {
      frame_buffers.erase (iter);

      FrameBufferDestroyNotify (frame_buffer);

      break;
    }
  }
}

/*
   Описание
*/

const char* RendererDispatch::GetDescription ()
{
  return "render::mid_level::window_driver::RendererDispatch";
}

/*
    Количество буферов кадра
*/

size_t RendererDispatch::GetFrameBuffersCount ()
{
  return frame_buffers.size ();
}

IFrameBuffer* RendererDispatch::GetFrameBuffer (size_t index)
{
  if (index >= GetFrameBuffersCount ())
    throw xtl::make_range_exception ("render::mid_level::window_driver::RendererDispatch::GetFrameBuffer", "index", index, frame_buffers.size ());

  return frame_buffers[index];
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

IRenderTarget* RendererDispatch::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return create_render_buffer (*device, width, height, RenderTargetType_DepthStencil);
}

IRenderTarget* RendererDispatch::CreateRenderBuffer (size_t width, size_t height)
{
  return create_render_buffer (*device, width, height, RenderTargetType_Color);
}

IClearFrame* RendererDispatch::CreateClearFrame ()
{
  return new ClearFrame;
}

/*
    Количество кадров / позиция вставки следующего кадра
*/

size_t RendererDispatch::FramesCount ()
{
  return frames_count;
}

void RendererDispatch::SetFramePosition (size_t position)
{
  if (position > frames_count)
    position = frames_count;

  frame_position = frames.begin ();
  
  advance (frame_position, position);
}

size_t RendererDispatch::GetFramePosition ()
{
  return size_t (distance (frames.begin (), frame_position));
}

/*
    Добавление кадра в список отрисовки
*/

void RendererDispatch::AddFrame (IFrame* in_frame)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::RendererDispatch::AddFrame";

  if (!in_frame)
    throw xtl::make_null_argument_exception (METHOD_NAME, "frame");
    
  BasicFrame* frame = dynamic_cast<BasicFrame*> (in_frame);
  
  if (!frame)  
    throw xtl::make_argument_exception (METHOD_NAME, "frame", typeid (in_frame).name (),
      "Frame type incompatible with render::mid_level::window_driver::BasicFrame");
      
  frames.insert (frame_position, frame);  

  frames_count++;  
}

/*
    Конец отрисовки / сброс отрисовки
*/

void RendererDispatch::DrawFrames ()
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
  
    //сброс команд
    
  device->Flush ();

    //вывод сформированной картинки
    
  for (FrameBufferArray::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)  
    (*iter)->Present ();    
}

void RendererDispatch::CancelFrames ()
{
  frames.clear ();
  
  frame_position = frames.end ();
  frames_count   = 0;
}

/*
   Подписка на события системы рендеринга
*/

void RendererDispatch::AttachListener (IRendererListener* listener)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::RendererDispatch::AttachListener";

  if (!listener)
    throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

  ListenerSet::iterator iter = listeners.find (listener);

  if (iter != listeners.end ())
    throw xtl::format_operation_exception (METHOD_NAME, "Listener already attached");

  listeners.insert (listener);
}

void RendererDispatch::DetachListener (IRendererListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::mid_level::window_driver::RendererDispatch::DetachListener", "listener");

  listeners.erase (listener);
}

/*
   Создание ресурсов
*/

render::mid_level::renderer2d::ITexture* RendererDispatch::CreateTexture (const media::Image& image)
{
  return renderer2d->CreateTexture (image);
}

render::mid_level::renderer2d::ITexture* RendererDispatch::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  return renderer2d->CreateTexture (width, height, pixel_format);
}

render::mid_level::renderer2d::IPrimitive* RendererDispatch::CreatePrimitive ()
{
  return renderer2d->CreatePrimitive ();
}

render::mid_level::renderer2d::IFrame* RendererDispatch::CreateFrame ()
{
  return renderer2d->CreateFrame ();
}

/*
   Оповещения
*/

void RendererDispatch::FrameBufferCreateNotify (IFrameBuffer* frame_buffer)
{
  for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
    (*iter)->OnFrameBufferCreate (frame_buffer);
}

void RendererDispatch::FrameBufferDestroyNotify (IFrameBuffer* frame_buffer)
{
  for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
    (*iter)->OnFrameBufferDestroy (frame_buffer);
}

void RendererDispatch::FrameBufferUpdateNotify (IFrameBuffer* frame_buffer)
{
  for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
    (*iter)->OnFrameBufferUpdate (frame_buffer);
}

void RendererDispatch::FrameBufferResizeNotify (IFrameBuffer* frame_buffer, size_t width, size_t height)
{
  for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
    (*iter)->OnFrameBufferResize (frame_buffer, width, height);
}
