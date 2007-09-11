#include <common/strwrap.h>
#include "wxf_lexer.h"
#include "context.h"
#include "dispatch.h"

using namespace common;

const size_t MAX_INCLUDE_DEPTH = 1024; //максимальная глубина вложений инклюдов
const size_t GROW_SMALL        = 2;    //скорость роста буфера разбора для маленьких документов
const size_t GROW_MEDIUM       = 8;    //скорость роста буфера разбора для средних документов
const size_t GROW_LARGE        = 32;   //скорость роста буфера разбора для больших документов

struct WXFParser
{
  WXFLexer      lex;
  ParseContext& context;
  size_t        include_depth;
  const char*   name;
    
  WXFParser (char* buf,ParseContext& context,size_t include_depth=0);
  
  void error   (const char* fmt,...); //протоколироване ошибки разбора
  void warning (const char* fmt,...); //протоколироване предупреждения разбора
  
  void process_lex_error ();
  void parse_directive ();
  void parse_pragma ();
  void parse_warning ();
  void parse_error ();
  void parse_include ();
  bool parse_nextline ();
  void parse_attributes ();
  void parse_node ();
  void parse_frame ();
  void parse_root ();  
  void goto_eol ();  
};

WXFParser::WXFParser (char* buf,ParseContext& _context,size_t _include_depth)
  : context (_context), include_depth (_include_depth)
{
  if (!buf)
    return;

  name = context.GetTextSourceName (buf);

  lex.reset  (buf);
  parse_root ();
}

void WXFParser::error (const char* fmt,...)
{
  va_list list;

  va_start          (list,fmt);  
  context.VLogError (name,lex.line (),fmt,list);
}

void WXFParser::warning (const char* fmt,...)
{
  va_list list;

  va_start            (list,fmt);  
  context.VLogWarning (name,lex.line (),fmt,list);
}

void WXFParser::process_lex_error ()
{
  switch (lex.error ())
  {
    case WXFLexer::WRONG_CHAR:       error ("wrong character on line '%c' (at position %u)",lex.curchar (),lex.offs ()); break;
    case WXFLexer::WRONG_IDENTIFIER: error ("wrong identifier '%s' (at position %u)",lex.token (),lex.offs ()); break;
    case WXFLexer::WRONG_VALUE:      error ("wrong value '%s' (at position %u)",lex.token (),lex.offs ()); break;
    case WXFLexer::UNCLOSED_COMMENT: error ("unclosed comment (at position %u)",lex.offs ()); break;
    case WXFLexer::UNCLOSED_STRING:  error ("unclosed string '%s' (at position %u)",lex.token (),lex.offs ()); break;
    case WXFLexer::WRONG_DIRECTIVE:  error ("wrong directive '%s' (at position %u)",lex.token (),lex.offs ()); break;
    default:                         error ("undefined WXFLexer error. Call to developer"); break;
  }  
}

void WXFParser::goto_eol ()
{
  for (;;lex.next ())
  {
    switch (lex.lexem ())
    {
      case WXFLexer::END_OF_FILE:
      case WXFLexer::NEW_LINE: return;
      default: break;
    }
  }
}

void WXFParser::parse_include ()
{
  if (lex.lexem () != WXFLexer::STRING)
  {
    error ("expected include file name");
    return;
  }
  
  const char* file_name = lex.token ();
    
  if (include_depth >= MAX_INCLUDE_DEPTH)
  {
    error ("too many include files (depth=%u)",MAX_INCLUDE_DEPTH);
    return;
  }

  WXFParser (context.AttachTextSource (file_name),context,include_depth+1);

  lex.next ();
}

void WXFParser::parse_pragma ()
{
  if (!lex.token ())
  {
    error ("empty pragma");
    return;
  }

  if (!common::strcmp (lex.token (),"reserve"))
  {
    size_t size [2];
    
    for (int i=0;i<2;i++)
    {
      lex.next ();    
      
      if (lex.lexem () != WXFLexer::VALUE)
      {
        error ("syntax error. Number expected (must be #pragma reserve <nodes_count> <attributes_count>)");
        return;
      }
      
      char* end;
      
      size [i] = strtoul (lex.token (),&end,0);
    }

    if (!context.Reserve (size [0],size [1]))
      warning ("Can not reserve %u nodes and %u attributes",size [0],size [1]);
  }
  else if (!common::strcmp (lex.token (),"profile"))
  {
    lex.next ();
    
    if (lex.lexem () != WXFLexer::IDENTIFIER)
    {
      error ("syntax error. Identifier expected (small, medium or large)");
      return;
    }
    
    if      (!common::strcmp (lex.token (),"small"))  context.SetGrow (GROW_SMALL);
    else if (!common::strcmp (lex.token (),"medium")) context.SetGrow (GROW_MEDIUM);
    else if (!common::strcmp (lex.token (),"large"))  context.SetGrow (GROW_LARGE);
    else
    {
      error ("unknown profile \"%s\"",lex.token ());
      return;
    }
  }
  
  lex.next ();
}

void WXFParser::parse_error ()
{
  if (!lex.token ())
  {
    error ("");
    return;
  }
  
  if (lex.lexem () != WXFLexer::STRING)
  {
    error ("syntax error. String expected (must be #error \"Message\")");
    return;
  }
  
  error (lex.token ());
  
  lex.next ();
}

