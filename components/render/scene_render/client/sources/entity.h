///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Entity: public Node
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Entity  (scene_graph::Entity&, SceneManager&, interchange::NodeType node_type = interchange::NodeType_Entity);
    ~Entity ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    scene_graph::Entity& SourceNode () { return static_cast<scene_graph::Entity&> (Node::SourceNode ()); }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCore (client::Context&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
