#include "shared.h"

using namespace scene_graph;

namespace
{

const float  DEFAULT_BINDING_MISMATCH_WEIGHT = 10.f;
const float  DEFAULT_CORNER_SHADOW_OFFSET    = 15.f;
const size_t DEFAULT_FIND_BEST_CURL_STEPS    = 1000;
const size_t DEFAULT_GRID_SIZE               = 100;
const float  DEFAULT_SHADOW_LOG_BASE         = 4.f;
const float  DEFAULT_SHADOW_MIN_LOG_VALUE    = 0.3f;
const float  DEFAULT_SHADOW_WIDTH            = 0.25f;

}

/*
    Описание реализации перелистывания страницы
*/

struct PageCurl::Impl: public xtl::instance_counter<PageCurl>
{
  PageCurlMode   mode;                               //режим страниц
  stl::string    materials [PageCurlPageType_Num];   //материалы
  math::vec2f    size;                               //полный размер страниц
  math::vec2f    curl_point;                         //точка перетаскивания
  math::vec2f    curl_point_position;                //позиция точки перетаскивания
  float          curl_radius;                        //радиус загиба
  float          minimum_curl_radius;                //минимальный радиус загиба
  math::vec2ui   grid_size;                          //количество разбиений сетки
  math::vec4f    color;                              //цвет страницы
  float          corner_shadow_offset;               //смещение тени от угла
  float          shadow_width;                       //ширина тени
  float          shadow_log_base;                    //основание логарифма генерации тени
  float          shadow_min_log_value;               //минимальное значение тени при логарифмировании
  size_t         find_best_curl_steps;               //количество итераций поиска наилучшей позиции загиба
  float          binding_mismatch_weight;            //вес отклонения позиции сгиба страницы при поиске наилучешй позиции загиба
  bool           is_rigid_page;                      //является ли страница жесткой
  float          rigid_page_perspective_factor;      //коэффициент увеличения края жесткой страницы для симуляции перспективы

  Impl ()
    : mode (PageCurlMode_SinglePage)
    , size (1.f)
    , curl_radius (0.f)
    , minimum_curl_radius (0.f)
    , grid_size (DEFAULT_GRID_SIZE)
    , color (1.f)
    , corner_shadow_offset (DEFAULT_CORNER_SHADOW_OFFSET)
    , shadow_width (DEFAULT_SHADOW_WIDTH)
    , shadow_log_base (DEFAULT_SHADOW_LOG_BASE)
    , shadow_min_log_value (DEFAULT_SHADOW_MIN_LOG_VALUE)
    , find_best_curl_steps (DEFAULT_FIND_BEST_CURL_STEPS)
    , binding_mismatch_weight (DEFAULT_BINDING_MISMATCH_WEIGHT)
    , is_rigid_page (false)
    , rigid_page_perspective_factor (1.f)
    {}
};

/*
    Конструктор / деструктор
*/

PageCurl::PageCurl ()
  : impl (new Impl)
{
  UpdateBoundsNotify ();
}

PageCurl::~PageCurl ()
{
  delete impl;
}

/*
    Создание
*/

PageCurl::Pointer PageCurl::Create ()
{
  return Pointer (new PageCurl, false);
}

/*
   Установка режима страниц
*/

void PageCurl::SetMode (PageCurlMode mode)
{
  switch (mode)
  {
    case PageCurlMode_SinglePage:
    case PageCurlMode_DoublePageSingleMaterial:
    case PageCurlMode_DoublePageDoubleMaterial:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::PageCurl::SetMode", "mode", mode);
  }

  impl->mode = mode;

  UpdateNotify ();
}

PageCurlMode PageCurl::Mode () const
{
  return impl->mode;
}

/*
   Установка материалов
*/

