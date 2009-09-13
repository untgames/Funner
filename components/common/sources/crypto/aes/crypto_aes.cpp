#include <cstring>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/crypto.h>
#include <common/component.h>

typedef size_t        aes_uint32_t;
typedef unsigned char byte;

#include "aes_le.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t CRYPTO_BLOCK_SIZE = 16; //размер блока шифрования в байтах

/*
    Реализация алгоритма AES
*/

struct AesParam
{
  aes_uint32_t key_expansion [64];
  aes_uint32_t number_of_rounds;
};

enum AesOperation
{
  AesOperation_Encrypt,
  AesOperation_Decrypt,
};

//инициализация контекста шифрования
void aes_init (AesParam* ap, const byte* key, size_t key_bits, AesOperation operation)
{
  static const char* METHOD_NAME = "common::aes_init";

    //проверка корректности аргументов

  if (!key)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key");

  if ((key_bits & 63) || key_bits < 128 || key_bits > 256)
    throw xtl::make_argument_exception (METHOD_NAME, "key_bits", key_bits, "Must be in range [128;256]");

    //подготовка контекста

  if (((key_bits & 63) == 0) && (key_bits >= 128) && (key_bits <= 256))
  {
    aes_uint32_t* rk, t, i, j;

    ap->number_of_rounds = 6 + (key_bits >> 5);

    rk = ap->key_expansion;

    memcpy (rk, key, key_bits >> 3);

    i = 0;

    if (key_bits == 128)
    {
      for (;;)
      {
        t = rk[3];
        t = (_ae4 [(t >>  8) & 0xff] & 0x000000ff) ^
            (_ae4 [(t >> 16) & 0xff] & 0x0000ff00) ^
            (_ae4 [(t >> 24)       ] & 0x00ff0000) ^
            (_ae4 [(t      ) & 0xff] & 0xff000000) ^
             _arc [i];

        rk [4] = (t ^= rk [0]);
        rk [5] = (t ^= rk [1]);
        rk [6] = (t ^= rk [2]);
        rk [7] = (t ^= rk [3]);

        if (++i == 10)
          break;

        rk += 4;
      }
    }
    else if (key_bits == 192)
    {
      for (;;)
      {
        t = rk [5];
        t = (_ae4 [(t >>  8) & 0xff] & 0x000000ff) ^
            (_ae4 [(t >> 16) & 0xff] & 0x0000ff00) ^
            (_ae4 [(t >> 24)       ] & 0x00ff0000) ^
            (_ae4 [(t      ) & 0xff] & 0xff000000) ^
             _arc [i];

        rk [6] = (t ^= rk [0]);
        rk [7] = (t ^= rk [1]);
        rk [8] = (t ^= rk [2]);
        rk [9] = (t ^= rk [3]);

        if (++i == 8)
          break;

        rk [10] = (t ^= rk [4]);
        rk [11] = (t ^= rk [5]);
        rk += 6;
      }
    }
    else if (key_bits == 256)
    {
      for (;;)
      {
        t = rk [7];
        t = (_ae4 [(t >>  8) & 0xff] & 0x000000ff) ^
            (_ae4 [(t >> 16) & 0xff] & 0x0000ff00) ^
            (_ae4 [(t >> 24)       ] & 0x00ff0000) ^
            (_ae4 [(t      ) & 0xff] & 0xff000000) ^
             _arc [i];
        rk [8]  = (t ^= rk [0]);
        rk [9]  = (t ^= rk [1]);
        rk [10] = (t ^= rk [2]);
        rk [11] = (t ^= rk [3]);

        if (++i == 7)
          break;

        t = (_ae4[(t      ) & 0xff] & 0x000000ff) ^
            (_ae4[(t >>  8) & 0xff] & 0x0000ff00) ^
            (_ae4[(t >> 16) & 0xff] & 0x00ff0000) ^
            (_ae4[(t >> 24)       ] & 0xff000000);

        rk [12] = (t ^= rk [4]);
        rk [13] = (t ^= rk [5]);
        rk [14] = (t ^= rk [6]);
        rk [15] = (t ^= rk [7]);

        rk += 8;
      }
    }

    if (operation == AesOperation_Decrypt)
    {
      rk = ap->key_expansion;

      for (i = 0, j = (ap->number_of_rounds << 2); i < j; i += 4, j -= 4)
      {
        t = rk [i  ]; rk [i  ] = rk [j  ]; rk [j  ] = t;
        t = rk [i+1]; rk [i+1] = rk [j+1]; rk [j+1] = t;
        t = rk [i+2]; rk [i+2] = rk [j+2]; rk [j+2] = t;
        t = rk [i+3]; rk [i+3] = rk [j+3]; rk [j+3] = t;
      }

      for (i = 1; i < ap->number_of_rounds; i++)
      {
        rk += 4;
        rk [0] =
          _ad0 [_ae4 [(rk[0]      ) & 0xff] & 0xff] ^
          _ad1 [_ae4 [(rk[0] >>  8) & 0xff] & 0xff] ^
          _ad2 [_ae4 [(rk[0] >> 16) & 0xff] & 0xff] ^
          _ad3 [_ae4 [(rk[0] >> 24)       ] & 0xff];
        rk [1] =
          _ad0 [_ae4 [(rk[1]      ) & 0xff] & 0xff] ^
          _ad1 [_ae4 [(rk[1] >>  8) & 0xff] & 0xff] ^
          _ad2 [_ae4 [(rk[1] >> 16) & 0xff] & 0xff] ^
          _ad3 [_ae4 [(rk[1] >> 24)       ] & 0xff];
        rk [2] =
          _ad0 [_ae4 [(rk[2]      ) & 0xff] & 0xff] ^
          _ad1 [_ae4 [(rk[2] >>  8) & 0xff] & 0xff] ^
          _ad2 [_ae4 [(rk[2] >> 16) & 0xff] & 0xff] ^
          _ad3 [_ae4 [(rk[2] >> 24)       ] & 0xff];
        rk [3] =
          _ad0 [_ae4 [(rk[3]      ) & 0xff] & 0xff] ^
          _ad1 [_ae4 [(rk[3] >>  8) & 0xff] & 0xff] ^
          _ad2 [_ae4 [(rk[3] >> 16) & 0xff] & 0xff] ^
          _ad3 [_ae4 [(rk[3] >> 24)       ] & 0xff];
      }
    }
  }
}

