#include "shared.h"

using namespace scene_graph;
using namespace common;

namespace
{

class WordParser
{
  public:
    WordParser (const char* string)
      : pos ((unsigned char*)string)
    {
    }

    typedef stl::pair<const char*, const char*> Word;

    Word NextWord ()
    {
      if (!*pos)
        return Word ((const char*)pos, (const char*)pos);

      //cut leading spaces        
      
      bool loop = true;
      
      for (;loop; pos++)
      {
        switch (*pos)
        {
          case ' ':
          case '\t':
            break;
          default:
            loop = false;
            pos--;
            break;
        }
      }

      const unsigned char *first = 0, *last = 0;

      first = pos;
      
      loop = true;

      for (;loop; pos++)
      {
        switch (*pos)
        {
          case ';':
          case ',':
          case '\0':
            loop = false;
            pos--;
            break;          
          default:
            break;
        }
      }

      last = pos;

      if (*pos)
        pos++;

      if (last != first)
      {
        loop = true;
        
        for (--last; loop; last--)
        {
          switch (*last)
          {
            case ' ':
            case '\t':
              break;
            default:
              loop = false;
              last++;
              break;
          }
        }        

        ++last;
      }

      return Word ((const char*)first, (const char*)last);
    }

    bool EndOfString () const { return *pos == '\0'; }

  private:
    unsigned char* pos;
};

}

namespace scene_graph
{

void parse_attribute (const common::ParseNode& decl, const char* name, size_t size, float* value)
{
  ParseNode node = decl.First (name);
  
  if (!node || !node.AttributesCount ())
    return;

  const char* string_value = node.Attribute (0);
  
  WordParser parser (string_value);
  
  size_t token_index = 0;
  
  char parse_buffer [32];

  do
  {
    WordParser::Word word = parser.NextWord ();

    if (word.first != word.second)
    {
      size_t length = word.second - word.first;
      
      if (length >= sizeof (parse_buffer))
        raise_parser_exception (node, "Internal error: string is too big for tokenize '%s' (max token size is %u)", string_value, sizeof (parse_buffer) - 1);

      if (token_index >= size)
        raise_parser_exception (node, "Too many tokens in '%s' (expected %u tokens)", string_value, size);
        
      memcpy (parse_buffer, word.first, length);
      
      parse_buffer [length] = 0;      

      if (!xtl::io::read_and_cast<float> (parse_buffer, value [token_index]))
        raise_parser_exception (node, "Can't parse token '%s' while parsing value '%s'", parse_buffer, string_value);
        
      token_index++;
    }

  } while (!parser.EndOfString ());  

  if (token_index != size)
    raise_parser_exception (node, "Too little tokens in '%s' (expected %u tokens)", string_value, size);
}

}
