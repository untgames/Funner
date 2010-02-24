#include "shared.h"

namespace
{

const float  PHYS_TARGET_REACH_DELAY  = 4.0f;
const float  PHYS_ANGLE_REACH_DELAY   = 1.5f;
const float  MIN_ATTACK_DIRECTION     = 20.0f;
const size_t STATE_MAX_DURATION       = 5000;
const float  GUARD_DISTANCE           = 280.0f;
const float  MIN_ROTATION_ANGLE       = 5.0f;
const float  MIN_VELOCITY             = 120.0f;
const float  GUARD_FORCE_FACTOR       = 40.0f;
const float  MAX_SHOT_ANGLE           = 20.f;

const math::vec4f ENEMY_SHOT_COLOR (1.f, 0.4f, 0.f, 1.f);
const float       ENEMY_SHOT_DISTANCE = 1000.f;

enum State
{
  State_Attack,
  State_Guard
};

float crand ()
{
  return (float)((((double)rand () / (double)RAND_MAX) - 0.5) * 2.0);
}

math::vec3f get_guard_force (const math::vec3f& center_point, const math::vec3f& enemy_point)
{
  math::vec3f direction = enemy_point - center_point;
  float       distance  = length (direction);
  
  return -direction * GUARD_FORCE_FACTOR / distance / distance;
}

}

struct EnemyAi::Impl: public xtl::reference_counter
{
  Test&                           test;
  Node&                           enemy_node;
  physics::low_level::IRigidBody& enemy_body;  
  State                           state;
  size_t                          state_change_time;
  math::vec3f                     guard_target_point;
  
  Impl (Test& in_test, Node& in_node, physics::low_level::IRigidBody& in_body)
    : test (in_test) 
    , enemy_node (in_node)    
    , enemy_body (in_body)
    , state (State_Guard)
    , state_change_time (common::milliseconds ())
  {
  }  

  bool CheckAngle (const math::vec3f& current_ort, const math::vec3f& target_ort, const math::anglef& max_angle)
  {
    math::anglef rotation_angle = find_angle (current_ort, target_ort);

    return rotation_angle < max_angle;
  }
  
  bool CompensateDirections (const math::vec3f& current_ort, const math::vec3f& target_ort)
  {
    math::vec3f  rotation_axis  = cross (current_ort, target_ort);
    math::anglef rotation_angle = find_angle (current_ort, target_ort);
    math::quatf  rotation       = to_quat (rotation_angle, rotation_axis);    

    if (abs (rotation_angle) > math::degree (MIN_ROTATION_ANGLE))
    {
      math::anglef pitch, yaw, roll;
      
      to_euler_angles (rotation, pitch, yaw, roll);        

      math::vec3f target_angles (radian (pitch), radian (yaw), radian (roll)),
                  current_angles_speed = enemy_body.AngularVelocity (),
                  torque              = 2.0f * (target_angles - current_angles_speed * PHYS_ANGLE_REACH_DELAY) / PHYS_ANGLE_REACH_DELAY / PHYS_ANGLE_REACH_DELAY;

      enemy_body.AddTorque (torque);
      
      return false;
    }
    else return true;    
  }
};

EnemyAi::EnemyAi (Test& in_test, Node& in_node, physics::low_level::IRigidBody& in_body)
  : impl (new Impl (in_test, in_node, in_body))
{
}

EnemyAi::EnemyAi (const EnemyAi& ai)
  : impl (ai.impl)
{
  addref (impl);
}

EnemyAi::~EnemyAi ()
{
  release (impl);
}
  
void EnemyAi::operator () (float dt)
{
  physics::low_level::IRigidBody* target_body  = 0;
  float                           min_distance = 0;
  math::vec3f                     target_force;              

  for (SceneManager::NodesArray::iterator iter=impl->test.scene_manager.MainShips ().begin (), end=impl->test.scene_manager.MainShips ().end (); iter!=end; ++iter)
  {
    if (*iter == &impl->enemy_node)
      continue;
    
    Node&                           main_ship_node = **iter;
    physics::low_level::IRigidBody& main_ship_body = *impl->test.physics_bodies [&main_ship_node]->rigid_body;
    float                           distance       = length (main_ship_body.WorldTransform ().position - impl->enemy_body.WorldTransform ().position);
    
    if (!target_body || distance < min_distance)
    {
      distance    = min_distance;
      target_body = &main_ship_body;
    }
    
    target_force += get_guard_force (impl->enemy_body.WorldTransform ().position, main_ship_body.WorldTransform ().position);
  }
   
  for (SceneManager::NodesArray::iterator iter=impl->test.scene_manager.Shattles ().begin (), end=impl->test.scene_manager.Shattles ().end (); iter!=end; ++iter)
  {
    if (*iter == &impl->enemy_node)
      continue;
    
    Node&                           ship_node = **iter;
    physics::low_level::IRigidBody& ship_body = *impl->test.physics_bodies [&ship_node]->rigid_body;
//    float                           distance  = length (ship_body.WorldTransform ().position - impl->enemy_body.WorldTransform ().position);

    target_force += get_guard_force (impl->enemy_body.WorldTransform ().position, ship_body.WorldTransform ().position);    
  }  
  
  if (!target_body)
    return;
    
  math::vec3f target_position  = impl->state == State_Attack ? target_body->WorldTransform ().position : impl->guard_target_point,
              target_direction = target_position - impl->enemy_body.WorldTransform ().position;
              
  if (impl->state_change_time < common::milliseconds ())
  {
    if (length (target_direction) < MIN_ATTACK_DIRECTION)
    {
      impl->state              = State_Guard;
      impl->guard_target_point = target_position + to_quat (math::degree (crand () * 30.0f), math::degree (crand () * 30.0f), math::degree (crand () * 30.0f)) * normalize (-target_direction) * GUARD_DISTANCE;
    }
    else
    {
      impl->state = State_Attack;
    }

    impl->state_change_time = common::milliseconds () + rand () % STATE_MAX_DURATION;
  }

  if (impl->state == State_Guard)
    target_direction = impl->guard_target_point - impl->enemy_body.WorldTransform ().position;
    
  math::vec3f current_velocity    = impl->enemy_body.LinearVelocity (),
              target_acceleration = 2.0f * (target_direction - current_velocity * PHYS_TARGET_REACH_DELAY) / PHYS_TARGET_REACH_DELAY / PHYS_TARGET_REACH_DELAY;
              
  target_force += target_acceleration * impl->enemy_body.Mass ();

  float current_speed = length (current_velocity);
              
  if (current_speed < MIN_VELOCITY)
  {
    if (current_speed < 0.01f)
      current_velocity = math::vec3f (0, 0, 1);

    target_force += (normalize (current_velocity) * (MIN_VELOCITY - current_speed)) / PHYS_TARGET_REACH_DELAY;
  }

  impl->enemy_body.AddForce (target_force);      

  impl->CompensateDirections (impl->enemy_node.WorldOrtZ (), normalize (target_force));

  if (impl->CheckAngle (impl->enemy_node.WorldOrtZ (), normalize (target_direction), math::degree (MAX_SHOT_ANGLE)))
  {
    Node::Pointer gun = impl->enemy_node.FindChild (GUN_NODE_NAME);

/*    if (gun)
      impl->test.gfx_manager.PerformShot (*gun, ENEMY_SHOT_COLOR, ENEMY_SHOT_DISTANCE);*/
  }
//    CompensateDirections (enemy_node.WorldOrtY (), math::vec3f (0.0f, 1.0f, 0.0f));        
 
//  printf ("distance=%.3f\n", length (target_direction));
}
