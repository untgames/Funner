#include <ctype.h>
#include <string.h>
#include "xml_lexer.h"

using namespace common;

char* XMLLexer::null_ptr  = NULL;
char  XMLLexer::null_char = ' ';
static char  default_buf [] = "";

XMLLexer::XMLLexer ()
{
  reset (default_buf);
}

void XMLLexer::reset (char* buf)
{
  cur_lexem   = UNDEFINED;
  cur_error   = NO_ERROR;
  pos         = buf;
  cursor      = pos;
  cur_token   = NULL;
  line_number = 1;
  line_start  = pos;

  erased_char     = ' ';
  erased_char_pos = &null_char;

  memset (name_char_map,0,sizeof (name_char_map));

  static const char* spec_name_chars       = ".-_:#";
  static size_t      spec_name_chars_count = strlen (spec_name_chars);

  for (int i=0;i<256;i++)
    name_char_map [i] = isalnum (i) != 0;

  const unsigned char* s = (const unsigned char*)spec_name_chars;

  for (size_t i=0;i<spec_name_chars_count;i++,s++)
    name_char_map [*s] = true;
}

void XMLLexer::SetError (Error err,char* _pos=NULL)
{
  cur_error = err;
  cur_lexem = UNDEFINED;
  cur_token = NULL;
  cursor    = _pos?_pos:pos;
}

void XMLLexer::nextline ()
{
  line_number++;
  line_start = ++pos;
}

void XMLLexer::skip_block_comment ()
{
  for (;;pos++)
    switch (*pos)
    {
      case '\0':
        SetError (UNCLOSED_COMMENT);
        return;
      case '-':
        if (pos [1] == '-' && pos [2] == '>')
        {
          pos+=3;
          return;
        }
        break;
      case '\n':
        nextline ();
        break;
    }
}

void XMLLexer::skip ()
{
  for (;;)
    switch (*pos)
    {
      case '\n':
        nextline();
        break;
      case '\t':
      case '\r':
      case ' ':
        pos++;
        break;
      case '<':
        if (pos [1] == '!' && pos [2] == '-' && pos [3] == '-')
        {
          pos += 4;
          skip_block_comment ();
          break;
        }
      default:
        return;
    }
}

struct Marker
{
  const char* marker;
  size_t      marker_size;
  char        replacement;
};

void XMLLexer::read_shifted_string (char border)
{
  static Marker markers [] = {{"lt",2,'<'},{"gt",2,'>'},{"amp",3,'&'},{"apos",4,'\''},{"quot",4,'"'}};
  const int MARKERS_NUM = sizeof (markers)/sizeof (Marker);

  char* write_pos = pos;

  for (;;pos++,write_pos++)
    switch (*pos)
    {
      case '\0':
      case '\n':
      case '\r':
        SetError (UNCLOSED_STRING,cur_token);
        return;
      case '&':
      {
        Marker* m = markers;

        for (int i=0;i<MARKERS_NUM;i++,m++)
          if (!strncmp (pos+1,m->marker,m->marker_size))
          {
            *write_pos = m->replacement;
            pos += m->marker_size;
            break;
          }

        if (m == markers+MARKERS_NUM) //если соответствие не найдено
          *write_pos = *pos;

        break;
      }
      default:
        if (*pos == border)
        {
          *write_pos = 0;
          pos++;
          return;
        }
        else
          *write_pos = *pos;
  
        break;
    }
}

void XMLLexer::read_string (char border)
{
  cur_token = pos;
  cur_lexem = STRING;
  cursor    = pos;

  for (;;pos++)
    switch (*pos)
    {
      case '\0':
      case '\n':
      case '\r':
        SetError (UNCLOSED_STRING,cur_token);
        return;
      case '&':
        read_shifted_string (border);
        return;
      default:
        if (*pos == border)
        {
          *pos++ = 0;
          return;
        }
        break;
    }
}

