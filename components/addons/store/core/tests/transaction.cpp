#include "shared.h"

const char* RECEIPT = "Receipt";

int main ()
{
  printf ("Results of transaction_test:\n");
  
  try
  {
    Transaction transaction;

    printf ("default transaction\n");

    dump (transaction);

    transaction.SetState (TransactionState_Purchased);
    transaction.SetStatus ("Success");
    transaction.SetProductId ("ProductId");
    transaction.SetQuantity (4);
    transaction.SetReceipt (xtl::xstrlen (RECEIPT), RECEIPT);
    transaction.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    transaction.SetProperties (properties);

    printf ("filled transaction\n");

    dump (transaction);

    transaction.Finish ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
