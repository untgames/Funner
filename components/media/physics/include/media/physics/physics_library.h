#ifndef MEDIA_PHYSICS_PHYSICS_LIBRARY_HEADER
#define MEDIA_PHYSICS_PHYSICS_LIBRARY_HEADER

#include <common/serializer_manager.h>

#include <media/physics/material.h>
#include <media/physics/rigid_body.h>
#include <media/physics/shape.h>
#include <media/physics/triangle_mesh.h>

namespace xtl
{

//forward decalration
template <class T> class iterator;

}

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������� 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class PhysicsLibraryCollection
{
  public:
    typedef T                         Item;
    typedef xtl::iterator<Item>       Iterator;
    typedef xtl::iterator<const Item> ConstIterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsLibraryCollection ();
    PhysicsLibraryCollection (const PhysicsLibraryCollection&);
    ~PhysicsLibraryCollection ();

    PhysicsLibraryCollection& operator = (const PhysicsLibraryCollection&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ���������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          Item* Find (const char* name);
    const Item* Find (const char* name) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach    (const char* id, Item&);
    void Detach    (const char* id);
    void DetachAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PhysicsLibraryCollection&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PhysicsLibrary
{
  public:
    typedef PhysicsLibraryCollection<RigidBody>    RigidBodyCollection;
    typedef PhysicsLibraryCollection<Material>     MaterialCollection;
    typedef PhysicsLibraryCollection<Shape>        ShapeCollection;
    typedef PhysicsLibraryCollection<TriangleMesh> TriangleMeshCollection;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsLibrary  ();
    PhysicsLibrary  (const PhysicsLibrary&);
    PhysicsLibrary  (const char* file_name);
    ~PhysicsLibrary ();

    PhysicsLibrary& operator = (const PhysicsLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RigidBodyCollection&    RigidBodies    () const;
          RigidBodyCollection&    RigidBodies    ();
    const MaterialCollection&     Materials      () const;
          MaterialCollection&     Materials      ();
    const ShapeCollection&        Shapes         () const;
          ShapeCollection&        Shapes         ();
    const TriangleMeshCollection& TriangleMeshes () const;
          TriangleMeshCollection& TriangleMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PhysicsLibrary&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PhysicsLibrary&, PhysicsLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager
<
  void (const char* file_name, PhysicsLibrary& library),
  void (const char* file_name, const PhysicsLibrary& library)
> PhysicsLibraryManager;

}

}

#endif
