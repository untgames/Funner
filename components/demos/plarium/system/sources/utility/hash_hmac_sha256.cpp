#include "shared.h"

#ifndef __APPLE__

namespace
{

}

#endif

namespace plarium
{

namespace utility
{

void hmac_sha256 (const void* key, size_t key_size, const void* data, size_t data_size, unsigned char result_hash_value [32])
{
#ifdef __APPLE__
  CCHmac (kCCHmacAlgSHA256, key, key_size, data, data_size, result_hash_value);
#else
#endif
}

}

}
