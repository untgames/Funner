#include "shared.h"

using namespace common;

namespace components
{

namespace json_parser
{

/*
    Константы
*/

const char*  ARRAY_NODE_NAME            = "";
const char*  ITEM_NODE_NAME             = "";
const char*  OBJECT_NODE_NAME           = "";
const char*  JSON_PARSER_COMPONENT_NAME = "common.parsers.json"; //имя компонента json-парсера
const size_t ATTRIBUTES_CACHE_SIZE      = 256;                   //количество кэшируемых атрибутов

/*
    Парсер JSON-текста
*/

class JsonParser
{
  public:
///Грамматический разбор
    static void Parse (ParseTreeBuilder& builder, ParseLog& log, const char* name, size_t buffer_size, char* buffer)
    {
      try
      {
        JsonParser (builder, log, name, buffer_size, buffer);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::JsonParser::Parse");
        throw;
      }
    }

  private:    
///Конструктор
    JsonParser (ParseTreeBuilder& in_builder, ParseLog& in_log, const char* in_name, size_t buffer_size, char* buffer)
      : builder (in_builder)
      , log (in_log)
      , name (in_name)
      , attributes (ATTRIBUTES_CACHE_SIZE)
      , attributes_count (0)
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

///Обработка лексической ошибки
    void ProcessLexError ()
    {
      switch (lexer.Status ())
      {
        case JsonLexerStatus_WrongChar:
          Error ("wrong character on line '%c' (at position %u)", lexer.CurrentChar (), lexer.LinePosition ());
          break;
        case JsonLexerStatus_WrongNumber:
          Error ("wrong number (at position %u)", lexer.LinePosition ());
          break;
        case JsonLexerStatus_WrongKeyword:
          Error ("wrong keyword (at position %u)", lexer.LinePosition ());
          break;
        case JsonLexerStatus_UnclosedString:
          Error ("unclosed string");
          break;
        case JsonLexerStatus_InvalidCharacterReference:
          Error ("invalid character reference");
          break;
        case JsonLexerStatus_NoError:
          break;
        default:
          Error ("undefined JsonLexer error. Token %s at %u", lexer.Token(), lexer.LinePosition ());
          break;
      }
    }

///Утилиты разбора
    void AddArrayValue (const char* value, bool has_objects)
    {
      if (has_objects)
      {
        builder.BeginNode (ITEM_NODE_NAME, lexer.LineNumber ());

        builder.AddAttribute (value);

        builder.EndNode ();
      }
      else
      {
        AddAttribute (value);
      }
    }

    void ParseArray ()
    {
      attributes_count = 0;

      builder.BeginNode (ARRAY_NODE_NAME, lexer.LineNumber ());

      bool has_objects  = lexer.ArrayHasObjects (),
           value_parsed = false, //value was parsed, but comma was not occured
           comma_active = false; //no value was parsed after last comma

      lexer.NextLexem ();

      for (;;lexer.NextLexem ())
      {
        switch (lexer.Lexem ())
        {
          case JsonLexem_String:
          case JsonLexem_Number:
          case JsonLexem_Boolean:
            if (value_parsed)
              Error ("syntax error. Unexpected token '%s'. Comma or array end bracket expected.", lexer.Token ());

            AddArrayValue (lexer.Token (), has_objects);
            value_parsed = true;
            comma_active = false;
            break;
          case JsonLexem_Null:
            if (value_parsed)
              Error ("syntax error. Unexpected token '%s'. Comma or array end bracket expected.", lexer.Token ());

            AddArrayValue ("", has_objects);
            value_parsed = true;
            comma_active = false;
            break;
          case JsonLexem_ObjectBeginBracket:
            if (value_parsed)
              Error ("syntax error. Unexpected token '%s'. Comma or array end bracket expected.", lexer.Token ());

            ParseObject ();
            value_parsed = true;
            comma_active = false;
            break;
          case JsonLexem_ArrayBeginBracket:
            if (value_parsed)
              Error ("syntax error. Unexpected token '%s'. Comma or array end bracket expected.", lexer.Token ());

            ParseArray ();
            value_parsed = true;
            comma_active = false;
            break;
          case JsonLexem_ArrayEndBracket:
            if (comma_active)
              Error ("syntax error. Unexpected array close. No values after last comma.");

            FlushAttributes ();
            builder.EndNode ();
            return;
          case JsonLexem_EndOfFile:
            FlushAttributes ();
            builder.EndNode ();
            Error ("syntax error. Unexpected EOF. Array not closed.");
            return;
          case JsonLexem_Undefined:
            ProcessLexError ();
            break;
          case JsonLexem_Comma:
            if (value_parsed)
            {
              value_parsed = false;
              continue;
            }

            comma_active = true;

          default:
            Error ("syntax error. Unexpected token '%s'. Object member or end bracket expected.", lexer.Token ());
            break;
        }
      }

      builder.EndNode ();
    }

