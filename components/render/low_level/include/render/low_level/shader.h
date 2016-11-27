#ifndef RENDER_LOW_LEVEL_SHADER_HEADER
#define RENDER_LOW_LEVEL_SHADER_HEADER

#include <render/low_level/common.h>
#include <render/low_level/object.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShaderDesc
{
  const char*  name;             //имя шейдера (для вывода диагностических сообщений)
  unsigned int source_code_size; //длина буфера с исходным кодом шейдера ((unsigned int)(-1) - длина определяется автоматически)
  const char*  source_code;      //буфер с исходным кодом
  const char*  profile;          //профиль шейдера
  const char*  options;          //опции компиляции (в зависимости от профиля)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Шейдер
///////////////////////////////////////////////////////////////////////////////////////////////////
class IProgram: virtual public IObject
{
};

}

}

#endif
