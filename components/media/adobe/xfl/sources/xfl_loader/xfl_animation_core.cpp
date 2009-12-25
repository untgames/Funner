#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t DEFAULT_PROPERTIES_COUNT = 8;

}

/*
    Разбор кадра анимации
*/

void XflParser::ParseAnimationCore (Parser::Iterator iter, AnimationCore& animation)
{
  float timescale = get<float> (*iter, "TimeScale");

  animation.SetDuration     (get<float> (*iter, "duration") / timescale);
  animation.SetOrientToPath (get<bool>  (*iter, "metadata.Settings.orientToPath"));

  animation.Properties ().Reserve (DEFAULT_PROPERTIES_COUNT);

  for (Parser::NamesakeIterator i = iter->First ("PropertyContainer"); i; ++i)
    ParsePropertyAnimationContainer (i, animation, "");

  for (AnimationCore::PropertyAnimationList::Iterator property = animation.Properties ().CreateIterator (); property; ++property)
    for (PropertyAnimation::KeyframeList::Iterator keyframe = property->Keyframes ().CreateIterator (); keyframe; ++keyframe)
      keyframe->timevalue /= timescale;
}

/*
    Разбор анимируемых свойств
*/

void XflParser::ParsePropertyAnimationContainer (Parser::Iterator iter, AnimationCore& animation, const char* in_property_prefix)
{
  stl::string property_prefix;
  const char  *container_id = get<const char*> (*iter, "id");

  if (xtl::xstrlen (in_property_prefix))
    property_prefix = common::format ("%s.%s", in_property_prefix, container_id);
  else
    property_prefix = container_id;

  for (Parser::NamesakeIterator i = iter->First ("PropertyContainer"); i; ++i)
    ParsePropertyAnimationContainer (i, animation, property_prefix.c_str ());

  for (Parser::NamesakeIterator i = iter->First ("Property"); i; ++i)
  {
    PropertyAnimation new_property;

    ParsePropertyAnimation (i, new_property, property_prefix.c_str ());

    animation.Properties ().Add (new_property);
  }
}
