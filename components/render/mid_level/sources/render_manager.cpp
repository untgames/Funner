#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации менеджера рендеринга
*/

struct RenderManagerImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

RenderManagerImpl::RenderManagerImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::RenderManagerImpl");
}

RenderManagerImpl::~RenderManagerImpl ()
{
}

/*
    Строка описания устройства рендеринга
*/

const char* RenderManagerImpl::Description ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Description");
}

/*
    Создание окна рендеринга
*/

WindowPtr RenderManagerImpl::CreateWindow (syslib::Window& window, common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateWindow");
}

/*
    Перебор окон рендеринга
*/

size_t RenderManagerImpl::WindowsCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::WindowsCount");
}

WindowPtr RenderManagerImpl::Window (size_t index)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Window");
}

/*
    Создание целей рендеринга
*/

RenderTargetPtr RenderManagerImpl::CreateRenderBuffer (size_t width, size_t height, render::mid_level::PixelFormat format)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateRenderBuffer");
}

RenderTargetPtr RenderManagerImpl::CreateDepthStencilBuffer (size_t width, size_t height)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateDepthStencilBuffer");
}

/*
    Создание примитивов
*/

PrimitivePtr RenderManagerImpl::CreatePrimitive ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreatePrimitive()");
}

PrimitivePtr RenderManagerImpl::CreatePrimitive (const char* name, ResourceInstanceMode mode)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreatePrimitive(const char*,ResourceInstanceMode)");
}

FramePtr RenderManagerImpl::CreateFrame ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateFrame");
}

TexturePtr RenderManagerImpl::CreateTexture (const media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(const media::Image&)");
}

TexturePtr RenderManagerImpl::CreateTexture (const media::Image& image, render::mid_level::TextureDimension dimension)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(const media::Image&,TextureDimension)");
}

TexturePtr RenderManagerImpl::CreateTexture (render::mid_level::TextureDimension, size_t width, size_t height, size_t depth, render::mid_level::PixelFormat format)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(TextureDimension,size_t,size_t,size_t,PixelFormat)");
}

MaterialPtr RenderManagerImpl::CreateMaterial ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateMaterial");
}

/*
    Загрузка ресурсов
*/

ResourceLibraryPtr RenderManagerImpl::Load (const char* resource_name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const char*)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::MaterialLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::MaterialLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::EffectLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::EffectLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::geometry::MeshLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::geometry::MeshLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::ShaderLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::ShaderLibrary&)");
}

/*
    Регистрация на события
*/

xtl::connection RenderManagerImpl::RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::RegisterWindowEventHandler");
}
