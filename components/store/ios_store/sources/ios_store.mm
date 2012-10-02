#include "shared.h"

#import <Foundation/NSSet.h>

#import <StoreKit/SKPayment.h>
#import <StoreKit/SKPaymentQueue.h>
#import <StoreKit/SKPaymentTransaction.h>
#import <StoreKit/SKProduct.h>
#import <StoreKit/SKProductsRequest.h>

using namespace store;
using namespace store::ios_store;

@class TransactionObserver;
@class ProductsRequestDelegate;

namespace
{

const char* LOG_NAME            = "store.ios_store.IOSStore";
const char* SESSION_DESCRIPTION = "IOSStore";

void finish_transaction_handler (const Transaction& transaction)
{
  if (!transaction.Handle ())
    return;

  [[SKPaymentQueue defaultQueue] finishTransaction:(SKPaymentTransaction*)transaction.Handle ()];
}

// Слушатель событий обновления транзакций
class ITransactionObserverListener
{
  public:
    //Деструктор
    virtual ~ITransactionObserverListener () {}

    //События
    virtual void OnTransactionsRemoved (NSArray* transactions) = 0;
    virtual void OnDownloadsUpdated (NSArray* downloads) = 0;
    virtual void OnTransactionsUpdated (NSArray* transactions) = 0;
    virtual void OnRestoreCompletedTransactionsFinished () = 0;
    virtual void OnRestoreCompletedTransactionsFailedWithError (NSError* error) = 0;
};

// Слушатель событий обновления транзакций
class IProductsRequestListener
{
  public:
    //Деструктор
    virtual ~IProductsRequestListener () {}

    //События
    virtual void OnProductsRequestFinished (ProductsRequestDelegate* delegate) = 0;
    virtual void OnProductsRequestFailed (ProductsRequestDelegate* delegate, NSError* error) = 0;
};

}

@interface TransactionObserver : NSObject <SKPaymentTransactionObserver>
{
  @private
    ITransactionObserverListener* listener;
}

@end

@implementation TransactionObserver

-(id)initWithListener:(ITransactionObserverListener*) in_listener
{
  self = [super init];

  if (!self)
    return nil;

  listener = in_listener;

  return self;
}

-(void)paymentQueue:(SKPaymentQueue*) queue removedTransactions:(NSArray*) transactions
{
  listener->OnTransactionsRemoved (transactions);
}

-(void)paymentQueue:(SKPaymentQueue*) queue restoreCompletedTransactionsFailedWithError:(NSError*) error
{
  listener->OnRestoreCompletedTransactionsFailedWithError (error);
}

-(void)paymentQueue:(SKPaymentQueue*) queue updatedDownloads:(NSArray*) downloads
{
  listener->OnDownloadsUpdated (downloads);
}

-(void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
  listener->OnTransactionsUpdated (transactions);
}

-(void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue*)queue
{
  listener->OnRestoreCompletedTransactionsFinished ();
}

@end

@interface ProductsRequestDelegate : NSObject <SKProductsRequestDelegate>
{
  @private
    SKProductsRequest*          request;
    Store::LoadProductsCallback callback;
    IProductsRequestListener*   listener;
}

@end

@implementation ProductsRequestDelegate

-(id)initWithRequest:(SKProductsRequest*)in_request callback:(const Store::LoadProductsCallback&)in_callback listener:(IProductsRequestListener*) in_listener
{
  self = [super init];

  if (!self)
    return nil;

  request  = in_request;
  callback = in_callback;
  listener = in_listener;

  return self;
}

-(void)dealloc
{
  request.delegate = nil;

  [request release];

  [super dealloc];
}

-(void)cancelRequest
{
  [request cancel];
}

