#include "shared.h"

using namespace network;

void print (const InetAddress& address)
{
  size_t address_size = address.Size ();

  printf ("Address size is %u\n", address_size);

  printf ("Address bytes:\n");

  for (size_t i = 0; i < address_size; i++)
    printf ("%u.", address [i]);

  printf ("\n");

  printf ("Address string is '%s'\n", address.ToString ());

//  printf ("IsAny       - %c\n", address.IsAny () ? 'y' : 'n');
  printf ("IsLoopback  - %c\n", address.IsLoopback () ? 'y' : 'n');
//  printf ("IsBroadcast - %c\n", address.IsBroadcast () ? 'y' : 'n');
  printf ("IsNone      - %c\n", address.IsNone () ? 'y' : 'n');
//  printf ("IsMulticast - %c\n", address.IsMulticast () ? 'y' : 'n');
}

int main ()
{
  printf ("Results of inet_address1_test:\n");

  try
  {
    unsigned char ipv4_value [4]  = { 10, 1, 0, 1 },
                  ipv6_value [16] = { 10, 1, 0, 1, 10, 1, 0, 1, 10, 1, 0, 1, 10, 1, 0, 1 };

    InetAddress default_address, host_address ("www.untgames.com"), array_address (4, ipv4_value),
                ipv4_address (ipv4_value), ipv6_address (ipv6_value), loopback_address (InetAddress::Loopback ());

    printf ("Is default address == ipv4 address: %c\n", default_address == ipv4_address ? 'y' : 'n');
    printf ("Is array address == ipv4 address: %c\n", array_address == ipv4_address ? 'y' : 'n');
    printf ("Is default address != ipv4 address: %c\n", default_address != ipv4_address ? 'y' : 'n');
    printf ("Is array address != ipv4 address: %c\n", array_address != ipv4_address ? 'y' : 'n');

    printf ("Default address:\n");
    print (default_address);

    printf ("Host address:\n");
    print (host_address);

    printf ("array address:\n");
    print (array_address);

    printf ("ipv4 address:\n");
    print (ipv4_address);

    printf ("ipv6 address:\n");
    print (ipv6_address);

    printf ("loopback address:\n");
    print (loopback_address);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
