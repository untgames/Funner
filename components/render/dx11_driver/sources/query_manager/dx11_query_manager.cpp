#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджера запросов
*/

struct QueryManager::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

QueryManager::QueryManager (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

QueryManager::~QueryManager ()
{
}

/*
    Создание предикатов
*/

IPredicate* QueryManager::CreatePredicate ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManager::CreatePredicate");
    throw;
  }
}

IQuery* QueryManager::CreateQuery (QueryType type)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManager::CreateQuery");
    throw;
  }
}
