#include "shared.h"

/*
    –азбор библиотеки геометрии
*/

void DaeParser::ParseLibraryGeometries (Parser::Iterator iter)
{
  if (!test (iter, "geometry"))
  {
    parse_log.Warning (iter, "Incorrect 'library_geometries' node. Must be at least one 'geometry' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "geometry", bind (&DaeParser::ParseGeometry, this, _1));  
}

/*
    –азбор геометрии
*/

void DaeParser::ParseGeometry (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  Parser::Iterator mesh_iter = iter->First ("mesh");
  
  if (!mesh_iter)
    return;
  
  if (mesh_iter->NextNamesake ())
    LogWarning (mesh_iter->NextNamesake (), "Second mesh will be ignored");    
    
    //разбор меша
    
  Mesh mesh;
      
  mesh.SetId (id);

  ParseMesh (mesh_iter, mesh);
  
    //добавление меша в библиотеку
    
  model.Meshes ().Insert (mesh);
}
