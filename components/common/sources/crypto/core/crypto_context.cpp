#include "shared.h"

using namespace common;

/*
    Константы
*/

namespace
{

const char* CRYPTO_COMPONENTS_MASK = "common.crypto.*"; //маска имён компонентов шифрования

}

/*
    Описание реализации контекста шифрования
*/

struct CryptoContext::Impl
{
  stl::auto_ptr<ICryptoContext> context;
};

/*
    Конструкторы / деструктор
*/

CryptoContext::CryptoContext (const char* method, const void* key, size_t key_bits)
  : impl (new Impl)
{
  try
  {        
    if (!method)
      throw xtl::make_null_argument_exception ("", "method");      
      
    static ComponentLoader loader (CRYPTO_COMPONENTS_MASK);

    impl->context = CryptoSystemSingleton::Instance ()->GetCrypter (method)(method, key, key_bits);
    
    if (!impl->context)
      throw xtl::format_operation_exception ("", "Can't create crypto context for method '%s'", method);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoContext::CryptoContext");
    throw;
  }
}

CryptoContext::~CryptoContext ()
{
}

/*
    Размер блока шифрования
*/

size_t CryptoContext::BlockSize () const
{
  return impl->context->GetBlockSize ();
}

/*
    Шифрование / дешифровка данных
*/

size_t CryptoContext::Update (size_t data_size, const void* src_buffer, void* dst_buffer)
{
  try
  {
    if (!src_buffer)
      throw xtl::make_null_argument_exception ("", "src_buffer");

    if (!dst_buffer)
      throw xtl::make_null_argument_exception ("", "dst_buffer");

    return impl->context->Update (data_size, src_buffer, dst_buffer);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CryptoContext::Update");
    throw;
  }
}

/*
    Обмен
*/

void CryptoContext::Swap (CryptoContext& context)
{
  swap (impl, context.impl);
}

namespace common
{

void swap (CryptoContext& context1, CryptoContext& context2)
{
  context1.Swap (context2);
}

}
