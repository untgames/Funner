#include "shared.h"

int main ()
{
  printf ("Results of constraints_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr body1_static  (scene->CreateRigidBody (sphere_shape.get (), 0), false),
                 body2_static  (scene->CreateRigidBody (sphere_shape.get (), 0), false),
                 body3_static  (scene->CreateRigidBody (sphere_shape.get (), 0), false),
                 body4_static  (scene->CreateRigidBody (sphere_shape.get (), 0), false),
                 body1_dynamic (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body2_dynamic (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body3_dynamic (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body4_dynamic (scene->CreateRigidBody (sphere_shape.get (), 1), false);

    Transform body_transform;

    body_transform.position.x = -5;
    body_transform.position.y = 0;
    body1_static->SetWorldTransform (body_transform);
    body_transform.position.y = -2;
    body1_dynamic->SetWorldTransform (body_transform);

    body_transform.position.x = -3;
    body_transform.position.y = 0;
    body2_static->SetWorldTransform (body_transform);
    body_transform.position.y = -2;
    body2_dynamic->SetWorldTransform (body_transform);

    body_transform.position.x = -1;
    body_transform.position.y = 0;
    body3_static->SetWorldTransform (body_transform);
    body_transform.position.y = -2;
    body3_dynamic->SetWorldTransform (body_transform);

    body_transform.position.x = 1;
    body_transform.position.y = 0;
    body4_static->SetWorldTransform (body_transform);
    body_transform.position.y = -2;
    body4_dynamic->SetWorldTransform (body_transform);

    printf ("body1_static state:\n");
    dump_body_position (body1_static.get ());
    printf ("body1_dynamic state:\n");
    dump_body_position (body1_dynamic.get ());
    printf ("body2_static state:\n");
    dump_body_position (body2_static.get ());
    printf ("body2_dynamic state:\n");
    dump_body_position (body2_dynamic.get ());
    printf ("body3_static state:\n");
    dump_body_position (body3_static.get ());
    printf ("body3_dynamic state:\n");
    dump_body_position (body3_dynamic.get ());
    printf ("body4_static state:\n");
    dump_body_position (body4_static.get ());
    printf ("body4_dynamic state:\n");
    dump_body_position (body4_dynamic.get ());

    ConeTwistJointDesc joint_desc;

    joint_desc.anchor [0]  = math::vec3f (0, -2, 0);
    joint_desc.anchor [1]  = math::vec3f (0, 2, 0);
    joint_desc.axis [0]    = math::vec3f (0, -1, 0);
    joint_desc.axis [1]    = math::vec3f (0, 1, 0);
    joint_desc.swing_limit = 30;
    joint_desc.twist_limit = 30;

    JointPtr spherical_joint  = JointPtr (scene->CreateSphericalJoint (body1_static.get (), body1_dynamic.get (), *(SphericalJointDesc*)&joint_desc), false),
             cone_twist_joint = JointPtr (scene->CreateConeTwistJoint (body1_static.get (), body1_dynamic.get (), joint_desc), false),
             hinge_joint      = JointPtr (scene->CreateHingeJoint     (body1_static.get (), body1_dynamic.get (), *(HingeJointDesc*)&joint_desc), false),
             prismatic_joint  = JointPtr (scene->CreatePrismaticJoint (body1_static.get (), body1_dynamic.get (), *(PrismaticJointDesc*)&joint_desc), false);

    scene->PerformSimulation (1.f);

    printf ("Performing one second simulation and adding joints\n");

    printf ("body1_static state:\n");
    dump_body_position (body1_static.get ());
    printf ("body1_dynamic state:\n");
    dump_body_position (body1_dynamic.get ());
    printf ("body2_static state:\n");
    dump_body_position (body2_static.get ());
    printf ("body2_dynamic state:\n");
    dump_body_position (body2_dynamic.get ());
    printf ("body3_static state:\n");
    dump_body_position (body3_static.get ());
    printf ("body3_dynamic state:\n");
    dump_body_position (body3_dynamic.get ());
    printf ("body4_static state:\n");
    dump_body_position (body4_static.get ());
    printf ("body4_dynamic state:\n");
    dump_body_position (body4_dynamic.get ());

    printf ("spherical joint objects count = %u\n", spherical_joint->ObjectsCount ());

    body1_static = 0;

    printf ("spherical joint objects count after deleting body1_static = %u\n", spherical_joint->ObjectsCount ());

    scene->PerformSimulation (1.f);

    printf ("Performing one second simulation\n");

    printf ("body1_dynamic state:\n");
    dump_body_position (body1_dynamic.get ());
    printf ("body2_static state:\n");
    dump_body_position (body2_static.get ());
    printf ("body2_dynamic state:\n");
    dump_body_position (body2_dynamic.get ());
    printf ("body3_static state:\n");
    dump_body_position (body3_static.get ());
    printf ("body3_dynamic state:\n");
    dump_body_position (body3_dynamic.get ());
    printf ("body4_static state:\n");
    dump_body_position (body4_static.get ());
    printf ("body4_dynamic state:\n");
    dump_body_position (body4_dynamic.get ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
