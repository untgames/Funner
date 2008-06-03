#include <xtl/string.h>
#include "xml_lexer.h"
#include "context.h"
#include "dispatch.h"

const float PARSE_VERSION = 1.0f;

using namespace common;
using namespace stl;

struct XMLParser
{
  XMLLexer       lex;
  ParseContext&  context;  
  const char*    name;
    
  enum Instruction {
    INSTRUCTION_UNDEFINED,  //неопределена
    INSTRUCTION_VERSION,    //версия формата
    INSTRUCTION_ENCODING,   //кодовая страница
    INSTRUCTION_STANDALONE, //нет ссылок на внешне файлы
  };
  
  XMLParser (char* buf,ParseContext& context);
  
  void error   (const char* fmt,...); //протоколироване ошибки разбора
  void warning (const char* fmt,...); //протоколироване предупреждения разбора
  void error   (size_t line,const char* fmt,...); //протоколироване ошибки разбора
  void warning (size_t line,const char* fmt,...); //протоколироване предупреждения разбора  
  
  void process_lex_error ();
  void process_error ();
  void parse_PI ();
  void parse_content_text ();
  void parse_content_elements ();
  void parse_attrlist ();
  void parse_element ();
  void parse_document ();
  void parse_instrlist ();
  void process_instruction (const char* name,const char* value,size_t line);
  bool read_attribute (const char*& name,const char*& value,size_t& line);
  
  static Instruction instruction (const char* name);
};

struct String2Instruction
{
  const char*            name;
  XMLParser::Instruction instruction;
};

static String2Instruction instruction_map [] = {
  {"version",XMLParser::INSTRUCTION_VERSION},
  {"encoding",XMLParser::INSTRUCTION_ENCODING},
  {"standalone",XMLParser::INSTRUCTION_STANDALONE},
  {NULL,XMLParser::INSTRUCTION_UNDEFINED}
};

XMLParser::XMLParser (char* buf,ParseContext& _context)
  : context (_context)
{ 
  if (!buf)
    return;

  name = context.GetTextSourceName (buf);

  lex.reset (buf);
  parse_document ();
}

void XMLParser::error (const char* fmt,...)
{
  va_list list;

  va_start          (list,fmt);  
  context.VLogError (name,lex.line (),fmt,list);
}

void XMLParser::warning (const char* fmt,...)
{
  va_list list;

  va_start            (list,fmt);  
  context.VLogWarning (name,lex.line (),fmt,list);
}

void XMLParser::error (size_t line,const char* fmt,...)
{
  va_list list;

  va_start          (list,fmt);  
  context.VLogError (name,line,fmt,list);
}

void XMLParser::warning (size_t line,const char* fmt,...)
{
  va_list list;

  va_start            (list,fmt);  
  context.VLogWarning (name,line,fmt,list);
}

void XMLParser::process_lex_error ()
{
  switch (lex.error ())
  {
    case XMLLexer::WRONG_CHAR:       error ("wrong character on line '%c' (at position %u)",lex.curchar (),lex.offs ()); break;
    case XMLLexer::WRONG_IDENTIFIER: error ("wrong identifier (at position %u)",lex.offs ()); break;
    case XMLLexer::UNCLOSED_COMMENT: error ("unclosed comment"); break;
    case XMLLexer::UNCLOSED_STRING:  error ("unclosed string"); break;
    case XMLLexer::UNCLOSED_CDATA:   error ("unclosed CDATA"); break;
    default:                         error ("undefined XMLLexer error. Call to developer %s at %u", lex.token(), lex.offs ()); break;
  }    
}

void XMLParser::process_error ()
{
  switch (lex.lexem ())
  {
    case XMLLexer::IDENTIFIER:
      error ("syntax error. Unexpected identifier \"%s\"",lex.token ());
      break;
    case XMLLexer::TOKEN:
      error ("syntax error. Unexpected token \"%s\"",lex.token ());
      break;
    case XMLLexer::EQUAL:
      error ("syntax error. Unexpected '='");
      break;
    case XMLLexer::STRING:
      error ("syntax error. Unexpected string \"%s\"",lex.token ());
      break;
    case XMLLexer::CDATA:
      error ("syntax error. CDATA block can be only inside element contents");
      break;
    case XMLLexer::KEYWORD:
      lex.next ();
      error ("syntax error. Unexpected keyword \"%s\"",lex.token ()?lex.token ():"");
      break;
    case XMLLexer::END_OF_FILE:
      error ("syntax error. Unexpected EOF (unclosed element)");
      break;
    case XMLLexer::SECTOR_BEGIN_BRACKET:
      error ("syntax error. Unexpected '['");
      break;
    case XMLLexer::SECTOR_END_BRACKET:
      error ("syntax error. Unexpected ']'");
      break;        
    case XMLLexer::TAG_BEGIN_BRACKET:
      lex.next ();
      error ("syntax error. Unexpected new element \"%s\"",lex.token ()?lex.token ():"");
      break;
    case XMLLexer::TAG_END_BRACKET:
      error ("syntax error. Unexpected '>'");
      break;
    case XMLLexer::TAG_CLOSE:
      error ("syntax error. Unexpected '/>");
      break;      
    case XMLLexer::INSTRUCTION_BEGIN_BRACKET:
      lex.next ();
      error ("syntax error. Unexpected processing instruction \"%s\"",lex.token ()?lex.token ():"");
      break;
    case XMLLexer::INSTRUCTION_END_BRACKET:
      error ("syntax error. Unexpected '?>'");
      break;
    default:      
      process_lex_error (); 
      break;
  }
}

