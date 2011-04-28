#include "shared.h"

using namespace render;

/*
    Конструкторы
*/

RenderingContext::RenderingContext (FrameImpl& in_frame, RenderingContext* in_previous)
  : frame (in_frame)
  , previous (in_previous)
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
  
  if (!texture && previous)
    return previous->FindLocalTexture (name);
    
  return TexturePtr ();
}

RenderTargetDescPtr RenderingContext::FindRenderTarget (const char* name)
{
  if (!name)
    return RenderTargetDescPtr ();
    
  RenderTargetDescPtr desc = frame.FindRenderTargetDesc (name);
  
  if (!desc && previous)
    return previous->FindRenderTarget (name);
    
  return RenderTargetDescPtr ();
}
