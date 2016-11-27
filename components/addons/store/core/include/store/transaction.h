#ifndef STORE_TRANSACTION_HEADER
#define STORE_TRANSACTION_HEADER

#include <cstddef>
#include <xtl/functional_fwd>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace store
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние транзакции
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TransactionState
{
  TransactionState_New,         //Транзакция не отправлялась в магазин
  TransactionState_Purchasing,  //Транзакция отправлена в магазин
  TransactionState_Purchased,   //Транзакция успешно завершена
  TransactionState_Failed,      //Транзакция завершена с ошибкой
  TransactionState_Restored     //Транзакция восстановлена
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Транзакция
///////////////////////////////////////////////////////////////////////////////////////////////////
class Transaction
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор / копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (bool success, const char* status)> OnFinishedCallback;
    typedef xtl::function <void (const Transaction&, const OnFinishedCallback&)> FinishTransactionHandler;

    Transaction  (const FinishTransactionHandler& finish_handler = FinishTransactionHandler ());
    Transaction  (const Transaction&);
    ~Transaction ();

    Transaction& operator = (const Transaction&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор транзакции
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние
///////////////////////////////////////////////////////////////////////////////////////////////////
    TransactionState State    () const;
    void             SetState (TransactionState state);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Статус
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Status    () const;
    void        SetStatus (const char* status);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор продукта
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ProductId    () const;
    void        SetProductId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество покупаемых экземпляров продукта
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Quantity    () const;
    void   SetQuantity (size_t quantity);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чек покупки
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      ReceiptSize   () const;
    const void* ReceiptData   () const;
    const char* ReceiptBase64 () const;
    void        SetReceipt    (size_t size, const void* data);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Другие свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (const void*)> ReleaseHandleFunction;

    const void* Handle    () const;
    void        SetHandle (const void* handle, const ReleaseHandleFunction& release_function = ReleaseHandleFunction ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///Завершение транзакции (вызывать после окончания обработки транзакции для состояний Purchased, Failed и Restored)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Finish (const OnFinishedCallback& callback = OnFinishedCallback ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Transaction&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Transaction&, Transaction&);

}

#endif
