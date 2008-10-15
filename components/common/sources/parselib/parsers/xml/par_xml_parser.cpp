#include "shared.h"

using namespace common;

//ввести режимы лексического разбора (см. ParseContentText)!!!

namespace
{

/*
    Константы
*/

const double XML_VERSION               = 1.0f;                 //версия поддерживаемой спецификации XML
const char*  XML_PARSER_COMPONENT_NAME = "common.parsers.xml"; //имя компонента xml-парсера
const size_t ATTRIBUTES_CACHE_SIZE     = 256;                  //количество кэшируемых атрибутов

/*
    Парсер XML-документов
*/

class XmlParser
{
  public:
///Грамматический разбор
    static void Parse (ParseTreeBuilder& builder, ParseLog& log, const char* name, size_t, char* buffer)
    {
      XmlParser (builder, log, name, buffer);
    }

///Автоматическое определение возможности разбора парсером
    static bool Check (size_t, const char* buffer)
    {
      static const char*  PROLOG        = "<?xml";
      static const size_t PROLOG_LENGTH = strlen (PROLOG);      

      return strncmp (buffer, PROLOG, PROLOG_LENGTH) == 0;
    }

  private:    
///Конструктор
    XmlParser (ParseTreeBuilder& in_builder, ParseLog& in_log, const char* in_name, char* buffer)
      : builder (in_builder),
        log (in_log),
        name (in_name),
        attributes (ATTRIBUTES_CACHE_SIZE),
        attributes_count (0)
    {
      if (!buffer)
        return;

      lexer.Reset (buffer);

      ParseDocument ();
    }
    
///Работа с атрибутами
    void AddAttribute (const char* attribute)
    {
      if (attributes_count == attributes.size ())
        FlushAttributes ();

      attributes [attributes_count++] = attribute;
    }

    void FlushAttributes ()
    {
      if (!attributes_count)
        return;
      
      builder.AddAttributes (attributes_count, &attributes [0]);

      attributes_count = 0;
    }
    
///Протоколирование ошибки разбора
    void Error (const char* format, ...)
    {
      va_list list;

      va_start   (list, format);
      log.VError (name, lexer.LineNumber (), format, list);
    }
    
    void Error (size_t line_number, const char* format, ...)
    {
      va_list list;

      va_start   (list, format);
      log.VError (name, line_number, format, list);
    }

///Протоколирование предупреждения разбора
    void Warning (const char* format, ...)
    {
      va_list list;

      va_start     (list, format);
      log.VWarning (name, lexer.LineNumber (), format, list);
    }

