#include "shared.h"

/*
     онструктор / деструктор
*/

DaeParser::DaeParser (const char* file_name, Model& in_model, const LogHandler& log)
  : model (in_model), parser (parse_log, file_name), current_scope (0)
{
  try
  {
    ParseRoot (parser.Root ()->First ("COLLADA"));
  }
  catch (...)
  {
    PrintLog (log);
    throw;
  }
  
  PrintLog (log);  
}

DaeParser::~DaeParser ()
{
  for (VertexIndexMaps::iterator i=vertex_index_maps.begin (); i!=vertex_index_maps.end (); ++i)
    delete i->second;
}

/*
    ѕечать ошибок
*/

void DaeParser::PrintLog (const LogHandler& log)
{
  for (size_t i=0; i<parse_log.MessagesCount (); ++i)
    log (parse_log.Message (i));    
}

/*
    ѕротоколирование
*/

void DaeParser::LogError (Parser::Node* node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  parse_log.VError (node, format, list);  
  
  LogScope* scope = current_scope;
  
  if (!scope)
    return;
  
  if (scope->Node () == node)
    scope = scope->Prev ();
  
  for (; scope; scope=scope->Prev ())
  {
    Parser::Node* node = scope->Node ();
    const char* id = scope->Id () ? scope->Id () : get<const char*> (node, "id");

    if (id) parse_log.Error (node, "  at parse %s '%s'", node->Tag (), id);
    else    parse_log.Error (node, "  at parse %s", node->Tag ());
  }
}

void DaeParser::LogWarning (Parser::Node* node, const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  parse_log.VWarning (node, format, list);
}

/*
    —оздание и поиск карт вершинных индексов    
*/

VertexIndexMap* DaeParser::GetVertexIndicesMap (const char* mesh_id, size_t surface_index)
{
  if (!mesh_id)
    return 0;
    
  stl::string surface_name = common::format ("%s#%u", mesh_id, surface_index);
    
    //попытка поиска карты вершинных индексов
    
  VertexIndexMaps::iterator iter = vertex_index_maps.find (surface_name.c_str ());
  
  if (iter != vertex_index_maps.end ())
    return iter->second;
    
    //создание карты вершинных индексов
    
  VertexIndexMap* map = new VertexIndexMap;
  
    //регистраци€ карты вершинных индексов
    
  vertex_index_maps [surface_name.c_str ()] = map;
  
  return map;
}

/*
    –азбор корневого узла
*/

void DaeParser::ParseRoot (Parser::Iterator iter)
{
  if (!iter)
  {
    LogError (parser.Root (), "Wrong file format. No 'COLLADA' tag\n");
    return;
  }
  
  if (!test (iter, "version", "1.4.1"))  
    LogWarning (iter, "Currently supported Collada version - 1.4.1, document uses other version (%s), "
                      "some features may be not fully supported or unsupported", get<const char*> (iter, "version", "UNDEFINED"));
                      
    //разбор библиотек
    
  ParseLibraries (iter);
  
    //определение имени активной сцены
    
  const char* active_scene_name = get<const char*> (iter, "scene.instance_visual_scene.url");
  
  if (active_scene_name)
  {
    active_scene_name++; //избавл€емс€ от префиксного '#'
    
    model.SetActiveSceneName (active_scene_name);
  }
  else LogWarning (iter, "No 'scene.instance_visual_scene.url' detected");
}

/*
    –азбор библиотек
*/

void DaeParser::ParseLibraries (Parser::Iterator iter)
{
  for_each_child (iter, "library_effects", bind (&DaeParser::ParseLibraryEffects, this, _1));
  for_each_child (iter, "library_materials", bind (&DaeParser::ParseLibraryMaterials, this, _1));
  for_each_child (iter, "library_geometries", bind (&DaeParser::ParseLibraryGeometries, this, _1));
  for_each_child (iter, "library_controllers", bind (&DaeParser::ParseLibraryControllers, this, _1));
  for_each_child (iter, "library_lights", bind (&DaeParser::ParseLibraryLights, this, _1));
  for_each_child (iter, "library_cameras", bind (&DaeParser::ParseLibraryCameras, this, _1));
  for_each_child (iter, "library_visual_scenes", bind (&DaeParser::ParseLibraryVisualScenes, this, _1));
}

/*
    LogScope
*/

LogScope::LogScope (Parser::Node* in_node, DaeParser& in_parser, const char* in_id)
  : parser (in_parser), node (in_node), prev (parser.current_scope), id (in_id)
{
  parser.current_scope = this;
}

LogScope::~LogScope ()
{
  parser.current_scope = prev;
}

