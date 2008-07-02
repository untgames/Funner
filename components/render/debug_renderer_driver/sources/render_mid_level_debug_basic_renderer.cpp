#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

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

BasicRenderer::BasicRenderer ()  
  : frame_id (0)
{
  color_buffer         = RenderTargetPtr (CreateRenderBuffer (), false);
  depth_stencil_buffer = RenderTargetPtr (CreateDepthStencilBuffer (), false);  
  
  frames.reserve (FRAME_ARRAY_RESERVE_SIZE);
}

/*
   Описание
*/

const char* BasicRenderer::GetDescription ()
{
  return "render::mid_level::debug::BasicRenderer";
}

/*
    Внутренний идентификатор пула ресурсов
      (необходим для совместного использования ресурсов, созданных на разных IBasicRenderer)    
*/

size_t BasicRenderer::GetResourcePoolId ()
{
  return 0;
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
  return new RenderTarget (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_DepthStencil);
}

IRenderTarget* BasicRenderer::CreateRenderBuffer ()
{
  return new RenderTarget (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_Color);
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
      
  frames.push_back (frame);
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

  for (FrameArray::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
  {
    BasicFrame& frame = **iter;
    
    frame.Draw ();
  }
  
    //конец отрисовки
    
  log.Printf ("End draw frames (id=%u, frame_id=%u)", Id (), frame_id);
    
    //увеличение счётчика кадров
    
  frame_id++;
}

void BasicRenderer::CancelFrames ()
{
  frames.clear ();
  
  log.Printf ("Renderer cancel frames (id=%u)", Id ());
}
