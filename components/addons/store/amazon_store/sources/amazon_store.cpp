#include "shared.h"

using namespace store;
using namespace store::amazon_store;
using namespace syslib::android;

namespace
{

const char* LOG_NAME            = "store.android_store.AmazonStore";
const char* SESSION_DESCRIPTION = "AmazonStore";

void finish_transaction_handler (const Transaction& transaction, const Transaction::OnFinishedCallback& callback);

void on_initialized (JNIEnv& env, jobject, jboolean can_buy_products);
void on_purchase_initiated (JNIEnv& env, jobject, jstring sku);
void on_purchase_failed (JNIEnv& env, jobject, jstring sku, jstring error);
void on_purchase_succeeded (JNIEnv& env, jobject, jstring sku, jobject purchase, jstring receipt, jstring signature);
void on_purchase_restored (JNIEnv& env, jobject, jstring sku, jobject purchase, jstring receipt, jstring signature);
void on_consume_failed (JNIEnv& env, jobject, jobject purchase, jstring error);
void on_consume_succeeded (JNIEnv& env, jobject, jobject purchase);

/*
   Реализация магазина
*/

class AmazonStore
{
  public:
///Конструктор / деструктор
    AmazonStore  ()
      : log (LOG_NAME)
      , initialized (false)
      , initialize_started (false)
      , can_buy_products (false)
      , store_class (0)
      , store (0)
    {
    }

    ~AmazonStore ()
    {
      JNIEnv* env = &get_env ();

      DeleteStore (env);

      if (store_class)
        env->DeleteGlobalRef (store_class);
    }

///Удаление магазина
    void DeleteStore (JNIEnv* env)
    {
      if (!store)
        return;

      try
      {
        env->CallVoidMethod (store, stop_processing_method);
      }
      catch (...)
      {
      }

      env->DeleteGlobalRef (store);

      store = 0;
    }

///Инициализация
    xtl::connection Initialize (const Store::OnInitializedCallback& callback)
    {
      static const char* METHOD_NAME = "store::android_store::AndroidStore::Initialize";

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

      try
      {
        jmethodID store_init_method = find_method (env, store_class, "<init>", "(Landroid/app/Activity;)V");

        store = env->NewGlobalRef (env->NewObject (store_class, store_init_method, get_activity ()));

        if (!store)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't create store object");

        if (env->ExceptionOccurred ())
        {
          env->ExceptionClear ();
          throw xtl::format_operation_exception (METHOD_NAME, "Exception while creating amazon store object. Purchasing not supported.");
        }
      }
      catch (...)
      {
        DeleteStore (env);
        initialize_started = false;
        throw;
      }

      return return_value;
    }

    void OnInitialized (bool in_can_buy_products)
    {
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

        //TODO
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AmazonStore::LoadProducts");
        throw;
      }
    }

///Покупка / восстановление покупок
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
    {
      TransactionsArray transactions = pending_transactions;

      for (TransactionsArray::iterator iter = transactions.begin (), end = transactions.end (); iter != end; ++iter)
        callback ((*iter)->transaction);

      return store_signal.connect (callback);
    }

    void RestorePurchases ()
    {
      log.Printf ("Restoring transactions");

      //TODO
    }

    void NotifyTransactionUpdated (const Transaction& transaction)
    {
      try
      {
        store_signal (transaction);
      }
      catch (xtl::exception& e)
      {
        log.Printf ("Exception in store::android_store::AmazonStore::NotifyTransactionUpdated: '%s'", e.what ());
      }
      catch (...)
      {
        log.Printf ("Unknown exception in store::android_store::AmazonStore::NotifyTransactionUpdated");
      }
    }

