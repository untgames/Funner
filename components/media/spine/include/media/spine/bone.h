#ifndef MEDIALIB_SPINE_BONE_HEADER
#define MEDIALIB_SPINE_BONE_HEADER

#include <math/vector.h>

namespace media
{

namespace spine
{

//forward declarations
class BoneImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine bone
///////////////////////////////////////////////////////////////////////////////////////////////////
class Bone
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    Bone (const Bone&);
    ~Bone ();

    Bone& operator = (const Bone&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Transform
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPosition (const math::vec2f& position);
    void        SetPosition (float position_x, float position_y);
    math::vec2f Position    () const;
    void        SetRotation (float rotation);
    float       Rotation    () const;
    void        SetScale    (const math::vec2f& scale);
    void        SetScale    (float scale_x, float scale_y);
    math::vec2f Scale       () const;
    void        SetShear    (const math::vec2f& shear);
    void        SetShear    (float shear_x, float shear_y);
    math::vec2f Shear       () const;

    math::vec2f WorldPosition () const;
    math::vec2f WorldRotation () const;
    math::vec2f WorldScale    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Helpers for converting between coordinate spaces
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec2f WorldToLocal (const math::vec2f& position) const;
    math::vec2f WorldToLocal (float position_x, float position_y) const;
    float       WorldToWorld (float rotation) const;
    math::vec2f LocalToWorld (const math::vec2f& position) const;
    math::vec2f LocalToWorld (float position_x, float position_y) const;
    float       LocalToWorld (float rotation) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Linked bones
///////////////////////////////////////////////////////////////////////////////////////////////////
          Bone* Parent ();
    const Bone* Parent () const;

    unsigned int ChildrenCount () const;

          Bone& Child (unsigned int index);
    const Bone& Child (unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Bone&);

  protected:
    Bone (BoneImpl*);

  private:
    BoneImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Bone&, Bone&);

}

}

#endif
