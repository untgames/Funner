#include <xtl/common_exceptions.h>

#include "bone_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Linked bones, returned object is owned by callee.
*/

Bone* BoneImpl::Parent ()
{
  bool has_parent = HasParent ();

  //bone for slot can not be changed in current spine version, so no need to check if bone was changed
  if (has_parent && !parent)
  {
    parent = BonePtr (Wrappers::WrapToNew<media::spine::Bone, media::spine::BoneImpl> (CreateParentImpl ()));
  }

  return parent.get ();
}

Bone& BoneImpl::Child (unsigned int index)
{
  unsigned int children_count = ChildrenCount ();

  if (children.size () != children_count)
  {
    children.clear ();
    children.resize (children_count);
  }

  if (index >= children_count)
    throw xtl::make_range_exception ("media::spine::BoneImpl::Child", "index", index, size_t (0), children_count);

  if (!children [index])
    children [index] = BonePtr (Wrappers::WrapToNew<media::spine::Bone, media::spine::BoneImpl> (CreateChildImpl (index)));

  return *children [index];
}
