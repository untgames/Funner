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
void on_purchase_succeeded (JNIEnv& env, jobject, jstring sku, jstring user_id, jstring receipt_id);
void on_purchase_restored (JNIEnv& env, jobject, jstring sku, jstring user_id, jstring receipt_id);
void on_transaction_finish_failed (JNIEnv& env, jobject, jstring receipt_id, jstring error);
void on_transaction_finish_succeeded (JNIEnv& env, jobject, jstring receipt_id);

/*
   Store implementation
*/

class AmazonStore
{
  public:
///Constructor / destructor
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

///Delete store
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

///Initialization
    xtl::connection Initialize (const IStore::OnInitializedCallback& callback)
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

      if (!store_class)
      {
        OnInitialized (false);
        return xtl::connection ();
      }

      JNIEnv* env = &get_env ();

      try
      {
        jmethodID store_init_method = find_method (env, store_class, "<init>", "(Lcom/untgames/funner/application/EngineActivity;)V");

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
      initialized      = true;
      can_buy_products = in_can_buy_products;

      on_initialized_signal ();
      on_initialized_signal.disconnect_all ();
    }

///Can make purchases
    bool CanBuyProducts ()
    {
      return can_buy_products;
    }

///Load information about products (products_ids - space-separated list if product ids,
///products response may contain not all requested products)
    void LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback)
    {
      try
      {
        if (!product_ids)
          throw xtl::make_null_argument_exception ("", "product_ids");

        if (!can_buy_products)
          throw xtl::format_operation_exception ("", "Purchases not supported");

        common::StringArray products = common::split (product_ids);

        //TODO
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AmazonStore::LoadProducts");
        throw;
      }
    }

///Purchase / restore purchases
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
    {
      TransactionsArray transactions = pending_transactions;

      for (TransactionsArray::iterator iter = transactions.begin (), end = transactions.end (); iter != end; ++iter)
        callback ((*iter)->transaction);

      return store_signal.connect (callback);
    }

    void RestorePurchases (const Store::OnPurchasesRestoredCallback& finish_callback)
    {
      log.Printf ("Restoring transactions");

      if (!can_buy_products)
        throw xtl::format_operation_exception ("", "Purchases not supported");

      //TODO
    }

    void NotifyTransactionUpdated (Transaction transaction) //get copy of transaction, because original entry can be deleted in callbacks
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

        if (!can_buy_products)
          throw xtl::format_operation_exception ("", "Can't buy products, amazon user id not available");

        log.Printf ("Buy product '%s' requested", product_id);

        TransactionDescPtr new_transaction (new TransactionDesc (product_id), false);

        NotifyTransactionUpdated (new_transaction->transaction);

        JNIEnv* env = &get_env ();

        env->CallVoidMethod (store, buy_method, tojstring (product_id).get ());

        pending_transactions.push_back (new_transaction);

        return new_transaction->transaction;
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::android_store::AmazonStore::BuyProduct");
        throw;
      }
    }

///java-bindings initialization
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
       stop_processing_method    = find_method (env, store_class, "stopProcessing", "()V");
       buy_method                = find_method (env, store_class, "buyProduct", "(Ljava/lang/String;)V");
       finish_transaction_method = find_method (env, store_class, "finishTransaction", "(Ljava/lang/String;)V");

       static const JNINativeMethod methods [] = {
         {"onInitializedCallback", "(Z)V", (void*)&on_initialized},
         {"onPurchaseInitiatedCallback", "(Ljava/lang/String;)V", (void*)&on_purchase_initiated},
         {"onPurchaseFailedCallback", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_failed},
         {"onPurchaseSucceededCallback", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_succeeded},
         {"onPurchaseRestoredCallback", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_purchase_restored},
         {"onTransactionFinishFailedCallback", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)&on_transaction_finish_failed},
         {"onTransactionFinishSucceededCallback", "(Ljava/lang/String;)V", (void*)&on_transaction_finish_succeeded},
       };

       static const size_t methods_count = sizeof (methods) / sizeof (*methods);

       jint status = env->RegisterNatives (store_class, methods, methods_count);

       if (status)
         throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
     }
     catch (...)
     {
       if (store_class)
         env->DeleteGlobalRef (store_class);

       store_class = 0;

       throw;
     }
   }

