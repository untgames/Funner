#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

/*
    Константы
*/

const float DEFAULT_VISCOSITY = 0.1f;  //вязкость по умолчанию
const float TIME_STEP         = 0.03f; //шаг интеграции
const float NORMAL_Z_FACTOR   = -4.0f; //коэффициент масштабирования нормали

}

/*
    Описание реализации контроллера воды
*/

typedef stl::vector<float> WaterField;

struct Water::Impl: public xtl::instance_counter<Water>
{
  HeightMap*  height_map; //карта высот
  float       viscosity;  //вязкость
  float       current_dt; //текущее накопленный шаг времени
  WaterField  fields [2]; //поля расчёт воды
  float*      prev_field; //предыдущее поле
  float*      next_field; //следующее поле

  Impl (HeightMap& in_height_map)
    : height_map (&in_height_map)
    , viscosity (DEFAULT_VISCOSITY)
    , current_dt (0)
    , prev_field (0)
    , next_field (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

Water::Water (HeightMap& map)
  : Controller (map, ControllerTimeMode_Delta)
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

void Water::Update (const TimeValue& time_value)
{
  if (!impl->prev_field || !impl->next_field || !impl->height_map)
    return;
    
  float dt = time_value.cast<float> ();  
    
  impl->current_dt += dt;
  
  if (impl->current_dt < TIME_STEP)
    return;
    
  int                    rows_count    = (int)impl->height_map->RowsCount (),
                         columns_count = (int)impl->height_map->ColumnsCount ();
  HeightMap::VertexDesc* vertices      = impl->height_map->Vertices ();
  float                  normal_z      = -NORMAL_Z_FACTOR / stl::max (impl->height_map->RowsCount (), impl->height_map->ColumnsCount ()),
                         viscosity     = impl->viscosity;
                         
  for (;impl->current_dt >= TIME_STEP; impl->current_dt -= TIME_STEP)
  {
    bool need_update_vertices = impl->current_dt < TIME_STEP * 2.0f;
    
    for (int row=1; row<rows_count-1; row++)
    {
      size_t                 index            = row * impl->height_map->ColumnsCount () + 1;
      HeightMap::VertexDesc* vertex           = &vertices [index];
      const float*           prev_field_value = &impl->prev_field [index];
      float*                 next_field_value = &impl->next_field [index];
      
      for (int column=1; column<columns_count-1; column++, vertex++, prev_field_value++, next_field_value++)
      {
        if (need_update_vertices)
        {
          vertex->height = *prev_field_value;
          vertex->normal = math::vec3f (prev_field_value [-columns_count] - prev_field_value [columns_count],
                                        prev_field_value [-1] - prev_field_value [1],
                                        -normal_z);

    //      vertex->normal.z = -sqrt (1.0f - vertex->normal.x * vertex->normal.x - vertex->normal.y * vertex->normal.y);
        }

        float laplas = (prev_field_value [-columns_count] + prev_field_value [columns_count] +
                       prev_field_value [1] + prev_field_value [-1]) * 0.25f - *prev_field_value;

        *next_field_value = (2.0f - viscosity) * *prev_field_value - *next_field_value * (1.0f - viscosity) + laplas;
      }
    }

    stl::swap (impl->next_field, impl->prev_field);
  }
  
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

void Water::OnNodeDetached ()
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

void Water::PutStorm (const math::vec3f& position, float amplitude, float radius)
{
  if (!impl->height_map)
    return;        

  int    storm_cells   = (int)(stl::min (impl->height_map->RowsCount (), impl->height_map->ColumnsCount ()) * radius),
         row           = (int)((position.y + 0.5f) * impl->height_map->RowsCount ()),
         column        = (int)((position.x + 0.5f) * impl->height_map->ColumnsCount ()),
         rows_count    = (int)impl->height_map->RowsCount (),
         columns_count = (int)impl->height_map->ColumnsCount ();
         
  float max_v = (float)(2 * storm_cells * storm_cells);
         
  for (int i=-storm_cells; i<=storm_cells; i++)
  {
    int affected_row = i + row + storm_cells - 1;
    
    if (affected_row >= rows_count || affected_row < 0)
      continue;
    
    for (int j=-storm_cells; j<=storm_cells; j++)
    {
      int affected_column = j + column + storm_cells - 1;
      
      if (affected_column >= columns_count || affected_column < 0)
        continue;

      float v = max_v - (float)(i * i - j * j);

      impl->prev_field [affected_row * columns_count + affected_column] += v * amplitude;
    }
  }
}

void Water::PutWorldStorm (const math::vec3f& position, float amplitude, float radius)
{
  if (!impl->height_map)
    return;

  PutStorm (inverse (impl->height_map->WorldTM ()) * position, amplitude, radius);
}
