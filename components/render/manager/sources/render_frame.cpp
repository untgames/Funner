#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации кадра
*/

struct FrameImpl::Impl
{
};

/*
   Конструктор / деструктор
*/

FrameImpl::FrameImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::FrameImpl");
}

FrameImpl::~FrameImpl ()
{
}

/*
    Регистрация целевых буферов отрисовки
*/

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveAllRenderTargets ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение целевых буферов отрисовки
*/

RenderTargetPtr FrameImpl::FindRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RectAreaPtr FrameImpl::FindViewport (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RectAreaPtr FrameImpl::FindScissor (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

RenderTargetPtr FrameImpl::RenderTarget (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::RenderTarget";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetPtr target = FindRenderTarget (name);
  
  if (target)
    return target;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
}

RectAreaPtr FrameImpl::Viewport (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Viewport";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindViewport (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Viewport not found");
}

RectAreaPtr FrameImpl::Scissor (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Scissor";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindScissor (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Scissor not found");
}

/*
    Область отсечения
*/

void FrameImpl::SetScissorState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetScissorState");
}

bool FrameImpl::ScissorState ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ScissorState");
}

/*
    Флаги очистки кадра
*/

void FrameImpl::SetClearFlags (size_t clear_flags)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearFlags");
}

size_t FrameImpl::ClearFlags ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearFlags");
}

/*
    Параметры очистки буфера цвета
*/

void FrameImpl::SetClearColor (const math::vec4f& color)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearColor");
}

const math::vec4f& FrameImpl::ClearColor ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearColor");
}

/*
    Параметры очистки буфера попиксельного отсечения
*/

void FrameImpl::SetClearDepthValue (float depth_value)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearDepthValue");
}

void FrameImpl::SetClearStencilIndex (unsigned char stencil_index)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetClearStencilIndex");
}

float FrameImpl::ClearDepthValue ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearDepthValue");
}

unsigned char FrameImpl::ClearStencilIndex ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::ClearStencilIndex");
}

/*
    Локальные текстуры
*/

void FrameImpl::SetLocalTexture (const char* name, const TexturePtr& texture)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveLocalTexture (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::RemoveAllLocalTextures ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение локальной текстуры
*/

TexturePtr FrameImpl::FindLocalTexture (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

TexturePtr FrameImpl::LocalTexture (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::LocalTexture";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  TexturePtr texture = FindLocalTexture (name);
  
  if (texture)
    return texture;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Local texture not found");

}

/*
    Установка эффекта рендеринга
*/

void FrameImpl::SetEffect (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetEffect");
}

const char* FrameImpl::Effect ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Effect");
}

/*
    Свойства рендеринга
*/

void FrameImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::SetProperties");
}

const common::PropertyMap& FrameImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Properties");
}

/*
    Список отрисовки
*/

size_t FrameImpl::EntitiesCount ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::EntitiesCount");
}

void FrameImpl::AddEntity (const EntityPtr& entity)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddEntity");
}

void FrameImpl::RemoveAllEntities ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RemoveAllEntities");
}

/*
    Добавление пре-рендеринга и пост-рендеринга
*/

size_t FrameImpl::FramesCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void FrameImpl::AddFrame (const FramePtr& frame)
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::AddFrame");
}

void FrameImpl::RemoveAllFrames ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::RemoveAllFrames");
}

/*
    Рисование кадра
*/

void FrameImpl::Draw ()
{
  throw xtl::make_not_implemented_exception ("render::FrameImpl::Draw");
}
