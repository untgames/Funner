#include "shared.h"

using namespace media::spine;
using namespace script;
using namespace xtl;

namespace components
{

namespace media_spine_script_bind
{

/*
    Constants (libraries names)
*/

const char* ANIMATION_EVENT_LIBRARY       = "Media.SpineAnimationEvent";
const char* ANIMATION_STATE_DATA_LIBRARY  = "Media.SpineAnimationStateData";
const char* ANIMATION_STATE_LIBRARY       = "Media.SpineAnimationState";
const char* ATTACHMENT_LIBRARY            = "Media.SpineAttachment";
const char* ATTACHMENT_TYPE_LIBRARY       = "Media.SpineAttachmentType";
const char* BONE_LIBRARY                  = "Media.SpineBone";
const char* MIX_BLEND_LIBRARY             = "Media.SpineMixBlend";
const char* POINT_ATTACHMENT_DATA_LIBRARY = "Media.PointAttachmentData";
const char* SKELETON_LIBRARY              = "Media.SpineSkeleton";
const char* SKELETON_DATA_LIBRARY         = "Media.SpineSkeletonData";
const char* SLOT_LIBRARY                  = "Media.SpineSlot";
const char* SPINE_LIBRARY                 = "Media.Spine";
const char* TRACK_ENTRY_LIBRARY           = "Media.SpineTrackEntry";
const char* BINDER_NAME                   = "MediaSpine";
const char* COMPONENT_NAME                = "script.binds.MediaSpine";

///Register library to work with AnimationStateData
void bind_animation_state_data_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (ANIMATION_STATE_DATA_LIBRARY);

    //Register operations

  lib.Register ("get_DefaultMix", make_invoker (&AnimationStateData::DefaultMix));
  lib.Register ("set_DefaultMix", make_invoker (&AnimationStateData::SetDefaultMix));

  lib.Register ("CreateAnimationState", make_invoker (&AnimationStateData::CreateAnimationState));
  lib.Register ("GetMix",               make_invoker (&AnimationStateData::GetMix));
  lib.Register ("SetMix",               make_invoker (&AnimationStateData::SetMix));

  environment.RegisterType<AnimationStateData> (ANIMATION_STATE_DATA_LIBRARY);
}

///Register library to work with AnimationState
void bind_animation_state_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (ANIMATION_STATE_LIBRARY);

    //Register operations

  lib.Register ("get_TracksCount", make_invoker (&AnimationState::TracksCount));

  lib.Register ("SetAnimation",          make_invoker (&AnimationState::SetAnimation));
  lib.Register ("SetEmptyAnimation",     make_invoker (&AnimationState::SetEmptyAnimation));
  lib.Register ("SetEmptyAnimations",    make_invoker (&AnimationState::SetEmptyAnimations));
  lib.Register ("EnqueueAnimation",      make_invoker (&AnimationState::EnqueueAnimation));
  lib.Register ("EnqueueEmptyAnimation", make_invoker (&AnimationState::EnqueueEmptyAnimation));
  lib.Register ("ClearTrack",            make_invoker (&AnimationState::ClearTrack));
  lib.Register ("ClearTracks",           make_invoker (&AnimationState::ClearTracks));
  lib.Register ("HasTrackEntry",         make_invoker (&AnimationState::HasTrackEntry));
  lib.Register ("TrackEntry",            make_invoker (&AnimationState::TrackEntry));
  lib.Register ("Update",                make_invoker (&AnimationState::Update));

  lib.Register ("RegisterAnimationEventHandler", make_invoker (xtl::implicit_cast<xtl::connection (AnimationState::*)(AnimationEvent, const AnimationEventHandler&) const> (&AnimationState::RegisterEventHandler)));
  lib.Register ("RegisterUserEventHandler",      make_invoker (xtl::implicit_cast<xtl::connection (AnimationState::*)(const UserEventHandler&) const> (&AnimationState::RegisterEventHandler)));

  environment.RegisterType<AnimationState> (ANIMATION_STATE_LIBRARY);
}

///Register library to work with Attachment
void bind_attachment_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (ATTACHMENT_LIBRARY);

    //Register operations

  lib.Register ("get_Name",                make_invoker (&Attachment::Name));
  lib.Register ("get_Type",                make_invoker (&Attachment::Type));
  lib.Register ("get_PointAttachmentData", make_invoker (xtl::implicit_cast<PointAttachmentData* (Attachment::*) ()> (&Attachment::AttachmentData<PointAttachmentData>)));

  environment.RegisterType<Attachment> (ATTACHMENT_LIBRARY);
}

