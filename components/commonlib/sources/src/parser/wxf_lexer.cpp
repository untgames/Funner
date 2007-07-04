#include <ctype.h>
#include "wxf_lexer.h"

using namespace common;

//!!добавить таблицы в разборе identifier/value

char* WXFLexer::null_ptr  = NULL;
char  WXFLexer::null_char = ' ';
static char  default_buf [] = "";

WXFLexer::WXFLexer ()
{
  reset (default_buf);
}

void WXFLexer::reset (char* buf)
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
}

void WXFLexer::SetError (Error err,char* _pos)
{
  cur_error = err;
  cur_lexem = UNDEFINED;
  cur_token = NULL;
  cursor    = _pos?_pos:pos;
}

void WXFLexer::SetErrorAndSkip (Error err,char* _pos)
{
  cur_error = err;
  cur_lexem = UNDEFINED;
  cur_token = _pos;
  cursor    = _pos?_pos:pos;

  skip_wrong_token ();  

  erased_char     = *pos;
  erased_char_pos = pos;
  *pos            = 0;
}

void WXFLexer::skip_rest_of_line ()
{
  for (;;pos++)
    switch (*pos)
    {
      case '\n':
      case '\0':
        return;
    }  
}

void WXFLexer::nextline ()
{
  line_number++;
  line_start = pos+1;
}

void WXFLexer::skip_block_comment ()
{
  for (;;pos++)
    switch (*pos)
    {
      case '\0':
        SetError (UNCLOSED_COMMENT);
        return;
      case '*':
        if (pos [1] == '/')
        {
          pos+=2;
          return;
        }
        break;
      case '\n':
        nextline ();
    }
}

void WXFLexer::skip ()
{
  for (;;)
    switch (*pos)
    {
      case '\t':
      case '\r':
      case ' ':
        pos++;
        break;
      case '/':
        switch (pos [1])
        {
          case '/': 
            pos+=2;
            skip_rest_of_line ();
            return;
          case '*':
            pos+=2;
            skip_block_comment ();
            return;
        }
      default:
        return;
    }
}

void WXFLexer::skip_wrong_token ()
{
  for (;;pos++)
    switch (*pos)  
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

void WXFLexer::read_string (char border)
{
  cur_token = pos;
  cur_lexem = STRING;
  cursor    = pos;

  for (;;)
    switch (*pos)  
    {
      case '\0':
      case '\n':
      case '\r':
        SetErrorAndSkip (UNCLOSED_STRING,cur_token);
        return;
      default:
        if (*pos++ == border)
        {
          pos [-1] = 0;
          return;
        }  
        break;
    }
}

void WXFLexer::read_directive ()
{
  char* start_pos = pos;

  pos++;

  skip ();

  switch (*pos)
  {
    case '\0':
    case '\r':
    case '\n':
      SetError (WRONG_CHAR,start_pos);
      return;
  }

  if (!isalpha (*pos))
  {
    SetErrorAndSkip (WRONG_DIRECTIVE,pos);
    skip_rest_of_line ();
    return;
  }
      
  read_identifier ();

  if (cur_error != NO_ERROR)
  {
    cur_error = WRONG_DIRECTIVE;
    skip_rest_of_line ();
  }
  else cur_lexem = DIRECTIVE;
}

void WXFLexer::read_identifier ()
{
  cur_token = pos;
  cur_lexem = IDENTIFIER;
  cursor    = pos;

  while (isalnum (*pos) || *pos == '_') pos++;

  switch (*pos)
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
      erased_char     = *pos;
      erased_char_pos = pos;
      *pos = 0;
      break;
    default:
      SetErrorAndSkip (WRONG_IDENTIFIER,cur_token);
      break;
  }
}

void WXFLexer::read_value ()
{
  bool hex_value = false;

  cur_token = pos;
  cur_lexem = VALUE;
  cursor    = pos;
  
  switch (*pos)
  {
    case '+':
    case '-': pos++;
    default:  break;
  }
  
  if (*pos == '0')
  {
    pos++;
    
    switch (*pos)
    {
      case 'x':
      case 'X': hex_value = true; pos++;
      default:  break;
    }
  }
  
  if (!hex_value)
  {
    char* start_pos = pos;    
    
    while (isdigit (*pos)) pos++;

    if (*pos == '.')
    { 
      while (isdigit (*++pos));

      if (pos - start_pos == 1)
      {
        SetErrorAndSkip (WRONG_VALUE,cur_token);
        return;
      }
    }
    
    switch (*pos)
    {
      case 'e': case 'E':
      {
        switch (*++pos)
        {
          case '-':
          case '+':
            pos++;
            break;
          default:
            break;
        }
        
        char* start_pos = pos;
        
        while (isdigit (*pos)) pos++;
        
        if (start_pos == pos)
        {
          SetErrorAndSkip (WRONG_VALUE,cur_token);
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
    char* start_pos = pos;
    
    for (bool loop=true;loop;)
      switch (*pos)
      {
        default:
          loop = false;
          break;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          pos++;
          break;
      }
      
    if (start_pos == pos)
    {
      SetErrorAndSkip (WRONG_VALUE,cur_token);
      return;
    }
  }
  
  switch (*pos)
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
      erased_char     = *pos;
      erased_char_pos = pos;
      *pos = 0;
      break;
    default:
      SetErrorAndSkip (WRONG_VALUE,cur_token);
      break;
  }
}

WXFLexer::Lexem WXFLexer::next ()
{
  char* old_erased_char_pos = erased_char_pos;

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
      case '#':
        read_directive ();
        break;
      case '{':  
        pos++;
        cur_lexem = BEGIN_FRAME;  
        break;
      case '}':  
        pos++;
        cur_lexem = END_FRAME;    
        break;
      case '(':  
        pos++;
        cur_lexem = BEGIN_BRACKET; 
        break;
      case ')':  
        pos++;
        cur_lexem = END_BRACKET;   
        break;       
      case '\n':
        nextline ();
        cursor = ++pos;
        cur_lexem = NEW_LINE;
        break;
      case '"':
      case '\'':
        read_string (*pos++);
        break;  
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      case '8': case '9': case '.': case '+':
      case '-':
        read_value ();
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
        read_identifier ();
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
