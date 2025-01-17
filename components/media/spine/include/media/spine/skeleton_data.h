#ifndef MEDIALIB_SPINE_SKELETON_DATA_HEADER
#define MEDIALIB_SPINE_SKELETON_DATA_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace spine
{

//forward declarations
class AnimationStateData;
class Skeleton;
class SkeletonDataImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine skeleton data
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonData
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkeletonData  ();
    SkeletonData  (const char* skeleton_file_name, const char* atlas_file_name);
    SkeletonData  (const SkeletonData&);
    ~SkeletonData ();

    SkeletonData& operator = (const SkeletonData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Load from file
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* skeleton_file_name, const char* atlas_file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Create object instances
///////////////////////////////////////////////////////////////////////////////////////////////////
    Skeleton           CreateSkeleton           () const;
    AnimationStateData CreateAnimationStateData () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Width  () const;
    float Height () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animations
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int AnimationsCount   () const;
    const char*  AnimationName     (unsigned int index) const;
    float        AnimationDuration (unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Skins
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int SkinsCount      () const;
    const char*  SkinName        (unsigned int index) const;
    const char*  DefaultSkinName () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SkeletonData&);

  protected:
    SkeletonData (SkeletonDataImpl*);

  private:
    SkeletonDataImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SkeletonData&, SkeletonData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Class which loads skeleton data
///////////////////////////////////////////////////////////////////////////////////////////////////
class SerializerManager
{
  public:
    typedef xtl::function<SkeletonDataImpl* (const char* skeleton_file_name, const char* atlas_file_name)> SkeletonDataLoader;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Serializers registration
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterLoader       (const char* skeleton_file_extension, const char* spine_version, const SkeletonDataLoader& loader);
    static void UnregisterLoader     (const char* skeleton_file_extension, const char* spine_version);
    static void UnregisterAllLoaders (const char* skeleton_file_extension);
    static void UnregisterAllLoaders ();
};

}

}

#endif
