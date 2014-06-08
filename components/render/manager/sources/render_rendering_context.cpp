#include "shared.h"

using namespace render::manager;

/*
    Конструкторы
*/

RenderingContext::RenderingContext (FrameImpl& in_frame, const math::mat4f& in_vp_tm, RenderingContext* in_previous)
  : frame (in_frame)
  , previous (in_previous)
  , vp_tm (in_vp_tm)
  , inv_vp_tm (math::inverse (vp_tm))
{
}

/*
    Поиск локальных объектов
*/

TexturePtr RenderingContext::FindLocalTexture (const char* name)
{
  if (!name)
    return TexturePtr ();
    
  TexturePtr texture = frame.FindLocalTexture (name);
  
  if (texture)
    return texture;
  
  if (previous)
    return previous->FindLocalTexture (name);
    
  return TexturePtr ();
}

RenderTargetDescPtr RenderingContext::FindRenderTarget (const char* name)
{
  if (!name)
    return RenderTargetDescPtr ();

  RenderTargetDescPtr desc = frame.RenderTargets ().FindRenderTargetDesc (name);

  if (desc)
    return desc;
  
  if (previous)
    return previous->FindRenderTarget (name);
    
  return RenderTargetDescPtr ();
}
