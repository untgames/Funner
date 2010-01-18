#include "shared.h"

using namespace common;

namespace
{

/*
    Вспомогательные переменные
*/

char DUMMY_CHAR = ' '; //символ используемый для фиктивного затирания при лексическом разборе
char IDENTIFIER_CHAR_MAP [256]; //карта разрешенных символов разбора идентификатора

}

/*
    Конструктор
*/

XmlLexer::XmlLexer ()
{
  static bool is_identifier_char_map_initialized = false;

  if (!is_identifier_char_map_initialized)
  {
    memset (IDENTIFIER_CHAR_MAP, 0, sizeof IDENTIFIER_CHAR_MAP);

    static const char* spec_name_chars       = ".-_:#";
    static size_t      spec_name_chars_count = strlen (spec_name_chars);

    for (int i=0; i<256; i++)
      IDENTIFIER_CHAR_MAP [i] = isalnum (i) != 0;

    const unsigned char* s = (const unsigned char*)spec_name_chars;

    for (size_t i=0; i<spec_name_chars_count; i++, s++)
      IDENTIFIER_CHAR_MAP [*s] = true;

    is_identifier_char_map_initialized = true;
  }

  static char default_buffer [] = "";

  Reset (default_buffer);
}

/*
    Установка буфера разбора
*/

void XmlLexer::Reset (char* buffer)
{
  current_lexem        = XmlLexem_Undefined;
  current_status       = XmlLexerStatus_NoError;
  position             = buffer;
  cursor               = position;
  current_token        = "";
  line_number          = 1;
  line_start           = position;
  erased_char          = ' ';
  erased_char_position = &DUMMY_CHAR;
}

/*
    Установка ошибки разбора
*/

void XmlLexer::SetError (XmlLexerStatus error, const char* error_position = 0)
{
  current_status = error;
  current_lexem  = XmlLexem_Undefined;
  current_token  = "";
  cursor         = error_position ? error_position : position;
}

/*
    Утилиты разбора
*/

void XmlLexer::NextLine ()
{
  line_number++;

  line_start = ++position;
}

void XmlLexer::SkipBlockComment ()
{
  for (;; position++)
    switch (*position)
    {
      case '\0':
        SetError (XmlLexerStatus_UnclosedComment);
        return;
      case '-':
        if (position [1] == '-' && position [2] == '>')
        {
          position+=3;
          return;
        }
        break;
      case '\n':
        NextLine ();
        break;
    }
}

void XmlLexer::Skip ()
{
  for (;;)
    switch (*position)
    {
      case '\n':
        NextLine();
        break;
      case '\t':
      case '\r':
      case ' ':
        position++;
        break;
      case '<':
        if (position [1] == '!' && position [2] == '-' && position [3] == '-')
        {
          position += 4;

          SkipBlockComment ();

          break;
        }
      default:
        return;
    }
}

void XmlLexer::ReadShiftedString (char border)
{
  char* write_position = position;

  for (;; position++, write_position++)
    switch (*position)
    {
      case '\n':
      case '\0':
      case '\r':
        SetError (XmlLexerStatus_UnclosedString, current_token);
        return;
      case '&':
      {
        struct Marker
        {
          const char* marker;
          char        replacement;
        };

        static Marker       markers []    = {{"lt", '<'}, {"gt", '>'}, {"amp", '&'}, {"aposition", '\''}, {"quot", '"'}};
        static const size_t markers_count = sizeof markers / sizeof *markers;

        const Marker* m = markers;

        for (size_t i=0; i<markers_count; i++, m++)
        {
          const char* s1 = m->marker;
          char*       s2 = position + 1;

          for (; *s1 && *s2 && *s1 == *s2; s1++, s2++);

          if (!*s1) //соответствие найдено
          {
            position        = s2;
            *write_position = m->replacement;

            break;
          }
        }

        if (m == markers + markers_count) //если соответствие не найдено
          *write_position = *position;

        break;
      }
      default:
        if (*position == border)
        {
          *write_position = '\0';
          position++;

          return;
        }
        else
        {
          *write_position = *position;
        }

        break;
    }
}

void XmlLexer::ReadString (char border)
{
  current_token = position;
  current_lexem = XmlLexem_String;
  cursor        = position;

  for (;; position++)
    switch (*position)
    {
      case '\n':
      case '\0':
      case '\r':
        SetError (XmlLexerStatus_UnclosedString, current_token);
        return;
      case '&':
        ReadShiftedString (border);
        return;
      default:
        if (*position == border)
        {
          *position++ = '\0';
          return;
        }

        break;
    }
}

void XmlLexer::ReadTextString ()
{
  char* write_position = position;

  current_token = position;
  current_lexem = XmlLexem_String;
  cursor        = position;

  for (;; position++, write_position++)
    switch (*position)
    {
      case '&':
      {
        struct Marker
        {
          const char* marker;
          char        replacement;
        };

        static Marker       markers []    = {{"lt", '<'}, {"gt", '>'}, {"amp", '&'}};
        static const size_t markers_count = sizeof markers / sizeof *markers;

        const Marker* m = markers;

        for (size_t i=0; i<markers_count; i++, m++)
        {
          const char* s1 = m->marker;
          char*       s2 = position + 1;

          for (; *s1 && *s2 && *s1 == *s2; s1++, s2++);

          if (!*s1) //соответствие найдено
          {
            position        = s2;
            *write_position = m->replacement;

            break;
          }
        }

        if (m == markers + markers_count) //если соответствие не найдено
        {
          SetError (XmlLexerStatus_UnclosedString, current_token);
          return;
        }

        break;
      }
      case '\n':
      case '\0':
      case '\r':
      case ' ':
      case '<':
        if (write_position == position)
        {
          erased_char          = *write_position;
          erased_char_position = position;
        }

        *write_position = '\0';
        return;
      default:
        *write_position = *position;
    }
}

