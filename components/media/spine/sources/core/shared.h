#ifndef MEDIALIB_SPINE_CORE_SHARED_HEADER
#define MEDIALIB_SPINE_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/geometry/mesh.h>

#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/attachment.h>
#include <media/spine/bone.h>
#include <media/spine/material.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>
#include <media/spine/slot.h>

#include <animation_state_data_impl.h>
#include <animation_state_impl.h>
#include <attachment_impl.h>
#include <bone_impl.h>
#include <material_impl.h>
#include <skeleton_data_impl.h>
#include <skeleton_impl.h>
#include <slot_impl.h>
#include <track_entry_impl.h>
#include <wrappers.h>

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

}

}

#endif
