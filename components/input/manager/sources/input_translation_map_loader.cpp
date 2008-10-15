#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void translation_map_loader (const char* file_name, TranslationMap& target_map)
{
  static const char* METHOD_NAME = "input::translation_map_loader";
  Parser             p (file_name);
  ParseLog           log = p.Log ();
  Parser::Iterator   iter = p.Root ().First ("TranslationTable");

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case ParseLogMessageType_Error:
      case ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception (METHOD_NAME, log.Message (i));
      default:
        break;
    }

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid file format, no root 'TranslationTable' tag");

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
    if (!i->First ("Event") || !i->First ("Replacement"))
      throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, one of tag property missing at line %u", i->LineNumber ());

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
  {
    const char *event       = get<const char*> (*i, "Event", ""),
               *replacement = get<const char*> (*i, "Replacement", ""),
               *tag         = get<const char*> (*i, "Tag", "");

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
      TranslationMapManager::RegisterLoader ("xkeymap", &translation_map_loader);
    }
};

extern "C"
{

ComponentRegistrator<KeymapLoaderComponent> XKeymapLoader ("input.loaders.xkeymap");

}

}
