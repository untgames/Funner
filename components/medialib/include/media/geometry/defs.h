#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <stddef.h>

namespace medialib
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим копирования буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BufferCloneMode
{
  BufferCloneMode_Copy,     //копирование
  BufferCloneMode_Instance, //инстанцирование (увеличение числа ссылок)
  BufferCloneMode_Default   //сохраняется режим копирования источника
};

}

}

#endif
