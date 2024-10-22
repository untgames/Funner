#ifndef MEDIALIB_SPINE_ATTACHMENT_HEADER
#define MEDIALIB_SPINE_ATTACHMENT_HEADER

#include <xtl/type.h>

#include <math/vector.h>

namespace media
{

namespace spine
{

//forward declarations
class AttachmentImpl;
class PointAttachmentDataImpl;
class Wrappers;

//Possible attachment types (some types can be casted to inherited classes)
enum AttachmentType
{
  AttachmentType_Unknown,
  AttachmentType_Region,
  AttachmentType_BoundingBox,
  AttachmentType_Mesh,
  AttachmentType_LinkedMesh,
  AttachmentType_Path,
  AttachmentType_Point,       //data specific for attachment of this type can be fetched via AttachmentData method
  AttachmentType_Clipping
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine point attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
class PointAttachmentData
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    PointAttachmentData  (const PointAttachmentData&);
    ~PointAttachmentData ();

    PointAttachmentData& operator = (const PointAttachmentData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Point attachment parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec2f Position () const;
    float       Rotation () const;
    math::vec4f Color    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PointAttachmentData&);

  protected:
    PointAttachmentData (PointAttachmentDataImpl*);

  private:
    PointAttachmentDataImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PointAttachmentData&, PointAttachmentData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
class Attachment
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    Attachment  (const Attachment&);
    ~Attachment ();

    Attachment& operator = (const Attachment&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get Type and type-specific data. Returned object is owned by callee.
///////////////////////////////////////////////////////////////////////////////////////////////////
    AttachmentType Type () const;

    template <class T> const T* AttachmentData    () const;
    template <class T>       T* AttachmentData    ();
    template <class T>       T  GetAttachmentData () const;   //throws exception in case of type mismatch

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Attachment&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper method for AttachmentData method implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
    const PointAttachmentData* TypeSpecificData (xtl::type<PointAttachmentData>) const;

  protected:
    Attachment (AttachmentImpl*);

  private:
    AttachmentImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Attachment&, Attachment&);

#include <media/spine/detail/attachment.inl>

}

}

#endif
