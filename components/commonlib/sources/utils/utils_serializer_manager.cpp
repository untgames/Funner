#include <common/serializer_manager.h>
#include <common/exception.h>
#include <common/singleton.h>
#include <common/hash.h>

#include <stl/hash_map>

#include <xtl/shared_ptr.h>

using namespace common;

namespace
{

/*
    Ключ в таблице поиска сериализатора
*/

struct SerializerKey
{
  SerializerKey (const char* extension, const std::type_info& in_signature)
    : extension_hash (strihash (extension)), signature (&in_signature)
  {
    hash = crc32 (&signature, sizeof (std::type_info*), extension_hash);    
  }
  
  bool operator == (const SerializerKey& key) const { return hash == key.hash; }

  size_t                extension_hash;
  const std::type_info* signature;
  size_t                hash;
};

//функция хэширования для ключа поиска сериализатора
size_t hash (const SerializerKey& key)
{
  return key.hash;
}

/*
    Реализация менеджера сериализаторов
*/

class SerializerManagerImpl
{
  public:
      //регистрация сериализатора
    void Register (const char* extension, const std::type_info& signature, detail::ISerializerHolder* holder)        
    {
      static const char* METHOD_NAME = "common::SerializerManagerImpl::Register";
      
      if (!extension)
        RaiseNullArgument (METHOD_NAME, "extension");
        
      if (!holder)
        RaiseNullArgument (METHOD_NAME, "holder");

      serializers [SerializerKey (extension, signature)] = SerializerHolderPtr (holder);
    }

      //отмена регистрации сериализатора
    void Unregister (const char* extension, const std::type_info& signature)
    {
      if (!extension)
        return;
        
      serializers.erase (SerializerKey (extension, signature));
    }

      //отмена регистрации сериализаторов с указанной сигнатуров
    void UnregisterAll (const std::type_info& signature)
    {
      for (SerializerMap::iterator iter=serializers.begin (), end=serializers.end (); iter!=end;)
        if (iter->first.signature == &signature) serializers.erase (iter++);
        else                                     ++iter;
    }

    void UnregisterAll (const char* extension)
    {
      if (!extension)
        return;

      size_t extension_hash = strihash (extension);

      for (SerializerMap::iterator iter=serializers.begin (), end=serializers.end (); iter!=end;)
        if (iter->first.extension_hash == extension_hash) serializers.erase (iter++);
        else                                              ++iter;
    }    
    
      //отмена регистрации всех сериализаторов
    void UnregisterAll ()
    {
      serializers.clear ();
    }
    
      //поиск сериализатора
    detail::ISerializerHolder* Find (const char* extension, const std::type_info& signature, bool raise_exception)
    {
      static const char* METHOD_NAME = "common::SerializerManagerImpl::Find";
      
      if (!extension)
      {
        if (raise_exception)
          RaiseNullArgument (METHOD_NAME, "extension");

        return 0;
      }
      
      SerializerMap::iterator iter = serializers.find (SerializerKey (extension, signature));
      
      if (iter == serializers.end ())
      {
        if (raise_exception)
          Raise<ArgumentException> (METHOD_NAME, "Invalid argument <extension>='%s'. "
            "No serializer with this extension for signature %s", extension, signature.name ());

        return 0;
      }
      
      return get_pointer (iter->second);
    }

  private:
    typedef xtl::shared_ptr<detail::ISerializerHolder>        SerializerHolderPtr;
    typedef stl::hash_map<SerializerKey, SerializerHolderPtr> SerializerMap;

  private:
    SerializerMap serializers;
};

}

typedef Singleton<SerializerManagerImpl> SerializerManagerSingleton;

/*
    SerializerManager
*/

/*
    Регистрация сериализаторов
*/

void SerializerManager::Register (const char* extension, const std::type_info& signature, detail::ISerializerHolder* holder)
{
  SerializerManagerSingleton::Instance ().Register (extension, signature, holder);
}

void SerializerManager::Unregister (const char* extension, const std::type_info& signature)
{
  SerializerManagerSingleton::Instance ().Unregister (extension, signature);
}

void SerializerManager::UnregisterAll (const std::type_info& signature)
{
  SerializerManagerSingleton::Instance ().UnregisterAll (signature);
}

void SerializerManager::UnregisterAll (const char* extension)
{
  SerializerManagerSingleton::Instance ().UnregisterAll (extension);
}

void SerializerManager::UnregisterAll ()
{
  SerializerManagerSingleton::Instance ().UnregisterAll ();
}

/*
    Поиск сериализатора
*/
 
detail::ISerializerHolder* SerializerManager::Find (const char* extension, const std::type_info& signature, bool raise_exception)
{
  return SerializerManagerSingleton::Instance ().Find (extension, signature, raise_exception);
}
