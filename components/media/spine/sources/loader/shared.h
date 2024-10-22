#ifndef MEDIALIB_SPINE_LOADER_SHARED_HEADER
#define MEDIALIB_SPINE_LOADER_SHARED_HEADER

#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>
#include <spine/Atlas.h>
#include <spine/Attachment.h>
#include <spine/Bone.h>
#include <spine/MeshAttachment.h>
#include <spine/PointAttachment.h>
#include <spine/RegionAttachment.h>
#include <spine/Skeleton.h>
#include <spine/SkeletonBinary.h>
#include <spine/SkeletonClipping.h>
#include <spine/SkeletonJson.h>
#include <spine/Slot.h>

#include <stl/list>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/geometry/mesh.h>

#include "../shared/animation_state_data_impl.h"
#include "../shared/animation_state_impl.h"
#include "../shared/attachment_impl.h"
#include "../shared/bone_impl.h"
#include "../shared/material_impl.h"
#include "../shared/skeleton_clipping_impl.h"
#include "../shared/skeleton_data_impl.h"
#include "../shared/skeleton_impl.h"
#include "../shared/slot_impl.h"
#include "../shared/track_entry_impl.h"
#include "../shared/wrappers.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4250)
#endif

namespace media
{

namespace SPINE_NAMESPACE_NAME
{

//forward declarations
class AnimationStateSpineImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Base object class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Object: virtual public media::spine::IObject, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    Object () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Reference counting
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         AddRef  ()  { addref (this); }
    void         Release ()  { release (this); }
    unsigned int UseCount () { return (unsigned int)use_count (); }

  private:
    Object (const Object&); //no impl
    Object& operator = (const Object&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Holder for spine native object
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class SpineHandleHolder : public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (T*)> HandleDeleter;

    SpineHandleHolder (T* in_handle, HandleDeleter in_handle_deleter)
      : handle (in_handle)
      , handle_deleter (in_handle_deleter)
      {}

    ~SpineHandleHolder ()
    {
      if (handle)
        handle_deleter (handle);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get native object
///////////////////////////////////////////////////////////////////////////////////////////////////
    T* NativeHandle () { return handle; }

  private:
    T*            handle;
    HandleDeleter handle_deleter;
};

typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spAnimationState> >     SpineAnimationStatePtr;
typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spAnimationStateData> > SpineAnimationStateDataPtr;
typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spAtlas> >              SpineAtlasPtr;
typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spSkeleton> >           SpineSkeletonPtr;
typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spSkeletonClipping> >   SpineSkeletonClippingPtr;
typedef xtl::com_ptr<SpineHandleHolder< ::SPINE_NAMESPACE_NAME::spSkeletonData> >       SpineSkeletonDataPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Skeleton clipper implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonClippingSpineImpl : public Object, public media::spine::SkeletonClippingImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkeletonClippingSpineImpl (SpineSkeletonClippingPtr clipper);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Clipping
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsClipping ();
    void ClipStart  (media::spine::SlotImpl* slot, media::spine::AttachmentImpl* clip_attachment);
    void ClipEnd    (media::spine::SlotImpl* slot);                                                //ends clipping if this slot is a final clipping slot
    void ClipEnd    ();                                                                            //force ends clipping

    void            ClipTriangles         (float* vertices, int vertices_count, unsigned short* triangles, int triangles_count, float* uvs, int stride);
    unsigned int    ClippedVerticesCount  ();
    unsigned int    ClippedTrianglesCount ();
    const float*    ClippedVertices       ();
    const float*    ClippedTexcoords      ();
    const uint16_t* ClippedIndices        ();

  private:
    SpineSkeletonClippingPtr clipper; //clipper spine object
};

typedef xtl::com_ptr<SkeletonClippingSpineImpl> SkeletonClippingSpineImplPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Skeleton data implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonDataSpineImpl : public Object, public media::spine::SkeletonDataImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkeletonDataSpineImpl (SpineAtlasPtr atlas, SkeletonClippingSpineImplPtr clipper, SpineSkeletonDataPtr skeleton_data);

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get native handle
///////////////////////////////////////////////////////////////////////////////////////////////////
    ::SPINE_NAMESPACE_NAME::spSkeletonData* NativeHandle ();

