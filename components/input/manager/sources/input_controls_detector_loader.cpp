#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void controls_detector_loader (const char* file_name, ControlsDetector& target_detector)
{
  static const char* METHOD_NAME = "input::controls_detector_loader";
  Parser             p (file_name);
  ParseLog           log = p.Log ();
  Parser::Iterator   iter = p.Root ().First ("ControlsDetector");

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
    throw xtl::format_operation_exception (METHOD_NAME, "Invalid file format, no root 'ControlsDetector' tag");

  for (Parser::NamesakeIterator i = iter->First ("Filter"); i; i++)
    if (!i->First ("Action") || !i->First ("InputEventMask") || !i->First ("Replacement"))
      throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, one of tag property missing at line %u", i->LineNumber ());

  for (Parser::NamesakeIterator i = iter->First ("Filter"); i; i++)
  {
    const char *action           = get<const char*> (*i, "Action", ""),
               *input_event_mask = get<const char*> (*i, "InputEventMask", ""),
               *replacement      = get<const char*> (*i, "Replacement", "");

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
