#include "shared.h"

int main ()
{
  printf ("Results of product_list_test:\n");
  
  try
  {
    ProductList product_list;

    printf ("default product list\n");
    printf ("size = %d\n", product_list.Size ());
    printf ("is empty = %c\n", product_list.IsEmpty () ? 'y' : 'n');
    printf ("capacity = %d\n", product_list.Capacity ());

    product_list.Reserve (64);

    printf ("capacity = %d\n", product_list.Capacity ());

    Product product1, product2;

    product1.SetId ("Id1");
    product2.SetId ("Id2");

    printf ("add result = %d\n", product_list.Add (product1));
    printf ("add result = %d\n", product_list.Add (product2));

    printf ("size = %d\n", product_list.Size ());
    printf ("is empty = %c\n", product_list.IsEmpty () ? 'y' : 'n');

    product_list.Remove (0);

    printf ("product 0 id = '%s'\n", product_list [0].Id ());
    printf ("size = %d\n", product_list.Size ());

    product_list.Clear ();

    printf ("size = %d\n", product_list.Size ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
