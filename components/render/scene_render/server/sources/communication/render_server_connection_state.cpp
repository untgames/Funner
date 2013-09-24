#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации состояния соединения
*/

struct ConnectionState::Impl
{
  ServerImpl& server; //ссылка на сервер

/// Конструктор
  Impl (ServerImpl& in_server) : server (in_server) {}  
};

/*
    Конструктор / деструктор
*/

ConnectionState::ConnectionState (ServerImpl& server)
{
  try
  {
    impl.reset (new Impl (server));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionState::ConnectionState");
    throw;
  }
}

ConnectionState::~ConnectionState ()
{
}

/*
    Синхронизация свойств
*/

PropertyMapSynchronizer ConnectionState::CreateSynchronizer (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

common::PropertyMap ConnectionState::GetClientProperties (uint64 id)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Команды клиента
*/

void ConnectionState::LoadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::UnloadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::CreateViewport (uint32 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateViewport");
    throw;
  }
}

void ConnectionState::SetViewportArea (uint32 id, int8 left, int8 top, int8 width, int8 height)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportArea");
    throw;
  }
}

void ConnectionState::SetViewportZOrder (uint32 id, int8 zorder)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportZOrder");
    throw;
  }
}

void ConnectionState::SetViewportActive (uint32 id, bool8 is_active)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportActive");
    throw;
  }
}

void ConnectionState::SetViewportName (uint32 id, const char* name)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportName");
    throw;
  }
}

void ConnectionState::SetViewportTechnique (uint32 id, const char* name)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportTechnique");
    throw;
  }
}

void ConnectionState::SetViewportBackground (uint32 id, bool8 state, const math::vec4f& color)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportBackground");
    throw;
  }
}

void ConnectionState::SetViewportProperties (uint32 id, uint64 properties_id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetViewportProperties");
    throw;
  }
}

void ConnectionState::DestroyViewport (uint32 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DestroyViewport");
    throw;
  }
}

void ConnectionState::CreateRenderTarget (uint32 id, const char* name)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::CreateRenderTarget");
    throw;
  }
}

void ConnectionState::DestroyRenderTarget (uint32 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DestroyRenderTarget");
    throw;
  }
}

void ConnectionState::SetRenderTargetActive (uint32 id, bool8 active)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetActive");
    throw;
  }
}

void ConnectionState::SetRenderTargetScreenArea (uint32 id, int8 left, int8 top, int8 width, int8 height)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetScreenArea");
    throw;
  }
}

void ConnectionState::SetRenderTargetBackground (uint32 id, bool8 state, const math::vec4f& color)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::SetRenderTargetBackground");
    throw;
  }
}

void ConnectionState::AttachViewportToRenderTarget (uint32 render_target_id, uint32 viewport_id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::AttachViewportToRenderTarget");
    throw;
  }
}

void ConnectionState::DetachViewportFromRenderTarget (uint32 render_target_id, uint32 viewport_id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::DetachViewportFromRenderTarget");
    throw;
  }
}

void ConnectionState::UpdateRenderTarget (uint32 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UpdateRenderTarget");
    throw;
  }
}

void ConnectionState::OnWindowAttached (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().AttachWindow (id, name, init_string, handle, width, height, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowAttached");
    throw;
  }
}

void ConnectionState::OnWindowDetached (size_t id)
{
  try
  {
    impl->server.WindowManager ().DetachWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowDetached");
    throw;
  }
}

void ConnectionState::OnWindowSizeChanged (size_t id, size_t width, size_t height)
{
  try
  {
    impl->server.WindowManager ().SetWindowSize (id, width, height);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowSizeChanged");
    throw;
  }
}

void ConnectionState::OnWindowViewportChanged (size_t id, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().SetWindowViewport (id, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowViewportChanged");
    throw;
  }
}

void ConnectionState::OnWindowHandleChanged (size_t id, void* handle)
{
  try
  {
    impl->server.WindowManager ().SetWindowHandle (id, handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowHandleChanged");
    throw;
  }
}

void ConnectionState::OnWindowPaint (size_t id)
{
  try
  {
    impl->server.WindowManager ().RepaintWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowPaint");
    throw;
  }
}

void ConnectionState::UpdatePropertyMap (interchange::InputStream&)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::UpdatePropertyMap");
    throw;
  }
}

void ConnectionState::RemovePropertyMap (uint64 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemovePropertyMap");
    throw;
  }
}

void ConnectionState::RemovePropertyLayout (uint64 id)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::RemovePropertyLayout");
    throw;
  }
}
