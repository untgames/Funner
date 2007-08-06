#include "shared.h"

/*
    Разбор библиотеки контроллеров
*/

void DaeParser::ParseLibraryControllers (Parser::Iterator iter)
{
  if (!test (iter, "controller"))
  {
    parse_log.Warning (iter, "Incorrect 'library_controllers' node. Must be at least one 'controller' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "controller", bind (&DaeParser::ParseController, this, _1));
}

/*
    Разбор контроллера
*/

void DaeParser::ParseController (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  Parser::Iterator skin_iter  = iter->First ("skin"),
                   morph_iter = iter->First ("morph");
  
  if (skin_iter && morph_iter)
    LogError (skin_iter->NextNamesake (), "Only one 'skin' or 'morph' sub-tag allowed");          
  if (!skin_iter && !morph_iter)
    LogError (skin_iter->NextNamesake (), "Must be at least one 'skin' or 'morph' sub-tag");          

  if (skin_iter)
  { 
    if (skin_iter->NextNamesake ())
      LogError (skin_iter->NextNamesake (), "Only one 'skin' sub-tag allowed");          

//    ParseSkin (skin_iter, skin);
  }

  if (morph_iter)
  {
    if (morph_iter->NextNamesake ())
      LogError (morph_iter->NextNamesake (), "Only one 'morph' sub-tag allowed");          

    ParseMorph (morph_iter, id);
  }
}