///Finish transaction
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
           if (transaction_desc.transaction.State () == TransactionState_Purchased || transaction_desc.transaction.State () == TransactionState_Restored)
           {
             transaction_desc.finish_callback = callback;

             get_env ().CallVoidMethod (store, finish_transaction_method, tojstring (transaction_desc.receipt_id.c_str ()).get ());
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

   void OnTransactionFinishFailed (const stl::string& receipt_id, const stl::string& error)
   {
     try
     {
       log.Printf ("Transaction finish failed");

       for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
       {
         if (receipt_id == (*iter)->receipt_id)
         {
           (*iter)->finish_callback (false, error.c_str ());
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

   void OnTransactionFinishSucceeded (const stl::string& receipt_id)
   {
     try
     {
       log.Printf ("Transaction finish succeeded");

       for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
       {
         if (receipt_id == (*iter)->receipt_id)
         {
           Transaction::OnFinishedCallback finish_callback = (*iter)->finish_callback;

           pending_transactions.erase (iter);

           finish_callback (true, "");

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

///Transactions updates
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

   void OnPurchaseSucceeded (const stl::string& sku, const stl::string& user_id, const stl::string& receipt_id)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase succeeded for product '%s'", transaction_desc.transaction.ProductId ());

       transaction_desc.completed  = true;
       transaction_desc.receipt_id = receipt_id;

       transaction_desc.transaction.SetReceipt (receipt_id.size (), receipt_id.data ());
       transaction_desc.transaction.SetState (TransactionState_Purchased);

       transaction_desc.transaction.Properties ().SetProperty ("UserId", user_id.c_str ());

       NotifyTransactionUpdated (transaction_desc.transaction);
     }
     catch (xtl::exception& e)
     {
       e.touch ("store::android_store::AmazonStore::OnPurchaseSucceeded");
       throw;
     }
   }

   void OnPurchaseRestored (const stl::string& sku, const stl::string& user_id, const stl::string& receipt_id)
   {
     try
     {
       TransactionDesc& transaction_desc = FindPendingTransaction (sku.c_str ());

       log.Printf ("Purchase restored for product '%s'", transaction_desc.transaction.ProductId ());

       transaction_desc.completed  = true;
       transaction_desc.receipt_id = receipt_id;

       transaction_desc.transaction.SetReceipt (receipt_id.size (), receipt_id.data ());
       transaction_desc.transaction.SetState (TransactionState_Restored);

       transaction_desc.transaction.Properties ().SetProperty ("UserId", user_id.c_str ());

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
      bool                            completed;
      stl::string                     receipt_id;
      Transaction::OnFinishedCallback finish_callback;

      TransactionDesc (const char* product_id)
        : transaction (finish_transaction_handler)
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
///Find firs unfinished transaction by sku
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
    common::Log         log;                       //log
    bool                initialized;               //is store initialized
    bool                initialize_started;        //is store initialization started
    bool                can_buy_products;          //is purchasing available
    StoreSignal         store_signal;              //transactions updates notification signal
    OnInitializedSignal on_initialized_signal;     //store initialization complete notification signal
    jclass              store_class;               //Store class
    jobject             store;                     //Store object
    jmethodID           buy_method;                //purchase method
    jmethodID           finish_transaction_method; //finish transaction method
    jmethodID           stop_processing_method;    //stop store processing method
    TransactionsArray   pending_transactions;      //current unfinished transactions
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

void on_purchase_succeeded (JNIEnv& env, jobject, jstring sku, jstring user_id, jstring receipt_id)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseSucceeded, &*StoreSingleton::Instance (), tostring (sku), tostring (user_id), tostring (receipt_id)), common::ActionThread_Main);
}

void on_purchase_restored (JNIEnv& env, jobject, jstring sku, jstring user_id, jstring receipt_id)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnPurchaseRestored, &*StoreSingleton::Instance (), tostring (sku), tostring (user_id), tostring (receipt_id)), common::ActionThread_Main);
}

void on_transaction_finish_failed (JNIEnv& env, jobject, jstring receipt_id, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnTransactionFinishFailed, &*StoreSingleton::Instance (), tostring (receipt_id), tostring (error)), common::ActionThread_Main);
}

void on_transaction_finish_succeeded (JNIEnv& env, jobject, jstring receipt_id)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonStore::OnTransactionFinishSucceeded, &*StoreSingleton::Instance (), tostring (receipt_id)), common::ActionThread_Main);
}

void finish_transaction_handler (const Transaction& transaction, const Transaction::OnFinishedCallback& callback)
{
  StoreSingleton::Instance ()->FinishTransaction (transaction, callback);
}

}

/*
   Constructor / destructor
*/

StoreImpl::StoreImpl ()
{
}

StoreImpl::~StoreImpl ()
{
}

/*
   Initialization
*/

void StoreImpl::Initialize (const IStore::OnInitializedCallback& callback)
{
  on_initialized_connection = StoreSingleton::Instance ()->Initialize (callback);
}

/*
   Store description
*/

const char* StoreImpl::Description ()
{
  return SESSION_DESCRIPTION;
}

/*
   Is purchasing available
*/

bool StoreImpl::CanBuyProducts ()
{
  return StoreSingleton::Instance ()->CanBuyProducts ();
}

/*
   Load information about products (products_ids - space-separated list if product ids,
   products response may contain not all requested products)
*/

void StoreImpl::LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback)
{
  StoreSingleton::Instance ()->LoadProducts (product_ids, callback);
}

/*
   Purchase / restore purchases
*/

xtl::connection StoreImpl::RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
{
  return StoreSingleton::Instance ()->RegisterTransactionUpdateHandler (callback);
}

void StoreImpl::RestorePurchases (const Store::OnPurchasesRestoredCallback& finish_callback)
{
  StoreSingleton::Instance ()->RestorePurchases (finish_callback);
}

Transaction StoreImpl::BuyProduct (const char* product_id, size_t count, const common::PropertyMap& properties)
{
  return StoreSingleton::Instance ()->BuyProduct (product_id, count, properties);
}

/*
   Java-bindings initialization
*/

void StoreImpl::InitJavaBindings (JNIEnv* env)
{
  StoreSingleton::Instance ()->InitJavaBindings (env);
}
