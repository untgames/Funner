#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/crypto.h>
#include <common/singleton.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация системы управления шифрованием
///////////////////////////////////////////////////////////////////////////////////////////////////
class CryptoSystemImpl
{
  public:
    typedef CryptoSystem::CrypterCreator CrypterCreator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация систем шифрования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterCrypter       (const char* method, const CrypterCreator& creator);
    void UnregisterCrypter     (const char* method);
    void UnregisterAllCrypters ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск функции создания контекста шифрования
///////////////////////////////////////////////////////////////////////////////////////////////////
    CrypterCreator GetCrypter (const char* method) const;

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, CrypterCreator> CrypterCreatorMap;

  private:  
    CrypterCreatorMap creators; //карта систем шифрования
};

typedef Singleton<CryptoSystemImpl> CryptoSystemSingleton;

}
