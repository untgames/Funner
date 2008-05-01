#ifndef COMMONLIB_SERIALIZER_MANAGER_HEADER
#define COMMONLIB_SERIALIZER_MANAGER_HEADER

#include <xtl/function.h>

namespace common
{

namespace detail
{

//implementation forwards
class ISerializerHolder;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сериализаторов для различных типов ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
class SerializerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация сериализаторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature> static void Register   (const char* extension, const xtl::function<Signature>& fn);
    template <class Signature> static void Unregister (const char* extension);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Отмена регистрации групп
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature> static void UnregisterAll ();

    static void UnregisterAll (const char* extension);
    static void UnregisterAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск сериализатора
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature> static const xtl::function<Signature>& Get  (const char* extension);
    template <class Signature> static const xtl::function<Signature>* Find (const char* extension); //nothrow
    
  private:
    static void Register      (const char* extension, const std::type_info& signature, detail::ISerializerHolder* holder);
    static void Unregister    (const char* extension, const std::type_info& signature);
    static void UnregisterAll (const std::type_info& signature);
    
    static detail::ISerializerHolder* Find (const char* extension, const std::type_info& signature, bool raise_exception);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Шаблонная версия менеджера сериализации для определённого типа ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class LoaderSignature, class SaverSignature>
class ResourceSerializerManager
{
  public:
    typedef xtl::function<LoaderSignature> LoadHandler;
    typedef xtl::function<SaverSignature>  SaveHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация функций загрузки и сохранения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterLoader       (const char* extension, const LoadHandler& handler);
    static void RegisterSaver        (const char* extension, const SaveHandler& handler);
    static void UnregisterLoader     (const char* extension);
    static void UnregisterSaver      (const char* extension);
    static void UnregisterAllLoaders ();
    static void UnregisterAllSavers  ();
    static void UnregisterAll        ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск сериализатора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const LoadHandler& GetLoader  (const char* extension);
    static const LoadHandler* FindLoader (const char* extension); //nothrow
    static const SaveHandler& GetSaver   (const char* extension);
    static const SaveHandler* FindSaver  (const char* extension); //nothrow
};

#include <common/detail/serializer_manager.inl>

}

#endif
