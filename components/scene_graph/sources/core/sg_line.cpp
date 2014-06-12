#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Line
*/

struct Line::Impl: public xtl::instance_counter<Line>
{
  LineDesc line_desc; //описание спрайта
  
  Impl ()
  {
    line_desc.point [0].position   = vec3f (0);
    line_desc.point [0].color      = vec4f (1.0f, 1.0f, 1.0f, 1.0f);
    line_desc.point [0].tex_offset = vec2f (0);

    line_desc.point [1].position   = vec3f (0, 0, 1.0f);
    line_desc.point [1].color      = vec4f (1.0f, 1.0f, 1.0f, 1.0f);
    line_desc.point [1].tex_offset = vec2f (1.0f, 0);
  }
};

/*
    Конструктор / деструктор
*/

Line::Line ()
  : impl (new Impl)
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (0, 0, 0, 1.0f, 1.0f, 1.0f));
}

Line::~Line ()
{
}

/*
    Создание спрайта
*/

Line::Pointer Line::Create ()
{
  return Pointer (new Line, false);
}

/*
    Цвет спрайта
*/

namespace
{

float clamp (float x)
{
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  
  return x;
}

vec4f clamp (const vec4f& color)
{
  return vec4f (clamp (color.x), clamp (color.y), clamp (color.z), clamp (color.w));
}

}

void Line::SetColor (size_t point_index, const vec4f& color)
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::SetColor", "point_index", point_index, 2u);

  impl->line_desc.point [point_index].color = clamp (color);

  UpdateLineDescsNotify ();
}

void Line::SetColor (size_t point_index, float red, float green, float blue, float alpha)
{
  SetColor (point_index, vec4f (red, green, blue, alpha));
}

void Line::SetColor (size_t point_index, float red, float green, float blue)
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::SetColor", "point_index", point_index, 2u);

  SetColor (point_index, vec4f (red, green, blue, impl->line_desc.point [point_index].color.w));
}

void Line::SetAlpha (size_t point_index, float alpha)
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::SetAlpha", "point_index", point_index, 2u);

  impl->line_desc.point [point_index].color.w = clamp (alpha);

  UpdateLineDescsNotify ();
}

const vec4f& Line::Color (size_t point_index) const
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::Color", "point_index", point_index, 2u);

  return impl->line_desc.point [point_index].color;
}

float Line::Alpha (size_t point_index) const
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::Alpha", "point_index", point_index, 2u);

  return impl->line_desc.point [point_index].color.w;
}

/*
    Текстурные координаты
*/

void Line::SetTexOffset (size_t point_index, const math::vec2f& offset)
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::SetTexOffset", "point_index", point_index, 2u);

  impl->line_desc.point [point_index].tex_offset = offset;

  UpdateLineDescsNotify ();
}

const math::vec2f& Line::TexOffset (size_t point_index) const
{
  if (point_index >= 2)
    throw xtl::make_range_exception ("scene_graph::Line::TexOffset", "point_index", point_index, 2u);

  return impl->line_desc.point [point_index].tex_offset;
}


/*
    Метод, вызываемый при посещении объекта
*/

void Line::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    LineModel::AcceptCore (visitor);
}

/*
    Реализация получения количества спрайтов и массива спрайтов
*/

size_t Line::LineDescsCountCore ()
{
  return 1;
}

const LineDesc* Line::LineDescsCore ()
{
  return &impl->line_desc;
}

/*
    Связывание свойств
*/

namespace
{

template <size_t I> void set_color      (Line* line, const math::vec4f& color)  { line->SetColor (I, color); }
template <size_t I> void set_alpha      (Line* line, float alpha)               { line->SetAlpha (I, alpha); }
template <size_t I> void set_tex_offset (Line* line, const math::vec2f& offset) { line->SetTexOffset (I, offset); }

}

void Line::BindProperties (common::PropertyBindingMap& bindings)
{
  LineModel::BindProperties (bindings);

  bindings.AddProperty ("Color0", xtl::bind (&Line::Color, this, 0u), xtl::bind (&set_color<0>, this, _1));
  bindings.AddProperty ("Color1", xtl::bind (&Line::Color, this, 1u), xtl::bind (&set_color<1>, this, _1));
  bindings.AddProperty ("Alpha0", xtl::bind<float> (&Line::Alpha, this, 0u), xtl::bind (&set_alpha<0>, this, _1));
  bindings.AddProperty ("Alpha1", xtl::bind<float> (&Line::Alpha, this, 1u), xtl::bind (&set_alpha<1>, this, _1));
  bindings.AddProperty ("TexOffset0", xtl::bind (&Line::TexOffset, this, 0u), xtl::bind (&set_tex_offset<0>, this, _1));
  bindings.AddProperty ("TexOffset1", xtl::bind (&Line::TexOffset, this, 1u), xtl::bind (&set_tex_offset<1>, this, _1));
}
