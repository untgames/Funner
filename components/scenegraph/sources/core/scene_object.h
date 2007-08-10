#ifndef SCENE_GRAPH_SCENE_OBJECT_HEADER
#define SCENE_GRAPH_SCENE_OBJECT_HEADER

#include <sg/entity.h>

namespace scene_graph
{

//forward declarations
class SceneSpace;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneObject
{
  friend class SceneSpace;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneObject  (scene_graph::Entity&);
    ~SceneObject ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
          SceneObject* NextObject ()       { return next_object; }
    const SceneObject* NextObject () const { return next_object; }
          SceneObject* PrevObject ()       { return prev_object; }
    const SceneObject* PrevObject () const { return prev_object; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� entity
///////////////////////////////////////////////////////////////////////////////////////////////////
          scene_graph::Entity& Entity ()       { return entity; }
    const scene_graph::Entity& Entity () const { return entity; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� � �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindToSpace (SceneSpace*);

          SceneSpace* Space ()       { return space; }
    const SceneSpace* Space () const { return space; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsNotify ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBounds ();

  private:
    SceneObject (const SceneObject&); //no impl
    SceneObject& operator = (const SceneObject&); //no impl

  private:
    scene_graph::Entity& entity;                      //entity, ��������� � ��������
    SceneSpace*          space;                       //������������, �������� ���������� ������
    SceneObject          *prev_object, *next_object;  //������� �������� ������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneSpace
{
  friend class SceneObject;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    SceneSpace  ();
    ~SceneSpace ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t ObjectsCount () const { return objects_count; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
          SceneObject* FirstObject ()       { return first_object; }
    const SceneObject* FirstObject () const { return first_object; }

  private:
    SceneSpace (const SceneSpace&); //no impl
    SceneSpace& operator = (const SceneSpace&); //no impl

  private:
    SceneObject* first_object;  //������ ������ � ������� �������� �����
    size_t       objects_count; //���������� �������� � �����
};

}

#endif
