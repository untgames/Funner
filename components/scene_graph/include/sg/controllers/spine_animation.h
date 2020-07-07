#ifndef SCENE_GRAPH_CONTROLLERS_SPINE_ANIMATION_HEADER
#define SCENE_GRAPH_CONTROLLERS_SPINE_ANIMATION_HEADER

#include <sg/controller.h>

namespace media
{

namespace spine
{

//forward declaration
class AnimationState;

}

}

namespace scene_graph
{

namespace controllers
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation controller which animates all child nodes of SpineSkeleton type
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpineAnimation: public Controller
{
  public:
    typedef xtl::com_ptr<SpineAnimation>       Pointer;
    typedef xtl::com_ptr<const SpineAnimation> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Create controller
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create (Node& root_node, const media::spine::AnimationState& animation_state);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Get animation state
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::spine::AnimationState& AnimationState () const;
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / Destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpineAnimation  (Node& root_node, const media::spine::AnimationState& animation_state);
    ~SpineAnimation ();
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Update
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const TimeValue& time);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

#endif