///Register library to work with Bone
void bind_bone_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (BONE_LIBRARY);

    //Register operations

  lib.Register ("get_Name",          make_invoker (&Bone::Name));
  lib.Register ("get_Position",      make_invoker (&Bone::Position));
  lib.Register ("get_Rotation",      make_invoker (&Bone::Rotation));
  lib.Register ("get_Scale",         make_invoker (&Bone::Scale));
  lib.Register ("get_Shear",         make_invoker (&Bone::Shear));
  lib.Register ("get_WorldPosition", make_invoker (&Bone::WorldPosition));
  lib.Register ("get_WorldRotation", make_invoker (&Bone::WorldRotation));
  lib.Register ("get_WorldScale",    make_invoker (&Bone::WorldScale));
  lib.Register ("get_Parent",        make_invoker (xtl::implicit_cast<Bone* (Bone::*) ()> (&Bone::Parent)));
  lib.Register ("get_ChildrenCount", make_invoker (&Bone::ChildrenCount));

  lib.Register ("SetPosition",          make_invoker (make_invoker (xtl::implicit_cast<void (Bone::*) (float, float)> (&Bone::SetPosition)),
                                                      make_invoker (xtl::implicit_cast<void (Bone::*) (const math::vec2f&)> (&Bone::SetPosition))));
  lib.Register ("SetRotation",          make_invoker (&Bone::SetRotation));
  lib.Register ("SetScale",             make_invoker (make_invoker (xtl::implicit_cast<void (Bone::*) (float, float)> (&Bone::SetScale)),
                                                      make_invoker (xtl::implicit_cast<void (Bone::*) (const math::vec2f&)> (&Bone::SetScale))));
  lib.Register ("SetShear",             make_invoker (make_invoker (xtl::implicit_cast<void (Bone::*) (float, float)> (&Bone::SetShear)),
                                                      make_invoker (xtl::implicit_cast<void (Bone::*) (const math::vec2f&)> (&Bone::SetShear))));
  lib.Register ("PositionWorldToLocal", make_invoker (make_invoker (xtl::implicit_cast<math::vec2f (Bone::*) (float, float) const> (&Bone::WorldToLocal)),
                                                      make_invoker (xtl::implicit_cast<math::vec2f (Bone::*) (const math::vec2f&) const> (&Bone::WorldToLocal))));
  lib.Register ("RotationWorldToLocal", make_invoker (xtl::implicit_cast<float (Bone::*) (float) const> (&Bone::WorldToLocal)));
  lib.Register ("PositionLocalToWorld", make_invoker (make_invoker (xtl::implicit_cast<math::vec2f (Bone::*) (float, float) const> (&Bone::LocalToWorld)),
                                                      make_invoker (xtl::implicit_cast<math::vec2f (Bone::*) (const math::vec2f&) const> (&Bone::LocalToWorld))));
  lib.Register ("RotationLocalToWorld", make_invoker (xtl::implicit_cast<float (Bone::*) (float) const> (&Bone::LocalToWorld)));
  lib.Register ("Child",                make_invoker (xtl::implicit_cast<Bone& (Bone::*) (unsigned int)> (&Bone::Child)));

  environment.RegisterType<Bone> (BONE_LIBRARY);
}

///Register library to work with PointAttachmentData
void bind_point_attachment_data_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (POINT_ATTACHMENT_DATA_LIBRARY);

    //Register operations

  lib.Register ("get_Position", make_invoker (&PointAttachmentData::Position));
  lib.Register ("get_Rotation", make_invoker (&PointAttachmentData::Rotation));
  lib.Register ("get_Color",    make_invoker (&PointAttachmentData::Color));

  environment.RegisterType<PointAttachmentData> (POINT_ATTACHMENT_DATA_LIBRARY);
}

//Skeleton data functions
SkeletonData create_skeleton_data ()
{
  return SkeletonData ();
}

SkeletonData load_skeleton_data (const char* skeleton_file_name, const char* atlas_file_name)
{
  return SkeletonData (skeleton_file_name, atlas_file_name);
}


///Register library to work with SkeletonData
void bind_skeleton_data_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SKELETON_DATA_LIBRARY);

    //Register create functions

  lib.Register ("Create", make_invoker (make_invoker (&load_skeleton_data),
                                        make_invoker (&create_skeleton_data)));

    //Register operations

  lib.Register ("get_Width",           make_invoker (&SkeletonData::Width));
  lib.Register ("get_Height",          make_invoker (&SkeletonData::Height));
  lib.Register ("get_AnimationsCount", make_invoker (&SkeletonData::AnimationsCount));
  lib.Register ("get_SkinsCount",      make_invoker (&SkeletonData::SkinsCount));
  lib.Register ("get_DefaultSkinName", make_invoker (&SkeletonData::DefaultSkinName));

  lib.Register ("Load",                     make_invoker (&SkeletonData::Load));
  lib.Register ("CreateSkeleton",           make_invoker (&SkeletonData::CreateSkeleton));
  lib.Register ("CreateAnimationStateData", make_invoker (&SkeletonData::CreateAnimationStateData));
  lib.Register ("AnimationName",            make_invoker (&SkeletonData::AnimationName));
  lib.Register ("AnimationDuration",        make_invoker (&SkeletonData::AnimationDuration));
  lib.Register ("SkinName",                 make_invoker (&SkeletonData::SkinName));

  environment.RegisterType<SkeletonData> (SKELETON_DATA_LIBRARY);
}

