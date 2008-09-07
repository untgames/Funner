#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

FboFrameBufferManager::FboFrameBufferManager (const FrameBufferManager& in_frame_buffer_managaer)
  : frame_buffer_manager (in_frame_buffer_managaer)
{
}

/*
    Создание буферов рендеринга
*/

ITexture* FboFrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  try
  {
    return new FboRenderBuffer (frame_buffer_manager, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBufferManager::CreateRenderBuffer");
    throw;
  }
}

/*
    Создание буферов кадра
*/

//проверка возможности создания буфера кадра
bool FboFrameBufferManager::IsSupported (View* color_view, View* depth_stencil_view)
{
  View* views [2] = {color_view, depth_stencil_view};

  for (int i=0; i<2; i++)
    if (views [i])
    {
      ITexture* base_texture = views [i]->GetTexture ();

      if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<FboRenderBuffer*> (base_texture))
        return false;
    }

  return true;
}

//создание буфера кадра
IFrameBuffer* FboFrameBufferManager::CreateFrameBuffer (View* color_view, View* depth_stencil_view)
{
  try
  {
    return new FboFrameBuffer (frame_buffer_manager, color_view, depth_stencil_view);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBufferManager::CreateFrameBuffer");
    throw;
  }
}

/*
    Регистрация обработчиков создания буферов рендеринга и буферов кадра
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void register_fbo_manager (FrameBufferManager& frame_buffer_manager)
{
  xtl::shared_ptr<FboFrameBufferManager> manager (new FboFrameBufferManager (frame_buffer_manager));

  frame_buffer_manager.RegisterCreater (RenderBufferCreater (xtl::bind (&FboFrameBufferManager::CreateRenderBuffer, manager, _1)));
  frame_buffer_manager.RegisterCreater (&FboFrameBufferManager::IsSupported, xtl::bind (&FboFrameBufferManager::CreateFrameBuffer, manager, _1, _2));
}

}

}

}
