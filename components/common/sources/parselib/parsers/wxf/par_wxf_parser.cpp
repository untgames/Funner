#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const char*  WXF_PARSER_COMPONENT_NAME = "common.parsers.wxf";    //имя компонента wxf-парсера
const size_t MAX_INCLUDE_DEPTH         = 1024;                    //максимальная глубина вложений инклюдов
const size_t SMALL_BUFFER_SIZE         = 16384;                   //размер буфера разбора для маленьких документов
const size_t MEDIUM_BUFFER_SIZE        = 64 * SMALL_BUFFER_SIZE;  //размер буфера разбора для средних документов
const size_t LARGE_BUFFER_SIZE         = 16 * MEDIUM_BUFFER_SIZE; //размер буфера разбора для больших документов
const size_t ATTRIBUTES_CACHE_SIZE     = 256;                     //количество кэшируемых атрибутов

/*
    Парсер wxf-файлов
*/

class WxfParser
{
  public:  
///Разбор
    static void Parse (ParseTreeBuilder& builder, ParseLog& log, const char* name, size_t, char* buffer)
    {
      WxfParser (builder, log, name, buffer, 0);
    }
  
  private:
///Конструктор
    WxfParser (ParseTreeBuilder& in_builder, ParseLog& in_log, const char* in_name, char* buffer, size_t in_include_depth)
      : builder (in_builder),
        log (in_log),
        include_depth (in_include_depth),
        name (in_name),
        attributes (ATTRIBUTES_CACHE_SIZE),
        attributes_count (0)
    {
      if (!buffer)
        return;
            
      lexer.Reset (buffer);

      ParseRoot ();      
    }  
  
///Протоколирование ошибки разбора  
    void Error (const char* format, ...)
    {
      va_list list;

      va_start   (list, format);
      log.VError (name, lexer.LineNumber (), format, list);
    }

///Протоколирование предупреждения разбора    
    void Warning (const char* format, ...)
    {
      va_list list;

      va_start     (list, format);
      log.VWarning (name, lexer.LineNumber (), format, list);
    }
    
///Утилиты разбора
    void ProcessLexError ()
    {
      switch (lexer.Status ())
      {
        case WxfLexerStatus_WrongChar:
          Error ("wrong character on line '%c' (at position %u)", lexer.CurrentChar (), lexer.LinePosition ());
          break;
        case WxfLexerStatus_WrongIdentifier:
          Error ("wrong identifier '%s' (at position %u)", lexer.Token (), lexer.LinePosition ());
          break;
        case WxfLexerStatus_WrongValue:
          Error ("wrong value '%s' (at position %u)", lexer.Token (), lexer.LinePosition ());
          break;
        case WxfLexerStatus_UnclosedComment:
          Error ("unclosed comment (at position %u)", lexer.LinePosition ());
          break;
        case WxfLexerStatus_UnclosedString:
          Error ("unclosed string '%s' (at position %u)", lexer.Token (), lexer.LinePosition ());
          break;
        case WxfLexerStatus_WrongDirective:
          Error ("wrong directive '%s' (at position %u)", lexer.Token (), lexer.LinePosition ());
          break;
        default:
          Error ("undefined WxfLexer error");
          break;
      }
    }

    void ParseDirective ()
    {
      const char* directive = lexer.Token ();
      
      lexer.NextLexem ();

      if (!*directive)
        return;
        
      if      (!strcmp ("pragma",  directive)) ParsePragma ();
      else if (!strcmp ("include", directive)) ParseInclude ();
      else if (!strcmp ("error",   directive)) ParseError ();
      else if (!strcmp ("warning", directive)) ParseWarning ();
      else
      {
        Error   ("unknown directive '%s'", directive);
        GotoEol ();
      }

      if (lexer.Lexem () != WxfLexem_EndOfFile && lexer.Lexem () != WxfLexem_NewLine)
        Error ("unexpected characters on line");

      GotoEol ();      
    }

