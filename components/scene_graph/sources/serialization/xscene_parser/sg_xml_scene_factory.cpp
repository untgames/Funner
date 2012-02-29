#include "shared.h"

using namespace scene_graph;

namespace scene_graph
{

/*
    Константы
*/

const char* XSCENE_ROOT = "xscene"; //имя корневого узла XML сцен

}

namespace
{

/*
    Дескриптор сцены
*/

struct SceneDesc: public xtl::reference_counter
{
  SceneParserPtr parser; //парсер сцены
};

typedef xtl::intrusive_ptr<SceneDesc>                           SceneDescPtr;
typedef stl::hash_map<stl::hash_key<const char*>, SceneDescPtr> SceneDescMap;

}

/*
    Описание реализации фабрики XML сцен
*/

struct XmlSceneFactory::Impl
{
  SceneDescMap scenes; //сцены
};

/*
    Конструктор / деструктор
*/

XmlSceneFactory::XmlSceneFactory (const char* file_name, const LogHandler& log_handler)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
      //создание реализации
    
    impl = new Impl;
    
      //создание парсера
     
    common::Parser parser (file_name, "xml");
    
      //парсинг
      
    const common::ParseNode& xscene_root = parser.Root ().First ();
    
    if (strcmp (xscene_root.Name (), XSCENE_ROOT))
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Root node not found", file_name);
      
    bool partial = strcmp (common::get<const char*> (xscene_root, "partial", "false"), "true") == 0;
    
    if (partial)
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Partial definitions not allowed in this context", file_name);

    for (common::Parser::NamesakeIterator iter=xscene_root.First ("scene"); iter; ++iter)
    {
        //парсинг сцены
      
      const char* id = common::get<const char*> (*iter, "id", "");
      
      if (!*id)
      {
        parser.Log ().Error (*iter, "Scene attribute 'id' not found (anonymous scenes not allowed)");
        continue;
      }
        
      if (impl->scenes.find (id) != impl->scenes.end ())
      {
        parser.Log ().Error (*iter, "Scene '%s' has been already defined", id);
        continue;
      }

      try
      {
        SceneParserPtr parser = XmlSceneParserManagerSingleton::Instance ()->CreateParser (*iter);
        
        if (!parser)
          throw xtl::format_operation_exception ("scene_graph::XmlSceneFactory::XmlSceneFactory", "Internal error: parser is null");

          //предварительный разбор

        parser->Prepare ();

          //создание описателя сцены

        SceneDescPtr desc (new SceneDesc, false);

        desc->parser = parser;

          //регистрация сцены

        impl->scenes.insert_pair (id, desc);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
      }
      catch (...)
      {
        parser.Log ().Error (*iter, "unknown exception");
      }
    }

      //протоколирование

    parser.Log ().Print (log_handler);     
    
      //проверка ошибок

    if (parser.Log ().HasErrors ())
      throw xtl::format_operation_exception ("", "Scene file '%s' has bad format. Can't parse", file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneFactory::XmlSceneFactory");
    throw;
  }
}

XmlSceneFactory::~XmlSceneFactory ()
{
}

/*
    Получение информации о сцене
*/

bool XmlSceneFactory::GetSceneInfo (const char* name, ResourceGroup* resources)
{
  if (!name)
    return false;

  SceneDescMap::iterator iter = impl->scenes.find (name);

  if (iter == impl->scenes.end ())
    return false;

  if (resources)
    *resources = iter->second->parser->Resources ();

  return true;
}

/*
    Создание сцены
*/

void XmlSceneFactory::CreateScene (const char* name, Node& parent, SceneContext& scene_context)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    SceneDescMap::iterator iter = impl->scenes.find (name);
    
    if (iter == impl->scenes.end ())
      throw xtl::format_operation_exception ("", "Scene '%s' not found", name);
      
    SceneDesc& desc = *iter->second;
    
    desc.parser->CreateScene (parent, scene_context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneFactory::CreateScene");
    throw;
  }
}