-(void)productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
{
  try
  {
    ProductList products_list;

    for (SKProduct* sk_product in response.products)
    {
      Product product;

      product.SetDescription ([[sk_product localizedDescription] UTF8String]);
      product.SetId ([[sk_product productIdentifier] UTF8String]);
      product.SetHandle ([sk_product retain], release_ns_object);

      common::PropertyMap properties;

      properties.SetProperty ("Title", [[sk_product localizedTitle] UTF8String]);
      properties.SetProperty ("Price", (float)[sk_product.price doubleValue]);

      NSNumberFormatter *number_formatter = [[NSNumberFormatter alloc] init];

      [number_formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
      [number_formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
      [number_formatter setLocale:sk_product.priceLocale];

      properties.SetProperty ("PriceString", [[number_formatter stringFromNumber:sk_product.price] UTF8String]);

      [number_formatter release];

      product.SetProperties (properties);

      products_list.Add (product);
    }

    callback (products_list);
  }
  catch (xtl::exception& e)
  {
    common::Log (LOG_NAME).Printf ("Exception while calling products load callback: '%s'", e.what ());
  }
  catch (...)
  {
    common::Log (LOG_NAME).Printf ("Unknown exception while calling products load callback");
  }
}

-(void)request:(SKRequest*)request didFailWithError:(NSError*)error
{
  listener->OnProductsRequestFailed (self, error);
}

-(void)requestDidFinish:(SKRequest*)request
{
  listener->OnProductsRequestFinished (self);
}

@end

namespace
{

/*
   Реализация магазина
*/

class IOSStore : public ITransactionObserverListener, public IProductsRequestListener
{
  public:
///Конструктор / деструктор
    IOSStore  ()
      : log (LOG_NAME)
    {
      running_products_requests_delegats = [[NSMutableSet alloc] init];

      transaction_observer = [[TransactionObserver alloc] initWithListener:this];

      [[SKPaymentQueue defaultQueue] addTransactionObserver:transaction_observer];

      NSArray* sk_transactions = [[SKPaymentQueue defaultQueue] transactions];

      pending_transactions.reserve ([sk_transactions count]);

      for (SKPaymentTransaction* sk_transaction in sk_transactions)
      {
        log.Printf ("Transaction '%s' updated", [[sk_transaction description] UTF8String]);

        pending_transactions.push_back (TransactionDesc (sk_transaction));
      }
    }

    ~IOSStore ()
    {
      [[SKPaymentQueue defaultQueue] removeTransactionObserver:transaction_observer];

      [transaction_observer release];

      for (ProductsRequestDelegate* delegate in running_products_requests_delegats)
        [delegate cancelRequest];

      [running_products_requests_delegats release];
    }

///Можно ли осуществлять покупки
    bool CanBuyProducts ()
    {
      return [SKPaymentQueue canMakePayments];
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

        NSMutableSet* ns_products = [[NSMutableSet alloc] initWithCapacity:products.Size ()];

        for (size_t i = 0, count = products.Size (); i < count; i++)
          [ns_products addObject:[NSString stringWithUTF8String:products [i]]];

        SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:ns_products];

        [ns_products release];

        ProductsRequestDelegate* delegate = [[ProductsRequestDelegate alloc] initWithRequest:request callback:callback listener:this];

        [running_products_requests_delegats addObject:delegate];

        request.delegate = delegate;

        [request start];
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::ios_store::IOSStore::LoadProducts");
        throw;
      }
    }

    void OnProductsRequestFinished (ProductsRequestDelegate* delegate)
    {
      [running_products_requests_delegats removeObject:delegate];
    }

    void OnProductsRequestFailed (ProductsRequestDelegate* delegate, NSError* error)
    {
      OnProductsRequestFinished (delegate);

      log.Printf ("Can't load products information, error '%s'", [[error description] UTF8String]);
    }

///Покупка / восстановление покупок
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback& callback)
    {
      TransactionsArray transactions = pending_transactions;

      for (TransactionsArray::iterator iter = transactions.begin (), end = transactions.end (); iter != end; ++iter)
        callback (iter->transaction);

      return store_signal.connect (callback);
    }

    void RestorePurchases ()
    {
      log.Printf ("Restoring transactions");

      return [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    }

    Transaction BuyProduct (const char* product_id, size_t count)
    {
      try
      {
        if (!product_id)
          throw xtl::make_null_argument_exception ("", "product_id");

        if (!count)
          throw xtl::make_null_argument_exception ("", "count");

        log.Printf ("Buy product '%s' with count %u requested", product_id, count);

        TransactionDesc new_transaction (product_id, count);

        pending_transactions.push_back (new_transaction);

        SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:[NSString stringWithUTF8String:product_id]];

        payment.quantity = count;

        [[SKPaymentQueue defaultQueue] addPayment:payment];

        return new_transaction.transaction;
      }
      catch (xtl::exception& e)
      {
        e.touch ("store::ios_store::IOSStore::BuyProduct");
        throw;
      }
    }

    void OnTransactionsRemoved (NSArray* transactions)
    {
      for (SKPaymentTransaction* sk_transaction in transactions)
      {
        log.Printf ("Transaction '%s' removed", [[sk_transaction description] UTF8String]);

        for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
        {
          if (iter->transaction.Handle () == sk_transaction)
          {
            pending_transactions.erase (iter);

            break;
          }
        }
      }
    }

    void OnDownloadsUpdated (NSArray* downloads)
    {
      log.Printf ("Downloads updated");
    }

    void OnTransactionsUpdated (NSArray* transactions)
    {
      for (SKPaymentTransaction* sk_transaction in transactions)
      {
        log.Printf ("Transaction '%s' updated", [[sk_transaction description] UTF8String]);

        bool handle_found = false;

        for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
        {
          if (iter->transaction.Handle () == sk_transaction)
          {
            handle_found = true;

            iter->Update (sk_transaction);

            break;
          }
        }

        if (!handle_found)
        {
          for (TransactionsArray::iterator iter = pending_transactions.begin (), end = pending_transactions.end (); iter != end; ++iter)
          {
            if (!iter->transaction.Handle () && !xtl::xstrcmp (iter->transaction.ProductId (), [sk_transaction.payment.productIdentifier UTF8String]) &&
                iter->transaction.Quantity () == (size_t)sk_transaction.payment.quantity);
            {
              iter->Update (sk_transaction);

              break;
            }
          }
        }
      }
    }

    void OnRestoreCompletedTransactionsFinished ()
    {
      log.Printf ("Restore completed transactions finished");
    }

    void OnRestoreCompletedTransactionsFailedWithError (NSError* error)
    {
      log.Printf ("Restore completed transactions failed, error '%s'", [[error description] UTF8String]);
    }

  private:
    struct TransactionDesc
    {
      SKPaymentTransaction* sk_transaction;
      Transaction           transaction;

      TransactionDesc (const char* product_id, size_t quantity)
        : sk_transaction (0)
        , transaction (finish_transaction_handler)
      {
        transaction.SetState (TransactionState_New);
        transaction.SetProductId (product_id);
        transaction.SetQuantity (quantity);
      }

      TransactionDesc (SKPaymentTransaction* in_transaction)
        : sk_transaction (0)
        , transaction (finish_transaction_handler)
      {
        Update (in_transaction);
      }

      void Update (SKPaymentTransaction* in_transaction)
      {
        sk_transaction = in_transaction;

        transaction.SetProductId ([sk_transaction.payment.productIdentifier UTF8String]);
        transaction.SetQuantity (sk_transaction.payment.quantity);
        transaction.SetHandle ([sk_transaction retain], release_ns_object);

        switch (sk_transaction.transactionState)
        {
          case SKPaymentTransactionStatePurchasing:
            transaction.SetState (TransactionState_Purchasing);
            break;
          case SKPaymentTransactionStatePurchased:
            transaction.SetState (TransactionState_Purchased);
            break;
          case SKPaymentTransactionStateFailed:
            transaction.SetState (TransactionState_Failed);
            transaction.SetStatus ([[sk_transaction.error description] UTF8String]);
            break;
          case SKPaymentTransactionStateRestored:
            transaction.SetState (TransactionState_Restored);
            break;
        }

        if (sk_transaction.transactionState == SKPaymentTransactionStatePurchased || sk_transaction.transactionState == SKPaymentTransactionStateRestored)
        {
          SKPaymentTransaction *paid_transaction = sk_transaction.transactionState == SKPaymentTransactionStatePurchased ? sk_transaction : sk_transaction.originalTransaction;

          NSData* receipt = paid_transaction.transactionReceipt;

          transaction.SetReceipt ([receipt length], [receipt bytes]);

          common::PropertyMap properties;

          properties.SetProperty ("Id", [paid_transaction.transactionIdentifier UTF8String]);

          transaction.SetProperties (properties);
        }
      }
    };

    typedef xtl::signal<void (const Transaction&)> StoreSignal;
    typedef stl::vector<TransactionDesc>           TransactionsArray;

  public:
    common::Log          log;
    TransactionObserver* transaction_observer;
    StoreSignal          store_signal;
    NSMutableSet*        running_products_requests_delegats;
    TransactionsArray    pending_transactions;
};

typedef common::Singleton<IOSStore> StoreSingleton;

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

Transaction BuyProduct (const char* product_id, size_t count)
{
  return StoreSingleton::Instance ()->BuyProduct (product_id, count);
}
