#include "shared.h"

/*
    –азбор библиотеки геометрии
*/

void DaeParser::ParseLibraryGeometries (Parser::Iterator iter)
{
  if (!iter->First ("geometry"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_geometries' node. Must be at least one 'geometry' sub-tag");
    return;
  }
  
  for_each_child (*iter, "geometry", bind (&DaeParser::ParseGeometry, this, _1));  
}

/*
    –азбор геометрии
*/

void DaeParser::ParseGeometry (Parser::Iterator iter)
{
  Parser::Iterator mesh_iter = iter->First ("mesh");
  
  if (!mesh_iter)
    return;

  if (mesh_iter->NextNamesake ())
    raise_parser_exception (mesh_iter->NextNamesake (), "Only one 'mesh' tag allowed");

    //разбор меша

  Mesh mesh;
  
  const char* id = get<const char*> (*iter, "id");

  mesh.SetId (id);

  ParseMesh (mesh_iter, mesh);

    //добавление меша в библиотеку

  model.Meshes ().Insert (id, mesh);
}
