#include "shared.h"

using namespace store;

/*
   Реализация менеджера магазинов
*/

namespace
{

const char* COMPONENTS_MASK = "store.stores.*"; //маска имён компонентов магазинов

struct StoreDesc
{
  stl::string                             store_name_mask;
  store::StoreManager::CreateStoreHandler handler;

  StoreDesc (const char* in_mask, const store::StoreManager::CreateStoreHandler& in_handler)
    : store_name_mask (in_mask)
    , handler (in_handler)
    {}
};

}

struct StoreManagerImpl::Impl
{
  typedef stl::hash_map<stl::hash_key<const char*>, StoreDesc> StoreDescMap;

  StoreDescMap store_creators;

  ///Регистрация создателей магазинов
  void RegisterStore (const char* id, const char* store_name_mask, const store::StoreManager::CreateStoreHandler& handler)
  {
    static const char* METHOD_NAME = "store::StoreManager::RegisterStore";

    if (!id)
      throw xtl::make_null_argument_exception (METHOD_NAME, "id");

    if (!store_name_mask)
      throw xtl::make_null_argument_exception (METHOD_NAME, "store_name_mask");

    StoreDescMap::iterator iter = store_creators.find (id);

    if (iter != store_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Store with id '%s' already registered", id);

    store_creators.insert_pair (id, StoreDesc (store_name_mask, handler));
  }

  void UnregisterStore (const char* id)
  {
    if (!id)
      throw xtl::make_null_argument_exception ("store::StoreManager::UnregisterStore", "id");

    store_creators.erase (id);
  }

  void UnregisterAllStores ()
  {
    store_creators.clear ();
  }

  ///Поиск магазина
  StoreDescMap::iterator FindStoreDesc (const char* store_name)
  {
    for (StoreDescMap::iterator iter = store_creators.begin (), end = store_creators.end (); iter != end; ++iter)
    {
      if (common::wcmatch (store_name, iter->second.store_name_mask.c_str ()))
        return iter;
    }

    return store_creators.end ();
  }

  ///Проверка наличия магазина
  bool IsStoreRegistered (const char* store_name)
  {
    if (!store_name)
      throw xtl::make_null_argument_exception ("store::StoreManager::IsStoreRegistered", "store_name");

    LoadDefaultStores ();

    return FindStoreDesc (store_name) != store_creators.end ();
  }

  ///Создание магазина
  store::IStore* CreateStore (const char* store_name)
  {
    static const char* METHOD_NAME = "store::StoreManagerImpl::CreateStore";

    if (!store_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "store_name");

    LoadDefaultStores ();

    StoreDescMap::iterator iter = FindStoreDesc (store_name);

    if (iter == store_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Store '%s' not registered", store_name);

    return iter->second.handler (store_name);
  }

  ///Загрузка магазинов по умолчанию
  void LoadDefaultStores ()
  {
    static common::ComponentLoader loader (COMPONENTS_MASK);
  }
};

/*
   Конструктор / деструктор
*/

StoreManagerImpl::StoreManagerImpl ()
  : impl (new Impl)
  {}

StoreManagerImpl::~StoreManagerImpl ()
{
  delete impl;
}

/*
   Регистрация создателей магазинов
*/

void StoreManagerImpl::RegisterStore (const char* id, const char* store_name_mask, const store::StoreManager::CreateStoreHandler& handler)
{
  impl->RegisterStore (id, store_name_mask, handler);
}

void StoreManagerImpl::UnregisterStore (const char* id)
{
  impl->UnregisterStore (id);
}

void StoreManagerImpl::UnregisterAllStores ()
{
  impl->UnregisterAllStores ();
}

/*
   Проверка наличия магазинов
*/

bool StoreManagerImpl::IsStoreRegistered (const char* store_name)
{
  return impl->IsStoreRegistered (store_name);
}

/*
   Создание магазина
*/

store::IStore* StoreManagerImpl::CreateStore (const char* store_name)
{
  return impl->CreateStore (store_name);
}

/*
   Регистрация создателей магазинов
*/

void StoreManager::RegisterStore (const char* id, const char* store_name_mask, const CreateStoreHandler& handler)
{
  StoreManagerSingleton::Instance ()->RegisterStore (id, store_name_mask, handler);
}

void StoreManager::UnregisterStore (const char* id)
{
  StoreManagerSingleton::Instance ()->UnregisterStore (id);
}

void StoreManager::UnregisterAllStores ()
{
  StoreManagerSingleton::Instance ()->UnregisterAllStores ();
}

/*
   Проверка наличия магазина
*/
    
bool StoreManager::IsStoreRegistered (const char* store_name)
{
  return StoreManagerSingleton::Instance ()->IsStoreRegistered (store_name);
}