void PageCurl::SetPageMaterial (PageCurlPageType type, const char* name)
{
  static const char* METHOD_NAME = "scene_graph::PageCurl::SetPageMaterial";

  if (type >= PageCurlPageType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  impl->materials [type] = name;

  UpdateNotify ();
}

const char* PageCurl::PageMaterial (PageCurlPageType type) const
{
  if (type >= PageCurlPageType_Num)
    throw xtl::make_argument_exception ("scene_graph::PageCurl::PageMaterial", "type", type);

  return impl->materials [type].c_str ();
}

/*
   Установка полного размера страниц
*/

void PageCurl::SetSize (const math::vec2f& size)
{
  impl->size = size;

  UpdateBoundsNotify ();
}

void PageCurl::SetSize (float width, float height)
{
  SetSize (math::vec2f (width, height));
}

const math::vec2f& PageCurl::Size () const
{
  return impl->size;
}

/*
   Установка точки перетаскивания
*/

void PageCurl::SetCurlPoint (PageCurlCorner corner)
{
  switch (corner)
  {
    case PageCurlCorner_LeftTop:
      SetCurlPoint (math::vec2f (0, impl->size.y));
      break;
    case PageCurlCorner_LeftBottom:
      SetCurlPoint (math::vec2f (0, 0));
      break;
    case PageCurlCorner_RightTop:
      SetCurlPoint (math::vec2f (impl->size.x, impl->size.y));
      break;
    case PageCurlCorner_RightBottom:
      SetCurlPoint (math::vec2f (impl->size.x, 0));
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::CurlCurl::SetCurlPoint", "corner", corner);
  }
}

void PageCurl::SetCurlPoint (const math::vec2f& point)
{
  impl->curl_point = point;

  UpdateNotify ();
}

void PageCurl::SetCurlPoint (float x, float y)
{
  SetCurlPoint (math::vec2f (x, y));
}

const math::vec2f& PageCurl::CurlPoint () const
{
  return impl->curl_point;
}

void PageCurl::SetCurlPointPosition (const math::vec2f& position)
{
  impl->curl_point_position = position;

  UpdateNotify ();
}

void PageCurl::SetCurlPointPosition (float x, float y)
{
  SetCurlPointPosition (math::vec2f (x, y));
}

const math::vec2f& PageCurl::CurlPointPosition () const
{
  return impl->curl_point_position;
}

/*
   Настройка загиба
*/

void PageCurl::SetCurlRadius (float curl_radius)
{
  impl->curl_radius = curl_radius;

  UpdateBoundsNotify ();
}

float PageCurl::CurlRadius () const
{
  return impl->curl_radius;
}

void PageCurl::SetMinimumCurlRadius (float minimum_curl_radius)
{
  impl->minimum_curl_radius = minimum_curl_radius;

  UpdateNotify ();
}

float PageCurl::MinimumCurlRadius () const
{
  return impl->minimum_curl_radius;
}

/*
   Опциональные параметры
*/

void PageCurl::SetGridSize (const math::vec2ui& size)
{
  if (size.x < 2 || size.y < 2)
    throw xtl::format_operation_exception ("scene_graph::PageCurl::SetGridSize", "Invalid size %ux%u, has to be greater than 2 for both axises", size.x, size.y);

  impl->grid_size = size;

  UpdateNotify ();
}

void PageCurl::SetGridSize (size_t x, size_t y)
{
  SetGridSize (math::vec2ui (x, y));
}

const math::vec2ui& PageCurl::GridSize () const
{
  return impl->grid_size;
}

void PageCurl::SetPageColor (const math::vec4f& color)
{
  impl->color = color;

  UpdateNotify ();
}

void PageCurl::SetPageColor (float red, float green, float blue, float alpha)
{
  SetPageColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& PageCurl::PageColor () const
{
  return impl->color;
}

void PageCurl::SetCornerShadowOffset (float shadow_offset)
{
  impl->corner_shadow_offset = shadow_offset;

  UpdateNotify ();
}

float PageCurl::CornerShadowOffset () const
{
  return impl->corner_shadow_offset;
}

void PageCurl::SetShadowWidth (float width)
{
  impl->shadow_width = width;

  UpdateNotify ();
}

float PageCurl::ShadowWidth () const
{
  return impl->shadow_width;
}

void PageCurl::SetShadowLogBase (float log_base)
{
  impl->shadow_log_base = log_base;

  UpdateNotify ();
}

float PageCurl::ShadowLogBase () const
{
  return impl->shadow_log_base;
}

void PageCurl::SetShadowMinLogValue (float value)
{
  impl->shadow_min_log_value = value;
}

float PageCurl::ShadowMinLogValue () const
{
  return impl->shadow_min_log_value;
}

void PageCurl::SetFindBestCurlSteps (size_t count)
{
  impl->find_best_curl_steps = count;

  UpdateNotify ();
}

size_t PageCurl::FindBestCurlSteps () const
{
  return impl->find_best_curl_steps;
}

void PageCurl::SetBindingMismatchWeight (float weight)
{
  impl->binding_mismatch_weight = weight;

  UpdateNotify ();
}

float PageCurl::BindingMismatchWeight () const
{
  return impl->binding_mismatch_weight;
}

void PageCurl::SetRigidPage (bool state)
{
  impl->is_rigid_page = state;
}

bool PageCurl::IsRigidPage () const
{
  return impl->is_rigid_page;
}

void PageCurl::SetRigidPagePerspectiveFactor (float factor)
{
  impl->rigid_page_perspective_factor = factor;
}

float PageCurl::RigidPagePerspectiveFactor () const
{
  return impl->rigid_page_perspective_factor;
}

/*
   Рассчёт ограничивающего объёма
*/

void PageCurl::UpdateBoundsCore ()
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-impl->size.x / 2, -impl->size.y / 2, -impl->curl_radius, impl->size.x / 2, impl->size.y / 2, 0));
}

/*
    Метод, вызываемый при посещении данного объекта
*/

void PageCurl::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);  
}
