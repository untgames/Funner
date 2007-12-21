#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

View::View (ITexture* texture, const ViewDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::View::View";

  if (!texture)
    RaiseNullArgument (METHOD_NAME, "texture");

  TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  if (in_desc.layer >= texture_desc.layers)
    RaiseOutOfRange (METHOD_NAME, "desc.layer", in_desc.layer, texture_desc.layers);

  desc = in_desc;
}

/*
    Текстура, на которую строится отображение
*/

ITexture* View::GetTexture ()
{
  return &*texture;
}

/*
    Получение дескриптора
*/

void View::GetDesc (ViewDesc& out_desc)
{
  out_desc = desc;
}
