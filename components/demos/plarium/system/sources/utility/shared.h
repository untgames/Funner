#include <cstdio>
#include <cstdarg>
#include <string>

#ifdef __APPLE__

#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonHMAC.h>

#endif

#include <utility/hash.h>
#include <utility/utils.h>
