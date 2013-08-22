#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� ��������� ����������
*/

struct ConnectionState::Impl
{
};

/*
    ����������� / ����������
*/

ConnectionState::ConnectionState ()
{
  try
  {
    impl.reset (new Impl);
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
    ������� �������
*/

void ConnectionState::LoadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::UnloadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::OnSizeChanged (size_t width, size_t height)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::OnViewportChanged (size_t left, size_t top, size_t right, size_t bottom)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::OnHandleChanged (void* handle)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::OnPaint ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
