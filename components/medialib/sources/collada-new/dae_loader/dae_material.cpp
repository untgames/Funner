#include "shared.h"

/*
    Разбор библиотеки материалов
*/

void DaeParser::ParseLibraryMaterials (Parser::Iterator iter)
{
  if (!test (iter, "material"))
  {
    parse_log.Warning (iter, "Incorrect 'library_materials' node. Must be at least one 'material' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "material", bind (&DaeParser::ParseMaterial, this, _1));  
}

void DaeParser::ParseMaterial (Parser::Iterator iter)
{
  LogScope scope (iter, *this);

  const char* id = get<const char*> (iter, "id");
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }

  Parser::NamesakeIterator effect_iter = iter->First ("instance_effect");  
  
  if (!effect_iter)
  {
    LogError (iter, "No 'instance_effect' sub-tag");
    return;
  }
  
  const char* effect_id = get<const char*> (effect_iter, "url");
  
  if (!effect_id)
  {
    LogError (effect_iter, "No instance effect url");
    return;
  }
  
  effect_id++; //убираем префиксный '#'

  Effect* effect = effect_id ? model.Effects ().Find (effect_id) : 0;
  
  if (!effect)
  {
    LogError (effect_iter, "Incorrect url '%s'. No effect in library", effect_id);
    return;
  }
 
  if (effect_iter->Next ())
  {
    LogError (iter, "More than one child 'instance_effect' detected");
    return;
  }  

  model.Materials ().Create (*effect, id);
}
