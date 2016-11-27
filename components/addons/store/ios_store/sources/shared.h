#ifndef STORE_IOS_STORE_SHARED_HEADER
#define STORE_IOS_STORE_SHARED_HEADER

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <store/store.h>

namespace store
{

namespace ios_store
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Магазин
///////////////////////////////////////////////////////////////////////////////////////////////////
class StoreImpl : public IStore
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StoreImpl  ();
    ~StoreImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize (const IStore::OnInitializedCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание магазина
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Можно ли осуществлять покупки
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool CanBuyProducts ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о товарах (products_ids - разделенный пробелами список идентификаторов продуктов,
///products ответа может содержать не все запрошенные продукты)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Покупка / восстановление покупок
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback&);
    void            RestorePurchases                 (const Store::OnPurchasesRestoredCallback& finish_callback);
    Transaction     BuyProduct                       (const char* product_id, size_t count, const common::PropertyMap& properties);
};

void release_ns_object (const void* handle);

}

}

#endif
