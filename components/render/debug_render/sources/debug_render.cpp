#include <xtl/common_exceptions.h>

#include <math/angle.h>

#include <render/debug_render.h>

using namespace render::debug;

namespace
{

const size_t CIRCLE_SEGMENTS_COUNT = 16;

}

/*
   Рендер вспомогательной геометрии
*/

/*
   Рисование примитивов
*/

void PrimitiveRender::DrawLine (const RenderablePoint& from, const RenderablePoint& to)
{
  RenderablePoint points [2] = { from, to };

  DrawLinesCore (1, points);
}

void PrimitiveRender::DrawLines (size_t lines_count, const RenderablePoint* points)
{
  if (!points)
    throw xtl::make_null_argument_exception ("render::debug::PrimitiveRender::DrawLines", "points");

  DrawLinesCore (lines_count, points);
}

void PrimitiveRender::DrawTriangle (const RenderablePoint points [3])
{
  RenderablePoint vertices [6] = { points [0], points [1], points [2], points [1], points [0], points [2] };

  DrawLinesCore (3, vertices);
}

void PrimitiveRender::DrawText (const char* text, const math::vec3f& position, const math::vec4f& color)
{
  if (!text)
    throw xtl::make_null_argument_exception ("render::debug::PrimitiveRender::DrawText", "text");

  DrawTextCore (text, position, color);
}

void PrimitiveRender::DrawWireSphere (const math::vec3f& p, float radius, const math::vec4f& color)
{
  DrawCircle (p, math::vec3f (1.f, 0.f, 0.f), radius, color);
  DrawCircle (p, math::vec3f (0.f, 1.f, 0.f), radius, color);
  DrawCircle (p, math::vec3f (0.f, 0.f, 1.f), radius, color);
}

void PrimitiveRender::DrawWireBox (const math::vec3f& min, const math::vec3f& max, const math::vec4f& color)
{
  RenderablePoint vertices [24];

  for (size_t i = 0; i < 24; i++)
    vertices [i].color = color;

  vertices [0].position  = math::vec3f (min.x, min.y, min.z);
  vertices [1].position  = math::vec3f (max.x, min.y, min.z);

  vertices [2].position  = math::vec3f (max.x, min.y, min.z);
  vertices [3].position  = math::vec3f (max.x, max.y, min.z);

  vertices [4].position  = math::vec3f (max.x, max.y, min.z);
  vertices [5].position  = math::vec3f (min.x, max.y, min.z);

  vertices [6].position  = math::vec3f (min.x, max.y, min.z);
  vertices [7].position  = math::vec3f (min.x, min.y, min.z);

  vertices [8].position  = math::vec3f (min.x, min.y, min.z);
  vertices [9].position  = math::vec3f (min.x, min.y, max.z);

  vertices [10].position = math::vec3f (max.x, min.y, min.z);
  vertices [11].position = math::vec3f (max.x, min.y, max.z);

  vertices [12].position = math::vec3f (max.x, max.y, min.z);
  vertices [13].position = math::vec3f (max.x, max.y, max.z);

  vertices [14].position = math::vec3f (min.x, max.y, min.z);
  vertices [15].position = math::vec3f (min.x, max.y, max.z);

  vertices [16].position = math::vec3f (min.x, min.y, max.z);
  vertices [17].position = math::vec3f (max.x, min.y, max.z);

  vertices [18].position = math::vec3f (max.x, min.y, max.z);
  vertices [19].position = math::vec3f (max.x, max.y, max.z);

  vertices [20].position = math::vec3f (max.x, max.y, max.z);
  vertices [21].position = math::vec3f (min.x, max.y, max.z);

  vertices [22].position = math::vec3f (min.x, max.y, max.z);
  vertices [23].position = math::vec3f (min.x, min.y, max.z);

  DrawLinesCore (12, vertices);
}

void PrimitiveRender::DrawWireCone (const math::vec3f& position, float radius, const math::vec3f& normal, float height, const math::vec4f& color)
{
  math::vec3f normalized_normal = math::normalize (normal), perpendicular;

  DrawCircle (position, normal, radius, color);

  RenderablePoint vertices [8];

  for (size_t i = 0; i < 8; i++)
    vertices [i].color = color;

  if (normalized_normal.x == 1)
    perpendicular = math::vec3f (0.f, 0.f, 1.f);
  else
    perpendicular = math::vec3f (normalized_normal.z,  normalized_normal.y * normalized_normal.z / (normalized_normal.x - 1),  1 + (normalized_normal.z * normalized_normal.z) / (normalized_normal.x - 1));

  math::vec3f another_perpendicular = math::cross (normalized_normal, perpendicular);

  vertices [0].position = position + normalized_normal * height;
  vertices [1].position = position + perpendicular * radius;
  vertices [2].position = vertices [0].position;
  vertices [3].position = position - perpendicular * radius;
  vertices [4].position = vertices [0].position;
  vertices [5].position = position + another_perpendicular * radius;
  vertices [6].position = vertices [0].position;
  vertices [7].position = position - another_perpendicular * radius;

  DrawLinesCore (4, vertices);
}

