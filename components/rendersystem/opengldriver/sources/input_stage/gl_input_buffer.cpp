#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///Конструктор / деструктор
Buffer::Buffer (const BufferDesc&)
{
}

///Получение дескриптора
void Buffer::GetDesc (BufferDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Buffer::GetDesc");
}