  private:
    SpineAtlasPtr                atlas;          //skeleton data should hold atlas object
    SkeletonClippingSpineImplPtr clipper;        //clipper object shared between all spine objects
    SpineSkeletonDataPtr         skeleton_data;
};

typedef xtl::com_ptr<SkeletonDataSpineImpl> SkeletonDataSpineImplPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Skeleton implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonSpineImpl : public Object, public media::spine::SkeletonImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkeletonSpineImpl (SpineAtlasPtr atlas, SkeletonClippingSpineImplPtr clipper, SpineSkeletonDataPtr skeleton_data, SpineSkeletonPtr skeleton);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animating
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetToSetupPose      ();
    void ApplyAnimationState (media::spine::AnimationStateImpl* animation);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with bones
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int            BonesCount     ();
    media::spine::BoneImpl* CreateBoneImpl (unsigned int index);
    int                     RootBoneIndex  ();
    int                     FindBoneIndex  (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with slots
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int            SlotsCount             ();
    media::spine::SlotImpl* CreateSlotForDrawOrder (unsigned int index);
    media::spine::SlotImpl* CreateSlotImpl         (unsigned int index);
    int                     FindSlotIndex          (const char* name);
    int                     SlotForDrawOrder       (unsigned int index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Change skin (use 0 to set default skin)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Skin          ();
    bool        SetSkin       (const char* skin_name);
    bool        SetAttachment (const char* slot_name, const char* attachment_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update transform after animation/bones manipulation
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateWorldTransform ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::MaterialImpl*         CreateMaterialImpl (const char* material_name, const char* texture_path, media::spine::BlendMode blend_mode, media::spine::TexcoordWrap texcoord_wrap_u, media::spine::TexcoordWrap texcoord_wrap_v);
    media::spine::SkeletonClippingImpl* Clipper            ();

  private:
    typedef stl::hash_map< ::SPINE_NAMESPACE_NAME::spSlot*, int> SlotToIndexMap;

  private:
    SpineAtlasPtr                atlas;           //skeleton should hold atlas object
    SkeletonClippingSpineImplPtr clipper;         //clipper object shared between all spine objects
    SpineSkeletonDataPtr         skeleton_data;   //skeleton should hold skeleton data object
    SpineSkeletonPtr             skeleton;        //native skeleton object
    SlotToIndexMap               slots_index_map; //hash map for fast search of slot index by slot pointer
};

typedef xtl::com_ptr<SkeletonSpineImpl> SkeletonSpineImplPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Slot implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class SlotSpineImpl : public Object, public media::spine::SlotImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SlotSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineSkeletonPtr skeleton, ::SPINE_NAMESPACE_NAME::spSlot* slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec4f             Color          ();
    math::vec4f             DarkColor      ();
    media::spine::BlendMode BlendMode      ();
    bool                    HasBone        ();
    media::spine::BoneImpl* CreateBoneImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool                          HasAttachment        ();
    const char*                   AttachmentName       ();
    media::spine::AttachmentImpl* CreateAttachmentImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Native spine handle
///////////////////////////////////////////////////////////////////////////////////////////////////
    ::SPINE_NAMESPACE_NAME::spSlot* NativeHandle () { return slot; }

  private:
    SpineAtlasPtr                   atlas;          //slot should hold atlas object
    SpineSkeletonDataPtr            skeleton_data;  //slot should hold skeleton data object
    SpineSkeletonPtr                skeleton;       //slot should hold skeleton object
    ::SPINE_NAMESPACE_NAME::spSlot* slot;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Attachment implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class AttachmentSpineImpl : public Object, public media::spine::AttachmentImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    AttachmentSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineSkeletonPtr skeleton, ::SPINE_NAMESPACE_NAME::spAttachment* attachment);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                TexturePath   ();
    media::spine::TexcoordWrap TexcoordWrapU ();
    media::spine::TexcoordWrap TexcoordWrapV ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get Type and type-specific data.
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::AttachmentType Type ();

    media::spine::PointAttachmentDataImpl* CreatePointAttachmentDataImpl ();
    math::vec4f                            Color                         ();
    unsigned int                           VerticesCount                 ();
    unsigned int                           TrianglesCount                ();
    void                                   ComputeWorldVertices          (media::spine::SlotImpl* slot, float* vertices, int offset, int stride);
    const float*                           Texcoords                     ();
    const uint16_t*                        Indices                       ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Native spine handle
///////////////////////////////////////////////////////////////////////////////////////////////////
    ::SPINE_NAMESPACE_NAME::spAttachment* NativeHandle () { return attachment; }

  private:
    SpineAtlasPtr                         atlas;          //attachment should hold atlas object
    SpineSkeletonDataPtr                  skeleton_data;  //attachment should hold skeleton data object
    SpineSkeletonPtr                      skeleton;       //attachment should hold skeleton object
    ::SPINE_NAMESPACE_NAME::spAttachment* attachment;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Point attachment data implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class PointAttachmentDataSpineImpl : public Object, public media::spine::PointAttachmentDataImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    PointAttachmentDataSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineSkeletonPtr skeleton, ::SPINE_NAMESPACE_NAME::spPointAttachment* attachment);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Point attachment parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec2f Position ();
    float       Rotation ();
    math::vec4f Color    ();

