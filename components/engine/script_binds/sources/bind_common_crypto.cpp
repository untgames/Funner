#include "shared.h"

#include <common/crypto.h>
#include <common/hash.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* COMMON_CRYPTO_LIBRARY = "Common.Crypto";
const char* COMMON_BASE64_LIBRARY = "Common.Base64";

}

namespace engine
{

/*
    Регистрация библиотек
*/

namespace
{

stl::string decompress (const stl::string& src)
{
  stl::string result;
  
  result.fast_resize (src.size () * 2);

  decompress_buffer (src.size (), src.c_str (), &result [0]);
  
  return result;
}

size_t crc32_wrapper (const char* string, size_t initial_value)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::crc32_wrapper", "string");

  return crc32 (string, strlen (string), initial_value);
}

stl::string md5_wrapper (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::md5_wrapper", "string");

  unsigned char result [16];
  
  md5 (string, strlen (string), result);  

  return decompress (stl::string ((char*)result, sizeof (result)));
}

stl::string sha256_wrapper (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::sha256_wrapper", "string");

  unsigned char result [32];
  
  sha256 (string, strlen (string), result);  

  return decompress (stl::string ((char*)result, sizeof (result)));
}

stl::string sha1_wrapper (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::sha1_wrapper", "string");

  unsigned char result [20];

  sha1 (string, strlen (string), result);

  return decompress (stl::string ((char*)result, sizeof (result)));
}

stl::string crypto_encoding (const char* string, const char* encoding, const char* key, const char* init_string, bool encoding_direction)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::crypto_encoding", "string");
    
  if (!encoding)
    throw xtl::make_null_argument_exception ("engine::crypto_encoding", "encoding");
    
  if (!key)
    key = "";
    
  if (!init_string)
    init_string = "";
    
  CryptoContext context ((stl::string (encoding) + (encoding_direction ? ".encrypt" : ".decrypt")).c_str (), key, strlen (key) * 8, init_string);
  
  stl::string result;
  
  result.fast_resize (strlen (string));
  
  size_t encoded_size = context.Update (result.size (), string, &result [0]);
  
  if (encoded_size != result.size ())
    throw xtl::format_operation_exception ("engine::crypto_encoding", "Crypted size %u is not equal to string length %u (string is '%s')", encoded_size, result.size (), string);
    
  return result;
}

stl::string base64_encode (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::base64_encode", "string");
    
  stl::string result;
  
  encode_base64 (strlen (string), string, result);
  
  return result;
}

stl::string base64_decode (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("engine::base64_decode", "string");
    
  stl::string result;
  
  decode_base64 (strlen (string), string, result);
  
  return result;
}

}

void bind_common_crypto (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_CRYPTO_LIBRARY);

  lib.Register ("CRC32",  make_invoker (
    make_invoker (&crc32_wrapper),
    make_invoker<size_t (const char*)> (xtl::bind (&crc32_wrapper, _1, ~0u))
  ));
  lib.Register ("MD5", make_invoker (&md5_wrapper));
  lib.Register ("SHA256", make_invoker (&sha256_wrapper));
  lib.Register ("SHA1", make_invoker (&sha1_wrapper));
  lib.Register ("Encode", make_invoker (
    make_invoker<stl::string (const char*, const char*, const char*, const char*)> (xtl::bind (&crypto_encoding, _1, _2, _3, _4, true)),
    make_invoker<stl::string (const char*, const char*, const char*)> (xtl::bind (&crypto_encoding, _1, _2, _3, "", true))
  ));
  lib.Register ("Decode", make_invoker (
    make_invoker<stl::string (const char*, const char*, const char*, const char*)> (xtl::bind (&crypto_encoding, _1, _2, _3, _4, false)),
    make_invoker<stl::string (const char*, const char*, const char*)> (xtl::bind (&crypto_encoding, _1, _2, _3, "", false))
  ));
}

void bind_common_base64 (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_BASE64_LIBRARY);

  lib.Register ("Encode", make_invoker (&base64_encode));
  lib.Register ("Decode", make_invoker (&base64_decode));
}

}