    void Warning (size_t line_number, const char* format, ...)
    {
      va_list list;

      va_start     (list, format);
      log.VWarning (name, line_number, format, list);
    }        

///Обработка лексической ошибки
    void ProcessLexError ()
    {
      switch (lexer.Status ())
      {
        case XmlLexerStatus_WrongChar:
          Error ("wrong character on line '%c' (at position %u)", lexer.CurrentChar (), lexer.LinePosition ());
          break;
        case XmlLexerStatus_WrongIdentifier:
          Error ("wrong identifier (at position %u)", lexer.LinePosition ());
          break;
        case XmlLexerStatus_UnclosedComment:
          Error ("unclosed comment");
          break;
        case XmlLexerStatus_UnclosedString:
          Error ("unclosed string");
          break;
        case XmlLexerStatus_UnclosedCData:
          Error ("unclosed CDATA");
          break;
        case XmlLexerStatus_NoError:
          break;
        default:
          Error ("undefined XmlLexer error. Token %s at %u", lexer.Token(), lexer.LinePosition ());
          break;
      }
    }

///Обработка ошибки
    void ProcessError ()
    {
      switch (lexer.Lexem ())
      {
        case XmlLexem_Identifier:
          Error ("syntax error. Unexpected identifier '%s'", lexer.Token ());
          break;
        case XmlLexem_Token:
          Error ("syntax error. Unexpected token '%s'", lexer.Token ());
          break;
        case XmlLexem_Equal:
          Error ("syntax error. Unexpected '='");
          break;
        case XmlLexem_String:
          Error ("syntax error. Unexpected string '%s'", lexer.Token ());
          break;
        case XmlLexem_CData:
          Error ("syntax error. CDATA block can be only inside element contents");
          break;
        case XmlLexem_Keyword:
          lexer.NextLexem ();
          Error ("syntax error. Unexpected keyword '%s'", lexer.Token ()?lexer.Token ():"");
            break;
        case XmlLexem_EndOfFile:
          Error ("syntax error. Unexpected EOF (unclosed element '%s')", builder.NodeName ());
          break;
        case XmlLexem_SectorBeginBracket:
          Error ("syntax error. Unexpected '['");
          break;
        case XmlLexem_SectorEndBracket:
          Error ("syntax error. Unexpected ']'");
          break;        
        case XmlLexem_TagBeginBracket:
          lexer.NextLexem ();

          Error ("syntax error. Unexpected new element '%s'", lexer.Token ());
          
          for (bool loop=true; loop;)
          {
            switch (lexer.NextLexem ())
            {
              case XmlLexem_TagEndBracket:
              case XmlLexem_TagClose:
              case XmlLexem_EndOfFile:
                loop = false;
                break;
              default:
                break;
            }
          }

          break;
        case XmlLexem_TagEndBracket:
          Error ("syntax error. Unexpected '>'");
          break;
        case XmlLexem_TagClose:
          Error ("syntax error. Unexpected '/>'");
          break;
        case XmlLexem_TagCloseFrame:
          Error ("syntax error. Unexpected '</'");
          break;
        case XmlLexem_InstructionBeginBracket:
          lexer.NextLexem ();
          Error ("syntax error. Unexpected processing instruction '%s'", lexer.Token ());
          break;
        case XmlLexem_InstructionEndBracket:
          Error ("syntax error. Unexpected '?>'");
          break;
        default:      
          ProcessLexError (); 
          break;
      }      
    }

///Утилиты разбора
    void ParseProcessingInstruction ()
    {
      lexer.NextLexem ();
      
      if (lexer.Lexem () != XmlLexem_Identifier)
      {
        Error ("syntax error. Expected processing directive identifier");
        return;
      }  
      
      if (xtl::xstrnicmp ("xml", lexer.Token (), 3))
      {
        Error ("processing directive must start from 'xml' prefix");
        return;
      }

      if (lexer.NextLexem () == XmlLexem_Identifier) 
        ParseInstructionList ();

      for (;;lexer.NextLexem ())
        switch (lexer.Lexem ())
        {
          case XmlLexem_InstructionEndBracket:
            return;
          default:
            ProcessError ();
            return;
        }
    }

    void ParseContentText ()
    {
      builder.BeginNode ("#text", lexer.LineNumber ());

      attributes_count = 0;

      for (;; lexer.NextLexem ())
        switch (lexer.Lexem ())
        {
          case XmlLexem_Token:
          case XmlLexem_Identifier:
          case XmlLexem_String:
          case XmlLexem_CData:
          case XmlLexem_Equal:
          case XmlLexem_Undefined:
            AddAttribute (lexer.Token ());
            break;
          default:
            FlushAttributes ();
            builder.EndNode ();
            return;
        }
    }

    void ParseContentElements ()
    {
      for (;;)
      {
        switch (lexer.Lexem ())
        {
          case XmlLexem_Token:
          case XmlLexem_Identifier:
          case XmlLexem_String:
          case XmlLexem_CData:
          case XmlLexem_Equal:
          case XmlLexem_Undefined:
            ParseContentText();
            continue;
          case XmlLexem_TagBeginBracket:
            ParseElement ();

            if (lexer.Lexem () == XmlLexem_TagBeginBracket)
              continue;

            break;
          case XmlLexem_InstructionBeginBracket:
            ParseProcessingInstruction ();
            break;
          case XmlLexem_TagCloseFrame:
          {
            if (lexer.NextLexem () != XmlLexem_Identifier)
            {          
              Error ("syntax error. Expected identifier");

              ProcessError ();

              if (lexer.Lexem () == XmlLexem_TagEndBracket)
                return;
            }

            if (strcmp (builder.NodeName (), lexer.Token ()))
            {
              Error ("syntax error. Closing tag '%s' without opening. Expected '%s'.", lexer.Token (), builder.NodeName ());

              if (lexer.NextLexem () != XmlLexem_TagEndBracket)
                return;

              lexer.NextLexem ();

              continue;
            }
      
            if (lexer.NextLexem () != XmlLexem_TagEndBracket)
            {
              Error ("syntax error. Expected '>'");
              
              for (;;)
                switch (lexer.NextLexem ())
                {
                  case XmlLexem_EndOfFile:
                    Error ("unexpected EOF while search '>'");
                    return;
                  case XmlLexem_TagEndBracket:
                    return;
                  default:
                    break;
                }
            }
           
            return;
          }
          case XmlLexem_EndOfFile: 
            Error ("unexpected EOF (unclosed tag '%s')", builder.NodeName ());
            return;
          default:
            ProcessError ();
            break;
        }

        lexer.NextLexem ();
      }      
    }
    
