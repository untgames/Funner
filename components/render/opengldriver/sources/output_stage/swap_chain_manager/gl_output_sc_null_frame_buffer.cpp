#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

SwapChainNullFrameBuffer::SwapChainNullFrameBuffer (const FrameBufferManager& in_frame_buffer_manager)
  : ContextObject (in_frame_buffer_manager.GetContextManager ()),
    frame_buffer_manager (in_frame_buffer_manager)
  {}

/*
    Установка буфера в контекст OpenGL
*/

void SwapChainNullFrameBuffer::Bind ()
{
  try
  {
    frame_buffer_manager.SetFrameBuffer         (GetContextManager ().GetContextId (), 0, GL_NONE);
    frame_buffer_manager.SetFrameBufferActivity (false, false);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainNullFrameBuffer::Bind");
    throw;
  }
}
