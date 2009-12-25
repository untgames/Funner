#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

/*
    Разбор анимируемого свойства
*/

void XflParser::ParsePropertyAnimation (Parser::Iterator iter, PropertyAnimation& property, const char* property_prefix)
{
  stl::string property_id = common::format ("%s.%s", property_prefix, get<const char*> (*iter, "id"));

  property.SetId      (property_id.c_str ());
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
  keyframe.anchor    = ReadFloatFromVec2 (iter, "anchor");
  keyframe.next      = ReadFloatFromVec2 (iter, "next");
  keyframe.previous  = ReadFloatFromVec2 (iter, "previous");
  keyframe.roving    = get<bool>  (*iter, "roving");
  keyframe.timevalue = get<float> (*iter, "timevalue");
}