  private:
    SpineAtlasPtr                              atlas;          //attachment should hold atlas object
    SpineSkeletonDataPtr                       skeleton_data;  //attachment should hold skeleton data object
    SpineSkeletonPtr                           skeleton;       //attachment should hold skeleton object
    ::SPINE_NAMESPACE_NAME::spPointAttachment* attachment;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Bone implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class BoneSpineImpl : public Object, public media::spine::BoneImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    BoneSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineSkeletonPtr skeleton, ::SPINE_NAMESPACE_NAME::spBone* bone);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Transform
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPosition (const math::vec2f& position);
    void        SetPosition (float position_x, float position_y);
    math::vec2f Position    ();
    void        SetRotation (float rotation);
    float       Rotation    ();
    void        SetScale    (const math::vec2f& scale);
    void        SetScale    (float scale_x, float scale_y);
    math::vec2f Scale       ();
    void        SetShear    (const math::vec2f& shear);
    void        SetShear    (float shear_x, float shear_y);
    math::vec2f Shear       ();

    math::vec2f WorldPosition ();
    math::vec2f WorldRotation ();
    math::vec2f WorldScale    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Helpers for converting between coordinate spaces
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec2f WorldToLocal (const math::vec2f& position);
    math::vec2f WorldToLocal (float position_x, float position_y);
    float       WorldToLocal (float rotation);
    math::vec2f LocalToWorld (const math::vec2f& position);
    math::vec2f LocalToWorld (float position_x, float position_y);
    float       LocalToWorld (float rotation);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Linked bones
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool                    HasParent        ();
    media::spine::BoneImpl* CreateParentImpl ();
    unsigned int            ChildrenCount    ();
    media::spine::BoneImpl* CreateChildImpl  (unsigned int index);

  private:
    SpineAtlasPtr                   atlas;          //bone should hold atlas object
    SpineSkeletonDataPtr            skeleton_data;  //bone should hold skeleton data object
    SpineSkeletonPtr                skeleton;       //bone should hold skeleton object
    ::SPINE_NAMESPACE_NAME::spBone* bone;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation state data implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationStateDataSpineImpl : public Object, public media::spine::AnimationStateDataImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationStateDataSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineAnimationStateDataPtr animation_state_data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Create object instance
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::AnimationStateImpl* CreateAnimationState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation mixing
///////////////////////////////////////////////////////////////////////////////////////////////////
    float DefaultMix    ();
    void  SetDefaultMix (float default_mix);

    float GetMix (const char* animation_from, const char* animation_to);
    void  SetMix (const char* animation_from, const char* animation_to, float mix);

  private:
    SpineAtlasPtr              atlas;                  //animation state data should hold atlas object
    SpineSkeletonDataPtr       skeleton_data;          //animation state data should hold skeleton data object
    SpineAnimationStateDataPtr animation_state_data;
};

