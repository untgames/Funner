#include "shared.h"

using namespace script;
using namespace store;

namespace
{

/*
    Константы
*/

const char* BINDER_NAME               = "Store";
const char* COMPONENT_NAME            = "script.binds.Store";
const char* PRODUCT_LIBRARY           = "Store.Product";
const char* PRODUCT_LIST_LIBRARY      = "Store.ProductList";
const char* STORE_LIBRARY             = "Store.Store";
const char* TRANSACTION_LIBRARY       = "Store.Transaction";
const char* TRANSACTION_STATE_LIBRARY = "Store.TransactionState";

void bind_product_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (PRODUCT_LIBRARY);

  lib.Register ("get_Description", make_invoker (&Product::Description));
  lib.Register ("get_Id",          make_invoker (&Product::Id));
  lib.Register ("get_Properties",  make_invoker (xtl::implicit_cast<const common::PropertyMap& (Product::*) () const> (&Product::Properties)));

  environment.RegisterType<Product> (PRODUCT_LIBRARY);
}

const Product& get_product_list_item (const ProductList& collection, size_t index)
{
  return collection [index];
}


void bind_product_list_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (PRODUCT_LIST_LIBRARY);

  lib.Register ("get_Size",    make_invoker (&ProductList::Size));
  lib.Register ("get_IsEmpty", make_invoker (&ProductList::IsEmpty));
  lib.Register ("Get",         make_invoker (&get_product_list_item));

  environment.RegisterType<ProductList> (PRODUCT_LIST_LIBRARY);
}

void bind_transaction_state_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (TRANSACTION_STATE_LIBRARY);

  lib.Register ("get_New",        make_const (TransactionState_New));
  lib.Register ("get_Purchasing", make_const (TransactionState_Purchasing));
  lib.Register ("get_Purchased",  make_const (TransactionState_Purchased));
  lib.Register ("get_Failed",     make_const (TransactionState_Failed));
  lib.Register ("get_Restored",   make_const (TransactionState_Restored));
}

void bind_transaction_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (TRANSACTION_LIBRARY);

  lib.Register ("get_Id",         make_invoker (&Transaction::Id));
  lib.Register ("get_State",      make_invoker (&Transaction::State));
  lib.Register ("get_Status",     make_invoker (&Transaction::Status));
  lib.Register ("get_ProductId",  make_invoker (&Transaction::ProductId));
  lib.Register ("get_Quantity",   make_invoker (&Transaction::Quantity));
  lib.Register ("get_Receipt",    make_invoker (&Transaction::ReceiptBase64));
  lib.Register ("get_Properties", make_invoker (xtl::implicit_cast<const common::PropertyMap& (Transaction::*) () const> (&Transaction::Properties)));

  lib.Register ("Finish", make_invoker (&Transaction::Finish));

  environment.RegisterType<Transaction> (TRANSACTION_LIBRARY);
}

Store create_store (const char* name)
{
  return Store (name);
}

void bind_store_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (STORE_LIBRARY);

  lib.Register ("Create", make_invoker (&create_store));

  lib.Register ("get_Description",    make_invoker (&Store::Description));
  lib.Register ("get_CanBuyProducts", make_invoker (&Store::CanBuyProducts));

  lib.Register ("CreateLoadProductsCallback",       make_callback_invoker<store::Store::LoadProductsCallback::signature_type> ());
  lib.Register ("LoadProducts",                     make_invoker (&Store::LoadProducts));
  lib.Register ("CreatePurchaseCallback",           make_callback_invoker<store::Store::PurchaseCallback::signature_type> ());
  lib.Register ("RegisterTransactionUpdateHandler", make_invoker (&Store::RegisterTransactionUpdateHandler));
  lib.Register ("BuyProduct",         make_invoker (
      make_invoker (xtl::implicit_cast<Transaction (Store::*)(const char*, size_t, const Store::PurchaseCallback&) const> (&Store::BuyProduct)),
      make_invoker (xtl::implicit_cast<Transaction (Store::*)(const char*, size_t) const> (&Store::BuyProduct))
  ));
  lib.Register ("RestorePurchases",                 make_invoker (&Store::RestorePurchases));

  environment.RegisterType<Store> (STORE_LIBRARY);
}

void bind_store_libraries (Environment& environment)
{
  bind_product_library           (environment);
  bind_product_list_library      (environment);
  bind_transaction_state_library (environment);
  bind_transaction_library       (environment);
  bind_store_library             (environment);
}

}

namespace components
{

namespace store_script_binds
{

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_store_libraries (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> StoreScriptBind (COMPONENT_NAME);

}

}

}
