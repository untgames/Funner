#include "shared.h"

using namespace common;

//!!добавить таблицы в разборе identifier/value

namespace
{

/*
    Константы
*/

char DUMMY_CHAR = ' '; //символ используемый для фиктивного затирания при лексическом разборе

}

/*
    Конструктор
*/

WxfLexer::WxfLexer ()
{
  static char DEFAULT_BUFFER [] = "";

  Reset (DEFAULT_BUFFER);
}

/*
    Сброс буфера разбора
*/

void WxfLexer::Reset (char* buffer)
{
  current_lexem        = WxfLexem_Undefined;
  current_status       = WxfLexerStatus_NoError;
  position             = buffer;
  cursor               = position;
  current_token        = 0;
  line_number          = 1;
  line_start           = position;
  erased_char          = ' ';
  erased_char_position = &DUMMY_CHAR;
}

/*
    Установка ошибки
*/

void WxfLexer::SetError (WxfLexerStatus status, char* error_position)
{
  current_status = status;
  current_lexem  = WxfLexem_Undefined;
  current_token  = 0;
  cursor         = error_position ? error_position : position;
}

void WxfLexer::SetErrorAndSkip (WxfLexerStatus status, char* error_position)
{
  current_status = status;
  current_lexem  = WxfLexem_Undefined;
  current_token  = error_position;
  cursor         = error_position ? error_position : position;

  SkipWrongToken ();  

  erased_char          = *position;
  erased_char_position = position;
  *position            = 0;
}

/*
    Утилиты разбора
*/

void WxfLexer::SkipRestOfLine ()
{
  for (;; position++)
    switch (*position)
    {
      case '\n':
      case '\0':
        return;
    }
}

void WxfLexer::NextLine ()
{
  line_number++;

  line_start = position + 1;
}

void WxfLexer::SkipBlockComment ()
{
  for (;; position++)
    switch (*position)
    {
      case '\0':
        SetError (WxfLexerStatus_UnclosedComment);
        return;
      case '*':
        if (position [1] == '/')
        {
          position += 2;
          return;
        }
        break;
      case '\n':
        NextLine ();
        break;
    }
}

void WxfLexer::Skip ()
{
  for (;;)
    switch (*position)
    {
      case '\t':
      case '\r':
      case ' ':
        position++;
        break;
      case '/':
        switch (position [1])
        {
          case '/': 
            position += 2;
            SkipRestOfLine ();
            return;
          case '*':
            position += 2;
            SkipBlockComment ();
            return;
        }
      default:
        return;
    }
}

void WxfLexer::SkipWrongToken ()
{
  for (;; position++)
    switch (*position)  
    {
      case '{':
      case '}':
      case '(':
      case ')':
      case '/':
      case '\t':
      case ' ':
      case '\r':
      case '\n':
      case '"':
      case '\'':
      case '\0': return;
      default:   break;        
    }
}

void WxfLexer::ReadString (char border)
{
  current_token = position;
  current_lexem = WxfLexem_String;
  cursor        = position;

  for (;;)
    switch (*position)  
    {
      case '\0':
      case '\n':
      case '\r':
        SetErrorAndSkip (WxfLexerStatus_UnclosedString, current_token);
        return;
      default:
        if (*position++ == border)
        {
          position [-1] = '\0';
          return;
        }
        break;
    }
}

void WxfLexer::ReadDirective ()
{
  char* start_position = position;

  position++;

  Skip ();

  switch (*position)
  {
    case '\0':
    case '\r':
    case '\n':
      SetError (WxfLexerStatus_WrongChar, start_position);
      return;
  }

  if (!isalpha (*position))
  {
    SetErrorAndSkip (WxfLexerStatus_WrongDirective, position);    
    SkipRestOfLine ();

    return;
  }
      
  ReadIdentifier ();

  if (current_status != WxfLexerStatus_NoError)
  {
    current_status = WxfLexerStatus_WrongDirective;

    SkipRestOfLine ();
  }
  else
  {
    current_lexem = WxfLexem_Directive;
  }
}

