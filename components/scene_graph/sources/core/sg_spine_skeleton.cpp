#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    SpineSkeleton implementation description
*/

struct SpineSkeleton::Impl: public xtl::instance_counter<SpineSkeleton>
{
  bool                   visual_structure_dirty;  //is visual structure should be updated
  media::spine::Skeleton skeleton;                //skeleton

  Impl (const media::spine::Skeleton& in_skeleton)
    : visual_structure_dirty (true)
    , skeleton (in_skeleton)
    {}
};

/*
    Constructor / destructor
*/

SpineSkeleton::SpineSkeleton (const media::spine::Skeleton& skeleton)
  : impl (new Impl (skeleton))
  {}

SpineSkeleton::~SpineSkeleton ()
{
  delete impl;
}

/*
    Model creation
*/

SpineSkeleton::Pointer SpineSkeleton::Create (const media::spine::Skeleton& skeleton)
{
  return Pointer (new SpineSkeleton (skeleton), false);
}

/*
   Get skeleton
*/

const media::spine::Skeleton& SpineSkeleton::Skeleton () const
{
  return impl->skeleton;
}

media::spine::Skeleton& SpineSkeleton::Skeleton ()
{
  return impl->skeleton;
}

/*
   Invalidating visual structure (dynamic meshes child nodes)
*/

void SpineSkeleton::SetVisualStructureDirty (bool state)
{
  impl->visual_structure_dirty = state;
}

bool SpineSkeleton::IsVisualStructureDirty  () const
{
  return impl->visual_structure_dirty;
}

/*
    Method called during object visiting
*/

void SpineSkeleton::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
