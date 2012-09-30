#include "shared.h"

using namespace store;

/*
   Реализация магазина
*/

struct Store::Impl : public xtl::reference_counter
{
  typedef stl::hash_map<size_t, PurchaseCallback> BuyProductCallbacks;

  IStore*              store;                         //магазин
  xtl::auto_connection transaction_update_connection; //соединение оповещения о обновлениях транзакций
  BuyProductCallbacks  buy_product_callbacks;         //колбеки обновления транзакций по продуктам

  Impl (const char* store_name)
    : store (0)
  {
    try
    {
      if (!store_name)
        throw xtl::make_null_argument_exception ("", "store_name");

      store = StoreManagerSingleton::Instance ()->CreateStore (store_name);

      transaction_update_connection = store->RegisterTransactionUpdateHandler (xtl::bind (&Store::Impl::OnTransactionUpdated, this, _1));
    }
    catch (xtl::exception& e)
    {
      delete store;

      e.touch ("store::Store::Store");
      throw;
    }
  }

  void OnTransactionUpdated (const Transaction& transaction)
  {
    BuyProductCallbacks::iterator iter = buy_product_callbacks.find (transaction.Id ());

    if (iter == buy_product_callbacks.end ())
      return;

    iter->second (transaction);

    switch (transaction.State ())
    {
      case TransactionState_Purchased:
      case TransactionState_Failed:
      case TransactionState_Restored:
        buy_product_callbacks.erase (iter);
        break;
      default:
        break;
    }
  }

  Transaction BuyProduct (const char* product_id, size_t count) const
  {
    static const char* METHOD_NAME = "store::Store::BuyProduct";

    if (!product_id)
      throw xtl::make_null_argument_exception (METHOD_NAME, "product_id");

    if (!count)
      throw xtl::make_null_argument_exception (METHOD_NAME, "count");

    return store->BuyProduct (product_id, count);
  }

  Transaction BuyProduct (const char* product_id, size_t count, const PurchaseCallback& callback)
  {
    Transaction return_value = BuyProduct (product_id, count);

    buy_product_callbacks.insert_pair (return_value.Id (), callback);

    return return_value;
  }

  ~Impl ()
  {
    delete store;
  }
};

/*
   Конструктор / деструктор / копирование
*/

Store::Store (const char* store_name)
  : impl (new Impl (store_name))
  {}

Store::Store (const Store& source)
  : impl (source.impl)
{
  addref (impl);
}

Store::~Store ()
{
  release (impl);
}

Store& Store::operator = (const Store& source)
{
  Store (source).Swap (*this);

  return *this;
}

/*
   Описание
*/

const char* Store::Description () const
{
  return impl->store->Description ();
}

/*
   Можно ли осуществлять покупки
*/

bool Store::CanBuyProducts () const
{
  return impl->store->CanBuyProducts ();
}

/*
   Получение информации о товарах (products_ids - разделенный пробелами список идентификаторов продуктов,
   products ответа может содержать не все запрошенные продукты)
*/

void Store::LoadProducts (const char* product_ids, const LoadProductsCallback& callback) const
{
  if (!product_ids)
    throw xtl::make_null_argument_exception ("store::Store::LoadProducts", "product_ids");

  impl->store->LoadProducts (product_ids, callback);
}

/*
   Покупка / восстановление покупок
*/

xtl::connection Store::RegisterTransactionUpdateHandler (const PurchaseCallback& callback)
{
  return impl->store->RegisterTransactionUpdateHandler (callback);
}

Transaction Store::BuyProduct (const char* product_id, size_t count, const PurchaseCallback& callback) const
{
  return impl->BuyProduct (product_id, count, callback);
}

Transaction Store::BuyProduct (const char* product_id, size_t count) const
{
  return impl->BuyProduct (product_id, count);
}

void Store::RestorePurchases () const
{
  impl->store->RestorePurchases ();
}

/*
   Обмен
*/

void Store::Swap (Store& source)
{
  stl::swap (source.impl, impl);
}

namespace store
{

/*
   Обмен
*/
void swap (Store&, Store&);

}
