#ifndef MEDIALIB_SPINE_CORE_SHARED_HEADER
#define MEDIALIB_SPINE_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/singleton.h>

#include <media/geometry/mesh.h>

#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/material.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>

#include <animation_state.h>
#include <animation_state_data.h>
#include <material.h>
#include <skeleton.h>
#include <skeleton_data.h>
#include <track_entry.h>

namespace media
{

namespace spine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Class which loads skeleton data
///////////////////////////////////////////////////////////////////////////////////////////////////
class SerializerManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SerializerManagerImpl ();
    ~SerializerManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Serializers search
///////////////////////////////////////////////////////////////////////////////////////////////////
    const SerializerManager::SkeletonDataLoader* FindLoader              (const char* skeleton_file_name, const char* version);
    const SerializerManager::SkeletonDataLoader* FindLoaderLatestVersion (const char* skeleton_file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Serializers registration
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterLoader       (const char* skeleton_file_extension, const char* spine_version, const SerializerManager::SkeletonDataLoader& loader);
    void UnregisterLoader     (const char* skeleton_file_extension, const char* spine_version);
    void UnregisterAllLoaders (const char* skeleton_file_extension);
    void UnregisterAllLoaders ();

  private:
    SerializerManagerImpl (const SerializerManagerImpl&); //no impl
    SerializerManagerImpl& operator = (const SerializerManagerImpl&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

typedef common::Singleton<SerializerManagerImpl> SerializerManagerSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Wrappers for objects creation
///////////////////////////////////////////////////////////////////////////////////////////////////
class Wrappers
{
  public:
    template <class Ret, class T>
    static Ret Wrap (T* ptr)
    {
      if (!ptr)
        throw xtl::format_operation_exception ("media::spine::Wrappers::Wrap", "Attempt to wrap null internal object of type '%s'", typeid (T).name ());

      return Ret (ptr);
    }
};

}

}

#endif