    void ParsePragma ()
    {
      if (!lexer.Token ())
      {
        Error ("empty pragma");
        return;
      }

      if (!strcmp (lexer.Token (), "profile"))
      {
        lexer.NextLexem ();
        
        if (lexer.Lexem () != WxfLexem_Identifier)
        {
          Error ("syntax error. Identifier expected (small,  medium or large)");
          return;
        }
        
        if      (!strcmp (lexer.Token (), "small"))  builder.ReserveBuffer (SMALL_BUFFER_SIZE);
        else if (!strcmp (lexer.Token (), "medium")) builder.ReserveBuffer (MEDIUM_BUFFER_SIZE);
        else if (!strcmp (lexer.Token (), "large"))  builder.ReserveBuffer (LARGE_BUFFER_SIZE);
        else
        {
          Error ("unknown profile \"%s\"", lexer.Token ());
          return;
        }
      }
      
      lexer.NextLexem ();      
    }

    void ParseWarning ()
    {
      if (!lexer.Token ())
      {
        Error ("");
        return;
      }
      
      if (lexer.Lexem () != WxfLexem_String)
      {
        Error ("syntax error. String expected (must be #warning \"Message\")");
        return;
      }
      
      Warning (lexer.Token ());

      lexer.NextLexem ();      
    }

    void ParseError ()
    {
      if (!lexer.Token ())
      {
        Error ("");
        return;
      }
      
      if (lexer.Lexem () != WxfLexem_String)
      {
        Error ("syntax error. String expected (must be #error \"Message\")");
        return;
      }

      Error (lexer.Token ());

      lexer.NextLexem ();      
    }

    void ParseInclude ()
    {
      if (lexer.Lexem () != WxfLexem_String)
      {
        Error ("expected include file name");
        return;
      }
      
      const char* file_name = lexer.Token ();
        
      if (include_depth >= MAX_INCLUDE_DEPTH)
      {
        Error ("too many include files (depth=%u)", MAX_INCLUDE_DEPTH);
        return;
      }
      
        //загрузка буфера

      stl::string buffer;

      FileSystem::LoadTextFile (file_name, buffer);

        //разбор

      builder.SetSource (file_name);

      WxfParser (builder, log, file_name, &buffer [0], include_depth + 1);

      builder.SetSource (name);

        //переход к следующей лексеме

      lexer.NextLexem ();      
    }

    bool ParseNextLine ()
    {
      for (;;)
      {
        switch (lexer.NextLexem ())
        {
          case WxfLexem_Directive:    ParseDirective ();
          case WxfLexem_NewLine:      break;
          case WxfLexem_BeginBracket: return true;
          default:                    return false;
        }
      }      
    }
    
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

    void ParseAttributesCore ()
    {
      int bracket_level = 0;

      for (;;)
      {
        switch (lexer.Lexem ())
        {      
          case WxfLexem_Value:
          case WxfLexem_Identifier:
          case WxfLexem_String:
            AddAttribute (lexer.Token ());
            break;
          case WxfLexem_BeginBracket:
            bracket_level++;
            break;
          case WxfLexem_EndBracket:
            if (bracket_level) bracket_level--;
            else               Error ("syntax error. unexpected bracket (at position %u)", lexer.LinePosition ());
            break;
          case WxfLexem_BeginFrame:
            if (!bracket_level)
              return;

            Error ("unexpected '{' (unclosed attribute list at position %u)", lexer.LinePosition ());

            break;
          case WxfLexem_EndFrame:
            if (!bracket_level)
              return;

            Error ("unexpected '}' (unclosed attribute list at position %u)", lexer.LinePosition ());

            break;
          case WxfLexem_Directive:
            ParseDirective ();
            continue;
          case WxfLexem_NewLine:
            if (!bracket_level)
            {
              if (ParseNextLine ()) 
                continue;
                
              return;             
            }
            break;
          case WxfLexem_EndOfFile: 
            if (bracket_level)
              Error ("unexpected EOF (unclosed attribute list at position %u)", lexer.LinePosition ());              
              
            return;
          default:
            ProcessLexError ();
            break;
        }

        lexer.NextLexem ();
      }
    }
    
