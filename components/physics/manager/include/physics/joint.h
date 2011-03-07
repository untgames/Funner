#ifndef PHYSICS_MANAGER_JOINT_HEADER
#define PHYSICS_MANAGER_JOINT_HEADER

namespace physics
{

//implementation forwards
class JointImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
struct JointBind
{
  RigidBody   body;
  math::vec3f anchor;
  math::vec3f axis;
  
   ///+constructors
   ///+methods
};

class Joint
{
  public:
    Joint (const Joint&);
    ~Joint ();
    
    Joint& operator = (const Joint&);
    
    size_t BodiesCount () const;
    
    const RigidBody& Body (size_t index) const;
          RigidBody& Body (size_t index);

    void Swap (Joint&);

  protected:
    Joint (JointImpl*);
  
  private:
    JointImpl* impl;
};

void swap (Joint&, Joint&);

}

#endif
