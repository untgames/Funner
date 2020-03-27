#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

///Spine animation controller which animates all child nodes of SpineSkeleton type
struct SpineAnimation::Impl: public xtl::trackable, public xtl::instance_counter<SpineAnimation>
{
  typedef stl::list<SpineSkeleton::Pointer> SkeletonsList;

  media::spine::AnimationState animation_state;   //animation state
  SkeletonsList                skeletons;         //skeletons to apply animation to

  Impl (Node& root_node, const media::spine::AnimationState& in_animation_state)
    : animation_state (in_animation_state)
  {
    struct SpineSkeletonVisitor: public xtl::visitor<void, SpineSkeleton>
    {
      SpineSkeletonVisitor (SkeletonsList& in_skeletons)
        : skeletons_to_fill (in_skeletons)
        {}

      void visit (SpineSkeleton& skeleton)
      {
        skeletons_to_fill.push_back (&skeleton);
      }

      SkeletonsList& skeletons_to_fill;
    };

    SpineSkeletonVisitor visitor (skeletons);

    root_node.VisitEach (visitor, NodeTraverseMode_Default);

    connect_tracker (root_node.RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&Impl::BindNode, this, _2)));
    connect_tracker (root_node.RegisterEventHandler (NodeSubTreeEvent_BeforeUnbind, xtl::bind (&Impl::UnbindNode, this, _2)));
  }

  void Update (const TimeValue& dt)
  {
    animation_state.Update (dt.cast<float> ());

    for (SkeletonsList::iterator iter = skeletons.begin (), end = skeletons.end (); iter != end; ++iter)
    {
      (*iter)->Skeleton ().Apply (animation_state);
      (*iter)->Skeleton ().UpdateWorldTransform ();

      (*iter)->SetVisualStructureDirty (true);
    }
  }

  ///Subtree node attached event
  void BindNode (Node& child)
  {
    if (scene_graph::SpineSkeleton* skeleton = dynamic_cast<scene_graph::SpineSkeleton*> (&child))
      skeletons.push_back (skeleton);
  }

  ///Subtree node detached event
  void UnbindNode (Node& child)
  {
    //check if unbinded node was of skeleton type, so we do not have to search for it in our list in case of node is of other type
    if (!dynamic_cast<scene_graph::SpineSkeleton*> (&child))
      return;

    for (SkeletonsList::iterator iter = skeletons.begin (), end = skeletons.end (); iter != end; ++iter)
    {
      if (iter->get () == &child)
      {
        skeletons.erase (iter);
        return;
      }
    }
  }
};

///Constructor / destructor
SpineAnimation::SpineAnimation (Node& root_node, const media::spine::AnimationState& animation_state)
  : Controller (root_node, ControllerTimeMode_Delta)
  , impl (new Impl (root_node, animation_state))
  {}

SpineAnimation::~SpineAnimation ()
{
}

/*
   Create controller
*/

SpineAnimation::Pointer SpineAnimation::Create (Node& root_node, const media::spine::AnimationState& animation_state)
{
  return Pointer (new SpineAnimation (root_node, animation_state), false);
}

/*
   Get animation state
*/

const media::spine::AnimationState& SpineAnimation::AnimationState () const
{
  return impl->animation_state;
}

/*
    Update
*/

void SpineAnimation::Update (const TimeValue& dt)
{
  impl->Update (dt);
}
