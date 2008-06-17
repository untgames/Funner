#include "shared.h"

using namespace stl;
using namespace common;
using namespace input;

namespace
{

size_t NO_ARGUMENT = ~0;

}

struct input::EventTranslator::Token
{
  string prefix;
  size_t argument_index;

  Token (const char* in_prefix, size_t in_index) : prefix (in_prefix), argument_index (in_index) {}
};

namespace input
{

/*
   Конструктор/деструктор
*/

EventTranslator::EventTranslator (const char* input_event, const char* event_replacement, const char* tag)
  : str_event_wildcard (input_event), str_event_replacement (event_replacement), str_tag (tag)
{
  parse_format_string (event_replacement, xtl::bind (&EventTranslator::ParseFormatString, this, _1, _2));

  split_event (input_event, event_wildcard);

  if (event_wildcard.empty ())
    throw xtl::make_argument_exception ("input::EventTranslator::EventTranslator", "input_event", input_event, "Empty input event");
  if (replacement_tokens.empty ())
    throw xtl::make_argument_exception ("input::EventTranslator::EventTranslator", "event_replacement", event_replacement, "Empty event replacement");
}

EventTranslator::~EventTranslator ()
{
}

/*
   Выполнение замены
*/

bool EventTranslator::Replace (const vector<string>& event_components, string& result)
{
  if (event_components.size () != event_wildcard.size ())
    return false;

  for (size_t i = 0; i < event_components.size (); i++)
    if (!wcimatch (event_components[i].c_str (), event_wildcard[i].c_str ()))
      return false;

  for (size_t i = 0; i < replacement_tokens.size (); i++)
  {
    result.append (replacement_tokens[i].prefix);

    if (replacement_tokens[i].argument_index >= event_components.size ())
      continue;

    if (replacement_tokens[i].argument_index != NO_ARGUMENT)
      result.append (event_components[replacement_tokens[i].argument_index]);  
  }
      
  return true;
}

/*
   Разбиение строки замены на составляющие
*/

void EventTranslator::ParseFormatString (const char* prefix, const char* replacement_tag)
{
  size_t argument_index = xtl::io::get<size_t> (replacement_tag, NO_ARGUMENT);

  replacement_tokens.push_back (Token (prefix, argument_index));
}

}
