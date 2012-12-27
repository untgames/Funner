#include "shared.h"

using namespace store;
using namespace store::ios_store;

namespace
{

const char* COMPONENT_NAME  = "store.stores.ios_store"; //имя компонента
const char* STORE_ID        = "IOSStore";               //идентификатор магазина
const char* STORE_NAME_MASK = "IOSStore";               //маска имен магазинов

}

/*
   Компонент магазина
*/

namespace components
{

namespace ios_store
{

class IOSStoreComponent
{
  public:
    IOSStoreComponent ()
    {
      StoreManager::RegisterStore (STORE_ID, STORE_NAME_MASK, xtl::bind (&IOSStoreComponent::CreateStore, this, _1));
    }

  private:
    IStore* CreateStore (const char* store_name)
    {
      static const char* METHOD_NAME = "store::ios_store::IOSStoreComponent::CreateStore";

      if (xtl::xstrcmp (store_name, STORE_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "store_name", store_name);

      return new StoreImpl ();
    }
};

extern "C"
{

common::ComponentRegistrator<IOSStoreComponent> IOSStore (COMPONENT_NAME);

}

}

}