inline XMLParser::Instruction XMLParser::instruction (const char* name)
{
  if (!name)
    return INSTRUCTION_UNDEFINED;

  for (String2Instruction* i=instruction_map;i->name;i++)
    if (!xtl::xstricmp (i->name,name))
      return i->instruction;
      
  return INSTRUCTION_UNDEFINED;
}

void XMLParser::process_instruction (const char* name,const char* value,size_t line)
{
  switch (instruction (name))
  {
    case INSTRUCTION_VERSION:
    {
      float version = (float)atof (value);
      
      if (version == 0.0f)
      {
        warning (line,"wrong XML version %s",value);
        break;
      }
      
      if (version > PARSE_VERSION)
      {
        warning (line,"this parser has written for XML1.0. This document require XML%s",value);
        break;
      }
      
      break;
    }
    case INSTRUCTION_ENCODING:
      warning (line,"not yet implement. Encoding=\"%s\"",value);
      break;
    case INSTRUCTION_STANDALONE:
    {
      int result = -1;
      
      if      (!xtl::xstricmp (value,"yes")) result = 1;
      else if (!xtl::xstricmp (value,"no"))  result = 0;
      
      if (result == -1)
      {
        error (line,"wrong value \"%s\" (expected \"yes\" or \"no\"",value);
        break;
      }
      
      if (!result)
      {
        warning (line,"extern references doesn't supported");
        break;
      }

      break;
    }
    default:
      error (line,"unknown instruction \"%s\"",value);
      break;
  }
}

void XMLParser::parse_instrlist ()
{
  const char* name  = NULL;
  const char* value = NULL;
  size_t      line  = 0;

  for (;;lex.next ())
    switch (lex.lexem ())
    {
      case XMLLexer::IDENTIFIER:
        if (read_attribute (name,value,line))
          process_instruction (name,value,line);
          
        break;
      default:
        process_error ();
      case XMLLexer::INSTRUCTION_END_BRACKET:
        return;
    }
}

void XMLParser::parse_PI ()
{
  lex.next ();
  
  if (lex.lexem () != XMLLexer::IDENTIFIER)
  {
    error ("syntax error. Expected processing directive identifier");
    return;
  }  
  
  if (xtl::xstrnicmp ("xml",lex.token (),3))
  {
    error ("processing directive must start from 'xml' prefix");
    return;
  }

  if (lex.next () == XMLLexer::IDENTIFIER) 
    parse_instrlist ();

  for (;;lex.next ())
    switch (lex.lexem ())
    {
      case XMLLexer::INSTRUCTION_END_BRACKET: return;
      default:                                process_error (); return;
    }
}

void XMLParser::parse_content_text ()
{
  context.BeginElement ("#text",name,lex.line ());

  for (;;lex.next ())
    switch (lex.lexem ())
    {      
      case XMLLexer::TOKEN:
      case XMLLexer::IDENTIFIER:
      case XMLLexer::STRING:
      case XMLLexer::CDATA:
      case XMLLexer::EQUAL:
      case XMLLexer::UNDEFINED:
        context.AppendAttr (lex.token ());
        break;
      default:
        context.EndElement ();
        return;
    }
}

