#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации парсера
*/

struct XmlSceneParser::Impl
{
  common::ParseNode root;      //корневой узел
  SceneParserCache  cache;     //кэш парсера
  ResourceGroup     resources; //ресурсы сцены
  
///Конструктор
  Impl (const common::ParseNode& in_root) : root (in_root) {}
  
///Построение списка ресурсов
  void PrepareResources (const common::ParseNode& node)
  {
    //////???????????????
  }
};

/*
    Конструктор / деструктор
*/

XmlSceneParser::XmlSceneParser (const common::ParseNode& root)
{
  try
  { 
      //создание реализации            
    
    impl = new Impl (root);
    
      //построение списка ресурсов
      
    impl->PrepareResources (root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::XmlSceneParser");
    throw;
  }
}

XmlSceneParser::~XmlSceneParser ()
{
}

/*
    Создание парсера
*/

ISceneParser* XmlSceneParser::Create (const common::ParseNode& root)
{
  try
  {
    return new XmlSceneParser (root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Create");
    throw;
  }
}

/*
    Получение списка ресурсов
*/

const ResourceGroup& XmlSceneParser::Resources ()
{
  return impl->resources;
}

/*
    Создание сцены
*/

void XmlSceneParser::CreateScene (Node& parent, SceneContext& context, const LogHandler& log_handler)
{
  try
  {
    /////???????????????????
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::CreateScene");
    throw;
  }
}

/*
    Корневой узел
*/

const common::ParseNode& XmlSceneParser::Root () const
{
  return impl->root;
}

/*
    Кэш
*/

SceneParserCache& XmlSceneParser::Cache ()
{
  return impl->cache;
}

/*
    Разбор узла
*/

void XmlSceneParser::ParseDispatch (const common::ParseNode& decl, Node& parent)
{
}

/*
    Парсинг
*/

void XmlSceneParser::Parse (const common::ParseNode& decl, Node& parent, SceneContext& context, const LogHandler& log_handler)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Node& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Entity& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Camera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Light& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, PointLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, VisualModel& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, TextLine& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Sprite& node, Node& parent, SceneContext& context)
{
}
