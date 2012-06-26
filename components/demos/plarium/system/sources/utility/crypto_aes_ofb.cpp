#include "shared.h"

using namespace plarium::utility;

namespace
{

/* The input and output encrypted as though 128bit ofb mode is being
 * used.  The extra state information to record how much of the
 * 128bit block we have used is contained in *num;
 */
void ofb128_encrypt (const unsigned char *in, unsigned char *out,
            size_t len, AesContext& context,
            unsigned char ivec[16], int *num)
{
    unsigned int n;
    size_t l=0;

    n = *num;

    if (16%sizeof(size_t) == 0) do { /* always true actually */
        while (n && len) {
            *(out++) = *(in++) ^ ivec[n];
            --len;
            n = (n+1) % 16;
        }
        while (len>=16) {
            context.Update (1, ivec, ivec);
            for (; n<16; n+=sizeof(size_t))
                *(size_t*)(out+n) =
                *(size_t*)(in+n) ^ *(size_t*)(ivec+n);
            len -= 16;
            out += 16;
            in  += 16;
            n = 0;
        }
        if (len) {
            context.Update (1, ivec, ivec);

            while (len--) {
                out[n] = in[n] ^ ivec[n];
                ++n;
            }
        }
        *num = n;
        return;
    } while(0);
    /* the rest would be commonly eliminated by x86* compiler */
    while (l<len) {
        if (n==0) {
            context.Update (1, ivec, ivec);
        }
        out[l] = in[l] ^ ivec[n];
        ++l;
        n = (n+1) % 16;
    }

    *num=n;
}

}

struct AesOfbContext::Impl
{
  AesContext    aes_context;
  unsigned char iv [16];
  int           num;

  Impl (const void* key, size_t key_bits, const unsigned char (&in_iv) [16])
    : aes_context (CryptoOperation_Encrypt, key, key_bits), num (0)
  {
    memcpy (iv, in_iv, sizeof (iv));
  }

  void Update (size_t data_size, const void* src_buffer, void* dst_buffer)
  {
    if (!src_buffer && data_size)
      throw sgi_stl::invalid_argument ("AesOfbContext::Update - null src_buffer");

    if (!dst_buffer && data_size)
      throw sgi_stl::invalid_argument ("AesOfbContext::Update - null dst_buffer");

    ofb128_encrypt ((const unsigned char*)src_buffer, (unsigned char*)dst_buffer, data_size, aes_context, iv, &num);
  }
};

namespace plarium
{

namespace utility
{

void aes_ofb (const void* key, size_t key_bits, size_t data_size, const void* src_buffer, void* dst_buffer, const unsigned char (&iv) [16])
{
  AesContext context (CryptoOperation_Encrypt, key, key_bits);

  int num = 0;

  unsigned char iv_copy [16];

  memcpy (iv_copy, iv, sizeof (iv_copy));

  ofb128_encrypt ((const unsigned char*)src_buffer, (unsigned char*)dst_buffer, data_size, context, iv_copy, &num);
}

}

}

/*
   Constructor / destructor (if iv == 0, constructor generates random iv)
*/

AesOfbContext::AesOfbContext (const void* key, size_t key_bits, const unsigned char (&iv) [16])
  : impl (new Impl (key, key_bits, iv))
  {}

AesOfbContext::~AesOfbContext ()
{
  delete impl;
}

/*
   Encryption / decryption
*/

void AesOfbContext::Update (size_t data_size, const void* src_buffer, void* dst_buffer)
{
  impl->Update (data_size, src_buffer, dst_buffer);
}
