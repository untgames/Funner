#include "shared.h"

using namespace common;

/*
    Регистрация систем шифрования
*/

void CryptoSystemImpl::RegisterCrypter (const char* method, const CrypterCreator& creator)
{
  static const char* METHOD_NAME = "common::CryptoSystemImpl::RegisterCrypter";

  if (!method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "method");
    
  CrypterCreatorMap::iterator iter = creators.find (method);
  
  if (iter != creators.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "method", method, "Method already registered");
    
  creators.insert_pair (method, creator);
}

void CryptoSystemImpl::UnregisterCrypter (const char* method)
{
  if (!method)
    return;
    
  creators.erase (method);
}

void CryptoSystemImpl::UnregisterAllCrypters ()
{
  creators.clear ();
}

/*
    Поиск функции создания контекста шифрования
*/

CryptoSystemImpl::CrypterCreator CryptoSystemImpl::GetCrypter (const char* method) const
{
  static const char* METHOD_NAME = "common::CryptoSystemImpl::GetCrypter";

  if (!method)
    throw xtl::make_null_argument_exception (METHOD_NAME, "method");
    
  CrypterCreatorMap::const_iterator iter = creators.find (method);
  
  if (iter == creators.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "method", method, "Unknown method");
    
  return iter->second;
}

/*
    Обёртки
*/

void CryptoSystem::RegisterCrypter (const char* method, const CrypterCreator& creator)
{
  CryptoSystemSingleton::Instance ()->RegisterCrypter (method, creator);
}

void CryptoSystem::UnregisterCrypter (const char* method)
{
  CryptoSystemSingleton::Instance ()->UnregisterCrypter (method);
}

void CryptoSystem::UnregisterAllCrypters ()
{
  CryptoSystemSingleton::Instance ()->UnregisterAllCrypters ();
}
