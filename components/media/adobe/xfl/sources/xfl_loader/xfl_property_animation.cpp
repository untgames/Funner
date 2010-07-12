#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

/*
    Разбор анимируемого свойства
*/

void XflParser::ParsePropertyAnimation (Parser::Iterator iter, PropertyAnimation& property, const char* property_prefix)
{
  stl::string property_id = common::format ("%s.%s", property_prefix, get<const char*> (*iter, "id"));
  
  property.SetName    (property_id.c_str ());
  property.SetEnabled (get<bool> (*iter, "enabled"));

  for (Parser::NamesakeIterator i = iter->First ("Keyframe"); i; ++i)
  {
    PropertyAnimationKeyframe new_keyframe;

    ParsePropertyAnimationKeyframe (i, new_keyframe);

    property.Keyframes ().Add (new_keyframe);
  }
}

void XflParser::ParsePropertyAnimationKeyframe (Parser::Iterator iter, PropertyAnimationKeyframe& keyframe)
{
  float anchor   = ReadFloatFromVec2 (iter, "anchor"),
        next     = ReadFloatFromVec2 (iter, "next"),
        previous = ReadFloatFromVec2 (iter, "anchor");

  if (anchor != next || anchor != previous)
    raise_parser_exception (*iter, "Wrong keyframe format, 'anchor', 'next' and 'previous' attributes must be equal");

  keyframe.value = anchor;
  keyframe.time  = get<float> (*iter, "timevalue");
}
