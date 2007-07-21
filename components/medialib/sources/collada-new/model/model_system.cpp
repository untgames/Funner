#include <common/exception.h>

#include "shared.h"

using namespace medialib::collada;
using namespace common;

/*
    Конструктор / деструктор
*/

ModelSystemImpl::ModelSystemImpl ()
{
  RegisterLoader ("dae", &Model::DefaultDaeLoader);
}

/*
    Регистрация пользовательских функций загрузки / сохранения
*/

bool ModelSystemImpl::RegisterLoader (const char* extension, const LoadFunction& loader)
{
  return loaders.insert_pair (extension, loader).second;
}

bool ModelSystemImpl::RegisterSaver (const char* extension, const SaveFunction& saver)
{
  return savers.insert_pair (extension, saver).second;
}

void ModelSystemImpl::UnregisterLoader (const char* extension)
{
  loaders.erase (extension);
}

void ModelSystemImpl::UnregisterSaver (const char* extension)
{
  savers.erase (extension);
}

void ModelSystemImpl::UnregisterAllLoaders ()
{
  loaders.clear ();
}

void ModelSystemImpl::UnregisterAllSavers ()
{
  savers.clear ();
}

/*
    Загрузка / сохранение
*/

namespace
{

const char* GetExtension (const char* file_name)
{
  const char*  ext;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return file_name;

  ext = file_name + len;

  for (; len && (*ext != '.'); len--, ext--);

  if (!len)
    return file_name;

  return ext + 1;
}

}

void ModelSystemImpl::Load (const char* file_name, Model& model, const LogFunction& log) const
{
  if (!file_name)
    RaiseNullArgument ("medialib::collada::ModelSystemImpl::Load", "file_name");

  const char* extension = GetExtension (file_name);

  LoadFunctions::const_iterator iter = loaders.find (extension);

  if (iter == loaders.end ())
    Raise<Exception> ("medialib::collada::ModelSystemImpl::Load", "Can't load model from file '%s'. Unregistered extension '%s'.", file_name, extension); 

  iter->second (file_name, model, log);
  
  model.Rename (file_name);
}

void ModelSystemImpl::Save (const char* file_name, const Model& model, const LogFunction& log) const
{
  if (!file_name)
    RaiseNullArgument ("medialib::collada::ModelSystemImpl::Save", "file_name");

  const char* extension = GetExtension (file_name);

  SaveFunctions::const_iterator iter = savers.find (extension);

  if (iter == savers.end ())
    Raise<Exception> ("medialib::collada::ModelSystemImpl::Save", "Can't save model to file '%s'. Unregistered extension '%s'.", file_name, extension);

  iter->second (file_name, model, log);
}

/*
    Врапперы вызовов ModelSystemImpl    
*/

namespace medialib
{

namespace collada
{

bool ModelSystem::RegisterLoader (const char* extension, const LoadFunction& loader)
{
  return ModelSystemSingleton::Instance ().RegisterLoader (extension, loader);
}

bool ModelSystem::RegisterSaver (const char* extension, const SaveFunction& saver)
{
  return ModelSystemSingleton::Instance ().RegisterSaver (extension, saver);
}

void ModelSystem::UnregisterLoader (const char* extension)
{
  ModelSystemSingleton::Instance ().UnregisterLoader (extension);
}

void ModelSystem::UnregisterSaver (const char* extension)
{
  ModelSystemSingleton::Instance ().UnregisterSaver (extension);
}

void ModelSystem::UnregisterAll ()
{
  ModelSystemSingleton::Instance ().UnregisterAllLoaders ();
  ModelSystemSingleton::Instance ().UnregisterAllSavers ();
}

}

}
