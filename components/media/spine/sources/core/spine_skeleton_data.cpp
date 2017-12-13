#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

SkeletonData::SkeletonData ()
  : impl (0)
  {}

SkeletonData::SkeletonData (ISkeletonData* in_impl)
  : impl (in_impl)
  {}

SkeletonData::SkeletonData (const char* skeleton_file_name, const char* atlas_file_name)
  : impl (0)
{
  try
  {
    Load (skeleton_file_name, atlas_file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::spine::SkeletonData::SkeletonData (const char*, const char*)");
    throw;
  }
}

SkeletonData::SkeletonData (const SkeletonData& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

SkeletonData::~SkeletonData ()
{
  if (impl)
    impl->Release ();
}

SkeletonData& SkeletonData::operator = (const SkeletonData& source)
{
  SkeletonData (source).Swap (*this);

  return *this;
}

/*
   Load from file
*/

void SkeletonData::Load (const char* skeleton_file_name, const char* atlas_file_name)
{
  static const char* METHOD_NAME = "media::spine::SkeletonData::Load";

  if (!skeleton_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "skeleton_file_name");

  if (!atlas_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "atlas_file_name");

  try
  {
    static common::ComponentLoader component_loader ("media.spine.loaders.*");

    //TODO implement file pre-parsing and find loader for corresponding version

    const SerializerManager::SkeletonDataLoader* loader = SerializerManagerSingleton::Instance ()->FindLoaderLatestVersion (skeleton_file_name);

    if (!loader)
      throw xtl::format_operation_exception ("", "Can't find loader for file '%s'", skeleton_file_name);

    SkeletonData ((*loader) (skeleton_file_name, atlas_file_name)).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
   Create object instances
*/

Skeleton SkeletonData::CreateSkeleton () const
{
  if (!impl)
    throw xtl::format_operation_exception ("media::spine::SkeletonData::CreateSkeleton", "Skeleton data not loaded.");

  return Wrappers::Wrap<Skeleton, ISkeleton> (impl->CreateSkeleton ());
}

AnimationStateData SkeletonData::CreateAnimationStateData () const
{
  if (!impl)
    throw xtl::format_operation_exception ("media::spine::SkeletonData::CreateSkeleton", "Skeleton data not loaded.");

  return Wrappers::Wrap<AnimationStateData, IAnimationStateData> (impl->CreateAnimationStateData ());
}

/*
   Params
*/

float SkeletonData::Width () const
{
  if (!impl)
    return 0.f;

  return impl->Width ();
}

float SkeletonData::Height () const
{
  if (!impl)
    return 0.f;

  return impl->Height ();
}

/*
   Animations
*/

unsigned int SkeletonData::AnimationsCount () const
{
  if (!impl)
    return 0;

  return impl->AnimationsCount ();
}

const char* SkeletonData::AnimationName (unsigned int index) const
{
  if (index >= AnimationsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::AnimationName", "index", index, 0, AnimationsCount ());

  return impl->AnimationName (index);
}

float SkeletonData::AnimationDuration (unsigned int index) const
{
  if (index >= AnimationsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::AnimationDuration", "index", index, 0, AnimationsCount ());

  return impl->AnimationDuration (index);
}

/*
   Skins
*/

unsigned int SkeletonData::SkinsCount () const
{
  if (!impl)
    return 0;

  return impl->SkinsCount ();
}

const char* SkeletonData::SkinName (unsigned int index) const
{
  if (index >= SkinsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::SkinName", "index", index, 0, SkinsCount ());

  return impl->SkinName (index);
}

const char* SkeletonData::DefaultSkinName () const
{
  if (!impl)
    return 0;

  return impl->DefaultSkinName ();
}

/*
   Swap
*/

void SkeletonData::Swap (SkeletonData& data)
{
  stl::swap (impl, data.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (SkeletonData& data1, SkeletonData& data2)
{
  data1.Swap (data2);
}

}

}
