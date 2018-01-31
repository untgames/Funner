#ifndef MEDIALIB_SPINE_SHARED_BONE_HEADER
#define MEDIALIB_SPINE_SHARED_BONE_HEADER

#include <stl/vector>

#include <xtl/shared_ptr.h>

#include <media/spine/bone.h>

#include <object.h>

namespace media
{

namespace spine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine bone
///////////////////////////////////////////////////////////////////////////////////////////////////
class BoneImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Name () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Transform
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPosition (const math::vec2f& position)        = 0;
    virtual void        SetPosition (float position_x, float position_y) = 0;
    virtual math::vec2f Position    ()                                   = 0;
    virtual void        SetRotation (float rotation)                     = 0;
    virtual float       Rotation    ()                                   = 0;
    virtual void        SetScale    (const math::vec2f& scale)           = 0;
    virtual void        SetScale    (float scale_x, float scale_y)       = 0;
    virtual math::vec2f Scale       ()                                   = 0;
    virtual void        SetShear    (const math::vec2f& shear)           = 0;
    virtual void        SetShear    (float shear_x, float shear_y)       = 0;
    virtual math::vec2f Shear       ()                                   = 0;

    virtual math::vec2f WorldPosition () = 0;
    virtual math::vec2f WorldRotation () = 0;
    virtual math::vec2f WorldScale    () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Helpers for converting between coordinate spaces
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual math::vec2f WorldToLocal (const math::vec2f& position)        = 0;
    virtual math::vec2f WorldToLocal (float position_x, float position_y) = 0;
    virtual float       WorldToLocal (float rotation)                     = 0;
    virtual math::vec2f LocalToWorld (const math::vec2f& position)        = 0;
    virtual math::vec2f LocalToWorld (float position_x, float position_y) = 0;
    virtual float       LocalToWorld (float rotation)                     = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Linked bones, returned object is owned by callee.
///////////////////////////////////////////////////////////////////////////////////////////////////
            Bone*        Parent           ();
    virtual bool         HasParent        () = 0;
    virtual BoneImpl*    CreateParentImpl () = 0;
    virtual unsigned int ChildrenCount    () = 0;
            Bone&        Child            (unsigned int index);
    virtual BoneImpl*    CreateChildImpl  (unsigned int index) = 0;

  private:
    typedef xtl::shared_ptr<media::spine::Bone> BonePtr;
    typedef stl::vector<BonePtr>                BonesArray;

  private:
    BonePtr    parent;
    BonesArray children;
};

}

}

#endif
