#include "shared.h"

/*
    Разбор библиотеки контроллеров
*/

void DaeParser::ParseLibraryControllers (Parser::Iterator iter)
{
  if (!iter->First ("controller"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_controllers' node. Must be at least one 'controller' sub-tag");
    return;
  }
  
  for_each_child (*iter, "controller", bind (&DaeParser::ParseController, this, _1));
}

/*
    Разбор контроллера
*/

void DaeParser::ParseController (Parser::Iterator iter)
{
  const char* id = get<const char*> (*iter, "id");

  Parser::Iterator skin_iter  = iter->First ("skin"),
                   morph_iter = iter->First ("morph");
  
  if (skin_iter && morph_iter)
  {
    raise_parser_exception (skin_iter->NextNamesake (), "Only one 'skin' or 'morph' sub-tag allowed");          
  }

  if (!skin_iter && !morph_iter)
  {
    raise_parser_exception (skin_iter->NextNamesake (), "Must be at least one 'skin' or 'morph' sub-tag");          
  }

  if (skin_iter)
  { 
    if (skin_iter->NextNamesake ())
      raise_parser_exception (skin_iter->NextNamesake (), "Only one 'skin' sub-tag allowed");          

    ParseSkin (skin_iter, id);
  }

  if (morph_iter)
  {
    if (morph_iter->NextNamesake ())
      raise_parser_exception (morph_iter->NextNamesake (), "Only one 'morph' sub-tag allowed");

    ParseMorph (morph_iter, id);
  }
}
