#ifndef RENDER_SCENE_INTERCHANGE_TYPES_HEADER
#define RENDER_SCENE_INTERCHANGE_TYPES_HEADER

#include <math/angle.h>
#include <math/vector.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определения базовых типов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef   signed long long int64;
typedef unsigned long long uint64;
typedef   signed int       int32;
typedef unsigned int       uint32;
typedef   signed short     int16;
typedef unsigned short     uint16;
typedef   signed char      int8;
typedef unsigned char      uint8;
typedef float              float32;
typedef int8               bool8;

typedef uint32             command_id_t;
typedef uint64             object_id_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Заголовок команды рендеру
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Command
{
  command_id_t command_id;   //идентификатор команды
  uint32       command_size; //размер команды
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Типы узлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeType
{
  NodeType_Node,        //узел
  NodeType_Entity,      //сущность
  NodeType_Scissor,     //область отсечения
  NodeType_VisualModel, //отображаемая модель
  NodeType_SpriteModel, //спрайты
  NodeType_StaticMesh,  //статический меш
  NodeType_PointLight,  //точечный источник света
  NodeType_SpotLight,   //конусоидальный источник света
  NodeType_DirectLight, //направленный источник света
  NodeType_PageCurl,    //разворот книги
  NodeType_SpriteList,  //список спрайтов
  NodeType_LineList,    //список линий

  NodeType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник света
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightParams
{
  math::vec3f  color;       //цвет
  float        intensity;   //интенсивность
  math::vec3f  attenuation; //затенение
  float        range;       //расстояние действия
  float        exponent;    //экспонента рассеивания света (для spot-light)
  math::anglef angle;       //угол действия (для spot-light)
  float        radius;      //радиус действия (для direct-light)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разворот книги
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PageCurlParams
{
  math::vec4f  page_color;                    //цвет страницы
  math::vec2f  size;                          //полный размера страниц
  math::vec2f  curl_point;                    //точка перетаскивания
  math::vec2f  curl_point_position;           //текущее положение точки перетаскивания
  math::vec2ui grid_size;                     //детализация разбиения сетки
  size_t       find_best_curl_steps;          //количество итераций поиска наилучшей позиции загиба
  int          mode;                          //режим (PageCurlMode)
  float        curl_radius;                   //настройка изгиба
  float        minimum_curl_radius;           //настройка изгиба
  float        corner_shadow_offset;          //смещение тени от угла
  float        shadow_width;                  //ширина тени
  float        shadow_log_base;               //основание логарифма генерации тени
  float        shadow_min_log_value;          //минимальное значение тени при логарифмировании
  float        binding_mismatch_weight;       //вес отклонения позиции сгиба страницы при поиске наилучешй позиции загиба
  float        rigid_page_perspective_factor; //коэффициент увеличения края жесткой страницы для симуляции перспективы
  bool         is_rigid_page;                 //является ли страница жесткой
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SpriteMode
{
  SpriteMode_Billboard,         //up vector is view up,  normal & rotation is ignored
  SpriteMode_Oriented,          //up vector is local up, normal & rotation is used
  SpriteMode_OrientedBillboard, //up vector is view up,  normal & rotation is used
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим использования примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveUsage
{
  PrimitiveUsage_Static,   //не обновляемая модель
  PrimitiveUsage_Dynamic,  //обновляемая модель
  PrimitiveUsage_Stream,   //часто обновляемая модель (каждый кадр)
  PrimitiveUsage_Batching, //пакетирование
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор спрайта
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SpriteDesc
{
  math::vec3f  position;   //положение центра спрайта
  math::vec2f  size;       //размер спрайта
  math::vec4f  color;      //цвет спрайта
  math::vec2f  tex_offset; //смещение начала спрайта в текстуре [0;1]
  math::vec2f  tex_size;   //размер спрайта в текстуре [0;1]
  math::vec3f  normal;     //нормаль
  math::anglef rotation;   //поворот относительно нормали
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Точка линии
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LinePoint
{
  math::vec3f position;   //положение точки в пространстве
  math::vec4f color;      //цвет точки
  math::vec2f tex_offset; //смещение точки в текстуре [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Линия
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LineDesc
{
  LinePoint point [2]; //концы линии
};

}

}

}

#endif
