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
        raise_null_argument (METHOD_NAME, "extension");
        
      if (!holder)
        raise_null_argument (METHOD_NAME, "holder");

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
    detail::ISerializerHolder* Find (const char* name, const std::type_info& signature, SerializerFindMode mode, bool raise_exception)
    {
      static const char* METHOD_NAME = "common::SerializerManagerImpl::Find";
      
      switch (mode)
      {
        case SerializerFindMode_ByExtension:
          break;
        case SerializerFindMode_ByName:
          name = GetExtension (name);
          break;
        default:
          if (raise_exception)
            raise_invalid_argument (METHOD_NAME, "mode", mode);

          return 0;
      }

      if (!name)
      {
        if (raise_exception)
          raise_null_argument (METHOD_NAME, "name");

        return 0;
      }
      
      SerializerMap::iterator iter = serializers.find (SerializerKey (name, signature));
      
      if (iter == serializers.end ())
      {
        if (raise_exception)
          raise<ArgumentException> (METHOD_NAME, "Invalid argument <name>='%s'. "
            "No serializer with this extension found (check signature)", name);

        return 0;
      }
      
      return get_pointer (iter->second);
    }
    
  private:
    static const char* GetExtension (const char* file_name)
    {
      for (const char* s=file_name+strlen (file_name); s!=file_name; --s)
        if (*s == '.')
          return s + 1;

      return file_name;
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
 
detail::ISerializerHolder* SerializerManager::Find (const char* extension, const std::type_info& signature, SerializerFindMode mode, bool raise_exception)
{
  return SerializerManagerSingleton::Instance ().Find (extension, signature, mode, raise_exception);
}