    void ParseAttributes ()
    {
      attributes_count = 0;

      ParseAttributesCore ();
      FlushAttributes ();
    }

    void ParseNode ()
    {
      builder.BeginNode (lexer.Token (), lexer.LineNumber ());

      lexer.NextLexem ();

      ParseAttributes ();
      
      bool   frame_parsed = false;
      size_t frame_start  = 0;

      for (;; lexer.NextLexem ())
        switch (lexer.Lexem ())
        {
          case WxfLexem_Directive:
            ParseDirective ();
            break;
          case WxfLexem_NewLine:
            break;
          case WxfLexem_BeginFrame:
          {
            if (frame_parsed)
            {
              Error ("unexpected '{'. Frame already defined at line %u", frame_start);
              break;
            }

            frame_start  = lexer.LineNumber ();
            frame_parsed = true;

            ParseFrame ();            

            switch (lexer.Lexem ())
            {
              case WxfLexem_EndFrame:
                lexer.NextLexem ();
                break;
              case WxfLexem_EndOfFile:
                Error ("unexpected EOF (unclosed frame at position %u)", lexer.LinePosition ());
                break;
              default:                 
                Error ("unprocessed error at parse_node");
                ProcessLexError ();
                break;
            }
          }
          default:
            builder.EndNode ();          
            return;
        }
    }

    void ParseFrame ()
    {
      lexer.NextLexem ();  

      for (;;)
      {
        switch (lexer.Lexem ())
        {
          case WxfLexem_Identifier:
            ParseNode ();
            continue;
          case WxfLexem_Value:
            Error ("syntax error. Unexpected value '%s'. Line must start from identifier", lexer.Token ());
            break;
          case WxfLexem_String:
            Error ("syntax error. Unexpected string \"%s\". Line must start from identifier", lexer.Token ());
            break;
          case WxfLexem_BeginFrame:
            Error ("syntax error. Unexpected '{'. There is no node defined in this context");
            break;
          case WxfLexem_BeginBracket:
            Error ("syntax error. Unexpected '('. You must define node tag before enumerate attributes");
            break;
          case WxfLexem_EndBracket:
            Error ("syntax error. Unexpected ')'. There is no attribute list defined in this context");
            break;
          case WxfLexem_Directive:
            ParseDirective ();
          case WxfLexem_NewLine:
            break;
          case WxfLexem_EndFrame:
          case WxfLexem_EndOfFile:
            return;
          default:
            ProcessLexError ();  
            break;
        }

        lexer.NextLexem ();
      }
    }

    void ParseRoot ()
    {
      for (;;)
      {
        ParseFrame ();

        switch (lexer.Lexem ())
        {
          case WxfLexem_EndOfFile:
            return;
          case WxfLexem_EndFrame:
            Error ("unexpected '}'. No frame opened");
            break;
          default:
            Error ("unexpected token on line");
            break;
        }
      }      
    }

    void GotoEol ()
    {
      for (;; lexer.NextLexem ())
      {
        switch (lexer.Lexem ())
        {
          case WxfLexem_EndOfFile:
          case WxfLexem_NewLine:
            return;
          default:
            break;
        }
      }      
    }

  private:
    typedef stl::vector<const char*> AttributeCache;

  private:
    ParseTreeBuilder& builder;          //построитель дерева грамматического разбора
    ParseLog&         log;              //протокол грамматического разбора
    WxfLexer          lexer;            //лексический анализатор
    size_t            include_depth;    //текущая глубина вложений инклюдов
    const char*       name;             //имя разбираемого текстового буфера
    AttributeCache    attributes;       //кэш атрибутов
    size_t            attributes_count; //количество атрибутов в кэше
};

/*
    Компонент, регистрирующий WXF-парсер
*/

class WxfParserComponent
{
  public:
    WxfParserComponent ()
    {
      ParserManager::RegisterParser ("wxf", &WxfParser::Parse);
    }
};

}

extern "C"
{

ComponentRegistrator<WxfParserComponent> WxfParser (WXF_PARSER_COMPONENT_NAME);

}
