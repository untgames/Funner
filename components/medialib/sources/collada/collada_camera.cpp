#include "h/shared.h"

ColladaCamera::ColladaCamera (const ColladaCameraImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaCameraType ColladaCamera::CameraType () const
{
  return impl->type;
}

float ColladaCamera::ZNear       () const
{
  return impl->znear;
}

float ColladaCamera::ZFar        () const
{
  return impl->zfar;
}

float ColladaCamera::AspectRatio () const
{
  return impl->aspect_ratio;
}

float ColladaCamera::XFov        () const
{
  return impl->x;
}

float ColladaCamera::YFov        () const
{
  return impl->y;
}

float ColladaCamera::MagnitudeX  () const
{
  return impl->x;
}

float ColladaCamera::MagnitudeY  () const
{
  return impl->y;
}

void ColladaImpl::parse_library_cameras (Parser::Iterator p)
{
  if(!test (p, "camera"))
  {
    log->Error(p, "Uncorrect 'library_cameras' tag. Must be at least one 'camera' sub-tag");
    return;
  }
  
  for_each_child (p, "camera", bind (&ColladaImpl::parse_camera, this, _1));  
}

void ColladaImpl::parse_camera (Parser::Iterator p)
{
  Parser::NamesakeIterator i = p->First("optics");

  if (i)
  {
    if(!test (i, "technique_common"))
    {
      log->Error (i, "Not detected child 'technique_common'.");
      return;
    }
    cameras.resize(cameras.size() + 1);
    read (p, "id", cameras.back().id, "No id");
    cameras.back().line = p->LineNumber();
    parse_technique_common (i->First("technique_common"), &cameras.back());

    if (++i)
      log->Error(p, "Detected more than one child 'optics'.");
  }
  else
  {
    log->Error(p, "Not detected child 'optics'.");
    return;
  }
}

void ColladaImpl::parse_technique_common (Parser::Iterator p, ColladaCameraImpl *destination)
{
  if(test (p, "perspective"))
  {
    p = p->First("perspective");
    destination->type = COLLADA_CAMERA_PERSPECTIVE;
  }
  else if (test (p, "orthographic")) 
  {
    p = p->First("orthographic");
    destination->type = COLLADA_CAMERA_ORTHOGRAPHIC;
  }
  else 
  {
    log->Error(p, "Uncorrect 'technique_common' tag, no expected sub-tag (one of 'perspective' or 'orthographic')");
    return;
  }

  destination->x = destination->y = destination->aspect_ratio = destination->znear = destination->zfar = 0.f;

  if (test (p, "xfov"))  
    read (p, "xfov.#text", destination->x, 1.0f);
  else if (test (p,"xmag"))
    read (p, "xmag.#text", destination->x, 1.0f);
  if (test (p, "yfov"))  
    read (p, "yfov.#text", destination->y, 1.0f);
  else if (test (p, "ymag"))
    read (p, "ymag.#text", destination->y, 1.0f);
  if (test (p, "aspect_ratio"))  
    read (p, "aspect_ratio.#text", destination->aspect_ratio, 1.333f);
  if (test (p, "znear"))  
    read (p, "znear.#text", destination->znear, 0.1f);
  if (test (p, "zfar"))  
    read (p, "zfar.#text", destination->zfar, 1000.f);
}
