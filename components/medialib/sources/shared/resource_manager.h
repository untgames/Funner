#ifndef MEDIALIB_RESOURCE_MANAGER_HEADER
#define MEDIALIB_RESOURCE_MANAGER_HEADER

#include <stl/hash_map>
#include <xtl/function.h>
#include <common/exception.h>
#include <common/strlib.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реестр обработчиков ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class HandlerT> class ResourceHandlerRegistry
{
  public:
    typedef HandlerT Handler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Register      (const char* extension, const Handler& handler);
    void Unregister    (const char* extension);
    void UnregisterAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск обработчика
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Handler& FindHandlerByExtension (const char* extension) const;
    const Handler& FindHandlerByFileName  (const char* file_name) const;

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, Handler> HandlerMap;
    
  private:
    HandlerMap handlers;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер отладки
///////////////////////////////////////////////////////////////////////////////////////////////////
class DebugManager
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация функции вывода отладочных сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetDebugLog (const LogHandler&);
    const LogHandler& GetDebugLog () const;
          LogHandler& GetDebugLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Print   (const char* message);
    void Printf  (const char* format, ...);
    void VPrintf (const char* format, va_list list);

  private:
    LogHandler debug_log; //функция протоколирования
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Loader, class Saver>
class ResourceManager: public DebugManager
{
  public:
    typedef Loader LoadHandler;
    typedef Saver  SaveHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация функций загрузки и сохранения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterLoader       (const char* extension, const LoadHandler&);
    void RegisterSaver        (const char* extension, const SaveHandler&);
    void UnregisterLoader     (const char* extension);
    void UnregisterSaver      (const char* extension);
    void UnregisterAllLoaders ();
    void UnregisterAllSavers  ();
    void UnregisterAll        ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение функций загрузки и сохранения
///////////////////////////////////////////////////////////////////////////////////////////////////
    const LoadHandler& GetLoader (const char* file_name) const;
    const SaveHandler& GetSaver  (const char* file_name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка и сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Resource>                     void Load (const char* file_name, Resource& resource) const;
    template <class Resource, class T1>           void Load (const char* file_name, Resource& resource, T1& arg1) const;
    template <class Resource, class T1, class T2> void Load (const char* file_name, Resource& resource, T1& arg1, T2& arg2) const;
    template <class Resource>                     void Save (const char* file_name, const Resource& resource) const;
    template <class Resource, class T1>           void Save (const char* file_name, const Resource& resource, T1& arg1) const;
    template <class Resource, class T1, class T2> void Save (const char* file_name, const Resource& resource, T1& arg1, T2& arg2) const;

  private:
    typedef ResourceHandlerRegistry<LoadHandler> LoaderRegistry;
    typedef ResourceHandlerRegistry<SaveHandler> SaverRegistry;

  private:
    LoaderRegistry loaders; //обработчики загрузки
    SaverRegistry  savers;  //обработчики сохранения
};

#include <shared/detail/resource_manager.inl>

}

#endif