void XMLLexer::read_cdata ()
{
  cur_token = pos;
  cur_lexem = CDATA;
  cursor    = pos;

  for (;;pos++)
    switch (*pos)
    {
      case ']':
        if (pos [1] == ']' && pos [2] == '>')
        {
          erased_char     = *pos;
          erased_char_pos = pos;
          *pos            = 0;
          pos += 3;
          return;
        }
        break;
      case '\0':
        SetError (UNCLOSED_CDATA, cur_token);
        return;
    }
}

void XMLLexer::read_identifier (bool identifier)
{
  cur_token = pos;
  cur_lexem = identifier ? IDENTIFIER : TOKEN;
  cursor    = pos;

  while (name_char_map [*(unsigned char*)pos]) pos++;

  switch (*pos)
  {
    case '<':
    case '>':
    case '[':
    case ']':
    case '/':
    case '\t':
    case ' ':
    case '\r':
    case '\n':
    case ':':
    case '"':
    case '=':
    case '\'':
    case '\0':
      erased_char     = *pos;
      erased_char_pos = pos;
      *pos = 0;
      break;
    default:
      SetError (WRONG_IDENTIFIER,cur_token);
      break;
  }
}

XMLLexer::Lexem XMLLexer::next ()
{
  static const char CDATA_TAG [] = "[CDATA[";
  const int CDATA_TAG_SIZE = sizeof (CDATA_TAG)-1;

  char *old_erased_char_pos = erased_char_pos;

  *erased_char_pos = erased_char;

  cur_lexem   = UNDEFINED;
  cur_error   = NO_ERROR;
  cur_token   = NULL;
  erased_char = ' ';
  erased_char_pos = &null_char;

  skip ();

  cursor = pos;

  if (cur_error == NO_ERROR)
  {
    switch (*pos)
    {
      case '\0':
        cur_lexem = END_OF_FILE;
        break;
      case '<':
        switch (*++pos)
        {
          case '/':
            pos++;
            cur_lexem = TAG_CLOSE;
            break;
          case '?':
            pos++;
            cur_lexem = INSTRUCTION_BEGIN_BRACKET;
            break;
          case '!':
            pos++;
            cur_lexem = KEYWORD;
            if (!strncmp (pos,CDATA_TAG,CDATA_TAG_SIZE))
            {
              pos += CDATA_TAG_SIZE;
              read_cdata ();
              cur_lexem = CDATA;
            }
            break;
          default:
            cur_lexem = TAG_BEGIN_BRACKET;
            break;
        }
        break;
      case '?':
        if (pos[1] == '>')
        {
          pos += 2;
          cur_lexem = INSTRUCTION_END_BRACKET;
        }
        else
          SetError (WRONG_CHAR,pos++);
        break;
      case '>':
        pos++;
        cur_lexem = TAG_END_BRACKET;
        break;
      case '[':
        pos++;
        cur_lexem = SECTOR_BEGIN_BRACKET;
        break;
      case ']':
        pos++;
        cur_lexem = SECTOR_END_BRACKET;
        break;
      case '/':
        if (pos[1] == '>')
        {
          pos += 2;
          cur_lexem = TAG_CLOSE;
        }
        else
          SetError (WRONG_CHAR, pos++);
        break;
      case '"':
      case '\'':
        read_string (*pos++);
        break;
      case '=':
        pos++;
        cur_lexem = EQUAL;
        break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      case '8': case '9': case '-': case '.':
        read_identifier (false);
        break;
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
      case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
      case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
      case 'Y': case 'Z':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
      case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
      case 's': case 't': case 'u': case 'v': case 'w': case 'x':
      case 'y': case 'z':
      case '_': case ':': case '#':
        read_identifier (true);
        break;
      default:
        erased_char_pos = pos;
        erased_char     = *pos;      
        SetError (WRONG_CHAR,pos++);
        break;
    }
  }

  *old_erased_char_pos = 0;

  return cur_lexem;
}