void XmlLexer::ReadCData ()
{
  current_token = position;
  current_lexem = XmlLexem_CData;
  cursor        = position;

  for (;; position++)
    switch (*position)
    {
      case '\n':
        NextLine ();
        break;
      case ']':
        if (position [1] == ']' && position [2] == '>')
        {
          erased_char          = *position;
          erased_char_position = position;
          *position            = '\0';

          position += 3;

          return;
        }

        break;
      case '\0':
        SetError (XmlLexerStatus_UnclosedCData, current_token);
        return;
    }
}

void XmlLexer::ReadIdentifier (bool identifier)
{
  current_token = position;
  current_lexem = identifier ? XmlLexem_Identifier : XmlLexem_Token;
  cursor        = position;

  while (IDENTIFIER_CHAR_MAP [*(unsigned char*)position]) position++;

  switch (*position)
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
      erased_char          = *position;
      erased_char_position = position;
      *position            = '\0';
      break;
    default:
      SetError (XmlLexerStatus_WrongIdentifier, current_token);
      break;
  }
}

void XmlLexer::ProcessTagBeginBracket (bool process_cdata)
{
  static const char CDATA_TAG []   = "[CDATA[";
  static const int  CDATA_TAG_SIZE = strlen (CDATA_TAG);

  switch (*++position)
  {
    case '/':
      position++;
      current_lexem = XmlLexem_TagCloseFrame;
      break;
    case '?':
      position++;
      current_lexem = XmlLexem_InstructionBeginBracket;
      break;
    case '!':
      position++;

      current_lexem = XmlLexem_Keyword;

      if (process_cdata && !strncmp (position, CDATA_TAG, CDATA_TAG_SIZE))
      {
        position += CDATA_TAG_SIZE;

        ReadCData ();

        current_lexem = XmlLexem_CData;
      }

      break;
    default:
      current_lexem = XmlLexem_TagBeginBracket;
      break;
  }
}

XmlLexem XmlLexer::NextLexem ()
{
  char* old_erased_char_position = erased_char_position;

  *erased_char_position = erased_char;
  current_lexem         = XmlLexem_Undefined;
  current_status        = XmlLexerStatus_NoError;
  current_token         = "";
  erased_char           = ' ';
  erased_char_position  = &DUMMY_CHAR;

  Skip ();

  cursor = position;

  if (current_status == XmlLexerStatus_NoError)
  {
    switch (*position)
    {
      case '\0':
        current_lexem = XmlLexem_EndOfFile;
        break;
      case '<':
        ProcessTagBeginBracket (false);
        break;
      case '?':
        if (position [1] == '>')
        {
          position      += 2;
          current_lexem  = XmlLexem_InstructionEndBracket;
        }
        else
        {
          SetError (XmlLexerStatus_WrongChar, position++);
        }

        break;
      case '>':
        position++;
        current_lexem = XmlLexem_TagEndBracket;
        break;
      case '[':
        position++;
        current_lexem = XmlLexem_SectorBeginBracket;
        break;
      case ']':
        position++;
        current_lexem = XmlLexem_SectorEndBracket;
        break;
      case '/':
        if (position [1] == '>')
        {
          position += 2;
          current_lexem = XmlLexem_TagClose;
        }
        else
        {
          SetError (XmlLexerStatus_WrongChar, position++);
        }

        break;
      case '"':
      case '\'':
        ReadString (*position++);
        break;
      case '=':
        position++;
        current_lexem = XmlLexem_Equal;
        break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      case '8': case '9': case '-': case '.':
        ReadIdentifier (false);
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
        ReadIdentifier (true);
        break;
      case '\n':
        NextLine ();
        break;
      default:
        erased_char_position = position;
        erased_char          = *position;

        SetError (XmlLexerStatus_WrongChar, position++);
        break;
    }
  }

  *old_erased_char_position = '\0';

  return current_lexem;
}

XmlLexem XmlLexer::NextTextLexem ()
{
  char* old_erased_char_position = erased_char_position;

  *erased_char_position = erased_char;
  current_lexem         = XmlLexem_Undefined;
  current_status        = XmlLexerStatus_NoError;
  current_token         = "";
  erased_char           = ' ';
  erased_char_position  = &DUMMY_CHAR;

  Skip ();

  cursor = position;

  if (current_status == XmlLexerStatus_NoError)
  {
    switch (*position)
    {
      case '\0':
        current_lexem = XmlLexem_EndOfFile;
        break;
      case '<':
        ProcessTagBeginBracket (true);
        break;
      case ']':
        if (position [1] == ']' && position [2] == '>')
          SetError (XmlLexerStatus_WrongChar, position++);
        break;
      case '\n':
        NextLine ();
        break;
      default:
        ReadTextString ();
    }
  }

  *old_erased_char_position = '\0';

  return current_lexem;
}
