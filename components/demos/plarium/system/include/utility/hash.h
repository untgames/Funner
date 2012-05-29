#ifndef UTILITY_HASH_HEADER
#define UTILITY_HASH_HEADER

namespace plarium
{

namespace utility
{

/*
   SHA256
*/

void sha256 (const void* data, size_t size, unsigned char result_hash_value [32]);

/*
   HMAC-SHA256
*/

void hmac_sha256 (const void* key, size_t key_size, const void* data, size_t data_size, unsigned char result_hash_value [32]);

}

}

#endif
