#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации текстуры
*/

struct TextureImpl::Impl
{
};

/*
    Конструктор / деструктор / присваивание
*/

TextureImpl::TextureImpl 
 (render::mid_level::TextureDimension dimension,
  size_t                              width,
  size_t                              height,
  size_t                              depth,
  render::mid_level::PixelFormat      format)
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::TextureImpl");
}

TextureImpl::~TextureImpl ()
{
}

/*
    Идентификатор текстуры в менеджере рендеринга
*/

const char* TextureImpl::Id ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Id");
}

void TextureImpl::SetId (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::SetId");
}

/*
    Размерность
*/

render::mid_level::TextureDimension TextureImpl::Dimension ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Dimension");
}

/*
    Формат и размеры
*/

render::mid_level::PixelFormat TextureImpl::Format ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Format");
}

size_t TextureImpl::Width ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Width");
}

size_t TextureImpl::Height ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Height");
}

size_t TextureImpl::Depth ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Depth");
}

/*
    Получение цели рендеринга
*/

RenderTargetPtr TextureImpl::RenderTarget (size_t layer, size_t mip_level)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::RenderTarget");
}

/*
    Обновление образа
*/

void TextureImpl::Update (const media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::TextureImpl::Update");
}
