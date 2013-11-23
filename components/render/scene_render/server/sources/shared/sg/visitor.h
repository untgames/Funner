#ifndef RENDER_SCENE_SERVER_VISITOR_SHARED_HEADER
#define RENDER_SCENE_SERVER_VISITOR_SHARED_HEADER

namespace render
{

namespace scene
{

namespace server
{

//forward declarations
class Node;
class Entity;
class VisualModel;
class Light;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������� ��� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISceneVisitor
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Visit (Entity& entity) {}
    virtual void Visit (VisualModel& entity) {}
    virtual void Visit (Light& entity) {}

  protected:
    virtual ~ISceneVisitor () {}
};
	
}

}

}

#endif
