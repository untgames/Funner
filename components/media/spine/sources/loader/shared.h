#ifndef MEDIALIB_SPINE_LOADER_SHARED_HEADER
#define MEDIALIB_SPINE_LOADER_SHARED_HEADER

#include "../shared/skeleton_data_impl.h"

namespace media
{

namespace SPINE_NAMESPACE_NAME
{

class SkeletonDataImpl : public media::spine::SkeletonDataImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Create object instances
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::SkeletonImpl*           CreateSkeleton           ();
    media::spine::AnimationStateDataImpl* CreateAnimationStateData ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Width  ();
    float Height ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animations
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int AnimationsCount   ();
    const char*  AnimationName     (unsigned int index);
    float        AnimationDuration (unsigned int index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Skins
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int SkinsCount      ();
    const char*  SkinName        (unsigned int index);
    const char*  DefaultSkinName ();
};

}

}

#endif
