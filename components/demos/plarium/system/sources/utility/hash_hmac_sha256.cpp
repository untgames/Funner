#include "shared.h"

using namespace plarium::utility;

#ifdef __APPLE__

struct HmacSha256Context::Impl
{
  bool          finished;  //was Finish called
  CCHmacContext context;

  Impl (const void* key, size_t key_size)
    : finished (false)
  {
    if (!key && key_size)
      throw std::invalid_argument ("HmacSha256Context::HmacSha256Context - null key");

    CCHmacInit (&context, kCCHmacAlgSHA256, key, key_size);
  }

  void Update (const void* data, size_t data_size)
  {
    if (finished)
      throw std::logic_error ("HmacSha256Context::Update - can't update after Finish");

    if (!data && data_size)
      throw std::invalid_argument ("HmacSha256Context::Update - null data");

    CCHmacUpdate (&context, data, data_size);
  }

  void Finish (unsigned char (&result_hash_value) [32])
  {
    if (finished)
      throw std::logic_error ("HmacSha256Context::Finish - can't finish after Finish");

    if (!result_hash_value)
      throw std::invalid_argument ("HmacSha256Context::Finish - null result_hash_value");

    CCHmacFinal (&context, result_hash_value);

    finished = true;
  }
};

#else

namespace
{

const size_t SHA256_BLOCK_SIZE  = 512 / 8;
const size_t SHA256_DIGEST_SIZE = 256 / 8;

typedef struct
{
  Sha256Context ctx_inside;
  Sha256Context ctx_outside;

  unsigned char block_ipad[SHA256_BLOCK_SIZE];
  unsigned char block_opad[SHA256_BLOCK_SIZE];
} hmac_sha256_ctx;

void hmac_sha256_init   (hmac_sha256_ctx *ctx, const unsigned char *key, unsigned int key_size);
void hmac_sha256_update (hmac_sha256_ctx *ctx, const unsigned char *message, unsigned int message_len);
void hmac_sha256_final  (hmac_sha256_ctx *ctx, unsigned char *mac);

void hmac_sha256_init (hmac_sha256_ctx *ctx, const unsigned char *key, unsigned int key_size)
{
  unsigned int fill;
  unsigned int num;

  const unsigned char *key_used;
  unsigned char key_temp [SHA256_DIGEST_SIZE];
  int i;

  if (key_size == SHA256_BLOCK_SIZE)
  {
    key_used = key;
    num = SHA256_BLOCK_SIZE;
  }
  else
  {
    if (key_size > SHA256_BLOCK_SIZE)
    {
      num = SHA256_DIGEST_SIZE;
      sha256(key, key_size, key_temp);
      key_used = key_temp;
    }
    else /* key_size > SHA256_BLOCK_SIZE */
    {
      key_used = key;
      num = key_size;
    }

    fill = SHA256_BLOCK_SIZE - num;

    memset(ctx->block_ipad + num, 0x36, fill);
    memset(ctx->block_opad + num, 0x5c, fill);
  }

  for (i = 0; i < (int) num; i++)
  {
    ctx->block_ipad[i] = key_used[i] ^ 0x36;
    ctx->block_opad[i] = key_used[i] ^ 0x5c;
  }

  ctx->ctx_inside.Update  (ctx->block_ipad, SHA256_BLOCK_SIZE);
  ctx->ctx_outside.Update (ctx->block_opad, SHA256_BLOCK_SIZE);
}

void hmac_sha256_update (hmac_sha256_ctx *ctx, const unsigned char *message, unsigned int message_len)
{
  ctx->ctx_inside.Update (message, message_len);
}

void hmac_sha256_final (hmac_sha256_ctx *ctx, unsigned char *mac)
{
  unsigned char digest_inside [SHA256_DIGEST_SIZE];

  ctx->ctx_inside.Finish  (digest_inside);
  ctx->ctx_outside.Update (digest_inside, SHA256_DIGEST_SIZE);
  ctx->ctx_outside.Finish (mac);
}

}

struct HmacSha256Context::Impl
{
  bool            finished;  //was Finish called
  hmac_sha256_ctx context;

  Impl (const void* key, size_t key_size)
    : finished (false)
  {
    if (!key && key_size)
      throw std::invalid_argument ("HmacSha256Context::HmacSha256Context - null key");

    hmac_sha256_init (&context, (const unsigned char*)key, key_size);
  }

  void Update (const void* data, size_t data_size)
  {
    if (finished)
      throw std::logic_error ("HmacSha256Context::Update - can't update after Finish");

    if (!data && data_size)
      throw std::invalid_argument ("HmacSha256Context::Update - null data");

    hmac_sha256_update (&context, (const unsigned char*)data, data_size);
  }

  void Finish (unsigned char (&result_hash_value) [32])
  {
    if (finished)
      throw std::logic_error ("HmacSha256Context::Finish - can't finish after Finish");

    if (!result_hash_value)
      throw std::invalid_argument ("HmacSha256Context::Finish - null result_hash_value");

    hmac_sha256_final (&context, result_hash_value);

    finished = true;
  }
};

#endif

namespace plarium
{

namespace utility
{

void hmac_sha256 (const void* key, size_t key_size, const void* data, size_t data_size, unsigned char (&result_hash_value) [32])
{
  if (!key)
    throw std::invalid_argument ("plarium::utility::hmac_sha256 - null key");

  if (!data && data_size)
    throw std::invalid_argument ("plarium::utility::hmac_sha256 - null data");

#ifdef __APPLE__
  CCHmac (kCCHmacAlgSHA256, key, key_size, data, data_size, result_hash_value);
#else
  hmac_sha256_ctx ctx;

  hmac_sha256_init   (&ctx, (const unsigned char*)key, key_size);
  hmac_sha256_update (&ctx, (const unsigned char*)data, data_size);
  hmac_sha256_final  (&ctx, result_hash_value);
#endif
}

}

}

/*
   Constructor / destructor
*/

HmacSha256Context::HmacSha256Context (const void* key, size_t key_size)
  : impl (new Impl (key, key_size))
{
}

HmacSha256Context::~HmacSha256Context ()
{
  delete impl;
}

/*
   Update
*/

void HmacSha256Context::Update (const void* data, size_t data_size)
{
  impl->Update (data, data_size);
}

/*
   Finish hash calculation and write result data to output buffer
*/

void HmacSha256Context::Finish (unsigned char (&result_hash_value) [32])
{
  impl->Finish (result_hash_value);
}
