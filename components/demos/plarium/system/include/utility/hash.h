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

}

}

#endif
