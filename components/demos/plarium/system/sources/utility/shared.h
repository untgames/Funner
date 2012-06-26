#include <cstdio>
#include <cstdarg>

#include <sgi_stl/string>
#include <sgi_stl/stdexcept>

#ifdef __APPLE__

#include <CoreFoundation/CFDate.h>

#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonHMAC.h>

#else

#include <ctime>

#endif

#include <zlib.h>

#include <utility/crypto.h>
#include <utility/hash.h>
#include <utility/utils.h>
