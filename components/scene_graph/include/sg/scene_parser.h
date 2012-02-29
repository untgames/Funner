#ifndef SCENE_GRAPH_SCENE_PARSER_HEADER
#define SCENE_GRAPH_SCENE_PARSER_HEADER

#include <sg/scene_manager.h>

namespace common
{

//forward declarations
class ParseNode;

}

namespace scene_graph
{

//forward declarations
class Entity;
class Camera;
class OrthoCamera;
class PerspectiveCamera;
class Light;
class DirectLight;
class SpotLight;
class PointLight;
class VisualModel;
class Sprite;
class TextLine;
class Listener;
class SoundEmitter;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISceneParser
{
  public:
    virtual ~ISceneParser () {}
    
//////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Prepare () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const ResourceGroup& Resources () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void CreateScene (Node& parent, SceneContext& context) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneParserCache
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneParserCache  ();
    SceneParserCache  (const SceneParserCache&);
    ~SceneParserCache ();
    
    SceneParserCache& operator = (const SceneParserCache&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void SetValue (const common::ParseNode& decl, const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ResetValue (const common::ParseNode& decl);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> T*       FindValue (const common::ParseNode& decl);
    template <class T> const T* FindValue (const common::ParseNode& decl) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SceneParserCache&);

  private:
    void                      SetValueCore  (const common::ParseNode&, detail::ISceneAttachment*);
    detail::ISceneAttachment* FindValueCore (const common::ParseNode&, const std::type_info&) const;

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SceneParserCache&, SceneParserCache&);

class XmlSceneParser: public ISceneParser
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    static ISceneParser* Create (const common::ParseNode&);
    
//////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Prepare ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ResourceGroup& Resources ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateScene (Node& parent, SceneContext& context);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlSceneParser  (const common::ParseNode&);
    ~XmlSceneParser ();    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const common::ParseNode& Root () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneParserCache& Cache ();
          
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Parse (const common::ParseNode& decl, Node& node, SceneContext& context);
    void Parse (const common::ParseNode& decl, Node& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, Entity& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, Camera& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, Light& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, Listener& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, SoundEmitter& node, Node& parent, SceneContext& context);    
    void Parse (const common::ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, PointLight& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, VisualModel& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, TextLine& node, Node& parent, SceneContext& context);
    void Parse (const common::ParseNode& decl, Sprite& node, Node& parent, SceneContext& context);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::vec2f&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::vec3f&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::vec4f&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::mat2f&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::mat3f&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::mat4f&);    
    static void ParseAttribute (const common::ParseNode& decl, const char* name, math::quatf&);
    static void ParseAttribute (const common::ParseNode& decl, const char* name, size_t size, float* value);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ���� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const common::ParseNode& decl, Node& parent, SceneContext& context)> ParseHandler;
    typedef xtl::function<void (const common::ParseNode& decl)>                                      PrepareHandler;    

    void RegisterParser       (const char* type, const ParseHandler& parse_handler, const PrepareHandler& prepare_handler = PrepareHandler ());
    void UnregisterParser     (const char* type);
    void UnregisterAllParsers ();

  private:
    template <class T> void CreateNode (const common::ParseNode& decl, Node& parent, SceneContext& context);

    void IncludeSubscene (const common::ParseNode& decl, Node& parent, SceneContext& context);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� XML ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlSceneParserManager
{
  public:
    typedef xtl::function<ISceneParser* (const common::ParseNode&)> SceneParserCreator;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterParser       (const char* version, const SceneParserCreator& creator);
    static void UnregisterParser     (const char* version);
    static void UnregisterAllParsers ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        SetDefaultVersion (const char* version);
    static const char* DefaultVersion    ();
};

#include <sg/detail/scene_parser.inl>

}

#endif
