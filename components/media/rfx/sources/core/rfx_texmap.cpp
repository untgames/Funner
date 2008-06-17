#include "shared.h"

using namespace common;
using namespace media::rfx;
using namespace media;
using namespace math;

/*
    Описание реализации Texmap
*/

struct TexcoordParams
{
  int          source; //источник текстурных координат
  TexcoordWrap wrap;   //отсечение текстурных координат
  
  TexcoordParams () : source (TexcoordSource_Default), wrap (TexcoordWrap_Default) {}
};

struct Texmap::Impl
{
  stl::string      image;                          //имя картинки
  mat4f            transform;                      //матрица преобразований текстурных координат
  TexcoordParams   texcoord_params [Texcoord_Num]; //параметры текстурных координат
  TexmapFilterType filters [TexmapFilter_Num];     //фильтры

  Impl ();
};

/*
    Impl
*/

Texmap::Impl::Impl ()
{
  for (size_t i=0; i<TexmapFilter_Num; i++)
    filters [i] = TexmapFilterType_Default;
}

/*
    Конструкторы / деструктор
*/

Texmap::Texmap ()
  : impl (new Impl)
  {}

Texmap::Texmap (const Texmap& texmap)
  : impl (new Impl (*texmap.impl))
  {}

Texmap::~Texmap ()
{
}

/*
    Присваивание
*/

Texmap& Texmap::operator = (const Texmap& texmap)
{
  Texmap (texmap).Swap (*this);

  return *this;
}

/*
    Имя картинки связанной с текстурой
*/

const char* Texmap::Image () const
{
  return impl->image.c_str ();
}

void Texmap::SetImage (const char* image_name)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("media::Texmap::SetImage", "image_name");
    
  impl->image = image_name;
}

/*
    Матрица преобразования текстурных координат
*/

const mat4f& Texmap::Transform () const
{
  return impl->transform;
}

void Texmap::SetTransform (const math::mat4f& tm)
{
  impl->transform = tm;
}

/*
    Источник текстурных координат
*/

int Texmap::Source (Texcoord coord) const
{
  if (coord < 0 || coord >= Texcoord_Num)
    throw xtl::make_argument_exception ("media::Texmap::Source", "coord", coord);
    
  return impl->texcoord_params [coord].source;
}

void Texmap::SetSource (Texcoord coord, int source)
{
  if (coord < 0 || coord >= Texcoord_Num)
    throw xtl::make_argument_exception ("media::Texmap::SetSource", "coord", coord);
    
  impl->texcoord_params [coord].source = source;
}

/*
    Отсечение текстурных координат на границе
*/

TexcoordWrap Texmap::Wrap (Texcoord coord) const
{
  if (coord < 0 || coord >= Texcoord_Num)
    throw xtl::make_argument_exception ("media::Texmap::Wrap", "coord", coord);
    
  return impl->texcoord_params [coord].wrap;
}

void Texmap::SetWrap (Texcoord coord, TexcoordWrap wrap)
{
  if (coord < 0 || coord >= Texcoord_Num)
    throw xtl::make_argument_exception ("media::Texmap::SetWrap", "coord", coord);
    
  impl->texcoord_params [coord].wrap = wrap;
}

/*
    Режимы фильтрации
*/

TexmapFilterType Texmap::FilterType (TexmapFilter filter) const
{
  if (filter < 0 || filter >= TexmapFilter_Num)
    throw xtl::make_argument_exception ("media::Texmap::FilterType", "filter", filter);
    
  return impl->filters [filter];
}

void Texmap::SetFilterType (TexmapFilter filter, TexmapFilterType type)
{
  if (filter < 0 || filter >= TexmapFilter_Num)
    throw xtl::make_argument_exception ("media::Texmap::SetFilterType", "filter", filter);
    
  switch (type)
  {
    case TexmapFilterType_Default:
    case TexmapFilterType_Point:
    case TexmapFilterType_Bilinear:
    case TexmapFilterType_Trilinear:
    case TexmapFilterType_Anisotropic:
      break;
    default:
      throw xtl::make_argument_exception ("media::Texmap::SetFilterType", "type", type);
      break;
  }
  
  impl->filters [filter] = type;
}

/*
    Обмен
*/  

void Texmap::Swap (Texmap& texmap)
{
  stl::swap (impl, texmap.impl);
}

namespace media
{

namespace rfx
{

void swap (Texmap& texmap1, Texmap& texmap2)
{
  texmap1.Swap (texmap2);  
}

/*
    Получение имени
*/

const char* get_name (Texcoord id)
{
  switch (id)
  {
    case Texcoord_S: return "s";
    case Texcoord_T: return "t";
    case Texcoord_Q: return "q";
    default:         throw xtl::make_argument_exception ("media::rfx::get_name(Texcoord)", "id", id);
  }
  
  return "";
}

const char* get_name (TexcoordSource id)
{
  switch (id)
  {
    case TexcoordSource_SphereMap:     return "sphere_map";
    case TexcoordSource_ReflectionMap: return "reflection_map";
    default:                           return "explicit";
  }
}

const char* get_name (TexcoordWrap id)
{
  switch (id)
  {
    case TexcoordWrap_Repeat:        return "repeat";
    case TexcoordWrap_Mirror:        return "mirror";
    case TexcoordWrap_Clamp:         return "clamp";
    case TexcoordWrap_ClampToBorder: return "clamp_to_border";
    default:                         throw xtl::make_argument_exception ("media::rfx::get_name(TexcoordWrap)", "id", id);
  }
  
  return "";
}

const char* get_name (TexmapFilter id)
{
  switch (id)
  {
    case TexmapFilter_Min: return "min";
    case TexmapFilter_Mag: return "mag";
    case TexmapFilter_Mip: return "mip";
    default:               throw xtl::make_argument_exception ("media::rfx::get_name(TexmapFilter)", "id", id);
  }
  
  return "";
}

const char* get_name (TexmapFilterType id)
{
  switch (id)
  {
    case TexmapFilterType_Default:     return "default";
    case TexmapFilterType_Point:       return "point";
    case TexmapFilterType_Bilinear:    return "bilinear";
    case TexmapFilterType_Trilinear:   return "trilinear";
    case TexmapFilterType_Anisotropic: return "anisotropic";
    default:                           throw xtl::make_argument_exception ("media::rfx::get_name(TexmapFilterType)", "id", id);
  }

  return "";  
}

}

}
