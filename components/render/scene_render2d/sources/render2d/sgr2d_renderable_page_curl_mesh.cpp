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
  PositionBuffer original_vertices;         //данные вершин плоской страницы
  VertexBuffer   vertices;                  //данные вершин трансформированной страницы
  BufferPtr      vertex_buffer;             //вершинный буфер
  BufferPtr      index_buffer;              //индексный буфер
  size_t         x_size;                    //размер сетки по горизонтали
  size_t         y_size;                    //размер сетки по вертикали
  size_t         triangles_count;           //количество треугольников
  size_t         vertices_count;            //количество вершин
  float          width;                     //ширина
  float          height;                    //высота
  float          min_s;                     //минимальная текстурная координата по горизонтали
  float          min_t;                     //минимальная текстурная координата по вертикали
  float          max_s;                     //максимальная текстурная координата по горизонтали
  float          max_t;                     //максимальная текстурная координата по вертикали
  float          last_curl_radius;          //радиус последней трансформации
  math::vec4ub   color;                     //цвет

  ///Конструктор
  Impl (low_level::IDevice& device, const math::vec2ui& grid_size)
    : x_size (grid_size.x)
    , y_size (grid_size.y)
    , width  (-1)
    , height (-1)
    , min_s  (-1)
    , min_t  (-1)
    , max_s  (-1)
    , max_t  (-1)
    , last_curl_radius (0)
    , color  (0)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurlMesh::RenderablePageCurlMesh";

    if (x_size < 2)
      throw xtl::make_argument_exception (METHOD_NAME, "x_size", x_size, "x_size must be greater than 1");

    if (y_size < 2)
      throw xtl::make_argument_exception (METHOD_NAME, "y_size", y_size, "y_size must be greater than 1");

    vertices_count = x_size * y_size;

    if (vertices_count > USHRT_MAX)
      throw xtl::format_operation_exception (METHOD_NAME, "x_size * y_size is larger than max unsigned short value (x_size = %u, y_size = %u)", x_size, y_size);

    triangles_count = (x_size - 1) * (y_size - 1) * 2;

    size_t indices_count = triangles_count * 3;

    original_vertices.resize (vertices_count, false);
    vertices.resize          (vertices_count, false);

    xtl::uninitialized_storage<unsigned short> indices (indices_count);

    for (size_t i = 0; i < (x_size - 1) * 2; i++)
    {
      for (size_t j = 0; j < y_size - 1; j++)
      {
        unsigned short* current_indices = &indices.data () [(j * (x_size - 1) * 2 + i) * 3];

        if (i % 2)
        {
          current_indices [0] = (j + 1) * x_size + i / 2;
          current_indices [1] = (j + 1) * x_size + i / 2 + 1;
          current_indices [2] = j * x_size + i / 2 + 1;
        }
        else
        {
          current_indices [0] = j * x_size + i / 2;
          current_indices [1] = (j + 1) * x_size + i / 2;
          current_indices [2] = j * x_size + i / 2 + 1;
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
  void Draw (low_level::IDevice& device)
  {
    vertex_buffer->SetData (0, sizeof (RenderableVertex) * vertices_count, vertices.data ());

    device.ISSetIndexBuffer  (index_buffer.get ());
    device.ISSetVertexBuffer (0, vertex_buffer.get ());

    device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, triangles_count * 3, 0);
  }

  //Трансформация страницы
  void Curl (const math::vec2f& corner_position, scene_graph::PageCurlCorner corner, float curl_x, float radius,
             float angle, size_t find_best_curl_steps, float binding_mismatch_weight)
  {
    last_curl_radius = radius;

    math::vec2f original_corner_location, top_binding, bottom_binding;

    switch (corner)
    {
      case scene_graph::PageCurlCorner_RightBottom:
        original_corner_location.x = width;
        original_corner_location.y = height;
        top_binding.x              = 0.f;
        top_binding.y              = 0.f;
        bottom_binding.x           = 0.f;
        bottom_binding.y           = height;
        break;
      case scene_graph::PageCurlCorner_RightTop:
        original_corner_location.x = width;
        original_corner_location.y = 0.f;
        top_binding.x              = 0.f;
        top_binding.y              = 0.f;
        bottom_binding.x           = 0.f;
        bottom_binding.y           = height;
        break;
      case scene_graph::PageCurlCorner_LeftBottom:
        original_corner_location.x = 0.f;
        original_corner_location.y = height;
        top_binding.x              = width;
        top_binding.y              = 0.f;
        bottom_binding.x           = width;
        bottom_binding.y           = height;
        break;
      case scene_graph::PageCurlCorner_LeftTop:
        original_corner_location.x = 0.f;
        original_corner_location.y = 0.f;
        top_binding.x              = width;
        top_binding.y              = 0.f;
        bottom_binding.x           = width;
        bottom_binding.y           = height;
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
          rotated_top_binding_location_x     = top_binding.x * cos_curl_angle - top_binding.y * sin_curl_angle,
          rotated_bottom_binding_location_x  = bottom_binding.x * cos_curl_angle - bottom_binding.y * sin_curl_angle,
          one_divide_radius                  = 1.f / radius;

    float best_curl_mismatch = FLT_MAX,
          best_curl_step     = width / find_best_curl_steps;

    for (float current_curl_x = -2.f * width; current_curl_x < 2.f * width; current_curl_x += best_curl_step)
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
      }
    }

    RenderableVertex*  v          = vertices.data ();
    const math::vec3f* original_v = original_vertices.data ();

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
      v->position.y = height - (x * sin_minus_curl_angle + y * cos_curl_angle);
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

  void SetSize (const math::vec2f& size)
  {
    if (width == size.x && height == size.y)
      return;

    width  = size.x;
    height = size.y;

    for (size_t i = 0; i < x_size; i++)
    {
      for (size_t j = 0; j < y_size; j++)
      {
        math::vec3f& vertex = original_vertices.data () [j * x_size + i];

        vertex.x = i / (float)(x_size - 1) * width;
        vertex.y = j / (float)(y_size - 1) * height;
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
          ds      = 1.f / (x_size - 1) * s_range,
          dt      = 1.f / (y_size - 1) * t_range,
          t       = max_t;

    RenderableVertex* vertex = vertices.data ();

    for (size_t j = 0; j < y_size; j++, t -= dt)
    {
      float s = min_s;

      for (size_t i = 0; i < x_size; i++, vertex++, s += ds)
      {
        vertex->texcoord.x = s;
        vertex->texcoord.y = t;
      }
    }
  }

  //Рассчет цвета вершин
  void CalculateShadow (bool front)
  {
    if (last_curl_radius == 0)
      return;

    math::vec4ub colors [256];

    math::vec4ub *current_color = colors;

    float intensity      = 0,
          intensity_step = 1.f / 255.f;

    for (size_t i = 0; i < 256; i++, current_color++, intensity += intensity_step)
    {
      current_color->x = (unsigned char)(color.x * intensity);
      current_color->y = (unsigned char)(color.y * intensity);
      current_color->z = (unsigned char)(color.z * intensity);
      current_color->w = color.w;
    }

    RenderableVertex* vertex = vertices.data ();

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
   const RenderableVertex& first_vertex = vertices.data () [x_size - 1];
   bool                    first_larger = first_vertex.position.z < last_curl_radius;

   for (size_t i = 0; i < y_size; i++)
   {
     const RenderableVertex& v = vertices.data () [i * x_size + x_size - 1];

     if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
       return true;
   }

   return false;
  }

  bool HasLeftSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  bool HasBottomSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size * (y_size - 1)];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [x_size * (y_size - 1) + i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  bool HasTopSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return true;
    }

    return false;
  }

  const math::vec3f& GetRightSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size - 1];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size + x_size - 1];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [(y_size - 1) * x_size + x_size - 1].position;
  }

  const math::vec3f& GetLeftSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [(y_size - 1) * x_size].position;
  }

  const math::vec3f& GetTopSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [x_size - 1].position;
  }

  const math::vec3f& GetBottomSideBendPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size * (y_size - 1)];
    bool                    first_larger = first_vertex.position.z < last_curl_radius;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [x_size * (y_size - 1) + i];

      if ((first_larger && v.position.z > last_curl_radius) || (!first_larger && v.position.z < last_curl_radius))
        return v.position;
    }

    return vertices.data () [x_size * (y_size - 1) + x_size - 1].position;
  }

  const math::vec3f& GetRightSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size - 1];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size + x_size - 1];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return vertices.data () [(y_size - 1) * x_size + x_size - 1].position;
  }

  const math::vec3f& GetLeftSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return vertices.data () [(y_size - 1) * x_size].position;
  }

  const math::vec3f& GetTopSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return first_vertex.position;
  }

  const math::vec3f& GetBottomSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size * (y_size - 1)];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [x_size * (y_size - 1) + i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return v.position;
    }

    return first_vertex.position;
  }

  bool HasRightSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size - 1];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size + x_size - 1];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasLeftSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < y_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i * x_size];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasTopSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [0];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  bool HasBottomSideDetachPosition ()
  {
    const RenderableVertex& first_vertex = vertices.data () [x_size * (y_size - 1)];
    bool                    first_larger = first_vertex.position.z > 0;

    for (size_t i = 0; i < x_size; i++)
    {
      const RenderableVertex& v = vertices.data () [x_size * (y_size - 1) + i];

      if ((first_larger && v.position.z == 0) || (!first_larger && v.position.z > 0))
        return true;
    }

    return false;
  }

  const math::vec3f& GetCornerPosition (scene_graph::PageCurlCorner corner)
  {
    switch (corner) {
      case scene_graph::PageCurlCorner_RightTop:
        return vertices.data () [x_size - 1].position;
      case scene_graph::PageCurlCorner_RightBottom:
        return vertices.data () [(y_size - 1) * x_size + x_size - 1].position;
      case scene_graph::PageCurlCorner_LeftTop:
        return vertices.data () [0].position;
      case scene_graph::PageCurlCorner_LeftBottom:
        return vertices.data () [(y_size - 1) * x_size].position;
      default:
        throw xtl::make_argument_exception ("render::obsolete::render2d::RenderablePageCurlMesh::GetCornerPosition", "corner", corner);
    }
  }
};

/*
    Конструктор
*/

RenderablePageCurlMesh::RenderablePageCurlMesh (low_level::IDevice& device, const math::vec2ui& grid_size)
  : impl (new Impl (device, grid_size))
{
}

RenderablePageCurlMesh::~RenderablePageCurlMesh ()
{
}

/*
    Рисование
*/

void RenderablePageCurlMesh::Draw (low_level::IDevice& device)
{
  impl->Draw (device);
}

/*
   Трансформация страницы
*/

void RenderablePageCurlMesh::Curl (const math::vec2f& corner_position, scene_graph::PageCurlCorner corner, float curl_x,
                                   float radius, float angle, size_t find_best_curl_steps, float binding_mismatch_weight)
{
  impl->Curl (corner_position, corner, curl_x, radius, angle, find_best_curl_steps, binding_mismatch_weight);
}

/*
   Рассчет цвета вершин
*/

void RenderablePageCurlMesh::CalculateShadow (bool front)
{
  impl->CalculateShadow (front);
}

/*
   Установка параметров
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
