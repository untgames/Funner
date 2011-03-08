#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации библиотеки ресурсов
*/

struct ResourceLibraryImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

ResourceLibraryImpl::ResourceLibraryImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::ResourceLibraryImpl::ResourceLibraryImpl");
}

ResourceLibraryImpl::~ResourceLibraryImpl ()
{
}

/*
    Присоединение библиотеки
*/

void ResourceLibraryImpl::Attach (const ResourceLibraryPtr& library)
{
  throw xtl::make_not_implemented_exception ("render::ResourceLibraryImpl::Attach");
}

void ResourceLibraryImpl::Detach (const ResourceLibraryPtr& library)
{
  throw xtl::make_not_implemented_exception ("render::ResourceLibraryImpl::Detach");
}

void ResourceLibraryImpl::DetachAllLibraries ()
{
  throw xtl::make_not_implemented_exception ("render::ResourceLibraryImpl::DetachAllLibraries");
}
