#include "shared.h"

int main ()
{
  printf ("Results of product_test:\n");
  
  try
  {
    Product product;

    printf ("default product\n");

    dump (product);

    product.SetDescription ("Description");
    product.SetId ("Id");
    product.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    product.SetProperties (properties);

    printf ("filled product\n");

    dump (product);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
