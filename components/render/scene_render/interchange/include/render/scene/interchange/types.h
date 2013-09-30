#ifndef RENDER_SCENE_INTERCHANGE_TYPES_HEADER
#define RENDER_SCENE_INTERCHANGE_TYPES_HEADER

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

}

}

}

#endif
