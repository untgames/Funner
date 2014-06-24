#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации менеджера материалов
*/

struct MaterialManager::Impl
{
  ClientImpl& client;  //ссылка на клиента
  Context&    context; //ссылка на контекст

  Impl (ClientImpl& in_client, Context& in_context)
    : client (in_client)
    , context (in_context)
  {
  }
};

/*
    Конструктор / деструктор
*/

MaterialManager::MaterialManager (ClientImpl& client, Context& context)
  : impl (new Impl (client, context))
{
}

MaterialManager::~MaterialManager ()
{
}

/*
    Создание ресурсов
*/

MaterialPtr MaterialManager::CreateMaterial (const char* name, const char* prototype, bool remove_on_destroy)
{
  try
  {
    return MaterialPtr (new Material (impl->context, name, prototype, remove_on_destroy), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::MaterialManager::CreateMaterial");
    throw;
  }
}

TexturePtr MaterialManager::CreateTexture (const char* name, bool remove_on_destroy)
{
  try
  {
    return TexturePtr (new Texture (impl->context, name, remove_on_destroy), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::MaterialManager::CreateTexture");
    throw;
  }
}
