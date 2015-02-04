#include "shared.h"

using namespace common;

namespace
{

/*
    Вспомогательные переменные
*/

char         IDENTIFIER_CHAR_MAP [256]; //карта разрешенных символов разбора идентификатора
char         DUMMY_CHAR         = ' ';  //символ используемый для фиктивного затирания при лексическом разборе
const size_t MAX_UTF8_CHAR_SIZE = 6;    //максимальный размер представления одного символа в utf-8 кодировке

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
  for (;;)
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

        position++;
        break;
      case '\n':
        NextLine ();
        break;
      default:
        position++;
        break;
    }
}

void XmlLexer::Skip ()
{
  for (;;)
  {
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
}

void XmlLexer::ReadSymbolReference (char*& write_position)
{
  if (position [1] == '#')
  {
    char* symbol_code_start = position + 2;
    int   base              = 10;

    if (position [2] == 'x')
    {
      base = 16;
      symbol_code_start++;
    }

    char*    symbol_code_end = 0;
    uint32_t symbol_code     = strtoul (symbol_code_start, &symbol_code_end, base);

    if (!symbol_code || !symbol_code_end || *symbol_code_end != ';')
      SetError (XmlLexerStatus_InvalidCharacterReference, current_token);
    else
    {
      position = symbol_code_end;

      unsigned char utf8_buffer [MAX_UTF8_CHAR_SIZE];

      size_t source_size      = sizeof (symbol_code),
             destination_size = sizeof (utf8_buffer);

#ifdef __BIG_ENDIAN__
      Encoding source_encoding = Encoding_UTF32BE;
#else
      Encoding source_encoding = Encoding_UTF32LE;
#endif

      const void* source = &symbol_code;
      void* destination  = utf8_buffer;

      convert_encoding (source_encoding, source, source_size, Encoding_UTF8, destination, destination_size);

      if (destination_size == sizeof (utf8_buffer))
      {
        SetError (XmlLexerStatus_InvalidCharacterReference, current_token);
        return;
      }

      size_t converted_size = sizeof (utf8_buffer) - destination_size;

      memcpy (write_position, utf8_buffer, converted_size);

      write_position += converted_size - 1;
    }
  }
  else
  {
    struct Marker
    {
      const char* marker;
      char        replacement;
    };

    static Marker       markers []    = {{"lt;", '<'}, {"gt;", '>'}, {"amp;", '&'}, {"quot;", '"'}};
    static const size_t markers_count = sizeof markers / sizeof *markers;

    const Marker* m = markers;

    for (size_t i=0; i<markers_count; i++, m++)
    {
      const char* s1 = m->marker;
      char*       s2 = position + 1;

      for (; *s1 && *s2 && *s1 == *s2; s1++, s2++);

      if (!*s1) //соответствие найдено
      {
        position           = s2 - 1;
        write_position [0] = m->replacement;

        break;
      }
    }

    if (m == markers + markers_count) //если соответствие не найдено
      SetError (XmlLexerStatus_InvalidCharacterReference, current_token);
  }
}

void XmlLexer::ReadString (char border, char* terminators, size_t terminators_count, bool content)
{
  current_token = position;
  current_lexem = XmlLexem_String;
  cursor        = position;

  char* write_position = position;

  for (;; write_position++, position++)
  {
    for (size_t i = 0; i < terminators_count; i++)
      if (*position == terminators [i])
      {
        if (position != write_position)
          *write_position = '\0';
        else
        {
          erased_char          = *position;
          erased_char_position = position;
          *position            = '\0';
        }

        return;
      }

    switch (*position)
    {
      case '\n':
      case '\0':
      case '\r':
        SetError (XmlLexerStatus_UnclosedString, current_token);
        return;
      case '&':
        ReadSymbolReference (write_position);
        break;
      case ']':
        if (content && (position [1] == ']' && position [2] == '>'))
        {
          SetError (XmlLexerStatus_WrongChar, position);

          erased_char          = *position;
          erased_char_position = position;
          *position            = '\0';

          position += 3;
          return;
        }
      default:
        if (*position == border)
        {
          *write_position = '\0';
          position++;
          return;
        }

        *write_position = *position;

        break;
    }
  }
}

void XmlLexer::ReadContentString ()
{
  char terminators [] = { ' ', '<', '\n', '\t', '\r', '\0' };

  ReadString (0, terminators, sizeof (terminators) / sizeof (*terminators), true);
}

void XmlLexer::ReadCData ()
{
  current_token = position;
  current_lexem = XmlLexem_CData;
  cursor        = position;

  for (;;)
  {
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

        position++;
        break;
      case '\0':
        SetError (XmlLexerStatus_UnclosedCData, current_token);
        return;
      default:
        position++;
        break;
    }
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

XmlLexem XmlLexer::NextLexem (bool content)
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
        ProcessTagBeginBracket (content);
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
        if (content)
        {
          ReadContentString ();
        }
        else if (position [1] == '>')
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
        if (content)
        {
          ReadContentString ();
        }
        else
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
        if (content)
        {
          ReadContentString ();
        }
        else
        {
          ReadIdentifier (true);
        }

        break;
      case '\n':
        NextLine ();
        break;
      default:
        if (content)
        {
          ReadContentString ();
        }
        else
        {
          erased_char_position = position;
          erased_char          = *position;

          SetError (XmlLexerStatus_WrongChar, position++);
        }

        break;
    }
  }

  *old_erased_char_position = '\0';

  return current_lexem;
}
