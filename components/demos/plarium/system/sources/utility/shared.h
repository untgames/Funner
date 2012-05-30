#include <cstdio>
#include <cstdarg>
#include <string>
#include <stdexcept>

#ifdef __APPLE__

#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonHMAC.h>

#endif

#include <utility/crypto.h>
#include <utility/hash.h>
#include <utility/utils.h>
