#include "shared.h"

using namespace store;
using namespace store::android_store;

namespace
{

const char* LOG_NAME            = "store.android_store.AndroidStore";
const char* SESSION_DESCRIPTION = "AndroidStore";

/*
   Реализация магазина
*/

class AndroidStore
{
  public:
///Конструктор / деструктор
    AndroidStore  ()
      : log (LOG_NAME)
    {
    }

    ~AndroidStore ()
    {
    }

///Инициализация
    void Initialize (const Store::OnInitializedCallback& callback)
    {
    }

///Можно ли осуществлять покупки
    bool CanBuyProducts ()
    {
      return true;
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

  private:
    typedef xtl::signal<void (const Transaction&)> StoreSignal;

  public:
    common::Log log;
    StoreSignal store_signal;
};

typedef common::Singleton<AndroidStore> StoreSingleton;

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
  StoreSingleton::Instance ()->Initialize (callback);
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