void WxfLexer::ReadIdentifier ()
{
  current_token = position;
  current_lexem = WxfLexem_Identifier;
  cursor        = position;

  while (isalnum (*position) || *position == '_') position++;

  switch (*position)
  {
    case '{':
    case '}':
    case '(':
    case ')':
    case '/':
    case '\t':
    case ' ':
    case '\r':
    case '\n':
    case '"':
    case '\'':
    case '\0':
      erased_char          = *position;
      erased_char_position = position;
      *position            = 0;
      break;
    default:
      SetErrorAndSkip (WxfLexerStatus_WrongIdentifier, current_token);
      break;
  }
}

void WxfLexer::ReadValue ()
{
  bool hex_value = false;

  current_token = position;
  current_lexem = WxfLexem_Value;
  cursor        = position;

  switch (*position)
  {
    case '+':
    case '-': position++;
    default:  break;
  }
  
  if (*position == '0')
  {
    position++;
    
    switch (*position)
    {
      case 'x':
      case 'X': hex_value = true; position++;
      default:  break;
    }
  }
  
  if (!hex_value)
  {
    char* start_position = position;    
    
    while (isdigit (*position)) position++;

    if (*position == '.')
    { 
      while (isdigit (*++position));

      if (position - start_position == 1)
      {
        SetErrorAndSkip (WxfLexerStatus_WrongValue, current_token);
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
        
        char* start_position = position;
        
        while (isdigit (*position)) position++;
        
        if (start_position == position)
        {
          SetErrorAndSkip (WxfLexerStatus_WrongValue, current_token);
          return;
        }
        
        break;
      }
      default:
        break;
    }
  }
  else
  {        
    char* start_position = position;
    
    for (bool loop=true; loop;)
      switch (*position)
      {
        default:
          loop = false;
          break;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          position++;
          break;
      }
      
    if (start_position == position)
    {
      SetErrorAndSkip (WxfLexerStatus_WrongValue, current_token);
      return;
    }
  }
  
  switch (*position)
  {      
    case '{':
    case '}':
    case '(':
    case ')':
    case '/':
    case '\t':
    case ' ':
    case '\r':
    case '\n':
    case '"':
    case '\'':
    case '\0':
      erased_char          = *position;
      erased_char_position = position;
      *position            = 0;
      break;
    default:
      SetErrorAndSkip (WxfLexerStatus_WrongValue, current_token);
      break;
  }
}

WxfLexem WxfLexer::NextLexem ()
{
  char* old_erased_char_position = erased_char_position;

  *erased_char_position = erased_char;
  current_lexem         = WxfLexem_Undefined;
  current_status        = WxfLexerStatus_NoError;
  current_token         = 0;
  erased_char           = ' ';
  erased_char_position  = &DUMMY_CHAR;

  Skip ();

  cursor = position;

  if (current_status == WxfLexerStatus_NoError)
  {
    switch (*position)
    {
      case '\0':
        current_lexem = WxfLexem_EndOfFile; 
        break;
      case '#':
        ReadDirective ();
        break;
      case '{':  
        position++;
        current_lexem = WxfLexem_BeginFrame;
        break;
      case '}':  
        position++;
        current_lexem = WxfLexem_EndFrame;
        break;
      case '(':  
        position++;
        current_lexem = WxfLexem_BeginBracket;
        break;
      case ')':  
        position++;
        current_lexem = WxfLexem_EndBracket;
        break;       
      case '\n':
        NextLine ();
        cursor = ++position;
        current_lexem = WxfLexem_NewLine;
        break;
      case '"':
      case '\'':
        ReadString (*position++);
        break;  
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      case '8': case '9': case '.': case '+':
      case '-':
        ReadValue ();
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
      case '_':
        ReadIdentifier ();
        break;
      default:
        erased_char_position = position;
        erased_char          = *position;
        
        SetError (WxfLexerStatus_WrongChar, position++);
        
        break;
    }
  }

  *old_erased_char_position = 0;

  return current_lexem;
}
