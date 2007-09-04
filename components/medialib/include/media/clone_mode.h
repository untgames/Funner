#ifndef MEDIALIB_CLONE_MODE_HEADER
#define MEDIALIB_CLONE_MODE_HEADER

namespace medialib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим копирования
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CloneMode
{
  CloneMode_Copy,     //копирование
  CloneMode_Instance, //инстанцирование (увеличение числа ссылок)
  
  CloneMode_Default = CloneMode_Copy
};

}

#endif

