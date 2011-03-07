#ifndef PHYSICS_MANAGER_SCENE_HEADER
#define PHYSICS_MANAGER_SCENE_HEADER

namespace render
{

namespace debug
{

///forward declarations
class IPrimitiveRender;

}

}

namespace physics
{

///implementation forwards
class SceneImpl;

typedef xtl::function<void (CollisionEventType event, RigidBody& first_body, RigidBody& second_body, const math::vec3f& collision_point)> CollisionCallback;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene
{
  public:
    Scene  (const Scene&);
    ~Scene ();
    
    Scene& operator = (const Scene&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody CreateBody (const char* name);
    RigidBody CreateBody (const Shape& shape, float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Joint CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2);
    Joint CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees = 360.0f, float twist_limit = 360.0f);
    Joint CreateHingeJoint     (const JointBind& bind1, const JointBind& bind2);
    Joint CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& Gravity    () const;
    void               SetGravity (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SimulationStep    () const;
    void  SetSimulationStep (float step);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PerformSimulation (float dt);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ��������. ������� ������������, ����:
/// - �� ����� ������;
/// - ����� ��������� ������ � collides = true;
/// - �������� ������ ���������� true � collides = true;
/// - �������� ������ ���������� false � collides = false.
///��������� ����������: ��� ����� �������� ������, ��� ���� ��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<bool (const RigidBody&, const RigidBody&)> BroadphaseCollisionFilter;

    size_t AddCollisionFilter        (const char* group1_mask, const char* group2_mask, bool collides, const BroadphaseCollisionFilter& filter = BroadphaseCollisionFilter ());
    void   RemoveCollisionFilter     (size_t id);
    void   RemoveAllCollisionFilters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler); 
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw (render::debug::IPrimitiveRender&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Scene&);
  
  protected:
    Scene (SceneImpl*);

  private:
    SceneImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Scene&, Scene&);

}

#endif
