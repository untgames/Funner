#include "shared.h"

using namespace common;
using namespace input;

/*
   Работа с пользовательскими функциями загрузки и сохранения
*/

void TranslationMapManagerImpl::RegisterLoader (const char* extension, const TranslationMapManager::LoadHandler& handler)
{
  LoadHandlersMap::iterator iter = load_handlers.find (extension);
  
  if (iter != load_handlers.end ())
    RaiseInvalidArgument ("input::TranslationMapManager::RegisterLoader", "extension", extension, "Handler already registered");

  load_handlers.insert_pair (extension, handler);
}

void TranslationMapManagerImpl::RegisterSaver (const char* extension, const TranslationMapManager::SaveHandler& handler)
{
  SaveHandlersMap::iterator iter = save_handlers.find (extension);
  
  if (iter != save_handlers.end ())
    RaiseInvalidArgument ("input::TranslationMapManager::RegisterSaver", "extension", extension, "Handler already registered");

  save_handlers.insert_pair (extension, handler);
}

void TranslationMapManagerImpl::UnregisterLoader (const char* extension)
{
  load_handlers.erase (extension);
}

void TranslationMapManagerImpl::UnregisterSaver (const char* extension)
{
  save_handlers.erase (extension);
}

void TranslationMapManagerImpl::UnregisterAllLoaders ()
{
  load_handlers.clear ();
}

void TranslationMapManagerImpl::UnregisterAllSavers ()
{
  save_handlers.clear ();
}

void TranslationMapManagerImpl::UnregisterAll ()
{
  UnregisterAllLoaders ();
  UnregisterAllSavers  ();
}

namespace
{

inline const char* get_extension (const char* file_name)
{
  for (const char* s=file_name+strlen (file_name); s!=file_name; --s)
    if (*s == '.')
      return s + 1;

  return file_name;
}

}

TranslationMapManagerImpl::TranslationMapManagerImpl ()
{
  RegisterLoader ("keymap", &translation_map_loader);
  RegisterSaver  ("keymap", &translation_map_saver);
}

void TranslationMapManagerImpl::Load (const char* file_name, TranslationMap& target_map) const
{
  if (!file_name)
    RaiseNullArgument ("input::TranslationMapManagerImpl::GetLoader", "file_name");

  LoadHandlersMap::const_iterator iter = load_handlers.find (get_extension (file_name));

  if (iter == load_handlers.end ())
    RaiseNotSupported ("input::TranslationMapManagerImpl::GetLoader", "Extension '%s' not registered", get_extension (file_name));

  iter->second (file_name, target_map);
}

void TranslationMapManagerImpl::Save (const char* file_name, const TranslationMap& source_map) const
{
  if (!file_name)
    RaiseNullArgument ("input::TranslationMapManagerImpl::GetSaver", "file_name");

  SaveHandlersMap::const_iterator iter = save_handlers.find (get_extension (file_name));

  if (iter == save_handlers.end ())
    RaiseNotSupported ("input::TranslationMapManagerImpl::GetSaver", "Extension '%s' not registered", get_extension (file_name));

  iter->second (file_name, source_map);
}

/*
   Менеджер таблиц трансляции
*/

void TranslationMapManager::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  if (!extension)
    RaiseNullArgument ("input::TranslationMapManager::RegisterLoader", "extension");

  TranslationMapManagerSingleton::Instance ().RegisterLoader (extension, handler);
}

void TranslationMapManager::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  if (!extension)
    RaiseNullArgument ("input::TranslationMapManager::RegisterSaver", "extension");

  TranslationMapManagerSingleton::Instance ().RegisterSaver (extension, handler);
}

void TranslationMapManager::UnregisterLoader (const char* extension)
{
  if (!extension)
    RaiseNullArgument ("input::TranslationMapManager::UnregisterLoader", "extension");

  TranslationMapManagerSingleton::Instance ().UnregisterLoader (extension);
}

void TranslationMapManager::UnregisterSaver (const char* extension)
{
  if (!extension)
    RaiseNullArgument ("input::TranslationMapManager::UnregisterSaver", "extension");

  TranslationMapManagerSingleton::Instance ().UnregisterSaver (extension);
}

void TranslationMapManager::UnregisterAllLoaders ()
{
  TranslationMapManagerSingleton::Instance ().UnregisterAllLoaders ();
}

void TranslationMapManager::UnregisterAllSavers ()
{
  TranslationMapManagerSingleton::Instance ().UnregisterAllSavers ();
}

void TranslationMapManager::UnregisterAll ()
{
  TranslationMapManagerSingleton::Instance ().UnregisterAll ();
}
