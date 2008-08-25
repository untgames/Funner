#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

BasicRenderer::BasicRenderer ()
  : frame_position (frames.end ()),
    frames_count (0),
    frame_id (0)
{
  frame_buffer = FrameBufferPtr (new FrameBuffer, false);
}

BasicRenderer::~BasicRenderer ()
{
    //оповещение об удалении буфера кадра
    
  try
  {
    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      (*iter)->OnFrameBufferDestroy (frame_buffer.get ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
   Описание
*/

const char* BasicRenderer::GetDescription ()
{
  return "render::mid_level::debug::BasicRenderer";
}

/*
    Перебор буферов кадра
*/

size_t BasicRenderer::GetFrameBuffersCount ()
{
  return 1;
}

IFrameBuffer* BasicRenderer::GetFrameBuffer (size_t index)
{
  return frame_buffer.get ();
}

/*
    Создание ресурсов
*/

IRenderTarget* BasicRenderer::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return new RenderBuffer (width, height, RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer (size_t width, size_t height)
{
  return new RenderBuffer (width, height, RenderTargetType_Color);
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
  static const char* METHOD_NAME = "render::mid_level::debug::BasicRenderer::AddFrame";

  if (!in_frame)
    throw xtl::make_null_argument_exception (METHOD_NAME, "frame");
    
  BasicFrame* frame = dynamic_cast<BasicFrame*> (in_frame);      
  
  if (!frame)  
    throw xtl::make_argument_exception (METHOD_NAME, "frame", typeid (in_frame).name (),
      "Frame type incompatible with render::mid_level::debug::BasicFrame");            

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
        
    //начало отрисовки
    
  log.Printf ("Begin draw frames (id=%u, frame_id=%u)", Id (), frame_id);

    //отрисовка кадров

  for (FrameList::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    BasicFrame& frame = **iter;
    
    frame.Draw ();
  }
  
    //очистка списка кадров
    
  frames.clear ();

  frame_position = frames.end ();
  frames_count   = 0;

    //конец отрисовки
    
  log.Printf ("End draw frames (id=%u, frame_id=%u)", Id (), frame_id);
    
    //увеличение счётчика кадров
    
  frame_id++;
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
  
  frame_position = frames.end ();
  frames_count   = 0;
  
  log.Printf ("Renderer cancel frames (id=%u)", Id ());
}

/*
    Подписка на события системы рендеринга
*/

void BasicRenderer::AttachListener (IRendererListener* listener)
{
  listeners.push_back (listener);
}

void BasicRenderer::DetachListener (IRendererListener* listener)
{
  listeners.erase (stl::remove (listeners.begin (), listeners.end (), listener), listeners.end ());
}
