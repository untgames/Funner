#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void controls_detector_saver (const char* file_name, const ControlsDetector& source_detector)
{
  XmlWriter        writer (file_name);
  XmlWriter::Scope scope (writer, "ControlsDetector");

  for (ControlsDetector::Iterator iter=source_detector.CreateIterator (); iter; ++iter)
  {    
    ControlsDetector::IFilter& filter = *iter;

    XmlWriter::Scope scope (writer, "Filter");

    writer.WriteAttribute ("Action",         filter.Action ());
    writer.WriteAttribute ("InputEventMask", filter.EventMask ());
    writer.WriteAttribute ("Replacement",    filter.Replacement ());
  }
}

/*
   Компонент сохранения определителей контролов
*/

class KeydetSaverComponent
{
  public:
    //загрузка компонента
    KeydetSaverComponent () 
    {
      ControlsDetectorManager::RegisterSaver ("xkeydet", &controls_detector_saver);
    }
};

extern "C"
{

ComponentRegistrator<KeydetSaverComponent> XKeydetSaver ("input.savers.xkeydet");

}

}
