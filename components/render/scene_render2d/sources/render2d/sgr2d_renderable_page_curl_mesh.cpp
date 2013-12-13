#include "shared.h"

using namespace render;
using namespace render::obsolete::render2d;

typedef xtl::com_ptr<low_level::IBuffer> BufferPtr;

namespace
{

const float PI = 3.1415926f;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры вершины необходимые для визуализации
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableVertex
{
  math::vec3f  position; //положение вершины в пространстве
  math::vec2f  texcoord; //текстурные координаты
  math::vec4ub color;    //цвет вершины
};

typedef xtl::uninitialized_storage<math::vec3f>      PositionBuffer;
typedef xtl::uninitialized_storage<RenderableVertex> VertexBuffer;

}

/*
    Описание реализации сетки страницы
*/

struct RenderablePageCurlMesh::Impl
{
  PositionBuffer original_vertices;             //данные вершин плоской страницы
  VertexBuffer   vertices;                      //данные вершин трансформированной страницы
  BufferPtr      vertex_buffer;                 //вершинный буфер
  BufferPtr      index_buffer;                  //индексный буфер
  bool           is_rigid;                      //является ли страница жесткой
  float          rigid_page_perspective_factor; //коэффициент увеличения жесткой обложки для симуляции перспективы
  math::vec2ui   grid_size;                     //размер сетки
  size_t         triangles_count;               //количество треугольников
  size_t         vertices_count;                //количество вершин
  math::vec2f    size;                          //размер
  float          min_s;                         //минимальная текстурная координата по горизонтали
  float          min_t;                         //минимальная текстурная координата по вертикали
  float          max_s;                         //максимальная текстурная координата по горизонтали
  float          max_t;                         //максимальная текстурная координата по вертикали
  float          last_curl_radius;              //радиус последней трансформации
  math::vec4ub   color;                         //цвет

  ///Конструктор
  Impl (low_level::IDevice& device, bool in_is_rigid, const math::vec2ui& in_grid_size)
    : is_rigid  (in_is_rigid)
    , rigid_page_perspective_factor (1.f)
    , grid_size (in_grid_size)
    , size      (-1)
    , min_s     (-1)
    , min_t     (-1)
    , max_s     (-1)
    , max_t     (-1)
    , last_curl_radius (0)
    , color     (0)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurlMesh::RenderablePageCurlMesh";

    if (grid_size.x < 2)
      throw xtl::make_argument_exception (METHOD_NAME, "grid_size.x", (size_t)grid_size.x, "grid_size.x must be greater than 1");

    if (grid_size.y < 2)
      throw xtl::make_argument_exception (METHOD_NAME, "grid_size.y", (size_t)grid_size.y, "grid_size.y must be greater than 1");

    vertices_count = grid_size.x * grid_size.y;

    if (vertices_count > USHRT_MAX)
      throw xtl::format_operation_exception (METHOD_NAME, "grid_size.x * grid_size.y is larger than max unsigned short value (grid_size.x = %u, grid_size.y = %u)", grid_size.x, grid_size.y);

    triangles_count = (grid_size.x - 1) * (grid_size.y - 1) * 2;

    size_t indices_count = triangles_count * 3;

    original_vertices.resize (vertices_count, false);
    vertices.resize          (vertices_count, false);

    xtl::uninitialized_storage<unsigned short> indices (indices_count);

    for (size_t i = 0; i < (grid_size.x - 1) * 2; i++)
    {
      for (size_t j = 0; j < grid_size.y - 1; j++)
      {
        unsigned short* current_indices = &indices.data () [(j * (grid_size.x - 1) * 2 + i) * 3];

        if (i % 2)
        {
          current_indices [0] = (j + 1) * grid_size.x + i / 2;
          current_indices [1] = (j + 1) * grid_size.x + i / 2 + 1;
          current_indices [2] = j * grid_size.x + i / 2 + 1;
        }
        else
        {
          current_indices [0] = j * grid_size.x + i / 2;
          current_indices [1] = (j + 1) * grid_size.x + i / 2;
          current_indices [2] = j * grid_size.x + i / 2 + 1;
        }
      }
    }

    SetColor     (math::vec4ub (255));
    SetSize      (math::vec2f (1));
    SetTexCoords (0, 1, 0, 1);

