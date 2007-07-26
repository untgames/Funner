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
  
  Parser::Iterator skin_iter = iter->First ("skin");
  
  if (!skin_iter)
    return;
  
  if (skin_iter->Next ())
    LogWarning (skin_iter->Next (), "Second skin will be ignored");    
    
//  Skin& skin = model.Skins ().Create (id);

//  ParseSkin (skin_iter, skin);
}
