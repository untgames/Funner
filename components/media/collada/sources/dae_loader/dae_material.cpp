#include "shared.h"

/*
    Разбор библиотеки материалов
*/

void DaeParser::ParseLibraryMaterials (Parser::Iterator iter)
{
  if (!iter->First ("material"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_materials' node. Must be at least one 'material' sub-tag");
    return;
  }
  
  for_each_child (*iter, "material", bind (&DaeParser::ParseMaterial, this, _1));  
}

void DaeParser::ParseMaterial (Parser::Iterator iter)
{
  const char* id = get<const char*> (*iter, "id");

  Parser::NamesakeIterator effect_iter = iter->First ("instance_effect");

  if (!effect_iter)
    raise_parser_exception (*iter, "No 'instance_effect' sub-tag");

  const char* effect_id = get<const char*> (*effect_iter, "url");

  effect_id++; //убираем префиксный '#'

  Effect* effect = model.Effects ().Find (effect_id);

  if (!effect)
    raise_parser_exception (*effect_iter, "Incorrect url '%s'. No effect in library", effect_id);

  if (effect_iter->Next ())
    raise_parser_exception (*iter, "Only one 'instance_effect' tag allowed");

    //создание нового материала

  Material material;

  material.SetId    (id);
  material.SetEffect (effect_id);

    //добавление материала в библиотеку
    
  model.Materials ().Insert (id, material);
}
