#include "shared.h"

using namespace media::spine;

namespace
{

//Loaders map key. Spine version contains only major version, because only major versions should have incompatible formats and
//we are not going to have different minor versions for single major version
struct LoaderKey
{
  LoaderKey (const char* extension, const char* in_spine_version)
    : spine_version (strtol (in_spine_version, 0, 10))
    , extension_hash (common::strihash (extension))
    , hash (common::strihash (in_spine_version, extension_hash))
    {}

  bool operator == (const LoaderKey& key) const { return hash == key.hash; }

  int    spine_version;   //major version of spine loader
  size_t extension_hash;  //extension hash
  size_t hash;            //full hash
};

//hash function used by stl::hash_map
size_t hash (const LoaderKey& key)
{
  return key.hash;
}

typedef stl::hash_map<LoaderKey, SerializerManager::SkeletonDataLoader> LoadersMap;

}

//Serializer Manager implementation
struct SerializerManagerImpl::Impl
{
  LoadersMap loaders;  //registered loaders

  static const char* GetExtension (const char* file_name)
  {
    for (const char* s = file_name + strlen (file_name); s != file_name; --s)
      if (*s == '.')
        return s + 1;

    return file_name;
  }
};

/*
   Constructor / destructor
*/

SerializerManagerImpl::SerializerManagerImpl ()
  : impl (new Impl)
  {}

SerializerManagerImpl::~SerializerManagerImpl ()
{
  delete impl;
}

/*
   Serializers search
*/

const SerializerManager::SkeletonDataLoader* SerializerManagerImpl::FindLoader (const char* skeleton_file_name, const char* version)
{
  if (!skeleton_file_name)
    throw xtl::make_null_argument_exception ("media::spine::::SerializerManagerImpl::Find", "skeleton_file_name");

  const char* extension = Impl::GetExtension (skeleton_file_name);

  if (!extension)
    return 0;

  LoadersMap::iterator iter = impl->loaders.find (LoaderKey (extension, version));

  if (iter == impl->loaders.end ())
    return 0;

  return &iter->second;
}

const SerializerManager::SkeletonDataLoader* SerializerManagerImpl::FindLoaderLatestVersion (const char* skeleton_file_name)
{
  if (!skeleton_file_name)
    throw xtl::make_null_argument_exception ("media::spine::::SerializerManagerImpl::FindLatestVersion", "skeleton_file_name");

  const char* extension = Impl::GetExtension (skeleton_file_name);

  if (!extension)
    return 0;

  const SerializerManager::SkeletonDataLoader* return_value   = 0;
  int                                          latest_version = 0;
  size_t                                       extension_hash = common::strihash (extension);

  for (LoadersMap::iterator iter = impl->loaders.begin (), end = impl->loaders.end (); iter != end; ++iter)
  {
    if (iter->first.extension_hash == extension_hash)
    {
      if (iter->first.spine_version > latest_version)
      {
        latest_version = iter->first.spine_version;
        return_value   = &iter->second;
      }
    }
  }

  return return_value;
}

/*
   Serializers registration
*/

void SerializerManagerImpl::RegisterLoader (const char* skeleton_file_extension, const char* spine_version, const SerializerManager::SkeletonDataLoader& loader)
{
  static const char* METHOD_NAME = "media::spine::SerializerManagerImpl::RegisterLoader";

  if (!skeleton_file_extension)
    throw xtl::make_null_argument_exception (METHOD_NAME, "skeleton");

  if (!spine_version)
    throw xtl::make_null_argument_exception (METHOD_NAME, "spine_version");

  impl->loaders [LoaderKey (skeleton_file_extension, spine_version)] = loader;
}

void SerializerManagerImpl::UnregisterLoader (const char* skeleton_file_extension, const char* spine_version)
{
  if (!skeleton_file_extension || !spine_version)
    return;

  impl->loaders.erase (LoaderKey (skeleton_file_extension, spine_version));

}

void SerializerManagerImpl::UnregisterAllLoaders (const char* skeleton_file_extension)
{
  if (!skeleton_file_extension)
    return;

  size_t extension_hash = common::strihash (skeleton_file_extension);

  for (LoadersMap::iterator iter = impl->loaders.begin (), end = impl->loaders.end (); iter != end;)
  {
    if (iter->first.extension_hash == extension_hash) impl->loaders.erase (iter++);
    else                                              ++iter;
  }
}

void SerializerManagerImpl::UnregisterAllLoaders ()
{
  impl->loaders.clear ();
}

/*
   Serializer manager
*/

void SerializerManager::RegisterLoader (const char* skeleton_file_extension, const char* spine_version, const SkeletonDataLoader& loader)
{
  SerializerManagerSingleton::Instance ()->RegisterLoader (skeleton_file_extension, spine_version, loader);
}

void SerializerManager::UnregisterLoader (const char* skeleton_file_extension, const char* spine_version)
{
  SerializerManagerSingleton::Instance ()->UnregisterLoader (skeleton_file_extension, spine_version);
}

void SerializerManager::UnregisterAllLoaders (const char* skeleton_file_extension)
{
  SerializerManagerSingleton::Instance ()->UnregisterAllLoaders (skeleton_file_extension);
}

void SerializerManager::UnregisterAllLoaders ()
{
  SerializerManagerSingleton::Instance ()->UnregisterAllLoaders ();
}
