#include "shared.h"

using namespace store;
using namespace store::android_store;

namespace
{

const char* COMPONENT_NAME              = "store.stores.android_store";                        //имя компонента
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.android_store"; //имя компонента обработчика загрузки приложения
const char* LOAD_HANDLER_ID             = "AndroidStore";                                      //идентификатор обработчика загрузки приложения
const char* STORE_ID                    = "AndroidStore";                                      //идентификатор магазина
const char* STORE_NAME_MASK             = "AndroidStore";                                      //маска имен магазинов

}

/*
   Компонент магазина
*/

namespace components
{

namespace android_store
{

class AndroidStoreComponent
{
  public:
    AndroidStoreComponent ()
    {
      StoreManager::RegisterStore (STORE_ID, STORE_NAME_MASK, xtl::bind (&AndroidStoreComponent::CreateStore, this, _1));
    }

  private:
    IStore* CreateStore (const char* store_name)
    {
      static const char* METHOD_NAME = "store::android_store::AndroidStoreComponent::CreateStore";

      if (xtl::xstrcmp (store_name, STORE_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "store_name", store_name);

      return new StoreImpl ();
    }
};

class AndroidStoreLoadHandlerComponent
{
  public:
    AndroidStoreLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&AndroidStoreLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      syslib::android::log_error ("%s %d\n", __FUNCTION__, __LINE__);
    }
};

extern "C"
{

common::ComponentRegistrator<AndroidStoreComponent> AndroidStore (COMPONENT_NAME);
common::ComponentRegistrator<AndroidStoreLoadHandlerComponent> AndroidStoreLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
