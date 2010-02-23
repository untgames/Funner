#include "shared.h"

namespace
{

const float POSITION_DELAY_FACTOR = 0.25f;

}

struct FollowNode::Impl: public xtl::reference_counter
{
  Node& source;
  Node& follower;

  Impl (Node& in_source, Node& in_follower)
    : source (in_source)
    , follower (in_follower)
  {
  }
  
  void Update (float dt)
  { 
    math::vec3f current_position = follower.WorldPosition (),
                target_position  = source.WorldPosition (),
                direction        = target_position - current_position;

    follower.SetWorldPosition (current_position + direction * POSITION_DELAY_FACTOR);
    follower.LookTo           (target_position, NodeOrt_Z, NodeOrt_Y, NodeTransformSpace_World);
  }
};

FollowNode::FollowNode (Node& source, Node& follower)
  : impl (new Impl (source, follower))
{
}

FollowNode::FollowNode (const FollowNode& fn)
  : impl (fn.impl)
{
  addref (impl);
}

FollowNode::~FollowNode ()
{
  release (impl);
}

void FollowNode::operator () (float dt)
{
  impl->Update (dt);
}