//шифрование
void aes_encrypt (AesParam* ap, aes_uint32_t* dst, const aes_uint32_t* src)
{
  aes_uint32_t s0, s1, s2, s3;
  aes_uint32_t t0, t1, t2, t3;
  aes_uint32_t* rk = ap->key_expansion;

  s0 = src [0] ^ rk [0];
  s1 = src [1] ^ rk [1];
  s2 = src [2] ^ rk [2];
  s3 = src [3] ^ rk [3];

  etfs (4);    /* round 1 */
  esft (8);    /* round 2 */
  etfs (12);   /* round 3 */
  esft (16);   /* round 4 */
  etfs (20);   /* round 5 */
  esft (24);   /* round 6 */
  etfs (28);   /* round 7 */
  esft (32);   /* round 8 */
  etfs (36);   /* round 9 */

  if (ap->number_of_rounds > 10)
  {
    esft (40); /* round 10 */
    etfs (44); /* round 11 */

    if (ap->number_of_rounds > 12)
    {
      esft(48); /* round 12 */
      etfs(52); /* round 13 */
    }
  }

  rk += (ap->number_of_rounds << 2);

  elr (); /* last round */

  dst [0] = s0;
  dst [1] = s1;
  dst [2] = s2;
  dst [3] = s3;
}

//дешифровка
void aes_decrypt (AesParam* ap, aes_uint32_t* dst, const aes_uint32_t* src)
{
  aes_uint32_t s0, s1, s2, s3;
  aes_uint32_t t0, t1, t2, t3;
  aes_uint32_t* rk = ap->key_expansion;

  s0 = src [0] ^ rk [0];
  s1 = src [1] ^ rk [1];
  s2 = src [2] ^ rk [2];
  s3 = src [3] ^ rk [3];

  dtfs (4);    /* round 1 */
  dsft (8);    /* round 2 */
  dtfs (12);   /* round 3 */
  dsft (16);   /* round 4 */
  dtfs (20);   /* round 5 */
  dsft (24);   /* round 6 */
  dtfs (28);   /* round 7 */
  dsft (32);   /* round 8 */
  dtfs (36);   /* round 9 */

  if (ap->number_of_rounds > 10)
  {
    dsft (40); /* round 10 */
    dtfs (44); /* round 11 */

    if (ap->number_of_rounds > 12)
    {
      dsft (48); /* round 12 */
      dtfs (52); /* round 13 */
    }
  }

  rk += (ap->number_of_rounds << 2);

  dlr (); /* last round */

  dst [0] = s0;
  dst [1] = s1;
  dst [2] = s2;
  dst [3] = s3;
}

/*
    Контекст шифрования
*/

template <AesOperation operation> class AesContext: public ICryptoContext
{
  public:
    AesContext (const void* key, size_t key_bits)
    {
      aes_init (&ap, (const byte*)key, key_bits, operation);
    }

///Размер блока шифрования в байтах
    size_t GetBlockSize () { return CRYPTO_BLOCK_SIZE; }

///Обновление контекста
    size_t Update (size_t data_size, const void* src_data, void* dst_data)
    {
      const char* src    = (const char*)src_data;
      char*       dst    = (char*)dst_data;
      size_t      length = 0;

      while (data_size >= CRYPTO_BLOCK_SIZE)
      {
        if (operation == AesOperation_Encrypt) aes_encrypt (&ap, (aes_uint32_t*)dst, (const aes_uint32_t*)src);
        else                                   aes_decrypt (&ap, (aes_uint32_t*)dst, (const aes_uint32_t*)src);

        src       += CRYPTO_BLOCK_SIZE;
        dst       += CRYPTO_BLOCK_SIZE;
        data_size -= CRYPTO_BLOCK_SIZE;
        length    += CRYPTO_BLOCK_SIZE;
      }

      return length;
    }

  private:
    AesParam ap;
};

/*
    Компонент
*/

class AesComponent
{
  public:
    AesComponent ()
    {
      CryptoSystem::RegisterCrypter ("aes.encrypt", &CreateEncryptContext);
      CryptoSystem::RegisterCrypter ("aes.decrypt", &CreateDecryptContext);
    }

  private:
    static ICryptoContext* CreateEncryptContext (const char*, const void* key, size_t key_bits)
    {
      try
      {
        return new AesContext<AesOperation_Encrypt> (key, key_bits);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("common::AesComponent::CreateEncryptContext");
        throw;
      }
    }

    static ICryptoContext* CreateDecryptContext (const char*, const void* key, size_t key_bits)
    {
      try
      {
        return new AesContext<AesOperation_Decrypt> (key, key_bits);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("common::AesComponent::CreateDecryptContext");
        throw;
      }
    }
};

}

extern "C"
{

ComponentRegistrator<AesComponent> AesCrypto ("common.crypto.aes");

}
