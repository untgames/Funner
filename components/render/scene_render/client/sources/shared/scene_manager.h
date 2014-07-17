#ifndef RENDER_SCENE_CLIENT_SCENE_MANAGER_SHARED_HEADER
#define RENDER_SCENE_CLIENT_SCENE_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <shared/scene.h>

namespace render
{

namespace scene
{

namespace client
{

//forward declarations
class  ClientImpl;
class  Node;
class  SceneObject;
struct SceneUpdateList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneManager: public xtl::noncopyable
{
  friend class SceneObject;
  public:
    typedef xtl::intrusive_ptr<Node> NodePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneManager  (ClientImpl& client, Context& context);
    ~SceneManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScenePtr GetScene  (scene_graph::Scene&);
    ScenePtr FindScene (scene_graph::Scene&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClientImpl& Client ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    client::Context& Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    NodePtr GetNode  (scene_graph::Node&); //may be null if object type is unsupported
    NodePtr FindNode (scene_graph::Node&); //may be null if object is not found

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneUpdateList& UpdateList ();    

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif