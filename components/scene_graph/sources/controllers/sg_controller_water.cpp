#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

/*
    Константы
*/

const float DEFAULT_VISCOSITY = 0.1f;  //вязкость по умолчанию
const float PLANE_CURVATURE   = 0.1f;  //кривизна поверхности

}

/*
    Описание реализации контроллера воды
*/

typedef stl::vector<float> WaterField;

struct Water::Impl
{
  HeightMap*  height_map; //карта высот
  float       viscosity;  //вязкость
  WaterField  fields [2]; //поля расчёт воды
  float*      prev_field; //предыдущее поле
  float*      next_field; //следующее поле

  Impl (HeightMap& in_height_map)
    : height_map (&in_height_map)
    , viscosity (DEFAULT_VISCOSITY)
    , prev_field (0)
    , next_field (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

Water::Water (HeightMap& map)
  : Controller (map)
  , impl (new Impl (map))
{
  map.RegisterEventHandler (HeightMapEvent_OnSizesUpdate, xtl::bind (&Water::OnChangeSizes, this));
  
  OnChangeSizes ();
}

Water::~Water ()
{
}

/*
    Создание контроллера
*/

Water::Pointer Water::Create (HeightMap& map)
{
  return Pointer (new Water (map), false);
}

/*
    Вязкость
*/

void Water::SetViscosity (float value)
{
  impl->viscosity = value;
}

float Water::Viscosity () const
{
  return impl->viscosity;
}

/*
    Обновление
*/

void Water::Update (float)
{
  if (!impl->prev_field || !impl->next_field || !impl->height_map)
    return;
    
  int                    rows_count    = (int)impl->height_map->RowsCount (),
                         columns_count = (int)impl->height_map->ColumnsCount ();
  HeightMap::VertexDesc* vertices      = impl->height_map->Vertices ();
  float                  normal_z      = -4.0f / impl->height_map->RowsCount (), //magic value???
                         viscosity     = impl->viscosity,
                         ndy           = PLANE_CURVATURE / rows_count,
                         ny            = -PLANE_CURVATURE / 2.0f + ndy;

  for (int row=1; row<rows_count-1; row++, ny += ndy)
  {
    size_t                 index            = row * impl->height_map->ColumnsCount () + 1;
    HeightMap::VertexDesc* vertex           = &vertices [index];
    const float*           prev_field_value = &impl->prev_field [index];
    float*                 next_field_value = &impl->next_field [index];
    float                  ndx              = PLANE_CURVATURE / columns_count,
                           nx               = -PLANE_CURVATURE / 2.0f + ndx;
    
    for (int column=1; column<columns_count-1; column++, vertex++, prev_field_value++, next_field_value++, nx += ndx)
    {
      vertex->height = *prev_field_value;
      vertex->normal = math::vec3f (prev_field_value [-columns_count] - prev_field_value [columns_count],
                                    prev_field_value [-1] - prev_field_value [1],
                                    -normal_z);                                    
                                    
      vertex->normal.z = sqrt (1.0f - vertex->normal.x * vertex->normal.x - vertex->normal.y * vertex->normal.y);
                                    
      float laplas = (prev_field_value [-columns_count] + prev_field_value [columns_count] +
                     prev_field_value [1] + prev_field_value [-1]) * 0.25f - *prev_field_value;

      *next_field_value = (2.0f - viscosity) * *prev_field_value - *next_field_value * (1.0f - viscosity) + laplas;
    }
  }

  stl::swap (impl->next_field, impl->prev_field);
  
  impl->height_map->UpdateVerticesNotify ();
}

/*
    Изменение размеров карты
*/

void Water::OnChangeSizes ()
{
  if (!impl->height_map)
    return;
    
  try
  {
      //инициализация полей
    
    impl->fields [0].clear ();
    impl->fields [1].clear ();
    
    size_t cells_count = impl->height_map->RowsCount () * impl->height_map->ColumnsCount ();
    
    if (cells_count)
    {    
      impl->fields [0].resize (cells_count);
      impl->fields [1].resize (cells_count);
    
      impl->prev_field = &impl->fields [0][0];
      impl->next_field = &impl->fields [1][0];
    }
    else
    {
      impl->prev_field = 0;
      impl->next_field = 0;
    }
  }
  catch (...)
  {
    impl->prev_field = 0;
    impl->next_field = 0;
    
    impl->fields [0].clear ();
    impl->fields [1].clear ();
    
    throw;
  }
}

/*
    Узел был удалён
*/

void Water::OnNodeDestroyed ()
{
  if (!impl->height_map)
    return;
    
  impl->height_map = 0;
  impl->prev_field = 0;
  impl->next_field = 0;
  
  impl->fields [0].clear ();
  impl->fields [1].clear ();
}

/*
    Добавление возмущения
*/

void Water::PutStorm (const math::vec3f& position, float radius)
{
  if (!impl->height_map)
    return;    

  int    storm_rows    = (int)(impl->height_map->RowsCount () * radius),
         storm_columns = (int)(impl->height_map->ColumnsCount () * radius),
         row           = (int)((position.y + 0.5f) * impl->height_map->RowsCount ()),
         column        = (int)((position.x + 0.5f) * impl->height_map->ColumnsCount ()),
         rows_count    = (int)impl->height_map->RowsCount (),
         columns_count = (int)impl->height_map->ColumnsCount ();
         
  for (int i=-storm_rows; i<=storm_rows; i++)
  {
    int affected_row = i + row + storm_rows - 1;
    
    if (affected_row >= rows_count || affected_row < 0)
      continue;
    
    for (int j=-storm_columns; j<=storm_columns; j++)
    {
      int affected_column = j + column + storm_columns - 1;
      
      if (affected_column >= columns_count || affected_column < 0)
        continue;

      float v = 6.0f - i * i - j * j; //magic???

      if (v < 0.0f)
        v = 0.0f;
        
      impl->prev_field [affected_row * columns_count + affected_column] += v * 0.004f; //magic???
    }
  }
}

void Water::PutWorldStorm (const math::vec3f& position, float radius)
{
  if (!impl->height_map)
    return;

  PutStorm (inverse (impl->height_map->WorldTM ()) * position, radius);
}
