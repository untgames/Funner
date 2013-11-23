#ifndef RENDER_SCENE_SERVER_SCENE_MANAGER_SHARED_HEADER
#define RENDER_SCENE_SERVER_SCENE_MANAGER_SHARED_HEADER

#include <xtl/intrusive_ptr.h>

#include <bv/plane_list.h>

#include <render/scene/interchange/types.h>

namespace render
{

namespace scene
{

namespace server
{

//forward declaration
class  Entity;
class  Node;
class  ISceneVisitor;
struct TraverseResult;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene
{
  friend class Entity;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene  ();
    Scene  (const Scene&);
    ~Scene ();

    Scene& operator = (const Scene&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Traverse (const bound_volumes::plane_listf& frustum, ISceneVisitor& visitor);
    void Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result);
    void Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result, size_t filter);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachNode (const xtl::intrusive_ptr<Entity>& node);
    void DetachNode (const xtl::intrusive_ptr<Entity>& node);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneManager  ();
    ~SceneManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene CreateScene (interchange::object_id_t id);
    void  RemoveScene (interchange::object_id_t id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene& GetScene (interchange::object_id_t id) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::intrusive_ptr<Node> CreateNode (interchange::object_id_t id, interchange::NodeType type);
    void                     RemoveNode (interchange::object_id_t id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Node& GetNode (interchange::object_id_t id) const;

  private:
    SceneManager (const SceneManager&); //no impl
    SceneManager& operator = (const SceneManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class NodeFactory
{
  public:
    static Node* CreateNode (interchange::NodeType type);
};


}

}

}

#endif