    Transaction BuyProduct (const char* product_id, size_t count, const common::PropertyMap& properties)
    {
      //TODO implement subscriptions selling

      try
      {
        if (!product_id)
          throw xtl::make_null_argument_exception ("", "product_id");

        if (count > 1)
          throw xtl::make_range_exception ("", "count", count, 0u, 1u);

        log.Printf ("Buy product '%s' requested", product_id);

        bool consumable = properties.IsPresent ("Consumable") && properties.GetInteger ("Consumable");

        TransactionDescPtr new_transaction (new TransactionDesc (product_id, consumable), false);

        NotifyTransactionUpdated (new_transaction->transaction);

        JNIEnv* env = &get_env ();

        env->CallVoidMethod (store, buy_method, get_activity (), tojstring (product_id).get ());

        pending_transactions.push_back (new_transaction);

        return new_transaction->transaction;
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AmazonStore::BuyProduct");
        throw;
      }
    }

///Инициализация java-биндинга
   void InitJavaBindings (JNIEnv* env)
   {
     static const char* METHOD_NAME = "store::android_store::AmazonStore::InitJavaBindings";

     if (!env)
       throw xtl::make_null_argument_exception (METHOD_NAME, "env");

     jclass store_class_ref = env->FindClass ("com/untgames/funner/amazon_store/Store");

     if (!store_class_ref)
     {
       if (env->ExceptionOccurred ())
         env->ExceptionClear ();

       log_error ("Amazon store linked, but store class not found. Purchasing not supported.\n");
       return;
     }

     if (store_class)
       env->DeleteGlobalRef (store_class);

     store_class = (jclass)env->NewGlobalRef (store_class_ref);

     env->DeleteLocalRef (store_class_ref);

     try
     {
       stop_processing_method = find_method (env, store_class, "stopProcessing", "()V");
//       buy_method             = find_method (env, store_class, "buyProduct", "(Landroid/app/Activity;Ljava/lang/String;)V");
//       consume_method         = find_method (env, store_class, "consumeProduct", "(Lcom/untgames/funner/store/Purchase;)V");

/*       static const JNINativeMethod methods [] = {
         {"onInitializedCallback", "(Z)V", (void*)&on_initialized},
         {"onPurchaseInitiatedCallback", "(Ljava/lang/String;)V", (void*)&on_purchase_initiated},
         {"onPurchaseFailedCallback", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_failed},
         {"onPurchaseSucceededCallback", "(Ljava/lang/String;Lcom/untgames/funner/store/Purchase;Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_succeeded},
         {"onPurchaseRestoredCallback", "(Ljava/lang/String;Lcom/untgames/funner/store/Purchase;Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_restored},
         {"onConsumeFailedCallback", "(Lcom/untgames/funner/store/Purchase;Ljava/lang/String;)V", (void*)&on_consume_failed},
         {"onConsumeSucceededCallback", "(Lcom/untgames/funner/store/Purchase;)V", (void*)&on_consume_succeeded},
       };

       static const size_t methods_count = sizeof (methods) / sizeof (*methods);

       jint status = env->RegisterNatives (store_class, methods, methods_count);

       if (status)
         throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);*/
     }
     catch (...)
     {
       if (store_class)
         env->DeleteGlobalRef (store_class);

       store_class = 0;

       throw;
     }
   }

///Завершение транзакции
   void FinishTransaction (const Transaction& transaction, const Transaction::OnFinishedCallback& callback)
   {
     try
     {
       log.Printf ("Finishing transaction for product '%s'", transaction.ProductId ());

       for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
       {
         TransactionDesc& transaction_desc = **iter;

         if (transaction_desc.transaction.Id () == transaction.Id ())
         {
           if (transaction_desc.consumable && (transaction_desc.transaction.State () == TransactionState_Purchased || transaction_desc.transaction.State () == TransactionState_Restored))
           {
             transaction_desc.consume_callback = callback;

             get_env ().CallVoidMethod (store, consume_method, transaction_desc.purchase.get ());
           }
           else
           {
             pending_transactions.erase (iter);
             callback (true, "");
           }

           return;
         }
       }

       throw xtl::format_operation_exception ("", "Can't find pending transaction for finishing with product id '%s'", transaction.ProductId ());
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::FinishTransaction");
       throw;
     }
   }

   void OnConsumeFailed (global_ref<jobject> purchase, const stl::string& error)
   {
     try
     {
       log.Printf ("Consume failed");

       JNIEnv* env = &get_env ();

       for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
       {
         if (env->IsSameObject (purchase.get (), (*iter)->purchase.get ()))
         {
           (*iter)->consume_callback (false, error.c_str ());
           return;
         }
       }
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnConsumeFailed");
       throw;
     }
   }

   void OnConsumeSucceeded (global_ref<jobject> purchase)
   {
     try
     {
       log.Printf ("Consume succeeded");

       JNIEnv* env = &get_env ();

       for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
       {
         if (env->IsSameObject (purchase.get (), (*iter)->purchase.get ()))
         {
           Transaction::OnFinishedCallback consume_callback = (*iter)->consume_callback;

           pending_transactions.erase (iter);

           consume_callback (true, "");

           return;
         }
       }
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnConsumeSucceeded");
       throw;
     }
   }

///Обновление транзакций
   void OnPurchaseInitiated (const stl::string& sku)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase initiated for product '%s'", transaction_desc.transaction.ProductId ());

       transaction_desc.transaction.SetState (TransactionState_Purchasing);

