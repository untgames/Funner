#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации цели рендеринга
*/

struct RenderTarget::Impl: public xtl::reference_counter
{
  RenderTargetPtr render_target;

/// Конструктор
  Impl (RenderTargetImpl* in_rt_impl) : render_target (in_rt_impl) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

RenderTarget::RenderTarget ()
  : impl (new Impl (0))
{
}

RenderTarget::RenderTarget (RenderTargetImpl* rt_impl)
{
  if (!rt_impl)
    throw xtl::make_null_argument_exception ("render::scene::client::RenderTarget::RenderTarget(RenderTargetImpl*)", "impl");

  impl = new Impl (rt_impl);
}

RenderTarget::RenderTarget (const RenderTarget& target)
  : impl (target.impl)
{
  addref (impl);
}

RenderTarget::~RenderTarget ()
{
  release (impl);
}

RenderTarget& RenderTarget::operator = (const RenderTarget& target)
{
  RenderTarget tmp (target);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Экран (политика владения - weak-ref)
*/

void RenderTarget::SetScreen (scene_graph::Screen* screen)
{
  try
  {
    if (!impl->render_target)
      throw xtl::format_operation_exception ("", "RenderTarget is not binded to renderer");

    impl->render_target->SetScreen (screen);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTarget::SetScreen");
    throw;
  } 
}

scene_graph::Screen* RenderTarget::Screen () const
{
  try
  {
    if (!impl->render_target)
      return 0;

    return impl->render_target->Screen ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTarget::Screen");
    throw;
  } 
}

/*
    Проверка связи цели рендеринга с рендером
*/

bool RenderTarget::IsBindedToRenderer () const
{
  return impl->render_target != RenderTargetPtr ();
}

/*
    Обновление
*/

void RenderTarget::Update ()
{
  try
  {
    if (!impl->render_target)
      throw xtl::format_operation_exception ("", "RenderTarget is not binded to renderer");

    impl->render_target->Update ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTarget::Update");
    throw;
  }
}

/*
    Захват изображения (screen-shot)
*/

void RenderTarget::CaptureImage (media::Image& image)
{
  try
  {
    if (!impl->render_target)
      throw xtl::format_operation_exception ("", "RenderTarget is not binded to renderer");

    impl->render_target->CaptureImage (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTarget::CaptureImage(media::Image&)");
    throw;
  } 
}

void RenderTarget::CaptureImage (const char* image_name)
{
  try
  {
    if (!image_name)
      throw xtl::make_null_argument_exception ("", "image_name");

    if (!impl->render_target)
      throw xtl::format_operation_exception ("", "RenderTarget is not binded to renderer");

    media::Image image (1, 1, 1, media::PixelFormat_RGBA8);

    CaptureImage (image);

    image.Save (image_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTarget::CaptureImage(const char*)");
    throw;
  } 
}

/*
    Обмен
*/

void RenderTarget::Swap (RenderTarget& target)
{
  stl::swap (impl, target.impl);
}

namespace render
{

namespace scene
{

namespace client
{

void swap (RenderTarget& target1, RenderTarget& target2)
{
  target1.Swap (target2);
}

}

}

}