    low_level::BufferDesc buffer_desc;

    memset (&buffer_desc, 0, sizeof buffer_desc);

    buffer_desc.usage_mode   = low_level::UsageMode_Stream;
    buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
    buffer_desc.access_flags = low_level::AccessFlag_Write;
    buffer_desc.size         = sizeof (RenderableVertex) * vertices_count;

    vertex_buffer  = BufferPtr (device.CreateBuffer (buffer_desc), false);

      //создание индексного буфера

    memset (&buffer_desc, 0, sizeof buffer_desc);

    buffer_desc.usage_mode   = low_level::UsageMode_Stream;
    buffer_desc.bind_flags   = low_level::BindFlag_IndexBuffer;
    buffer_desc.access_flags = low_level::AccessFlag_Write;
    buffer_desc.size         = sizeof (unsigned short) * indices_count;

    index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

    index_buffer->SetData (0, sizeof (unsigned short) * indices_count, indices.data ());
  }

  //Отрисовка
  void Draw (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
    vertex_buffer->SetData (0, sizeof (RenderableVertex) * vertices_count, vertices.data ());

    context.ISSetIndexBuffer  (index_buffer.get ());
    context.ISSetVertexBuffer (0, vertex_buffer.get ());

    context.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, triangles_count * 3, 0);
  }

  //Трансформация страницы
  void Curl (const math::vec2f& corner_position, scene_graph::PageCurlCorner corner, float radius,
             float angle, size_t find_best_curl_steps, float binding_mismatch_weight)
  {
    last_curl_radius = radius;

    if (is_rigid)
    {
      float x                     = corner_position.x,
            x_offset_from_binding = x,
            min_x                 = 0,
            max_x                 = x;

      bool is_left_side = false;

      switch (corner)
      {
        case scene_graph::PageCurlCorner_LeftBottom:
        case scene_graph::PageCurlCorner_LeftTop:
          min_x                 = x;
          max_x                 = size.x;
          x_offset_from_binding = x - size.x;
          is_left_side          = true;
          break;
        default:
          break;
      }

      float perspective_factor = stl::max (0.f, (float)(1.f - fabs (x_offset_from_binding) / size.x) * (rigid_page_perspective_factor - 1.f)),
            min_z = 0,
            max_z = sqrt (size.x * size.x - x_offset_from_binding * x_offset_from_binding),
            dx    = (max_x - min_x) / (grid_size.x - 1),
            dz    = (max_z - min_z) / (grid_size.x - 1);

      if (is_left_side)
      {
        min_z = max_z;
        dz    = -dz;
      }

      RenderableVertex*  vertex     = vertices.data ();
      const math::vec3f* original_v = original_vertices.data ();

      for (size_t j = 0; j < grid_size.y; j++)
      {
        float x                  = min_x,
              z                  = min_z,
              min_y              = size.y - original_v [j * grid_size.x].y,
              perspective_y_grow = (min_y - size.y / 2) * perspective_factor / 2,
              dy                 = perspective_y_grow / (grid_size.x - 1),
              y                  = min_y;

        if (is_left_side)
        {
          y += perspective_y_grow;
          dy = -dy;
        }

        for (size_t i = 0; i < grid_size.x - 1; i++, vertex++, x += dx, y += dy, z += dz)
        {
          vertex->position.x = x;
          vertex->position.y = y;
          vertex->position.z = z;
        }

        vertex->position.x = max_x;
        vertex->position.y = y;
        vertex->position.z = max_z;

        vertex++;
      }

      return;
    }

    math::vec2f original_corner_location, opposite_original_corner_location, top_binding, bottom_binding;

    switch (corner)
    {
      case scene_graph::PageCurlCorner_RightBottom:
        original_corner_location.x          = size.x;
        original_corner_location.y          = size.y;
        opposite_original_corner_location.x = size.x;
        opposite_original_corner_location.y = 0.f;
        top_binding.x                       = 0.f;
        top_binding.y                       = 0.f;
        bottom_binding.x                    = 0.f;
        bottom_binding.y                    = size.y;
        break;
      case scene_graph::PageCurlCorner_RightTop:
        original_corner_location.x          = size.x;
        original_corner_location.y          = 0.f;
        opposite_original_corner_location.x = size.x;
        opposite_original_corner_location.y = size.y;
        top_binding.x                       = 0.f;
        top_binding.y                       = 0.f;
        bottom_binding.x                    = 0.f;
        bottom_binding.y                    = size.y;
        break;
      case scene_graph::PageCurlCorner_LeftBottom:
        original_corner_location.x          = 0.f;
        original_corner_location.y          = size.y;
        opposite_original_corner_location.x = 0.f;
        opposite_original_corner_location.y = 0.f;
        top_binding.x                       = size.x;
        top_binding.y                       = 0.f;
        bottom_binding.x                    = size.x;
        bottom_binding.y                    = size.y;
        break;
      case scene_graph::PageCurlCorner_LeftTop:
        original_corner_location.x          = 0.f;
        original_corner_location.y          = 0.f;
        opposite_original_corner_location.x = 0.f;
        opposite_original_corner_location.y = size.y;
        top_binding.x                       = size.x;
        top_binding.y                       = 0.f;
        bottom_binding.x                    = size.x;
        bottom_binding.y                    = size.y;
        break;
      default:
        break;
    }

    float sin_curl_angle       = sin (angle),
          sin_minus_curl_angle = sin (-angle),
          cos_curl_angle       = cos (angle);

    float pi_r                               = PI * radius,
          rotated_corner_location_x          = corner_position.x * cos_curl_angle - corner_position.y * sin_curl_angle,
          rotated_original_corner_location_x = original_corner_location.x * cos_curl_angle - original_corner_location.y * sin_curl_angle,
          rotated_opposite_corner_location_x = opposite_original_corner_location.x * cos_curl_angle - opposite_original_corner_location.y * sin_curl_angle,
          rotated_top_binding_location_x     = top_binding.x * cos_curl_angle - top_binding.y * sin_curl_angle,
          rotated_bottom_binding_location_x  = bottom_binding.x * cos_curl_angle - bottom_binding.y * sin_curl_angle,
          one_divide_radius                  = radius ? 1.f / radius : FLT_MAX,
          best_corner_location_z             = 0,
          best_opposite_corner_location_z    = 0,
          curl_x                             = 0;

    float best_curl_mismatch = FLT_MAX,
          best_curl_step     = size.x / find_best_curl_steps;

    for (float current_curl_x = -2.f * size.x; current_curl_x < 2.f * size.x; current_curl_x += best_curl_step)
    {
      float current_corner_location_x         = rotated_original_corner_location_x,
            current_top_binding_location_x    = rotated_top_binding_location_x,
            current_bottom_binding_location_x = rotated_bottom_binding_location_x;

      if (rotated_original_corner_location_x > current_curl_x + pi_r)
      {
        current_corner_location_x = current_curl_x - (rotated_original_corner_location_x - pi_r - current_curl_x);
      }
      else if (rotated_original_corner_location_x > current_curl_x)
      {
        float alpha = (rotated_original_corner_location_x - current_curl_x) * one_divide_radius;

        current_corner_location_x = current_curl_x + radius * sin (alpha);
      }

      if (rotated_top_binding_location_x > current_curl_x + pi_r)
      {
        current_top_binding_location_x = current_curl_x - (rotated_top_binding_location_x - pi_r - current_curl_x);
      }
      else if (rotated_top_binding_location_x > current_curl_x)
      {
        float alpha = (rotated_top_binding_location_x - current_curl_x) * one_divide_radius;

        current_top_binding_location_x = current_curl_x + radius * sin (alpha);
      }

      if (rotated_bottom_binding_location_x > current_curl_x + pi_r)
      {
        current_bottom_binding_location_x = current_curl_x - (rotated_bottom_binding_location_x - pi_r - current_curl_x);
      }
      else if (rotated_bottom_binding_location_x > current_curl_x)
      {
        float alpha = (rotated_bottom_binding_location_x - current_curl_x) * one_divide_radius;

        current_bottom_binding_location_x = current_curl_x + radius * sin (alpha);
      }

      float curl_mismatch = fabs (current_corner_location_x - rotated_corner_location_x) +
                            fabs (current_top_binding_location_x - rotated_top_binding_location_x) * binding_mismatch_weight +
                            fabs (current_bottom_binding_location_x - rotated_bottom_binding_location_x) * binding_mismatch_weight;

      if (curl_mismatch < best_curl_mismatch)
      {
        best_curl_mismatch = curl_mismatch;
        curl_x             = current_curl_x;

        if (rotated_original_corner_location_x > current_curl_x + pi_r)
        {
          best_corner_location_z = 2 * radius;
        }
        else if (rotated_original_corner_location_x > current_curl_x)
        {
          float alpha = (rotated_original_corner_location_x - current_curl_x) * one_divide_radius;

          best_corner_location_z = radius - radius * cos (alpha);
        }
        else
        {
          best_corner_location_z = 0;
        }

        if (rotated_opposite_corner_location_x > current_curl_x + pi_r)
        {
          best_opposite_corner_location_z = 2 * radius;
        }
        else if (rotated_opposite_corner_location_x > current_curl_x)
        {
          float alpha = (rotated_opposite_corner_location_x - current_curl_x) * one_divide_radius;

          best_opposite_corner_location_z = radius - radius * cos (alpha);
        }
        else
        {
          best_opposite_corner_location_z = 0;
        }
      }
    }

    RenderableVertex*  v          = vertices.data ();
    const math::vec3f* original_v = original_vertices.data ();

    if (best_corner_location_z <= best_opposite_corner_location_z * 0.5f && best_opposite_corner_location_z > 0)  //угол за который тянули гораздо ниже противоположного угла
    {
      for (size_t i = 0; i < vertices_count; i++, v++, original_v++)
      {
        v->position.x = original_v->x;
        v->position.y = size.y - original_v->y;
        v->position.z = 0;
      }

      return;
    }

    for (size_t i = 0; i < vertices_count; i++, v++, original_v++)
    {
      //rotate grid
      v->position.x = original_v->x * cos_curl_angle - original_v->y * sin_curl_angle;
      v->position.y = original_v->x * sin_curl_angle + original_v->y * cos_curl_angle;
      v->position.z = 0;       //clear z

      //curl vertex
      if (v->position.x > curl_x + pi_r)
      {
        v->position.x = curl_x - (v->position.x - pi_r - curl_x);
        v->position.z = 2 * radius;
      }
      else if (v->position.x > curl_x)
      {
        float alpha = (v->position.x - curl_x) * one_divide_radius;

        v->position.x = curl_x + radius * sin (alpha);
        v->position.z = radius - radius * cos (alpha);
      }

      //rotate grid back
      float x = v->position.x,
            y = v->position.y;

      v->position.x = x * cos_curl_angle - y * sin_minus_curl_angle;
      v->position.y = size.y - (x * sin_minus_curl_angle + y * cos_curl_angle);
    }
  }

  //Установка параметров
  void SetColor (const math::vec4ub& new_color)
  {
    if (color == new_color)
      return;

    color = new_color;

    RenderableVertex* vertex = vertices.data ();

    for (size_t i = 0; i < vertices_count; i++, vertex++)
      vertex->color = color;
  }

  void SetSize (const math::vec2f& in_size)
  {
    if (size == in_size)
      return;

    size = in_size;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      for (size_t j = 0; j < grid_size.y; j++)
      {
        math::vec3f& vertex = original_vertices.data () [j * grid_size.x + i];

        vertex.x = i / (float)(grid_size.x - 1) * size.x;
        vertex.y = j / (float)(grid_size.y - 1) * size.y;
        vertex.z = 0;
      }
    }
  }

  void SetTexCoords (float new_min_s, float new_min_t, float new_max_s, float new_max_t)
  {
    if (min_s == new_min_s && min_t == new_min_t && max_s == new_max_s && max_t == new_max_t)
      return;

    min_s = new_min_s;
    min_t = new_min_t;
    max_s = new_max_s;
    max_t = new_max_t;

    float s_range = max_s - min_s,
          t_range = max_t - min_t,
          ds      = 1.f / (grid_size.x - 1) * s_range,
          dt      = 1.f / (grid_size.y - 1) * t_range,
          t       = max_t;

    RenderableVertex* vertex = vertices.data ();

    for (size_t j = 0; j < grid_size.y; j++, t -= dt)
    {
      float s = min_s;

      for (size_t i = 0; i < grid_size.x; i++, vertex++, s += ds)
      {
        vertex->texcoord.x = s;
        vertex->texcoord.y = t;
      }
    }
  }

  //Рассчет цвета вершин
  void CalculateShadow (bool front, float max_shadow)
  {
    if (is_rigid)
      return;

    RenderableVertex* vertex = vertices.data ();

    if (last_curl_radius == 0)
    {
      for (size_t i = 0; i < vertices_count; i++, vertex++)
        vertex->color = color;

      return;
    }

    math::vec4ub colors [256];

    math::vec4ub *current_color = colors;

    float intensity      = 1.f - max_shadow,
          intensity_step = max_shadow / 255.f;

    for (size_t i = 0; i < 256; i++, current_color++, intensity += intensity_step)
    {
      current_color->x = (unsigned char)(color.x * intensity);
      current_color->y = (unsigned char)(color.y * intensity);
      current_color->z = (unsigned char)(color.z * intensity);
      current_color->w = color.w;
    }

    float z_to_light = 1 / (last_curl_radius * 2);

    if (front)
    {
      float k = z_to_light * 255.f;

      for (size_t i = 0; i < vertices_count; i++, vertex++)
      {
        vertex->color = colors [(unsigned char)(k * vertex->position.z)];
      }
    }
    else
    {
      float k = z_to_light * 255.f * 1.5f;

      for (size_t i = 0; i < vertices_count; i++, vertex++)
      {
        vertex->color = colors [(unsigned char)(255.f - k * vertex->position.z)];
      }
    }
  }

  //Получение информации после трансформации
  bool HasRightSideBendPosition ()
  {
   const RenderableVertex& first_vertex = vertices.data () [grid_size.x - 1];
   bool                    first_larger = first_vertex.position.z < last_curl_radius;

   for (size_t i = 0; i < grid_size.y; i++)
   {
     const RenderableVertex& v = vertices.data () [i * grid_size.x + grid_size.x - 1];

     if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
       return true;
   }

   return false;
  }

  bool HasLeftSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  bool HasBottomSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x * (grid_size.y - 1)];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [grid_size.x * (grid_size.y - 1) + i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  bool HasTopSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  const math::vec3f& GetRightSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x - 1];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x + grid_size.x - 1];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [(grid_size.y - 1) * grid_size.x + grid_size.x - 1].position;
  }

  const math::vec3f& GetLeftSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [(grid_size.y - 1) * grid_size.x].position;
  }

  const math::vec3f& GetTopSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [grid_size.x - 1].position;
  }

  const math::vec3f& GetBottomSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x * (grid_size.y - 1)];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [grid_size.x * (grid_size.y - 1) + i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [grid_size.x * (grid_size.y - 1) + grid_size.x - 1].position;
  }

  const math::vec3f& GetRightSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x - 1];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x + grid_size.x - 1];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return vertices.data () [(grid_size.y - 1) * grid_size.x + grid_size.x - 1].position;
  }

  const math::vec3f& GetLeftSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return vertices.data () [(grid_size.y - 1) * grid_size.x].position;
  }

  const math::vec3f& GetTopSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return first_vertex.position;
  }

  const math::vec3f& GetBottomSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x * (grid_size.y - 1)];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [grid_size.x * (grid_size.y - 1) + i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return first_vertex.position;
  }

  bool HasRightSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x - 1];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x + grid_size.x - 1];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasLeftSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.y; i++)
    {
      const RenderableVertex& v = vertices.data () [i * grid_size.x];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasTopSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasBottomSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [grid_size.x * (grid_size.y - 1)];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < grid_size.x; i++)
    {
      const RenderableVertex& v = vertices.data () [grid_size.x * (grid_size.y - 1) + i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  const math::vec3f& GetCornerPosition (scene_graph::PageCurlCorner corner)
  {
    switch (corner) {
      case scene_graph::PageCurlCorner_RightTop:
        return vertices.data () [grid_size.x - 1].position;
      case scene_graph::PageCurlCorner_RightBottom:
        return vertices.data () [(grid_size.y - 1) * grid_size.x + grid_size.x - 1].position;
      case scene_graph::PageCurlCorner_LeftTop:
        return vertices.data () [0].position;
      case scene_graph::PageCurlCorner_LeftBottom:
        return vertices.data () [(grid_size.y - 1) * grid_size.x].position;
      default:
        throw xtl::make_argument_exception ("render::obsolete::render2d::RenderablePageCurlMesh::GetCornerPosition", "corner", corner);
    }
  }
};

/*
    Конструктор
*/

RenderablePageCurlMesh::RenderablePageCurlMesh (low_level::IDevice& device, bool is_rigid, const math::vec2ui& grid_size)
  : impl (new Impl (device, is_rigid, grid_size))
{
}

RenderablePageCurlMesh::~RenderablePageCurlMesh ()
{
}

/*
   Получение размеров сетки
*/

const math::vec2ui& RenderablePageCurlMesh::GridSize () const
{
  return impl->grid_size;
}

/*
   Получение сетки
*/

const math::vec3f* RenderablePageCurlMesh::GridVertices () const
{
  return &impl->vertices.data ()->position;
}

size_t RenderablePageCurlMesh::GridVerticesStride () const
{
  return sizeof (RenderableVertex);
}

/*
    Рисование
*/

void RenderablePageCurlMesh::Draw (low_level::IDevice& device, low_level::IDeviceContext& context)
{
  impl->Draw (device, context);
}

/*
   Трансформация страницы
*/

void RenderablePageCurlMesh::Curl (const math::vec2f& corner_position, scene_graph::PageCurlCorner corner,
                                   float radius, float angle, size_t find_best_curl_steps, float binding_mismatch_weight)
{
  impl->Curl (corner_position, corner, radius, angle, find_best_curl_steps, binding_mismatch_weight);
}

/*
   Рассчет цвета вершин
*/

void RenderablePageCurlMesh::CalculateShadow (bool front, float max_shadow)
{
  impl->CalculateShadow (front, max_shadow);
}

/*
   Установка / получение параметров
*/

void RenderablePageCurlMesh::SetColor (const math::vec4ub& color)
{
  impl->SetColor (color);
}

void RenderablePageCurlMesh::SetSize (const math::vec2f& size)
{
  impl->SetSize (size);
}

void RenderablePageCurlMesh::SetTexCoords (float min_s, float min_t, float max_s, float max_t)
{
  impl->SetTexCoords (min_s, min_t, max_s, max_t);
}

void RenderablePageCurlMesh::SetRigidPagePerspectiveFactor (float factor)
{
  impl->rigid_page_perspective_factor = factor;
}

const math::vec4ub& RenderablePageCurlMesh::Color () const
{
  return impl->color;
}

const math::vec2f& RenderablePageCurlMesh::Size () const
{
  return impl->size;
}

/*
   Получение информации после трансформации
*/

bool RenderablePageCurlMesh::HasRightSideBendPosition ()
{
  return impl->HasRightSideBendPosition ();
}

bool RenderablePageCurlMesh::HasLeftSideBendPosition ()
{
  return impl->HasLeftSideBendPosition ();
}

bool RenderablePageCurlMesh::HasBottomSideBendPosition ()
{
  return impl->HasBottomSideBendPosition ();
}

bool RenderablePageCurlMesh::HasTopSideBendPosition ()
{
  return impl->HasTopSideBendPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetRightSideBendPosition ()
{
  return impl->GetRightSideBendPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetLeftSideBendPosition ()
{
  return impl->GetLeftSideBendPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetTopSideBendPosition ()
{
  return impl->GetTopSideBendPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetBottomSideBendPosition ()
{
  return impl->GetBottomSideBendPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetRightSideDetachPosition ()
{
  return impl->GetRightSideDetachPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetLeftSideDetachPosition ()
{
  return impl->GetLeftSideDetachPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetTopSideDetachPosition ()
{
  return impl->GetTopSideDetachPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetBottomSideDetachPosition ()
{
  return impl->GetBottomSideDetachPosition ();
}

bool RenderablePageCurlMesh::HasRightSideDetachPosition ()
{
  return impl->HasRightSideDetachPosition ();
}

bool RenderablePageCurlMesh::HasLeftSideDetachPosition ()
{
  return impl->HasLeftSideDetachPosition ();
}

bool RenderablePageCurlMesh::HasTopSideDetachPosition ()
{
  return impl->HasTopSideDetachPosition ();
}

bool RenderablePageCurlMesh::HasBottomSideDetachPosition ()
{
  return impl->HasBottomSideDetachPosition ();
}

const math::vec3f& RenderablePageCurlMesh::GetCornerPosition (scene_graph::PageCurlCorner corner)
{
  return impl->GetCornerPosition (corner);
}
