#include "shared.h"

using namespace scene_graph;

/*
    Регистрация парсеров
*/

void XmlSceneParserManagerImpl::RegisterParser (const char* version, const SceneParserCreator& parser)
{
  if (!version)
    throw xtl::make_null_argument_exception ("scene_graph::XmlSceneParserManagerImpl::RegisterParser", "version");
    
  creators.insert_pair (version, parser);
}

void XmlSceneParserManagerImpl::UnregisterParser (const char* version)
{
  if (!version)
    return;
    
  creators.erase (version);
}

void XmlSceneParserManagerImpl::UnregisterAllParsers ()
{
  creators.clear ();
}

/*
    Версия парсера по умолчанию
*/

void XmlSceneParserManagerImpl::SetDefaultVersion (const char* version)
{
  if (!version)
    throw xtl::make_null_argument_exception ("scene_graph::XmlSceneParserManagerImpl::SetDefaultVersion", "version");
}

const char* XmlSceneParserManagerImpl::DefaultVersion () const
{
  return default_version.c_str ();
}

/*
    Создание парсера
*/

SceneParserPtr XmlSceneParserManagerImpl::CreateParser (const common::ParseNode& decl)
{
  try
  {    
    const char* version = common::get<const char*> (decl, "version", default_version.c_str ());

    CreatorMap::iterator iter = creators.find (version);

    if (iter == creators.end ())
      throw xtl::format_operation_exception ("", "Can't create XML scene parser for version '%s'", version);
      
    const SceneParserCreator& creator = iter->second;

    return SceneParserPtr (creator (decl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParserManagerImpl::CreateParser");
    throw;
  }
}

/*
    XmlSceneParserManager
*/

void XmlSceneParserManager::RegisterParser (const char* version, const SceneParserCreator& creator)
{
  XmlSceneParserManagerSingleton::Instance ()->RegisterParser (version, creator);
}

void XmlSceneParserManager::UnregisterParser (const char* version)
{
  XmlSceneParserManagerSingleton::Instance ()->UnregisterParser (version);
}

void XmlSceneParserManager::UnregisterAllParsers ()
{
  XmlSceneParserManagerSingleton::Instance ()->UnregisterAllParsers ();
}

void XmlSceneParserManager::SetDefaultVersion (const char* version)
{
  XmlSceneParserManagerSingleton::Instance ()->SetDefaultVersion (version);
}

const char* XmlSceneParserManager::DefaultVersion ()
{
  return XmlSceneParserManagerSingleton::Instance ()->DefaultVersion ();
}
