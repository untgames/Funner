/*
    ResourceHandlerRegistry
*/

template <class T>
inline void ResourceHandlerRegistry<T>::Register (const char* extension, const Handler& handler)
{
  if (!extension)
    common::RaiseNullArgument ("medialib::ResourceHandlerRegistry::Register", "extension");

  HandlerMap::iterator iter = handlers.find (extension);
  
  if (iter != handlers.end ())
    common::RaiseInvalidArgument ("medialib::ResourcehandlerRegistry::Register", "extension", extension, "Handler already registered");

  handlers.insert_pair (extension, handler);
}

template <class T>
inline void ResourceHandlerRegistry<T>::Unregister (const char* extension)
{
  if (!extension)
    return;
    
  handlers.erase (extension);
}

template <class T>
inline void ResourceHandlerRegistry<T>::UnregisterAll ()
{
  handlers.clear ();
}

template <class T>
inline const T& ResourceHandlerRegistry<T>::FindHandlerByExtension (const char* extension) const
{
  if (!extension)
    common::RaiseNullArgument ("medialib::ResourceHandlerRegistry", "extension");
    
  HandlerMap::const_iterator iter = handlers.find (extension);

  if (iter == handlers.end ())
    common::RaiseNotSupported ("medialib::ResourceHandlerRegistry::FindHandlerByExtension", "Extension '%s' not registered", extension);

  return iter->second;
}

namespace detail
{

template <bool dummy> inline const char* get_extension (const char* file_name)
{
  for (const char* s=file_name+strlen (file_name); s!=file_name; --s)
    if (*s == '.')
      return s + 1;

  return file_name;
}

}

template <class T>
inline const T& ResourceHandlerRegistry<T>::FindHandlerByFileName (const char* file_name) const
{
  if (!file_name)
    common::RaiseNullArgument ("medialib::ResourceHandlerRegistry::FindHandlerByFileName", "file_name");

  return FindHandlerByExtension (detail::get_extension<true> (file_name));
}

/*
    DebugManager
*/

namespace detail
{

template <bool dummy>
inline void default_debug_log (const char*)
{
}

}

inline void DebugManager::SetDebugLog (const LogHandler& in_debug_log)
{
  debug_log = in_debug_log ? in_debug_log : &detail::default_debug_log<true>;
}

inline const DebugManager::LogHandler& DebugManager::GetDebugLog () const
{
  return debug_log;
}

inline DebugManager::LogHandler& DebugManager::GetDebugLog ()
{
  return debug_log;
}

inline void DebugManager::Print (const char* message)
{
  if (!message || !debug_log)
    return;
  
  try
  {
    debug_log (message);
  }
  catch (...)
  {
    //блокируем все исключения
  }
}

inline void DebugManager::VPrintf (const char* format, va_list list)
{
  if (!format)
    return;

  try
  {   
    Print (common::vformat (format, list).c_str ());
  }
  catch (...)
  {
    //блокируем все исключения
  }
}

inline void DebugManager::Printf (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VPrintf  (format, list);
}

/*
    ResourceManager
*/

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  loaders.Register (extension, handler);
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  savers.Register (extension, handler);
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::UnregisterLoader (const char* extension)
{
  loaders.Unregister (extension);
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::UnregisterSaver (const char* extension)
{
  savers.Unregister (extension);
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::UnregisterAllLoaders ()
{
  loaders.UnregisterAll ();
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::UnregisterAllSavers ()
{
  savers.UnregisterAll ();
}

template <class Loader, class Saver>
inline void ResourceManager<Loader, Saver>::UnregisterAll ()
{
  UnregisterAllLoaders ();
  UnregisterAllSavers ();
}

template <class Loader, class Saver>
inline const Loader& ResourceManager<Loader, Saver>::GetLoader (const char* file_name) const
{
  return loaders.FindHandlerByFileName (file_name);
}

template <class Loader, class Saver>
inline const Saver& ResourceManager<Loader, Saver>::GetSaver (const char* file_name) const
{
  return savers.FindHandlerByFileName (file_name);
}

template <class Loader, class Saver> template <class Resource>
inline void ResourceManager<Loader, Saver>::Load (const char* file_name, Resource& resource) const
{
  GetLoader (file_name)(file_name, resource);
}

template <class Loader, class Saver> template <class Resource, class T1>
inline void ResourceManager<Loader, Saver>::Load (const char* file_name, Resource& resource, T1& arg1) const
{
  GetLoader (file_name)(file_name, resource, arg1);
}

template <class Loader, class Saver> template <class Resource, class T1, class T2>
inline void ResourceManager<Loader, Saver>::Load (const char* file_name, Resource& resource, T1& arg1, T2& arg2) const
{
  GetLoader (file_name)(file_name, resource, arg1, arg2);
}

template <class Loader, class Saver> template <class Resource>
inline void ResourceManager<Loader, Saver>::Save (const char* file_name, const Resource& resource) const
{ 
  GetSaver (file_name)(file_name, resource);
}

template <class Loader, class Saver> template <class Resource, class T1>
inline void ResourceManager<Loader, Saver>::Save (const char* file_name, const Resource& resource, T1& arg1) const
{ 
  GetSaver (file_name)(file_name, resource, arg1);
}

template <class Loader, class Saver> template <class Resource, class T1, class T2>
inline void ResourceManager<Loader, Saver>::Save (const char* file_name, const Resource& resource, T1& arg1, T2& arg2) const
{ 
  GetSaver (file_name)(file_name, resource, arg1, arg2);
}
