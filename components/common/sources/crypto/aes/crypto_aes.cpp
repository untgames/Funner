#include <cstring>

#include <openssl/aes.h>
#include <openssl/modes.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/crypto.h>
#include <common/component.h>

using namespace common;

namespace components
{

namespace aes_crypto
{

/*
    Константы
*/

const size_t CRYPTO_BLOCK_SIZE = 16; //размер блока шифрования в байтах

/*
    Реализация алгоритма AES
*/

enum AesOperation
{
  AesOperation_Encrypt,
  AesOperation_Decrypt,
};


/*
    Контекст шифрования
*/

class AesContext: public ICryptoContext
{
  public:
    AesContext (AesOperation in_operation, const void* key, int key_bits, bool in_ofb, const char* init_string)
      : operation (in_operation)
      , ofb (in_ofb)
      , num ()
    {
      if (!init_string)
        init_string = "";
        
      if (ofb)
      {
        size_t iv_len = strlen (init_string);
        
        if (iv_len >= sizeof (iv))
          iv_len = sizeof (iv);
          
        memcpy (iv, init_string, iv_len);
        
        if (iv_len < sizeof (iv))
          memset (iv + iv_len, 0, sizeof (iv) - iv_len);
      }

      if (operation == AesOperation_Encrypt || ofb)
      {
        AES_set_encrypt_key ((const unsigned char*)key, key_bits, &aes_key);
      }
      else
      {
        AES_set_decrypt_key ((const unsigned char*)key, key_bits, &aes_key);
      }
    }

///Размер блока шифрования в байтах
    size_t GetBlockSize ()
    {
      return ofb ? 1 : CRYPTO_BLOCK_SIZE;
    }
    
///Обновление контекста
    size_t Update (size_t data_size, const void* src_data, void* dst_data)
    {
      const char* src = (const char*)src_data;
      char*       dst = (char*)dst_data;

      if (ofb)
      {
        AES_ofb128_encrypt ((const unsigned char*)src, (unsigned char*)dst, data_size, &aes_key, iv, &num);

        return data_size;
      }
      else
      {
        size_t length = 0;

        while (data_size >= CRYPTO_BLOCK_SIZE)
        {
          if (operation == AesOperation_Encrypt)
          {
            AES_encrypt ((const unsigned char*)src, (unsigned char*)dst, &aes_key);
          }
          else
          {
            AES_decrypt ((const unsigned char*)src, (unsigned char*)dst, &aes_key);
          }

          src       += CRYPTO_BLOCK_SIZE;
          dst       += CRYPTO_BLOCK_SIZE;
          data_size -= CRYPTO_BLOCK_SIZE;
          length    += CRYPTO_BLOCK_SIZE;
        }

        return length;
      }
    }

  private:
    AesOperation  operation;
    bool          ofb;
    AES_KEY       aes_key;
    unsigned char iv [16];
    int           num;
};

/*
    Компонент
*/

class AesComponent
{
  public:
    AesComponent ()
    {
      CryptoSystem::RegisterCrypter ("aes.encrypt", &CreateEncryptContext<false>);
      CryptoSystem::RegisterCrypter ("aes.decrypt", &CreateDecryptContext<false>);
      CryptoSystem::RegisterCrypter ("aes.ofb.encrypt", &CreateEncryptContext<true>);
      CryptoSystem::RegisterCrypter ("aes.ofb.decrypt", &CreateDecryptContext<true>);
    }

  private:
    template <bool ofb> static ICryptoContext* CreateEncryptContext (const char*, const void* key, unsigned short key_bits, const char* init_string)
    {
      try
      {
        return new AesContext (AesOperation_Encrypt, key, key_bits, ofb, init_string);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("common::AesComponent::CreateEncryptContext");
        throw;
      }
    }

    template <bool ofb> static ICryptoContext* CreateDecryptContext (const char*, const void* key, unsigned short key_bits, const char* init_string)
    {
      try
      {
        return new AesContext (AesOperation_Decrypt, key, key_bits, ofb, init_string);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("common::AesComponent::CreateDecryptContext");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<AesComponent> AesCrypto ("common.crypto.aes");

}

}

}
