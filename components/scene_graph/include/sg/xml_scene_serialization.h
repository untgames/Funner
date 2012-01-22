#ifndef SCENE_GRAPH_XML_SCENE_SERIALIZATION_HEADER
#define SCENE_GRAPH_XML_SCENE_SERIALIZATION_HEADER

#include <sg/scene_manager.h>

namespace common
{

//forward declarations
class ParseNode;
class XmlWriter;

}

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����, ������������ ��� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScenePrototype
{
  public:
    typedef xtl::function<void (Node& parent, SceneContext& scene_context)> CreateHandler;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    ScenePrototype  (const CreateHandler& creator, const scene_graph::ResourceGroup& resources);
    ScenePrototype  (const ScenePrototype& prototype);
    ~ScenePrototype ();

    ScenePrototype& operator = (const ScenePrototype&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const scene_graph::ResourceGroup& Resources () const; //������ �������� ������� ���������
          scene_graph::ResourceGroup& Resources ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateScene (Node& parent, SceneContext& scene_context);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ScenePrototype&);

  private:
    struct Impl;
    Impl* impl;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (ScenePrototype&, ScenePrototype&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ����� XML ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlSceneSerializationManager
{
  public:
    typedef xtl::function<ScenePrototype (const common::ParseNode&)>       SceneLoader;
    typedef xtl::function<bool (common::XmlWriter& xml_write, Node& node)> SceneSaver;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterLoader       (const char*        node_type,                             //��� ���� ����
                                      const SceneLoader& loader,                                //���������
                                      const char*        ignore_children_node_name_masks = ""); //����� ��������� �����, ������� ����� ��������������� ��� ������� �������� �����
    static void RegisterSaver        (const char* saver_name, const SceneSaver& saver);
    static void UnregisterLoader     (const char* node_type);
    static void UnregisterSaver      (const char* saver_name);
    static void UnregisterAllLoaders ();
    static void UnregisterAllSavers  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> struct SuperClassLoader
    {
      typedef xtl::function<void (const common::ParseNode& parse_node, T& node)> Type;
    };

    template <class T> static void RegisterSuperClassLoader       (const typename SuperClassLoader<T>::Type& loader);
    template <class T> static void UnregisterSuperClassLoader     ();
                       static void UnregisterAllSuperClassLoaders ();
    
    template <class T> static const typename SuperClassLoader<T>::Type* FindSuperClassLoader ();
    template <class T> static const typename SuperClassLoader<T>::Type& GetSuperClassLoader  ();    
};

}

#endif