       NotifyTransactionUpdated (transaction_desc.transaction);
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnPurchaseInitiated");
       throw;
     }
   }

   void OnPurchaseFailed (const stl::string& sku, const stl::string& error)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase failed for product '%s', reason '%s'", transaction_desc.transaction.ProductId (), error.c_str ());

       transaction_desc.completed = true;

       transaction_desc.transaction.SetState (TransactionState_Failed);
       transaction_desc.transaction.SetStatus (error.c_str ());

       NotifyTransactionUpdated (transaction_desc.transaction);
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnPurchaseFailed");
       throw;
     }
   }

   void OnPurchaseSucceeded (const stl::string& sku, global_ref<jobject> purchase, const stl::string& receipt, const stl::string& signature)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase succeeded for product '%s'", transaction_desc.transaction.ProductId ());

       transaction_desc.completed = true;
       transaction_desc.purchase  = purchase;

       transaction_desc.transaction.SetReceipt (receipt.size (), receipt.data ());
       transaction_desc.transaction.SetState (TransactionState_Purchased);

       transaction_desc.transaction.Properties ().SetProperty ("Signature", signature.c_str ());

       NotifyTransactionUpdated (transaction_desc.transaction);
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnPurchaseSucceeded");
       throw;
     }
   }

   void OnPurchaseRestored (const stl::string& sku, global_ref<jobject> purchase, const stl::string& receipt, const stl::string& signature)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase restored for product '%s'", transaction_desc.transaction.ProductId ());

       transaction_desc.completed = true;
       transaction_desc.purchase  = purchase;

       transaction_desc.transaction.SetReceipt (receipt.size (), receipt.data ());
       transaction_desc.transaction.SetState (TransactionState_Restored);

       transaction_desc.transaction.Properties ().SetProperty ("Signature", signature.c_str ());

       NotifyTransactionUpdated (transaction_desc.transaction);
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnPurchaseRestored");
       throw;
     }
   }

  private:
    struct TransactionDesc : public xtl::reference_counter
    {
      Transaction                     transaction;
      bool                            consumable;
      bool                            completed;
      global_ref<jobject>             purchase;
      Transaction::OnFinishedCallback consume_callback;

      TransactionDesc (const char* product_id, bool in_consumable)
        : transaction (finish_transaction_handler)
        , consumable (in_consumable)
        , completed (false)
      {
        transaction.SetState (TransactionState_New);
        transaction.SetProductId (product_id);
        transaction.SetQuantity (1);
      }

      private:
        TransactionDesc (const TransactionDesc&); //no impl
        TransactionDesc& operator = (const TransactionDesc&); //no impl
    };

  private:
///Поиск первой незавершенной транзакции по sku
   TransactionDesc& FindPendingTransaction (const char* sku)
   {
     static const char* METHOD_NAME = "store::android_store::AmazonStore::FindIncompleteTransaction";

     if (!sku)
       throw xtl::make_null_argument_exception (METHOD_NAME, "sku");

     for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
     {
       if (!(*iter)->completed && !xtl::xstrcmp ((*iter)->transaction.ProductId (), sku))
         return **iter;
     }

     throw xtl::format_operation_exception (METHOD_NAME, "Can't find pending transaction for sku '%s'", sku);
   }

  private:
    typedef xtl::signal<void (const Transaction&)> StoreSignal;
    typedef xtl::signal<void ()>                   OnInitializedSignal;
    typedef xtl::intrusive_ptr<TransactionDesc>    TransactionDescPtr;
    typedef stl::vector<TransactionDescPtr>        TransactionsArray;

  public:
    common::Log         log;                      //протокол
    bool                initialized;              //завершена ли инициализация магазина
    bool                initialize_started;       //начата ли инициализация магазина
    bool                can_buy_products;         //доступны ли покупки
    StoreSignal         store_signal;             //соединение оповещения обновления транзакций
    OnInitializedSignal on_initialized_signal;    //соединение оповещения завершения инициализации магазина
    jclass              store_class;              //класс Store
    jobject             store;                    //объект Store
    jmethodID           buy_method;               //метод покупки
    jmethodID           consume_method;           //метод поглощения покупки
    jmethodID           stop_processing_method;   //метод остановки обработки покупок
    TransactionsArray   pending_transactions;     //текущие незавершенные транзакции
};

typedef common::Singleton<AmazonStore> StoreSingleton;

void on_initialized (JNIEnv& env, jobject, jboolean can_buy_products)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnInitialized, &*StoreSingleton::Instance (), can_buy_products), common::ActionThread_Main);
}

void on_purchase_initiated (JNIEnv& env, jobject, jstring sku)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseInitiated, &*StoreSingleton::Instance (), tostring (sku)), common::ActionThread_Main);
}

void on_purchase_failed (JNIEnv& env, jobject, jstring sku, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseFailed, &*StoreSingleton::Instance (), tostring (sku), tostring (error)), common::ActionThread_Main);
}

void on_purchase_succeeded (JNIEnv& env, jobject, jstring sku, jobject purchase, jstring receipt, jstring signature)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseSucceeded, &*StoreSingleton::Instance (), tostring (sku), global_ref<jobject> (purchase), tostring (receipt), tostring (signature)), common::ActionThread_Main);
}

void on_purchase_restored (JNIEnv& env, jobject, jstring sku, jobject purchase, jstring receipt, jstring signature)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseRestored, &*StoreSingleton::Instance (), tostring (sku), global_ref<jobject> (purchase), tostring (receipt), tostring (signature)), common::ActionThread_Main);
}

void on_consume_failed (JNIEnv& env, jobject, jobject purchase, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnConsumeFailed, &*StoreSingleton::Instance (), global_ref<jobject> (purchase), tostring (error)), common::ActionThread_Main);
}

void on_consume_succeeded (JNIEnv& env, jobject, jobject purchase)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnConsumeSucceeded, &*StoreSingleton::Instance (), global_ref<jobject> (purchase)), common::ActionThread_Main);
}

void finish_transaction_handler (const Transaction& transaction, const Transaction::OnFinishedCallback& callback)
{
  StoreSingleton::Instance ()->FinishTransaction (transaction, callback);
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