typedef xtl::com_ptr<AnimationStateDataSpineImpl> AnimationStateDataSpineImplPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Track entry implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class TrackEntrySpineImpl : public Object, public media::spine::TrackEntryImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    TrackEntrySpineImpl (AnimationStateSpineImpl* animation_state_impl, ::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry);
    ~TrackEntrySpineImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*  Animation  ();
    float        Duration   ();
    unsigned int TrackIndex ();

    float       Alpha             ();
    void        SetAlpha          (float alpha);
    float       AnimationEnd      ();
    void        SetAnimationEnd   (float animation_end);
    float       AnimationLast     ();
    void        SetAnimationLast  (float animation_last);
    float       AnimationStart    ();
    void        SetAnimationStart (float animation_start);
    float       Delay             ();
    void        SetDelay          (float delay);
    bool        IsLooping         ();
    void        SetLooping        (bool is_looping);
    float       TimeScale         ();
    void        SetTimeScale      (float time_scale);
    float       TrackTime         ();
    void        SetTrackTime      (float track_time);

    //tracks blending (available for spine 3.8 or newer)
    media::spine::MixBlend MixBlend    ();
    void                   SetMixBlend (media::spine::MixBlend mix_blend);
    bool                   HoldPrevious    ();
    void                   SetHoldPrevious (bool hold);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Mixing parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    float                     MixDuration    ();
    void                      SetMixDuration (float mix_duration);
    float                     MixTime        ();
    void                      SetMixTime     (float mix_time);
    media::spine::TrackEntry* MixingFrom     ();
    media::spine::TrackEntry* Next           ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Check if track is already disposed (if track disposed, all other read calls returns default values and set calls are ignored)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsDisposed ();
    void Dispose ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get track entry object pointer
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::TrackEntry& ThisTrack ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Native spine handle
///////////////////////////////////////////////////////////////////////////////////////////////////
    ::SPINE_NAMESPACE_NAME::spTrackEntry* NativeHandle ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckDisposed (const char* method_name);

  private:
    AnimationStateSpineImpl*              animation_state_impl;  //parent animation state impl
    media::spine::TrackEntry*             this_track_entry;      //pointer to TrackEntry object which is hold in animation_state_impl
    ::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry;           //track entry
    bool                                  is_disposed;           //is this track disposed
};

typedef xtl::com_ptr<TrackEntrySpineImpl> TrackEntrySpineImplPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation state implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationStateSpineImpl : public Object, public media::spine::AnimationStateImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationStateSpineImpl (SpineAtlasPtr atlas, SpineSkeletonDataPtr skeleton_data, SpineAnimationStateDataPtr animation_state_data, SpineAnimationStatePtr animation_state);
    ~AnimationStateSpineImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with tracks
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::TrackEntryImpl* SetAnimation            (unsigned int track, const char* animation, bool looped);
    media::spine::TrackEntryImpl* SetEmptyAnimation       (unsigned int track, float mix_duration);
    void                          SetEmptyAnimations      (float mix_duration);
    media::spine::TrackEntryImpl* EnqueueAnimation        (unsigned int track, const char* animation, bool looped, float delay);
    media::spine::TrackEntryImpl* EnqueueEmptyAnimation   (unsigned int track, float mix_duration, float delay);
    unsigned int                  TracksCount             ();
    void                          ClearTracks             ();
    void                          ClearTrack              (unsigned int track);
    bool                          HasTrackEntry           (unsigned int track);
    media::spine::TrackEntryImpl* TrackEntry              (unsigned int track);
    media::spine::TrackEntry*     GetActiveTrackEntry     (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry);
    void                          DisposeTrack            (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update (only positive dt allowed)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (float dt);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Native handle
///////////////////////////////////////////////////////////////////////////////////////////////////
    ::SPINE_NAMESPACE_NAME::spAnimationState* NativeHandle ();

  private:
    typedef xtl::shared_ptr<media::spine::TrackEntry> TrackEntryPtr;

    struct TrackEntryDesc
    {
      TrackEntryPtr          track_entry;
      TrackEntrySpineImplPtr track_entry_impl;

      TrackEntryDesc (TrackEntryPtr in_track_entry, TrackEntrySpineImplPtr in_track_entry_impl)
        : track_entry (in_track_entry)
        , track_entry_impl (in_track_entry_impl)
        {}
    };

    typedef stl::list<TrackEntryDesc> TrackEntryList;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::TrackEntryImpl* GetActiveTrackEntryImplAddRef (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry);
    TrackEntryList::iterator      GetActiveTrack                (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry);

  private:
    SpineAtlasPtr              atlas;                  //animation state should hold atlas object
    SpineSkeletonDataPtr       skeleton_data;          //animation state should hold skeleton data object
    SpineAnimationStateDataPtr animation_state_data;   //animation state should hold animation state data object
    SpineAnimationStatePtr     animation_state;
    TrackEntryList             active_tracks;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Material implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialSpineImpl : public Object, public media::spine::MaterialImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialSpineImpl (const char* name, const char* texture_path, media::spine::BlendMode blend_mode, media::spine::TexcoordWrap texcoord_wrap_u, media::spine::TexcoordWrap texcoord_wrap_v)
      : media::spine::MaterialImpl (name, texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v)
      {}
};

}

}

#endif
