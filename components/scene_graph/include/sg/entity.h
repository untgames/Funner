#ifndef SCENE_GRAPH_ENTITY_HEADER
#define SCENE_GRAPH_ENTITY_HEADER

#include <sg/node.h>
#include <bv/axis_aligned_box.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������, ������� �������������� ������������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Entity: public Node
{
  public:
    typedef xtl::com_ptr<Entity>       Pointer;
    typedef xtl::com_ptr<const Entity> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetColor (const math::vec3f& color);
    void               SetColor (float red, float green, float blue);
    const math::vec3f& Color    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������������� �������
///����������: �������������� ������ �����-�������� �� ������ �� �������������� ����� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const bound_volumes::aaboxf& BoundBox      () const; //�������������� ����� ���� � ��������� ������� ���������
    const bound_volumes::aaboxf& WorldBoundBox () const; //�������������� ����� ���� � ������� ������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ���������� ���������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsNotify ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsInfiniteBounds () const; //��������: �������� �� �������������� ������ ���� ������������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bound_volumes::aaboxf ChildrenBoundBox      () const; //�������������� ����� �������� � ��������� ������� ��������� ����
    bound_volumes::aaboxf FullBoundBox          () const; //������ �������������� ����� ���� � ��������� � ��������� ������� ��������� ����
    bound_volumes::aaboxf WorldChildrenBoundBox () const; //�������������� ����� �������� � ������� ������� ���������
    bound_volumes::aaboxf WorldFullBoundBox     () const; //������ �������������� ����� ���� � ��������� � ������� ������� ���������

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Entity  ();
    ~Entity ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBoundBox (const bound_volumes::aaboxf&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetInfiniteBounds ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateBoundsCore ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);    

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AfterUpdateWorldTransformEvent ();
    void AfterSceneAttachEvent ();
    void BeforeSceneDetachEvent ();

  private:
    using Node::Create;

  private:
    void UpdateWorldBoundsNotify ();
    void UpdateWorldBounds () const;

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