    bool ReadAttribute (const char*& attr_name, const char*& attr_value, size_t& line)
    {
      attr_name = lexer.Token ();
      line      = lexer.LineNumber ();  
      
      lexer.NextLexem ();
      
      switch (lexer.Lexem ())
      {
        case XmlLexem_TagEndBracket:
        case XmlLexem_TagClose:
          return false;
        case XmlLexem_Equal:
          break;
        default:
          ProcessError ();
          return false;
      }

      lexer.NextLexem ();
      
      switch (lexer.Lexem ())
      {
        case XmlLexem_String:
          break;
        default:
          ProcessError ();
          return false;
      }

      attr_value = lexer.Token ();

      return true;
    }

    void ParseAttributeList ()
    {
      const char *attr_name = 0, *attr_value = 0;
      size_t      line      = 0;

      for (;;)
      {
        switch (lexer.Lexem ())
        {
          case XmlLexem_Identifier:
            if (!ReadAttribute (attr_name, attr_value, line))
            {
              switch (lexer.Lexem ())
              {
                case XmlLexem_Identifier:
                  continue;
                default:
                  return;
              }
            }

            builder.BeginNode    (attr_name, line);
            builder.AddAttribute (attr_value);
            builder.EndNode      ();

            break;
          case XmlLexem_TagBeginBracket:
            Error ("syntax error. Unexpected '<' (forgot '/>' ?)");
            return;
          case XmlLexem_TagClose:
          case XmlLexem_TagEndBracket:
            return;
          default:
            ProcessError ();
            return;
        }
        
        lexer.NextLexem ();
      }
    }

    void ParseElement ()
    {
      if (lexer.NextLexem () != XmlLexem_Identifier)
      {
        Error ("syntax error. Wrong element name '%s'", lexer.Token ());

        return;
      }

      builder.BeginNode (lexer.Token (), lexer.LineNumber ());
      
      lexer.NextLexem ();

      ParseAttributeList ();

      for (;;lexer.NextLexem ())
      {
        switch (lexer.Lexem ())
        {
          case XmlLexem_TagBeginBracket:
            builder.EndNode (); //восстановление после ошибки
            return;
          case XmlLexem_TagEndBracket:
            lexer.NextLexem ();
            ParseContentElements ();
          case XmlLexem_TagClose:
            builder.EndNode ();
            return;
          default:
            break;
        }
      }
    }

    void ParseDocument ()
    {
      lexer.NextLexem ();
      
      for (;;)
      {
        switch (lexer.Lexem ())
        {
          case XmlLexem_TagBeginBracket:
            ParseElement ();
            
            if (lexer.Lexem () == XmlLexem_TagBeginBracket)
              continue;
            
            break;
          case XmlLexem_InstructionBeginBracket:
            ParseProcessingInstruction ();
            break;
          case XmlLexem_EndOfFile:
            return;
          case XmlLexem_Keyword:
            Error ("There is no support of DTD in this version of parser");
            break;
          case XmlLexem_Identifier:
            Error ("syntax error. Unexpected identifier '%s'. Tag expected", lexer.Token ());
            break;
          case XmlLexem_Token:
            Error ("syntax error. Unexpected token '%s'. Tag expected", lexer.Token ());
            break;
          case XmlLexem_String:
            Error ("syntax error. Unexpected string '%s'. Tag expected", lexer.Token ());
            break;
          case XmlLexem_CData:
            Error ("syntax error. Unexpected CDATA tag. Tag expected");
            break;
          case XmlLexem_Equal:
            Error ("syntax error. Unexpected '='. Tag expected");
            break;
          case XmlLexem_SectorBeginBracket:
            Error ("syntax error. Unexpected '['. Tag expected");
            break;
          case XmlLexem_SectorEndBracket:
            Error ("syntax error. Unexpected ']'. Tag expected");
            break;
          case XmlLexem_TagEndBracket:
            Error ("syntax error. Unexpected '>'. Tag not opened");
            break;
          case XmlLexem_TagClose:
            Error ("syntax error. Unexpected '/>'. Tag not opened");
            break;
          case XmlLexem_TagCloseFrame:
            Error ("syntax error. Unexpected '</'. Tag not opened");
            break;            
          default:
            ProcessLexError ();
            break;
        }
        
        lexer.NextLexem ();
      }
    }
    
