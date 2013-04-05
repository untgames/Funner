#include "shared.h"

#include <syslib/thread.h>

using namespace store;
using namespace store::android_store;
using namespace syslib::android;

namespace
{

const char* LOG_NAME            = "store.android_store.AndroidStore";
const char* SESSION_DESCRIPTION = "AndroidStore";

void on_initialized (JNIEnv& env, jobject, jboolean can_buy_products);

/*
   Реализация магазина
*/

class AndroidStore
{
  public:
///Конструктор / деструктор
    AndroidStore  ()
      : log (LOG_NAME)
      , initialized (false)
      , initialize_started (false)
      , can_buy_products (false)
      , store_class (0)
    {
    }

    ~AndroidStore ()
    {
      if (store_class)
        get_env ().DeleteLocalRef (store_class);
    }

///Инициализация
    xtl::connection Initialize (const Store::OnInitializedCallback& callback)
    {
      printf ("INITIALIZE CALLED FROM THREAD %p\n", syslib::Thread::GetCurrentThreadId ());

      if (initialized)
      {
        callback ();
        return xtl::connection ();
      }

      xtl::connection return_value = on_initialized_signal.connect (callback);

      if (initialize_started)
        return return_value;

      initialize_started = true;

      JNIEnv* env = &get_env ();

      jmethodID initialize_method = find_static_method (env, store_class, "initialize", "(Landroid/content/Context;)V");

      env->CallStaticVoidMethod (store_class, initialize_method, get_activity ());

      return return_value;
    }

    void OnInitialized (bool in_can_buy_products)
    {
      printf ("ON INITIALIZED CALLBACK CALLED FROM THREAD %p\n", syslib::Thread::GetCurrentThreadId ());

      can_buy_products = in_can_buy_products;

      on_initialized_signal ();
      on_initialized_signal.disconnect_all ();
    }

///Можно ли осуществлять покупки
    bool CanBuyProducts ()
    {
      return can_buy_products;
    }

///Получение информации о товарах (products_ids - разделенный пробелами список идентификаторов продуктов,
///products ответа может содержать не все запрошенные продукты)
    void LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback)
    {
      try
      {
        if (!product_ids)
          throw xtl::make_null_argument_exception ("", "product_ids");

        common::StringArray products = common::split (product_ids);
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AndroidStore::LoadProducts");
        throw;
      }
    }

///Покупка / восстановление покупок
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
    {
      return store_signal.connect (callback);
    }

    void RestorePurchases ()
    {
      log.Printf ("Restoring transactions");
    }

    void NotifyTransactionUpdated (const Transaction& transaction)
    {
      try
      {
        store_signal (transaction);
      }
      catch (xtl::exception& e)
      {
        log.Printf ("Exception in store::android_store::AndroidStore::NotifyTransactionUpdated: '%s'", e.what ());
      }
      catch (...)
      {
        log.Printf ("Unknown exception in store::android_store::AndroidStore::NotifyTransactionUpdated");
      }
    }

    Transaction BuyProduct (const char* product_id, size_t count, const common::PropertyMap& properties)
    {
      try
      {
        if (!product_id)
          throw xtl::make_null_argument_exception ("", "product_id");

        if (!count)
          throw xtl::make_null_argument_exception ("", "count");

        log.Printf ("Buy product '%s' with count %u requested", product_id, count);

        return Transaction ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AndroidStore::BuyProduct");
        throw;
      }
    }

///Инициализация java-биндинга
   void InitJavaBindings (JNIEnv* env)
   {
     static const char* METHOD_NAME = "store::android_store::AndroidStore::InitJavaBindings";

     if (!env)
       throw xtl::make_null_argument_exception (METHOD_NAME, "env");

     jclass store_class_ref = env->FindClass ("com/untgames/funner/store/Store");

     if (!store_class_ref)
       throw xtl::format_operation_exception ("", "Can't find Store class\n");

     if (store_class)
       env->DeleteLocalRef (store_class);

     store_class = (jclass)env->NewGlobalRef (store_class_ref);

     try
     {
       static const JNINativeMethod methods [] = {
         {"onInitializedCallback", "(Z)V", (void*)&on_initialized},
       };

       static const size_t methods_count = sizeof (methods) / sizeof (*methods);

       jint status = env->RegisterNatives (store_class, methods, methods_count);

       if (status)
         throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
     }
     catch (...)
     {
       if (store_class)
         env->DeleteLocalRef (store_class);

       store_class = 0;

       throw;
     }
   }

  private:
    typedef xtl::signal<void (const Transaction&)> StoreSignal;
    typedef xtl::signal<void ()>                   OnInitializedSignal;

  public:
    common::Log         log;                      //протокол
    bool                initialized;              //завершена ли инициализация магазина
    bool                initialize_started;       //начата ли инициализация магазина
    bool                can_buy_products;         //доступны ли покупки
    StoreSignal         store_signal;             //соединение оповещения обновления транзакций
    OnInitializedSignal on_initialized_signal;    //соединение оповещения завершения инициализации магазина
    jclass              store_class;              //класс Store
};

typedef common::Singleton<AndroidStore> StoreSingleton;

void on_initialized (JNIEnv& env, jobject, jboolean can_buy_products)
{
  common::ActionQueue::PushAction (xtl::bind (&AndroidStore::OnInitialized, &*StoreSingleton::Instance (), can_buy_products), common::ActionThread_Main);
}

}

/*
   Конструктор / деструктор
*/

StoreImpl::StoreImpl ()
{
}

StoreImpl::~StoreImpl ()
{
}

/*
   Инициализация
*/

void StoreImpl::Initialize (const Store::OnInitializedCallback& callback)
{
  on_initialized_connection = StoreSingleton::Instance ()->Initialize (callback);
}

/*
   Описание магазина
*/

const char* StoreImpl::Description ()
{
  return SESSION_DESCRIPTION;
}

/*
   Можно ли осуществлять покупки
*/

bool StoreImpl::CanBuyProducts ()
{
  return StoreSingleton::Instance ()->CanBuyProducts ();
}

/*
   Получение информации о товарах (products_ids - разделенный пробелами список идентификаторов продуктов,
   products ответа может содержать не все запрошенные продукты)
*/

void StoreImpl::LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback)
{
  StoreSingleton::Instance ()->LoadProducts (product_ids, callback);
}

/*
   Покупка / восстановление покупок
*/

xtl::connection StoreImpl::RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
{
  return StoreSingleton::Instance ()->RegisterTransactionUpdateHandler (callback);
}

void StoreImpl::RestorePurchases ()
{
  StoreSingleton::Instance ()->RestorePurchases ();
}

Transaction StoreImpl::BuyProduct (const char* product_id, size_t count, const common::PropertyMap& properties)
{
  return StoreSingleton::Instance ()->BuyProduct (product_id, count, properties);
}

/*
   Инициализация java-биндинга
*/

void StoreImpl::InitJavaBindings (JNIEnv* env)
{
  StoreSingleton::Instance ()->InitJavaBindings (env);
}
