#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    Описание реализации менеджера окон
*/

struct WindowManager::Impl
{
};

/*
    Конструктор / деструктор
*/

WindowManager::WindowManager ()
  : impl (new Impl)
{

}

WindowManager::~WindowManager ()
{
}

/*
    Добавление и удаление окон  
*/

void WindowManager::AttachWindow (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, const manager::Rect& rect)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void WindowManager::DetachWindow (size_t id)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обновление окон
*/

void WindowManager::SetWindowSize (size_t id, size_t width, size_t height)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void WindowManager::SetWindowViewport (size_t id, const manager::Rect& rect)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void WindowManager::SetWindowHandle (size_t id, void* handle)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обновление окна
*/

void WindowManager::RepaintWindow (size_t id)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
