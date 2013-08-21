#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

struct ClientImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

ClientImpl::ClientImpl ()
  : impl (new Impl)
{
}

ClientImpl::~ClientImpl ()
{
}

/*
    Обработчики ответов сервера
*/

void ClientImpl::Dummy ()
{
}
