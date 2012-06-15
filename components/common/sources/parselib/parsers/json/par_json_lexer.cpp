#include "shared.h"

using namespace common;

namespace
{

/*
    Вспомогательные переменные
*/

char         DUMMY_CHAR         = ' ';  //символ используемый для фиктивного затирания при лексическом разборе
const size_t MAX_UTF8_CHAR_SIZE = 6;    //максимальный размер представления одного символа в utf-8 кодировке

}

/*
    Конструктор
*/

JsonLexer::JsonLexer ()
{
  static char default_buffer [] = "";

  Reset (default_buffer);
}

/*
    Установка буфера разбора
*/

void JsonLexer::Reset (char* buffer)
{
  current_lexem        = JsonLexem_Undefined;
  current_status       = JsonLexerStatus_NoError;
  position             = buffer;
  cursor               = position;
  current_token        = "";
  line_number          = 1;
  line_start           = position;
  erased_char          = DUMMY_CHAR;
  erased_char_position = &DUMMY_CHAR;
}

/*
    Установка ошибки разбора
*/

void JsonLexer::SetError (JsonLexerStatus error, const char* error_position = 0)
{
  current_status = error;
  current_lexem  = JsonLexem_Undefined;
  current_token  = "";
  cursor         = error_position ? error_position : position;
}

/*
    Утилиты разбора
*/

void JsonLexer::NextLine ()
{
  line_number++;

  line_start = ++position;
}

void JsonLexer::Skip ()
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
      default:
        return;
    }
  }
}

void JsonLexer::ReadSymbolReference (char*& write_position)
{
  char next_symbol = position [1];

  if (next_symbol == 'u')
  {
    char* symbol_code_start = position + 2;
    int   base              = 16;

    char*         symbol_code_end = 0;
    unsigned long symbol_code     = strtoul (symbol_code_start, &symbol_code_end, base);

    if (!symbol_code || !symbol_code_end || symbol_code_end - symbol_code_start != 4)
      SetError (JsonLexerStatus_InvalidCharacterReference, current_token);
    else
    {
      position = symbol_code_end - 1;

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
        SetError (JsonLexerStatus_InvalidCharacterReference, current_token);
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
      char marker;
      char replacement;
    };

    static Marker       markers []    = {{'"', '"'}, {'\\', '\\'}, {'/', '/'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'}};
    static const size_t markers_count = sizeof markers / sizeof *markers;

    const Marker* m = markers;

    for (size_t i=0; i<markers_count; i++, m++)
    {
      if (m->marker == next_symbol) //соответствие найдено
      {
        position += 1;
        write_position [0] = m->replacement;

        break;
      }
    }

    if (m == markers + markers_count) //если соответствие не найдено
      SetError (JsonLexerStatus_InvalidCharacterReference, current_token);
  }
}

void JsonLexer::ReadString ()
{
  current_token = position;
  current_lexem = JsonLexem_String;
  cursor        = position;

  char* write_position = position;

  for (;; write_position++, position++)
  {
    switch (*position)
    {
      case '\n':
      case '\0':
      case '\r':
        SetError (JsonLexerStatus_UnclosedString, current_token);
        return;
      case '\\':
        ReadSymbolReference (write_position);
        break;
      case '"':
        *write_position = '\0';
        position++;
        return;
      default:
        *write_position = *position;
        break;
    }
  }
}

void JsonLexer::ReadNumber ()
{
  current_token = position;
  current_lexem = JsonLexem_Number;
  cursor        = position;

  if (*position == '-')
    position++;

  char* start_position = position;

  while (isdigit (*position)) position++;

  if (position == start_position)
  {
    SetError (JsonLexerStatus_WrongNumber, current_token);
    return;
  }

  if (*position == '.')
  {
    start_position = position;

    while (isdigit (*++position));

    if (position - start_position == 1)
    {
      SetError (JsonLexerStatus_WrongNumber, current_token);
      return;
    }
  }

  switch (*position)
  {
    case 'e': case 'E':
    {
      switch (*++position)
      {
        case '-':
        case '+':
          position++;
          break;
        default:
          break;
      }

      start_position = position;

      while (isdigit (*position)) position++;

      if (start_position == position)
      {
        SetError (JsonLexerStatus_WrongNumber, current_token);
        return;
      }

      break;
    }
    default:
      break;
  }

  switch (*position)
  {
    case '\t':
    case ' ':
    case '\r':
    case '\n':
    case ',':
    case ']':
    case '}':
    case '\0':
      erased_char          = *position;
      erased_char_position = position;
      *position            = 0;
      break;
    default:
      SetError (JsonLexerStatus_WrongNumber, current_token);
      break;
  }
}

void JsonLexer::ReadKeyword (JsonLexem lexem, const char* keyword)
{
  current_token = position;
  current_lexem = lexem;
  cursor        = position;

  const char* s1 = keyword;
  char*       s2 = position;

  for (; *s1 && *s2 && *s1 == *s2; s1++, s2++);

  if (*s1) //соответствие не найдено
  {
    SetError (JsonLexerStatus_WrongKeyword, current_token);
    position++;
    return;
  }

  position = s2;

  switch (*position)
  {
    case '\t':
    case ' ':
    case '\r':
    case '\n':
    case ',':
    case ']':
    case '}':
    case '\0':
      erased_char          = *position;
      erased_char_position = position;
      *position            = 0;
      break;
    default:
      SetError (JsonLexerStatus_WrongKeyword, current_token);
      break;
  }
}

JsonLexem JsonLexer::NextLexem ()
{
  char* old_erased_char_position = erased_char_position;

  *erased_char_position = erased_char;
  current_lexem         = JsonLexem_Undefined;
  current_status        = JsonLexerStatus_NoError;
  current_token         = "";
  erased_char           = ' ';
  erased_char_position  = &DUMMY_CHAR;

  Skip ();

  cursor = position;

  if (current_status == JsonLexerStatus_NoError)
  {
    switch (*position)
    {
      case '\0':
        current_lexem = JsonLexem_EndOfFile;
        break;
      case '[':
        position++;
        current_lexem = JsonLexem_ArrayBeginBracket;
        break;
      case ']':
        position++;
        current_lexem = JsonLexem_ArrayEndBracket;
        break;
      case '{':
        position++;
        current_lexem = JsonLexem_ObjectBeginBracket;
        break;
      case '}':
        position++;
        current_lexem = JsonLexem_ObjectEndBracket;
        break;
      case ':':
        position++;
        current_lexem = JsonLexem_Colon;
        break;
      case ',':
        position++;
        current_lexem = JsonLexem_Comma;
        break;
      case '"':
        position++;
        ReadString ();
        break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      case '8': case '9': case '-':
        ReadNumber ();
        break;
      case 't':
        ReadKeyword (JsonLexem_Boolean, "true");
        break;
      case 'f':
        ReadKeyword (JsonLexem_Boolean, "false");
        break;
      case 'n':
        ReadKeyword (JsonLexem_Null, "null");
        break;
      default:
        erased_char_position = position;
        erased_char          = *position;

        SetError (JsonLexerStatus_WrongChar, position++);

        break;
    }
  }

  *old_erased_char_position = '\0';

  return current_lexem;
}

/*
   Проверка есть ли в текущем массиве вложенные массивы или объекты
*/

bool JsonLexer::ArrayHasObjects ()
{
  for (const char* cur_char = position; *cur_char; cur_char++)
  {
    switch (*cur_char)
    {
      case '{':
      case '[':
        return true;
      case ']':
        return false;
    }
  }

  return false;
}
