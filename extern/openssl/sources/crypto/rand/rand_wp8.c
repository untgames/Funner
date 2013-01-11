#include "cryptlib.h"
#include <openssl/rand.h>
#include "rand_lcl.h"

#ifdef WP8

int RAND_poll(void)
{
  return 0;
}

#endif