///Register library to work with Skeleton
void bind_skeleton_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SKELETON_LIBRARY);

    //Register operations

  lib.Register ("get_RootBone",    make_invoker (implicit_cast<Bone& (Skeleton::*) ()> (&Skeleton::RootBone)));
  lib.Register ("get_SlotsCount",  make_invoker (&Skeleton::SlotsCount));
  lib.Register ("get_Skin",        make_invoker (&Skeleton::Skin));

  lib.Register ("SetToSetupPose",       make_invoker (&Skeleton::SetToSetupPose));
  lib.Register ("Apply",                make_invoker (&Skeleton::Apply));
  lib.Register ("FindBone",             make_invoker (implicit_cast<Bone* (Skeleton::*) (const char*)> (&Skeleton::FindBone)));
  lib.Register ("FindSlot",             make_invoker (implicit_cast<Slot* (Skeleton::*) (const char*)> (&Skeleton::FindSlot)));
  lib.Register ("SetSkin",              make_invoker (make_invoker (&Skeleton::SetSkin),
                                                      make_invoker<bool (Skeleton&)> (xtl::bind (&Skeleton::SetSkin, _1, (const char*)0))));
  lib.Register ("SetAttachment",        make_invoker (make_invoker (&Skeleton::SetAttachment),
                                                      make_invoker<bool (Skeleton&, const char*)> (xtl::bind (&Skeleton::SetAttachment, _1, _2, (const char*)0))));
  lib.Register ("UpdateWorldTransform", make_invoker (&Skeleton::UpdateWorldTransform));

  environment.RegisterType<Skeleton> (SKELETON_LIBRARY);
}

///Register library to work with Slot
void bind_slot_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SLOT_LIBRARY);

    //Register operations

  lib.Register ("get_Name",       make_invoker (&Slot::Name));
  lib.Register ("get_Color",      make_invoker (&Slot::Color));
  lib.Register ("get_Bone",       make_invoker (&Slot::Bone));
  lib.Register ("get_Attachment", make_invoker (xtl::implicit_cast<Attachment* (Slot::*) ()> (&Slot::Attachment)));

  environment.RegisterType<Slot> (SLOT_LIBRARY);
}

///Register library with static spine data/functions
void bind_static_spine_library (Environment& environment)
{
  InvokerRegistry animation_event_lib = environment.CreateLibrary (ANIMATION_EVENT_LIBRARY);
  InvokerRegistry attachment_type_lib = environment.CreateLibrary (ATTACHMENT_TYPE_LIBRARY);
  InvokerRegistry mix_blend_lib       = environment.CreateLibrary (MIX_BLEND_LIBRARY);
  InvokerRegistry spine_lib           = environment.CreateLibrary (SPINE_LIBRARY);

  animation_event_lib.Register ("get_Start",     make_const (AnimationEvent_Start));
  animation_event_lib.Register ("get_Interrupt", make_const (AnimationEvent_Interrupt));
  animation_event_lib.Register ("get_End",       make_const (AnimationEvent_End));
  animation_event_lib.Register ("get_Complete",  make_const (AnimationEvent_Complete));
  animation_event_lib.Register ("get_Dispose",   make_const (AnimationEvent_Dispose));

  attachment_type_lib.Register ("get_Unknown",     make_const (AttachmentType_Unknown));
  attachment_type_lib.Register ("get_Region",      make_const (AttachmentType_Region));
  attachment_type_lib.Register ("get_BoundingBox", make_const (AttachmentType_BoundingBox));
  attachment_type_lib.Register ("get_Mesh",        make_const (AttachmentType_Mesh));
  attachment_type_lib.Register ("get_LinkedMesh",  make_const (AttachmentType_LinkedMesh));
  attachment_type_lib.Register ("get_Path",        make_const (AttachmentType_Path));
  attachment_type_lib.Register ("get_Point",       make_const (AttachmentType_Point));
  attachment_type_lib.Register ("get_Clipping",    make_const (AttachmentType_Clipping));

  mix_blend_lib.Register ("get_Setup",   make_const (MixBlend_Setup));
  mix_blend_lib.Register ("get_First",   make_const (MixBlend_First));
  mix_blend_lib.Register ("get_Replace", make_const (MixBlend_Replace));
  mix_blend_lib.Register ("get_Add",     make_const (MixBlend_Add));

  spine_lib.Register ("CreateAnimationEventHandler", make_callback_invoker<AnimationEventHandler::signature_type> ());
  spine_lib.Register ("CreateUserEventHandler",      make_callback_invoker<UserEventHandler::signature_type> ());

  environment.RegisterType<AnimationEvent> (ANIMATION_EVENT_LIBRARY);
}