    void ParseObjectMember ()
    {
      builder.BeginNode (lexer.Token (), lexer.LineNumber ());

      if (lexer.NextLexem () != JsonLexem_Colon)
      {
        builder.EndNode ();
        Error ("syntax error. Wrong object member, colon expected after name instead of '%s'", lexer.Token ());
        return;
      }

      lexer.NextLexem ();

      bool value_parsed = false;

      switch (lexer.Lexem ())
      {
        case JsonLexem_String:
        case JsonLexem_Number:
        case JsonLexem_Boolean:
          if (value_parsed)
            Error ("syntax error. Unexpected token '%s'. Comma or object end bracket expected.", lexer.Token ());
          builder.AddAttribute (lexer.Token ());
          break;
        case JsonLexem_Null:
          break;
        case JsonLexem_ArrayBeginBracket:
          ParseArray ();
          break;
        case JsonLexem_ObjectBeginBracket:
          ParseObject ();
          break;
        case JsonLexem_Undefined:
          ProcessLexError ();
          break;
        default:
          Error ("syntax error. Unexpected token '%s'. Object member value expected.", lexer.Token ());
          break;
      }

      builder.EndNode ();
    }

    void ParseObject ()
    {
      builder.BeginNode (OBJECT_NODE_NAME, lexer.LineNumber ());
      
      lexer.NextLexem ();

      bool member_parsed = false, //value was parsed, but comma was not occured
           comma_active  = false; //no value was parsed after last comma

      for (;;lexer.NextLexem ())
      {
        switch (lexer.Lexem ())
        {
          case JsonLexem_String:
            if (member_parsed)
              Error ("syntax error. Unexpected token '%s'. Comma or object end bracket expected.", lexer.Token ());

            ParseObjectMember ();
            member_parsed = true;
            comma_active  = false;
            break;
          case JsonLexem_ObjectEndBracket:
            if (comma_active)
              Error ("syntax error. Unexpected array close. No values after last comma.");

            builder.EndNode ();
            return;
          case JsonLexem_EndOfFile:
            builder.EndNode ();
            Error ("syntax error. Unexpected EOF. Object not closed.");
            return;
          case JsonLexem_Undefined:
            ProcessLexError ();
            break;
          case JsonLexem_Comma:
            if (member_parsed)
            {
              member_parsed = false;
              continue;
            }

            comma_active = true;

          default:
            Error ("syntax error. Unexpected token '%s'. Object member or end bracket expected.", lexer.Token ());
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
          case JsonLexem_ArrayBeginBracket:
            ParseArray ();
            
            break;
          case JsonLexem_ObjectBeginBracket:
            ParseObject ();
            break;
          case JsonLexem_EndOfFile:
            return;
          case JsonLexem_Undefined:
            ProcessLexError ();
            break;
          default:
            Error ("syntax error. Unexpected token '%s'. Object or array expected.", lexer.Token ());
            break;
        }
        
        lexer.NextLexem ();
      }
    }

  private:
    typedef stl::vector<const char*> AttributeCache;

  private:
    ParseTreeBuilder& builder;          //построитель дерева грамматического разбора
    ParseLog&         log;              //протокол грамматического разбора
    JsonLexer         lexer;            //лексический анализатор
    const char*       name;             //имя разбираемого буфера
    AttributeCache    attributes;       //кеш атрибутов
    size_t            attributes_count; //количество атрибутов в кеше
};

/*
    Компонент, регистрирующий JSON-парсер
*/

class JsonParserComponent
{
  public:
    JsonParserComponent ()
    {
      ParserManager::RegisterParser ("json", &JsonParser::Parse);
    }
};

extern "C"
{

ComponentRegistrator<JsonParserComponent> JsonParser (JSON_PARSER_COMPONENT_NAME);

}

}

}
