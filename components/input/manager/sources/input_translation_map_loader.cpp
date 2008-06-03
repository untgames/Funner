#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void translation_map_loader (const char* file_name, TranslationMap& target_map)
{
  static const char* METHOD_NAME = "input::translation_map_loader";
  ParseLog           log;
  Parser             p (log, file_name);
  Parser::Iterator   iter = p.Root ()->First ("TranslationTable");

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      raise <Exception> (METHOD_NAME, log.Message(0));

  if (!iter)
    raise<Exception> (METHOD_NAME, "Invalid file format, no root 'TranslationTable' tag");

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
    if (!test (i, "Event") || !test (i, "Replacement"))
      raise<Exception> (METHOD_NAME, "Incorrect file format, one of tag property missing at line %u", i->LineNumber ());

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
  {
    const char *event       = get<const char*> (i, "Event", ""),
               *replacement = get<const char*> (i, "Replacement", ""),
               *tag         = get<const char*> (i, "Tag", "");

    target_map.Add (event, replacement, tag);
  }
}

/*
   Компонент загрузки карт трансляции
*/

class KeymapLoaderComponent
{
  public:
    //загрузка компонента
    KeymapLoaderComponent () 
    {
      TranslationMapManager::RegisterLoader ("keymap", &translation_map_loader);
    }
};

extern "C"
{

ComponentRegistrator<KeymapLoaderComponent> KeymapLoader ("input.loaders.keymap");

}

}
