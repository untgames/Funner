#include <stdio.h>

#include <stl/string>

#include <common/strlib.h>
#include <common/exception.h>

#include "shared.h"

using namespace stl;
using namespace common;

struct Token
{
  string prefix;
  size_t argument_index;

  Token (const string& in_prefix, size_t in_index) : prefix (in_prefix), argument_index (in_index) {}
};

namespace
{

const size_t NO_ARGUMENT = ~0;

size_t parse_replacement (const char*& s)
{
  char* end;

  size_t index = strtoul (s+1, &end, 10);

  if (*end != '}')
    return NO_ARGUMENT;

  s = end + 1;

  return index;
}

void parse_event (const char* replacement, TokenArray& tokens)
{
  const char* prefix_begin = replacement, *s = replacement;
  
  string current_prefix;

  current_prefix.reserve (strlen (replacement));

  for (;;)
  {
    switch (*s)
    {
      case '\0':
        if (prefix_begin != s)
        {
          current_prefix.append (prefix_begin, s);
          
          tokens.push_back (Token (current_prefix, NO_ARGUMENT));
        }

        return;
      case '{':
      {
        if (s [1] == '{')
        {
          current_prefix.append (prefix_begin, ++s);

          prefix_begin = ++s;

          break;
        }

        const char* prefix_end = s;

        size_t argument_index = parse_replacement (s);

        if (argument_index != NO_ARGUMENT)
        {
          current_prefix.append (prefix_begin, prefix_end);
          
          tokens.push_back (Token (current_prefix, argument_index));

          prefix_begin = s;
          
          current_prefix.clear ();

          break;
        }
      }
      default:
        s++;
        break;
    }
  }
}

}

/*
   Конструктор/деструктор
*/

EventReplacer::EventReplacer (const char* input_event, const char* event_replacement, size_t in_id)
  : id (in_id)
{
  parse_event (event_replacement, replacement_tokens);
  
  event_wildcard = split (input_event, " ");

  if (event_wildcard.empty ())
    RaiseInvalidArgument ("input::EventReplacer::EventReplacer", "input_event", input_event, "Empty input event");
  if (replacement_tokens.empty ())
    RaiseInvalidArgument ("input::EventReplacer::EventReplacer", "event_replacement", event_replacement, "Empty event replacement");
}

EventReplacer::~EventReplacer ()
{
}

/*
   Выполнение замены
*/

bool EventReplacer::Replace (const vector<string>& event_components, string& result)
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
