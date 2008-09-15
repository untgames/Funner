#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void controls_detector_loader (const char* file_name, ControlsDetector& target_detector)
{
  static const char* METHOD_NAME = "input::controls_detector_loader";
  ParseLog           log;
  Parser             p (log, file_name);
  Parser::Iterator   iter = p.Root ()->First ("ControlsDetector");

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      throw xtl::format_operation_exception (METHOD_NAME, log.Message(0));

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid file format, no root 'ControlsDetector' tag");

  for (Parser::NamesakeIterator i = iter->First ("Filter"); i; i++)
    if (!test (i, "Action") || !test (i, "InputEventMask") || !test (i, "Replacement"))
      throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, one of tag property missing at line %u", i->LineNumber ());

  for (Parser::NamesakeIterator i = iter->First ("Filter"); i; i++)
  {
    const char *action           = get<const char*> (i, "Action", ""),
               *input_event_mask = get<const char*> (i, "InputEventMask", ""),
               *replacement      = get<const char*> (i, "Replacement", "");

    target_detector.Add (action, input_event_mask, replacement);
  }
}

/*
   Компонент загрузки определителей контролов
*/

class KeydetLoaderComponent
{
  public:
    //загрузка компонента
    KeydetLoaderComponent () 
    {
      ControlsDetectorManager::RegisterLoader ("xkeydet", &controls_detector_loader);
    }
};

extern "C"
{

ComponentRegistrator<KeydetLoaderComponent> XKeydetLoader ("input.loaders.xkeydet");

}

}
