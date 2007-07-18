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
  if(!p->Present("camera"))
  {
    log->Error(p, "Uncorrect 'library_cameras' tag. Must be at least one 'camera' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("camera"); i; i++)
    parse_camera (i);
}

void ColladaImpl::parse_camera (Parser::Iterator p)
{
  Parser::NamesakeIterator i = p->First("optics");

  if (i)
  {
    if(!i->Present("technique_common"))
    {
      log->Error (i, "Not detected child 'technique_common'.");
      return;
    }
    cameras.resize(cameras.size() + 1);
    p->Read ("id", cameras.back().id, "No id");
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
  if(p->Present("perspective"))
  {
    p = p->First("perspective");
    destination->type = COLLADA_CAMERA_PERSPECTIVE;
  }
  else if (p->Present("orthographic")) 
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

  if (p->Present("xfov"))  
    p->First("xfov")->Read ("#text", destination->x, 1.0f);
  else if (p->Present("xmag"))
    p->First("xmag")->Read ("#text", destination->x, 1.0f);
  if (p->Present("yfov"))  
    p->First("yfov")->Read ("#text", destination->y, 1.0f);
  else if (p->Present("ymag"))
    p->First("ymag")->Read ("#text", destination->y, 1.0f);
  if (p->Present("aspect_ratio"))  
    p->First("aspect_ratio")->Read ("#text", destination->aspect_ratio, 1.333f);
  if (p->Present("znear"))  
    p->First("znear")->Read ("#text", destination->znear, 0.1f);
  if (p->Present("zfar"))  
    p->First("zfar")->Read ("#text", destination->zfar, 1000.f);
}

