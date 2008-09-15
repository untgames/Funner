#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void translation_map_registry_loader (const char* file_name, TranslationMapRegistry& target_registry)
{
  static const char* METHOD_NAME = "input::translation_map_registry_loader";
  ParseLog           log;
  Parser             p (log, file_name);
  Parser::Iterator   iter = p.Root ()->First ("TranslationMapRegistry");

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      throw xtl::format_operation_exception (METHOD_NAME, log.Message(0));

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid file format, no root 'TranslationMapRegistry' tag");

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
    if (!test (i, "Profile") || !test (i, "TranslationMap"))
      throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, one of tag property missing at line %u", i->LineNumber ());

  for (Parser::NamesakeIterator i = iter->First ("Translation"); i; i++)
  {
    const char *profile         = get<const char*> (i, "Profile", ""),
               *translation_map = get<const char*> (i, "TranslationMap", "");

    target_registry.Register (profile, translation_map);
  }
}

/*
   Компонент загрузки карт трансляции
*/

class KeyregLoaderComponent
{
  public:
    //загрузка компонента
    KeyregLoaderComponent () 
    {
      TranslationMapRegistryManager::RegisterLoader ("xkeyreg", &translation_map_registry_loader);
    }
};

extern "C"
{

ComponentRegistrator<KeyregLoaderComponent> XKeyregLoader ("input.loaders.xkeyreg");

}

}
