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
    return new Query (impl->GetDeviceManager (), QueryType_OcclusionPredicate);
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
    return new Query (impl->GetDeviceManager (), type);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManager::CreateQuery");
    throw;
  }
}
