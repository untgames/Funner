#ifndef STORE_CORE_TESTS_SHARED_HEADER
#define STORE_CORE_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/string>

#include <xtl/function.h>
#include <xtl/string.h>

#include <common/property_map.h>

#include <store/store.h>

using namespace common;
using namespace store;

void dump (const PropertyMap& properties)
{
  printf ("    map has %u properties (hash=%08x, layout_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());

  for (size_t i=0, count=properties.Size (); i<count; i++)
  {
    stl::string value;

    properties.GetProperty (i, value);

    printf ("     #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump (const Product& product)
{
  printf ("Product:\n");
  printf ("  description '%s'\n", product.Description ());
  printf ("  id          '%s'\n", product.Id ());
  printf ("  handle       %d\n",  product.Handle ());
  printf ("  properties:\n");
  dump (product.Properties ());
}

const char* get_transaction_state_name (TransactionState state)
{
  switch (state)
  {
    case TransactionState_New:        return "New";
    case TransactionState_Purchasing: return "Purchasing";
    case TransactionState_Purchased:  return "Purchased";
    case TransactionState_Failed:     return "Failed";
    case TransactionState_Restored:   return "Restored";
    default:                          return "Unknown";
  }
}

void dump (const Transaction& transaction)
{
  printf ("Transaction:\n");
  printf ("  state       '%s'\n", get_transaction_state_name (transaction.State ()));
  printf ("  status      '%s'\n", transaction.Status ());
  printf ("  product id  '%s'\n", transaction.ProductId ());
  printf ("  quantity     %d\n", transaction.Quantity ());
  printf ("  receipt     '%s'\n", transaction.ReceiptBase64 ());
  printf ("  handle       %d\n",  transaction.Handle ());
  printf ("  properties:\n");
  dump (transaction.Properties ());
}

#endif
