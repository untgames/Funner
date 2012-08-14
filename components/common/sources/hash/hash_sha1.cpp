#include <string.h>

#include <openssl/sha.h>

#include <xtl/common_exceptions.h>

#include <common/hash.h>

using namespace common;

struct Sha1Context::Impl
{
  bool    need_init;  //init has to be called before hash calculation
  SHA_CTX context;

  Impl ()
    : need_init (true)
    {}

  void Init ()
  {
    SHA1_Init (&context);    

    need_init = false;
  }

  void Update (const void* data, size_t data_size)
  {
    if (!data && data_size)
      throw xtl::make_null_argument_exception ("common::Sha1Context::Update", "data");

    if (need_init)
      Init ();

    SHA1_Update (&context, data, data_size);
  }

  void Finish (unsigned char (&result_hash_value) [20])
  {
    if (!result_hash_value)
      throw xtl::make_null_argument_exception ("common::Sha1Context::Finish", "result_hash_value");

    if (need_init)
      Init ();

    SHA1_Final (result_hash_value, &context);

    need_init = true;
  }
};

namespace common
{

void sha1 (const void* data, size_t size, unsigned char (&result_hash_value) [20])
{
  if (!data && size)
    throw xtl::make_null_argument_exception ("common::sha1", "result_hash_value");
    
  Sha1Context context;
  
  context.Update (data, size);
  context.Finish (result_hash_value);
}

}

/*
   Constructor / destructor
*/

Sha1Context::Sha1Context ()
  : impl (new Impl)
{
}

Sha1Context::~Sha1Context ()
{
  delete impl;
}

/*
   Update
*/

void Sha1Context::Update (const void* data, size_t data_size)
{
  impl->Update (data, data_size);
}

/*
   Finish hash calculation and write result data to output buffer
*/

void Sha1Context::Finish (unsigned char (&result_hash_value) [20])
{
  impl->Finish (result_hash_value);
}
