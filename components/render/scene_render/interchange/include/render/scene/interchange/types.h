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
///ќпределени€ базовых типов
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
///«аголовок команды рендеру
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Command
{
  command_id_t command_id;   //идентификатор команды
  uint32       command_size; //размер команды
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///“ипы узлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeType
{
  NodeType_Node,        //узел
  NodeType_Entity,      //сущность
  NodeType_Scissor,     //область отсечени€
  NodeType_VisualModel, //отображаема€ модель
  NodeType_SpriteModel, //спрайты
  NodeType_StaticMesh,  //статический меш
  NodeType_PointLight,  //точечный источник света
  NodeType_SpotLight,   //конусоидальный источник света
  NodeType_DirectLight, //направленный источник света

  NodeType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///»сточник света
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightParams
{
  math::vec3f  color;       //цвет
  float        intensity;   //интенсивность
  math::vec3f  attenuation; //затенение
  float        range;       //рассто€ние действи€
  float        exponent;    //экспонента рассеивани€ света (дл€ spot-light)
  math::anglef angle;       //угол действи€ (дл€ spot-light)
  float        radius;      //радиус действи€ (дл€ direct-light)
};

}

}

}

#endif
