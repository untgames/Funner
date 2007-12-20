#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///Конструктор / деструктор
Buffer::Buffer (const BufferDesc& desc)
{
  bufferdesc = desc;
}

///Получение дескриптора
void Buffer::GetDesc (BufferDesc& desc)
{
  desc = bufferdesc;
}
