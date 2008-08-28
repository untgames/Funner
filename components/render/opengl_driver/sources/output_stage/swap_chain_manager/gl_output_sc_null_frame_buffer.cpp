#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

SwapChainNullFrameBuffer::SwapChainNullFrameBuffer (const FrameBufferManager& in_frame_buffer_manager)
  : ContextObject (in_frame_buffer_manager.GetContextManager ()),
    frame_buffer_manager (in_frame_buffer_manager)
  {}

/*
    ��������� ������ � �������� OpenGL
*/

void SwapChainNullFrameBuffer::Bind ()
{
  try
  {
    frame_buffer_manager.SetFrameBuffer         ((ISwapChain*)0, GL_NONE);
    frame_buffer_manager.SetFrameBufferActivity (false, false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainNullFrameBuffer::Bind");
    throw;
  }
}