void XMLParser::parse_content_elements ()
{
  for (;;)
  {
    switch (lex.lexem ())
    {
      case XMLLexer::TOKEN:
      case XMLLexer::IDENTIFIER:
      case XMLLexer::STRING:
      case XMLLexer::CDATA:
      case XMLLexer::EQUAL:
      case XMLLexer::UNDEFINED:
        parse_content_text();
        continue;
      case XMLLexer::TAG_BEGIN_BRACKET:
        parse_element ();
        break;
      case XMLLexer::INSTRUCTION_BEGIN_BRACKET:
        parse_PI ();
        break;
      case XMLLexer::TAG_CLOSE:
      {        
        if (lex.next () != XMLLexer::IDENTIFIER)
        {          
          error ("syntax error. Expected identifier");
          process_error ();
          
          if (lex.lexem () == XMLLexer::TAG_END_BRACKET)
            return;
        }

        if (strcmp(context.GetCurrentNodeName(), lex.token ()))
        {
          error ("syntax error. Closing tag '%s' without opening. Expected '%s'.", lex.token (), context.GetCurrentNodeName());
          if (lex.next () != XMLLexer::TAG_END_BRACKET)
            return;
          lex.next();
          continue;
        }
  
        if (lex.next () != XMLLexer::TAG_END_BRACKET)
        {
          error ("syntax error. Expected '>'");
          
          for (;;)
            switch (lex.next ())
            {
              case XMLLexer::END_OF_FILE:     error ("unexpected EOF while search '>'"); return;
              case XMLLexer::TAG_END_BRACKET: return;
              default:                        break;
            }
        }                
        
        return;
      }
      case XMLLexer::END_OF_FILE: 
        error ("unexpected EOF (unclosed tag)");
        return;
      default:
        process_error ();
        break;
    }
    
    lex.next ();
  }
}

inline bool XMLParser::read_attribute (const char*& name,const char*& value,size_t& line)
{
  name = lex.token ();
  line = lex.line ();  
  
  if (lex.next () != XMLLexer::EQUAL)
  {
    error ("syntax error. Expected '=', but found \"%s\"",lex.token ()?lex.token ():"");

    return false;
  }

  if (lex.next () != XMLLexer::STRING)
  {
    error ("syntax error. Attribute value must be a string");
    return false;
  }
  
  value = lex.token ();
  
  return true;
}

void XMLParser::parse_attrlist ()
{
  const char* attr_name = NULL;
  const char* value     = NULL;
  size_t      line      = 0;

  for (;;lex.next ())
    switch (lex.lexem ())
    {
      case XMLLexer::IDENTIFIER:
        if (read_attribute (attr_name,value,line))
        {
          context.BeginElement (attr_name,name,line);
          context.AppendAttr   (value);
          context.EndElement   ();
        }
        break;
      default:
        process_error ();
      case XMLLexer::TAG_CLOSE:
      case XMLLexer::TAG_END_BRACKET:
        return;
    }
}

void XMLParser::parse_element ()
{  
  if (lex.next () != XMLLexer::IDENTIFIER)
  {
    error ("syntax error. Element name expected");
    return;
  }
  
  context.BeginElement (lex.token (),name,lex.line ());
  context.EndElement   ();
  context.BeginFrame   ();
  
  if (lex.next () == XMLLexer::IDENTIFIER) 
    parse_attrlist ();

  for (;;lex.next ())
  {
    switch (lex.lexem ())
    {
      case XMLLexer::TAG_END_BRACKET:
        lex.next ();
        parse_content_elements ();
      case XMLLexer::TAG_CLOSE:
        context.EndFrame ();
        return;
      default:
        process_error ();
        break;
    }
  }
}

void XMLParser::parse_document ()
{
  for (;;)
  {
    switch (lex.next ())
    {
      case XMLLexer::TAG_BEGIN_BRACKET:         parse_element (); break;
      case XMLLexer::INSTRUCTION_BEGIN_BRACKET: parse_PI (); break;
      case XMLLexer::END_OF_FILE:               return;
      case XMLLexer::KEYWORD:                   error ("There is no suuport of DTD in this version of parser");
      case XMLLexer::IDENTIFIER:                error ("syntax error. Unexpected identifier %s. Tag expected",lex.token ()); break;
      case XMLLexer::TOKEN:                     error ("syntax error. Unexpected token %s. Tag expected",lex.token ()); break;
      case XMLLexer::STRING:                    error ("syntax error. Unexpected string \"%s\". Tag expected",lex.token ()); break;
      case XMLLexer::CDATA:                     error ("syntax error. Unexpected CDATA tag. Tag expected"); break;
      case XMLLexer::EQUAL:                     error ("syntax error. Unexpected \"=\". Tag expected"); break;
      case XMLLexer::SECTOR_BEGIN_BRACKET:      error ("syntax error. Unexpected '['. Tag expected"); break;
      case XMLLexer::SECTOR_END_BRACKET:        error ("syntax error. Unexpected ']'. Tag expected"); break;
      case XMLLexer::TAG_END_BRACKET:           error ("syntax error. Unexpected '>'. There was not tag opening"); break;
      case XMLLexer::TAG_CLOSE:                 error ("syntax error. Unexpected '/>'. There was not tag opening"); break;
      default:                                  process_lex_error (); break;
    }
  }
}

void ParseDispatch::ParseXML (char* buf,ParseContext& context)
{
  XMLParser (buf,context);
}
