namespace detail
{

/*
    Интерфейс и реализация хранилища сериализатора
*/

class ISerializerHolder
{
  public:
    virtual ~ISerializerHolder () {}
};

template <class Signature>
class SerializerHolder: public ISerializerHolder
{
  public:
    typedef xtl::function<Signature> Handler;

    SerializerHolder (const Handler& in_serializer) : serializer (in_serializer) {}

    Handler serializer;
};

}

/*
===================================================================================================
    SerializerManager
===================================================================================================
*/

/*
    Регистрация сериализаторов
*/

template <class Signature>
void SerializerManager::Register (const char* extension, const xtl::function<Signature>& fn)
{
  detail::ISerializerHolder* volatile holder = new detail::SerializerHolder<Signature> (fn);

  try
  {
    Register (extension, typeid (Signature), holder);
  }
  catch (...)
  {
    delete holder;
    throw;
  }
}

template <class Signature>
inline void SerializerManager::Unregister (const char* extension)
{
  Unregister (extension, typeid (Signature));
}

template <class Signature>
inline void SerializerManager::UnregisterAll ()
{
  UnregisterAll (typeid (Signature));
}

/*
    Поиск сериализатора
*/

template <class Signature>
inline const xtl::function<Signature>& SerializerManager::Get (const char* name, SerializerFindMode mode)
{
  return static_cast<detail::SerializerHolder<Signature>*> (Find (name, typeid (Signature), mode, true))->serializer;
}

template <class Signature>
inline const xtl::function<Signature>* SerializerManager::Find (const char* name, SerializerFindMode mode)
{
  detail::SerializerHolder<Signature>* holder = static_cast<detail::SerializerHolder<Signature>*> (Find (name, typeid (Signature), mode, false));
  
  return holder ? &holder->serializer : 0;
}

/*
===================================================================================================
    ResourceSerializerManager
===================================================================================================
*/

/*
    Регистрация функций загрузки и сохранения
*/

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  SerializerManager::Register<LoaderSignature> (extension, handler);
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  SerializerManager::Register<SaverSignature> (extension, handler);
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::UnregisterLoader (const char* extension)
{
  SerializerManager::Unregister<LoaderSignature> (extension);
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::UnregisterSaver (const char* extension)
{
  SerializerManager::Unregister<SaverSignature> (extension);
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::UnregisterAllLoaders ()
{
  SerializerManager::UnregisterAll<LoaderSignature> ();
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::UnregisterAllSavers ()
{
  SerializerManager::UnregisterAll<SaverSignature> ();
}

template <class LoaderSignature, class SaverSignature>
inline void ResourceSerializerManager<LoaderSignature, SaverSignature>::UnregisterAll ()
{
  UnregisterAllLoaders ();
  UnregisterAllSavers ();
}

/*
    Поиск сериализатора
*/

template <class LoaderSignature, class SaverSignature>
inline const typename ResourceSerializerManager<LoaderSignature, SaverSignature>::LoadHandler&
ResourceSerializerManager<LoaderSignature, SaverSignature>::GetLoader (const char* name, SerializerFindMode mode)
{
  return SerializerManager::Get<LoaderSignature> (name, mode);
}

template <class LoaderSignature, class SaverSignature>
inline const typename ResourceSerializerManager<LoaderSignature, SaverSignature>::LoadHandler*
ResourceSerializerManager<LoaderSignature, SaverSignature>::FindLoader (const char* name, SerializerFindMode mode)
{
  return SerializerManager::Find<LoaderSignature> (name, mode);
}

template <class LoaderSignature, class SaverSignature>
inline const typename ResourceSerializerManager<LoaderSignature, SaverSignature>::SaveHandler&
ResourceSerializerManager<LoaderSignature, SaverSignature>::GetSaver (const char* name, SerializerFindMode mode)
{
  return SerializerManager::Get<SaverSignature> (name, mode);
}

template <class LoaderSignature, class SaverSignature>
inline const typename ResourceSerializerManager<LoaderSignature, SaverSignature>::SaveHandler*
ResourceSerializerManager<LoaderSignature, SaverSignature>::FindSaver (const char* name, SerializerFindMode mode)
{
  return SerializerManager::Find<SaverSignature> (name, mode);
}