void WXFParser::parse_warning ()
{
  if (!lex.token ())
  {
    error ("");
    return;
  }
  
  if (lex.lexem () != WXFLexer::STRING)
  {
    error ("syntax error. String expected (must be #warning \"Message\")");
    return;
  }
  
  warning (lex.token ());

  lex.next ();
}

void WXFParser::parse_directive ()
{
  const char* directive = lex.token ();
  
  lex.next ();

  if (!*directive)
    return;
    
  if      (!common::strcmp ("pragma",directive))  parse_pragma ();
  else if (!common::strcmp ("include",directive)) parse_include ();
  else if (!common::strcmp ("error",directive))   parse_error ();
  else if (!common::strcmp ("warning",directive)) parse_warning ();    
  else    
  {
    error ("unknown directive '%s'",directive);
    goto_eol ();
  }
  
  if (lex.lexem () != WXFLexer::END_OF_FILE && lex.lexem () != WXFLexer::NEW_LINE)
    error ("unexpected characters on line");

  goto_eol ();
}

bool WXFParser::parse_nextline ()
{
  for (;;)
  {
    switch (lex.next ())
    {
      case WXFLexer::DIRECTIVE:     parse_directive ();
      case WXFLexer::NEW_LINE:      break;
      case WXFLexer::BEGIN_BRACKET: return true;
      default:                      return false;
    }
  }
}

void WXFParser::parse_attributes ()
{
  int bracket_level = 0;

  for (;;)
  {
    switch (lex.lexem ())
    {      
      case WXFLexer::VALUE:
      case WXFLexer::IDENTIFIER:
      case WXFLexer::STRING:
        context.AppendAttr (lex.token ());
        break;
      case WXFLexer::BEGIN_BRACKET:
        bracket_level++;
        break;
      case WXFLexer::END_BRACKET:
        if (bracket_level) bracket_level--;
        else               error ("syntax error. unexpected bracket (at position %u)",lex.offs ());
        break;
      case WXFLexer::BEGIN_FRAME:
        if (bracket_level)
          error ("unexpected '{' (unclosed attribute list at position %u)",lex.offs ());
        else return;
        break;
      case WXFLexer::END_FRAME:
        if (bracket_level)
          error ("unexpected '}' (unclosed attribute list at position %u)",lex.offs ());
        else return;
        break;
      case WXFLexer::DIRECTIVE:
        parse_directive ();
        continue;
      case WXFLexer::NEW_LINE:
        if (!bracket_level)
        {
          if (parse_nextline ()) 
            continue;

          return;             
        }
        break;
      case WXFLexer::END_OF_FILE: 
        if (bracket_level)
          error ("unexpected EOF (unclosed attribute list at position %u)",lex.offs ());
        return;
      default:
        process_lex_error ();
        break;
    }

    lex.next ();
  }
}

void WXFParser::parse_node ()
{
  context.BeginElement (lex.token (),name,lex.line ());
  
  lex.next ();

  parse_attributes ();

  context.EndElement ();

  for (;;lex.next ())
    switch (lex.lexem ())
    {
      case WXFLexer::DIRECTIVE: parse_directive ();
      case WXFLexer::NEW_LINE:  break;
      case WXFLexer::BEGIN_FRAME:
      {
        context.BeginFrame ();
        
        parse_frame ();        
        
        context.EndFrame ();

        switch (lex.lexem ())
        {
          case WXFLexer::END_FRAME:   lex.next ();  break;
          case WXFLexer::END_OF_FILE: error ("unexpected EOF (unclosed frame at position %u)",lex.offs ()); break;
          default:                 
            error ("unprocessed error at parse_node");
            process_lex_error ();
            break;
        }
      }
      default:
        return;
    }
}

void WXFParser::parse_frame ()
{
  lex.next ();  

  for (;;)
  {
    switch (lex.lexem ())
    {
      case WXFLexer::IDENTIFIER:    parse_node ();  continue;
      case WXFLexer::VALUE:         error ("syntax error. Unexpected value '%s'. Line must start from identifier",lex.token ());   break;
      case WXFLexer::STRING:        error ("syntax error. Unexpected string \"%s\". Line must start from identifier",lex.token ());  break;
      case WXFLexer::BEGIN_FRAME:   error ("syntax error. Unexpected '{'. There is no node defined in this context");  break;
      case WXFLexer::BEGIN_BRACKET: error ("syntax error. Unexpected '('. You must define node tag before enumerate attributes");  break;
      case WXFLexer::END_BRACKET:   error ("syntax error. Unexpected ')'. There is no attribute list defined in this context");  break;
      case WXFLexer::DIRECTIVE:     parse_directive ();      
      case WXFLexer::NEW_LINE:      break;
      case WXFLexer::END_FRAME:
      case WXFLexer::END_OF_FILE:   return;
      default:
        process_lex_error ();  
        break;
    }

    lex.next ();
  }
}

void WXFParser::parse_root ()
{
  for (;;)
  {
    parse_frame ();

    switch (lex.lexem ())
    {
      case WXFLexer::END_OF_FILE: return;
      case WXFLexer::END_FRAME:   error ("unexpected '}'. No frame opened"); break;
      default:                    error ("unexpected token on line"); break;
    }
  }
}

void ParseDispatch::ParseWXF (char* buf,ParseContext& context)
{
  WXFParser (buf,context);
}