    void ProcessInstruction (const char* name, const char* value, size_t line)
    {
      enum XmlInstruction
      {
        XmlInstruction_Undefined,  //неопределена
        XmlInstruction_Version,    //версия формата
        XmlInstruction_Encoding,   //кодовая страница
        XmlInstruction_Standalone, //нет ссылок на внешние файлы
      };

      struct String2Instruction
      {
        const char*    name;
        XmlInstruction instruction;
      };

      static const String2Instruction instructions [] = {
        {"version",    XmlInstruction_Version},
        {"encoding",   XmlInstruction_Encoding},
        {"standalone", XmlInstruction_Standalone},
        {0,            XmlInstruction_Undefined}
      };
      
      XmlInstruction instruction = XmlInstruction_Undefined;
      
      if (name)
      {
        for (const String2Instruction* i = instructions; i->name; i++)
          if (!xtl::xstricmp (i->name, name))
          {
            instruction = i->instruction;
            break;
          }
      }

      switch (instruction)
      {
        case XmlInstruction_Version:
        {
          float version = (float)atof (value);
          
          if (version == 0.0f)
          {
            Warning (line, "wrong XML version %s", value);
            break;
          }
          
          if (version > XML_VERSION)
          {
            Warning (line, "this parser has written for XML1.0. This document require XML%s", value);
            break;
          }
          
          break;
        }
        case XmlInstruction_Encoding:
          Warning (line, "not yet implement. Encoding='%s'", value);
          break;
        case XmlInstruction_Standalone:
        {
          int result = -1;

          if      (!xtl::xstricmp (value, "yes")) result = 1;
          else if (!xtl::xstricmp (value, "no"))  result = 0;

          if (result == -1)
          {
            Error (line, "wrong value '%s' (expected 'yes' or 'no'", value);
            break;
          }

          if (!result)
          {
            Warning (line, "extern references doesn't supported");
            break;
          }

          break;
        }
        default:
          Error (line, "unknown instruction '%s'", value);
          break;
      }      
    }    

    void ParseInstructionList ()
    {
      const char *name = 0, *value = 0;
      size_t      line = 0;

      for (;;lexer.NextLexem ())
        switch (lexer.Lexem ())
        {
          case XmlLexem_Identifier:
            if (ReadAttribute (name, value, line))
              ProcessInstruction (name, value, line);

            break;
          default:
            ProcessError ();
          case XmlLexem_InstructionEndBracket:
            return;
        }      
    }    

  private:
    typedef stl::vector<const char*> AttributeCache;

  private:
    ParseTreeBuilder& builder;          //построитель дерева грамматического разбора
    ParseLog&         log;              //протокол грамматического разбора
    XmlLexer          lexer;            //лексический анализатор
    const char*       name;             //имя разбираемого буфера
    AttributeCache    attributes;       //кеш атрибутов
    size_t            attributes_count; //количество атрибутов в кеше
};

/*
    Компонент, регистрирующий XML-парсер
*/

class XmlParserComponent
{
  public:
    XmlParserComponent ()
    {
      ParserManager::RegisterParser ("xml", &XmlParser::Parse, &XmlParser::Check);
    }
};

}

extern "C"
{

ComponentRegistrator<XmlParserComponent> XmlParser (XML_PARSER_COMPONENT_NAME);

}
