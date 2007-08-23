#include "shared.h"

using namespace medialib::geometry;
using namespace common;

/*
    Конструктор
*/

MeshModelSystemImpl::MeshModelSystemImpl ()
{
}

/*
    Регистрация пользовательских функций загрузки / сохранения
*/

bool MeshModelSystemImpl::RegisterLoader (const char* extension, const LoadFunction& loader)
{
  if (!extension)
    RaiseNullArgument ("medialib::geometry::MeshModelSystemImpl::RegisterLoader", "extension");

  return loaders.insert_pair (extension, loader).second;
}

bool MeshModelSystemImpl::RegisterSaver (const char* extension, const SaveFunction& saver)
{
  if (!extension)
    RaiseNullArgument ("medialib::geometry::MeshModelSystemImpl::RegisterSaver", "extension");

  return savers.insert_pair (extension, saver).second;
}

void MeshModelSystemImpl::UnregisterLoader (const char* extension)
{
  if (!extension)
    return;

  loaders.erase (extension);
}

void MeshModelSystemImpl::UnregisterSaver (const char* extension)
{
  if (!extension)
    return;

  savers.erase (extension);
}

void MeshModelSystemImpl::UnregisterAllLoaders ()
{
  loaders.clear ();
}

void MeshModelSystemImpl::UnregisterAllSavers ()
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

void MeshModelSystemImpl::Load (const char* file_name, MeshModel& model) const
{
  if (!file_name)
    RaiseNullArgument ("medialib::geometry::MeshModelSystemImpl::Load", "file_name");

  const char* extension = GetExtension (file_name);

  LoadFunctions::const_iterator iter = loaders.find (extension);

  if (iter == loaders.end ())
    Raise<Exception> ("medialib::geometry::MeshModelSystemImpl::Load", "Can't load model from file '%s'. Unregistered extension '%s'.", file_name, extension); 
    
  model.Rename (file_name);    

  iter->second (file_name, model);  
}

void MeshModelSystemImpl::Save (const char* file_name, const MeshModel& model) const
{
  if (!file_name)
    RaiseNullArgument ("medialib::geometry::MeshModelSystemImpl::Save", "file_name");

  const char* extension = GetExtension (file_name);

  SaveFunctions::const_iterator iter = savers.find (extension);

  if (iter == savers.end ())
    Raise<Exception> ("medialib::geometry::MeshModelSystemImpl::Save", "Can't save model to file '%s'. Unregistered extension '%s'.", file_name, extension);

  iter->second (file_name, model);
}

/*
    Врапперы вызовов MeshModelSystemImpl    
*/

namespace medialib
{

namespace geometry
{

bool MeshModelSystem::RegisterLoader (const char* extension, const LoadFunction& loader)
{
  return MeshModelSystemSingleton::Instance ().RegisterLoader (extension, loader);
}

bool MeshModelSystem::RegisterSaver (const char* extension, const SaveFunction& saver)
{
  return MeshModelSystemSingleton::Instance ().RegisterSaver (extension, saver);
}

void MeshModelSystem::UnregisterLoader (const char* extension)
{
  MeshModelSystemSingleton::Instance ().UnregisterLoader (extension);
}

void MeshModelSystem::UnregisterSaver (const char* extension)
{
  MeshModelSystemSingleton::Instance ().UnregisterSaver (extension);
}

void MeshModelSystem::UnregisterAll ()
{
  MeshModelSystemSingleton::Instance ().UnregisterAllLoaders ();
  MeshModelSystemSingleton::Instance ().UnregisterAllSavers ();
}

}

}