///Register library to work with TrackEntry
void bind_track_entry_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (TRACK_ENTRY_LIBRARY);

    //Register operations

  lib.Register ("get_Animation",      make_invoker (&TrackEntry::Animation));
  lib.Register ("get_Duration",       make_invoker (&TrackEntry::Duration));
  lib.Register ("get_TrackIndex",     make_invoker (&TrackEntry::TrackIndex));
  lib.Register ("get_Alpha",          make_invoker (&TrackEntry::Alpha));
  lib.Register ("set_Alpha",          make_invoker (&TrackEntry::SetAlpha));
  lib.Register ("get_AnimationEnd",   make_invoker (&TrackEntry::AnimationEnd));
  lib.Register ("set_AnimationEnd",   make_invoker (&TrackEntry::SetAnimationEnd));
  lib.Register ("get_AnimationLast",  make_invoker (&TrackEntry::AnimationLast));
  lib.Register ("set_AnimationLast",  make_invoker (&TrackEntry::SetAnimationLast));
  lib.Register ("get_AnimationStart", make_invoker (&TrackEntry::AnimationStart));
  lib.Register ("set_AnimationStart", make_invoker (&TrackEntry::SetAnimationStart));
  lib.Register ("get_Delay",          make_invoker (&TrackEntry::Delay));
  lib.Register ("set_Delay",          make_invoker (&TrackEntry::SetDelay));
  lib.Register ("get_IsLooping",      make_invoker (&TrackEntry::IsLooping));
  lib.Register ("set_IsLooping",      make_invoker (&TrackEntry::SetLooping));
  lib.Register ("get_TimeScale",      make_invoker (&TrackEntry::TimeScale));
  lib.Register ("set_TimeScale",      make_invoker (&TrackEntry::SetTimeScale));
  lib.Register ("get_TrackTime",      make_invoker (&TrackEntry::TrackTime));
  lib.Register ("set_TrackTime",      make_invoker (&TrackEntry::SetTrackTime));
  lib.Register ("get_MixBlend",       make_invoker (&TrackEntry::MixBlend));
  lib.Register ("set_MixBlend",       make_invoker (&TrackEntry::SetMixBlend));
  lib.Register ("get_HoldPrevious",   make_invoker (&TrackEntry::HoldPrevious));
  lib.Register ("set_HoldPrevious",   make_invoker (&TrackEntry::SetHoldPrevious));
  lib.Register ("get_MixDuration",    make_invoker (&TrackEntry::MixDuration));
  lib.Register ("set_MixDuration",    make_invoker (&TrackEntry::SetMixDuration));
  lib.Register ("get_MixTime",        make_invoker (&TrackEntry::MixTime));
  lib.Register ("set_MixTime",        make_invoker (&TrackEntry::SetMixTime));
  lib.Register ("get_MixingFrom",     make_invoker (&TrackEntry::MixingFrom));
  lib.Register ("get_Next",           make_invoker (&TrackEntry::Next));
  lib.Register ("get_IsDisposed",     make_invoker (&TrackEntry::IsDisposed));

  lib.Register ("RegisterAnimationEventHandler", make_invoker (xtl::implicit_cast<xtl::connection (TrackEntry::*)(AnimationEvent, const AnimationEventHandler&) const> (&TrackEntry::RegisterEventHandler)));
  lib.Register ("RegisterUserEventHandler",      make_invoker (xtl::implicit_cast<xtl::connection (TrackEntry::*)(const UserEventHandler&) const> (&TrackEntry::RegisterEventHandler)));

  environment.RegisterType<TrackEntry> (TRACK_ENTRY_LIBRARY);
}

/*
    Component
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_animation_state_data_library  (environment);
      bind_animation_state_library       (environment);
      bind_attachment_library            (environment);
      bind_bone_library                  (environment);
      bind_point_attachment_data_library (environment);
      bind_skeleton_data_library         (environment);
      bind_skeleton_library              (environment);
      bind_slot_library                  (environment);
      bind_static_spine_library          (environment);
      bind_track_entry_library           (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> MediaSpineScriptBind (COMPONENT_NAME);

}

}

}
