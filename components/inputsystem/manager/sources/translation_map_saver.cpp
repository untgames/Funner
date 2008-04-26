#include <common/xml_writer.h>

#include <input/translation_map.h>

#include "shared.h"

using namespace input;
using namespace common;

namespace input
{

void translation_map_saver (const char* file_name, const TranslationMap& source_map)
{
  XmlWriter        writer (file_name);
  XmlWriter::Scope scope (writer, "TranslationTable");

  for (size_t i = 0; i < source_map.Size (); i++)
  {    
    TranslationMap::Translation current_translation = source_map.Item (i);
    XmlWriter::Scope            scope (writer, "Translation");
    
    writer.WriteAttribute ("Event", current_translation.input_event);
    writer.WriteAttribute ("Replacement", current_translation.client_event_replacement);
  }
}

}
