#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

/*
    Печать лога парсинга
*/

void XflParser::PrintLog ()
{
  const ParseLog& parser_log = parser.Log ();

  try
  {  
    for (size_t i = 0; i < parser_log.MessagesCount (); ++i)
      log.Print (parser_log.Message (i));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
   Чтение цвета из строки вида #ffffff
*/

bool XflParser::ReadHexColor (const char* hex_string, math::vec3f& color)
{
  if (*hex_string != '#')
    return false;

  if (xtl::xstrlen (hex_string) != xtl::xstrlen ("#ffffff"))
    return false;

  char* end_ptr;

  errno = 0;

  unsigned int int_value = strtol (hex_string + 1, &end_ptr, 16);

  if (!int_value && errno)
    return false;

  color.x = (int_value / 256 / 256) / 255.0f;
  color.y = (int_value / 256 % 256) / 255.0f;
  color.z = (int_value % 256) / 255.0f;

  return true;
}

/*
   Чтение флоата из узла вида "0, value"
*/

float XflParser::ReadFloatFromVec2 (Parser::Iterator iter, const char* property_name)
{
  const char* vec_string = get<const char*> (*iter, property_name);
  float       return_value;

  if (sscanf (vec_string, "0,%f", &return_value) != 1)
    raise_parser_exception (*iter, "Wrong float value format, must be '0, value'");

  return return_value;
}

/*
   Проверка корректности прочитанного документа
*/

void XflParser::CheckDocument ()
{
  for (Document::SymbolList::Iterator iter = document.Symbols ().CreateIterator (); iter; ++iter)
    if (!CheckTimeline (iter->Timeline ()))
      log.Printf ("Removed invalid frame element from symbol '%s'", iter->Name ());

  for (Document::TimelineList::Iterator iter = document.Timelines ().CreateIterator (); iter; ++iter)
    if (!CheckTimeline (*iter))
      log.Printf ("Removing invalid frame element from timeline '%s'", iter->Name ());
}

bool XflParser::CheckTimeline (Timeline& timeline)
{
  bool timeline_valid = true;

  for (Timeline::LayerList::Iterator layer_iter = timeline.Layers ().CreateIterator (); layer_iter; ++layer_iter)
    for (Layer::FrameList::Iterator frame_iter = layer_iter->Frames ().CreateIterator (); frame_iter; ++frame_iter)
    {
      ICollection<FrameElement>& elements = frame_iter->Elements ();

      for (size_t i = 0; i < elements.Size ();)
      {
        if (!CheckFrameElement (elements [i]))
        {
          log.Printf ("Removing invalid frame element '%s'", elements [i].Name ());
          elements.Remove (i);
          timeline_valid = false;
        }
        else
          i++;
      }
    }

  return timeline_valid;
}

bool XflParser::CheckFrameElement (const FrameElement& element)
{
  switch (element.Type ())
  {
    case FrameElementType_ResourceInstance: return document.Resources ().Find (element.Name ()) != 0;
    case FrameElementType_SymbolInstance:   return document.Symbols ().Find (element.Name ()) != 0;
  }

  return false;
}