void PrimitiveRender::DrawWireTruncatedCone (const math::vec3f& position, float radius, const math::vec3f& normal, float height, float truncate_height, const math::vec4f& color)
{
  math::vec3f normalized_normal = math::normalize (normal),
              top_center        = position + normalized_normal * truncate_height;
  float       truncate_radius   = (height - truncate_height) * radius / height;

  DrawCircle (position, normal, radius, color);
  DrawCircle (top_center, normal, truncate_radius, color);

  RenderablePoint vertices [8];

  for (size_t i = 0; i < 8; i++)
    vertices [i].color = color;

  math::vec3f perpendicular;

  if (normalized_normal.x == 1)
    perpendicular = math::vec3f (0.f, 0.f, 1.f);
  else
    perpendicular = math::vec3f (normalized_normal.z,  normalized_normal.y * normalized_normal.z / (normalized_normal.x - 1),  1 + (normalized_normal.z * normalized_normal.z) / (normalized_normal.x - 1));

  math::vec3f another_perpendicular = math::cross (normalized_normal, perpendicular);

  vertices [0].position = top_center + perpendicular * truncate_radius;
  vertices [1].position = position + perpendicular * radius;
  vertices [2].position = top_center - perpendicular * truncate_radius;
  vertices [3].position = position - perpendicular * radius;
  vertices [4].position = top_center + another_perpendicular * truncate_radius;
  vertices [5].position = position + another_perpendicular * radius;
  vertices [6].position = top_center - another_perpendicular * truncate_radius;
  vertices [7].position = position - another_perpendicular * radius;

  DrawLinesCore (4, vertices);
}

void PrimitiveRender::DrawWireCylinder (const math::vec3f& position, float radius, const math::vec3f& normal, float height, const math::vec4f& color)
{
  math::vec3f normalized_normal = math::normalize (normal), perpendicular, height_vec = normalized_normal * height;

  DrawCircle (position, normal, radius, color);
  DrawCircle (position + height_vec, normal, radius, color);

  RenderablePoint vertices [8];

  for (size_t i = 0; i < 8; i++)
    vertices [i].color = color;

  if (normalized_normal.x == 1)
    perpendicular = math::vec3f (0.f, 0.f, 1.f);
  else
    perpendicular = math::vec3f (normalized_normal.z,  normalized_normal.y * normalized_normal.z / (normalized_normal.x - 1),  1 + (normalized_normal.z * normalized_normal.z) / (normalized_normal.x - 1));

  math::vec3f another_perpendicular = math::cross (normalized_normal, perpendicular);

  vertices [0].position = position + perpendicular * radius;
  vertices [1].position = vertices [0].position + height_vec;
  vertices [2].position = position - perpendicular * radius;
  vertices [3].position = vertices [2].position + height_vec;
  vertices [4].position = position + another_perpendicular * radius;
  vertices [5].position = vertices [4].position + height_vec;
  vertices [6].position = position - another_perpendicular * radius;
  vertices [7].position = vertices [6].position + height_vec;

  DrawLinesCore (4, vertices);
}

void PrimitiveRender::DrawCircle (const math::vec3f& position, const math::vec3f& normal, float radius, const math::vec4f color)
{
  math::vec3f normalized_normal = math::normalize (normal), perpendicular;

  if (normalized_normal.x == 1)
    perpendicular = math::vec3f (0.f, 0.f, 1.f);
  else
    perpendicular = math::vec3f (normalized_normal.z,  normalized_normal.y * normalized_normal.z / (normalized_normal.x - 1),  1 + (normalized_normal.z * normalized_normal.z) / (normalized_normal.x - 1));

  math::vec3f another_perpendicular = math::cross (normalized_normal, perpendicular);

  RenderablePoint vertices [CIRCLE_SEGMENTS_COUNT * 2];

  for (size_t i = 0; i < CIRCLE_SEGMENTS_COUNT * 2; i++)
    vertices [i].color = color;

  math::anglef angle = math::degree (0.f);

  vertices [0].position = radius * math::cos (angle) * perpendicular + radius * another_perpendicular + position;

  angle = math::degree (360.f / CIRCLE_SEGMENTS_COUNT);

  vertices [1].position = radius * math::cos (angle) * perpendicular + radius * another_perpendicular + position;

  for (size_t i = 1; i < CIRCLE_SEGMENTS_COUNT; i++)
  {
    angle = math::degree (360.f / CIRCLE_SEGMENTS_COUNT * i);

    vertices [i * 2].position     = vertices [i * 2 - 1].position;
    vertices [i * 2 + 1].position = radius * math::cos (angle) * perpendicular + radius * math::sin (angle) * another_perpendicular + position;
  }

  DrawLinesCore (CIRCLE_SEGMENTS_COUNT, vertices);
}
