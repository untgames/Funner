#include "shared.h"

using namespace common;

//TODO: ввести режимы лексического разбора (см. ParseContentText)!!!

namespace
{

enum XmlEncoding
{
  XmlEncoding_Unknown,
  XmlEncoding_UCS4BE,
  XmlEncoding_UCS4LE,
  XmlEncoding_UTF8,
  XmlEncoding_UTF16LE,
  XmlEncoding_UTF16BE,
  XmlEncoding_FromPI
};

struct PrologDesc
{
  const unsigned char* prolog;
  size_t              length;

  template <size_t Length>
  PrologDesc (const unsigned char (& in_prolog) [Length])
    : prolog ((const unsigned char*)&in_prolog)
    , length (Length)
    {}
};

}

namespace components
{

namespace xml_parser
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
    static void Parse (ParseTreeBuilder& builder, ParseLog& log, const char* name, size_t buffer_size, char* buffer)
    {
      try
      {
        XmlParser (builder, log, name, buffer_size, buffer);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::XmlParser::Parse");
        throw;
      }
    }

///Автоматическое определение возможности разбора парсером
    static bool Check (size_t buffer_size, const char* buffer)
    {
      static const unsigned char PROLOG_UCS4BE []      = { 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x6c };
      static const unsigned char PROLOG_UCS4LE []      = { 0x3c, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00 };
      static const unsigned char PROLOG_UTF8 []        = { 0x3c, 0x3f, 0x78, 0x6d, 0x6c };
      static const unsigned char PROLOG_UTF16LE []     = { 0x3c, 0x00, 0x3f, 0x00, 0x78, 0x00, 0x6d, 0x00, 0x6c, 0x00 };
      static const unsigned char PROLOG_UTF16BE []     = { 0x00, 0x3c, 0x00, 0x3f, 0x00, 0x78, 0x00, 0x6d, 0x00, 0x6c };
      static const unsigned char PROLOG_UTF8_BOM []    = { 0xef, 0xbb, 0xbf, 0x3c, 0x3f, 0x78, 0x6d, 0x6c };
      static const unsigned char PROLOG_UTF16BE_BOM [] = { 0xfe, 0xff, 0x00, 0x3c, 0x00, 0x3f, 0x00, 0x78, 0x00, 0x6d, 0x00, 0x6c };
      static const unsigned char PROLOG_UTF16LE_BOM [] = { 0xff, 0xfe, 0x3c, 0x00, 0x3f, 0x00, 0x78, 0x00, 0x6d, 0x00, 0x6c, 0x00 };
      static const unsigned char PROLOG_UCS4BE_BOM []  = { 0x00, 0x00, 0xfe, 0xff, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x6c };
      static const unsigned char PROLOG_UCS4LE_BOM []  = { 0x00, 0x00, 0xff, 0xfe, 0x3c, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00 };

      static const PrologDesc PROLOGS [] = { PROLOG_UCS4BE, PROLOG_UCS4LE, PROLOG_UTF8, PROLOG_UTF16LE,
                                             PROLOG_UTF16BE, PROLOG_UTF8_BOM, PROLOG_UTF16BE_BOM,
                                             PROLOG_UTF16LE_BOM, PROLOG_UCS4BE_BOM, PROLOG_UCS4LE_BOM};                                             

      for (size_t i = 0, count = sizeof (PROLOGS) / sizeof (*PROLOGS); i < count; i++)
      {
        const PrologDesc& prolog = PROLOGS [i];

        if (prolog.length > buffer_size)
          continue;

        if (!memcmp (buffer, prolog.prolog, prolog.length))
          return true;
      }

      return false;
    }

  private:    
///Конструктор
    XmlParser (ParseTreeBuilder& in_builder, ParseLog& in_log, const char* in_name, size_t buffer_size, char* in_buffer)
      : builder (in_builder)
      , log (in_log)
      , encoding (XmlEncoding_Unknown)
      , buffer (in_buffer)
      , name (in_name)
      , attributes (ATTRIBUTES_CACHE_SIZE)
      , attributes_count (0)
    {
      static const char* METHOD_NAME = "common::XmlParser::XmlParser";

      if (!buffer)
        return;

      size_t text_offset = 0;

      encoding = DetectEncoding ((unsigned char*)buffer, buffer_size, text_offset);
      
      if (encoding != XmlEncoding_Unknown && encoding != XmlEncoding_UTF8)
      {
        const char* source_encoding;

        switch (encoding)
        {
          case XmlEncoding_UCS4BE:
            source_encoding = "UCS-4BE";
            break;
          case XmlEncoding_UCS4LE:
            source_encoding = "UCS-4LE";
            break;
          case XmlEncoding_UTF16LE:
            source_encoding = "UTF-16LE";
            break;
          case XmlEncoding_UTF16BE:
            source_encoding = "UTF-16BE";
            break;
          default:
            throw xtl::format_operation_exception (METHOD_NAME, "Unknown encoding %d", encoding);
        }        
        
        Warning (0, "non utf-8 encoding %s used (this decreases parsing perfomance)", source_encoding);

        utf8_buffer.resize (buffer_size + sizeof (unsigned char) - text_offset, false);

        StringConverter string_converter (source_encoding, "UTF-8");

        const void* source_buffer           = buffer + text_offset;
        void*       destination_buffer      = utf8_buffer.data ();
        size_t      source_buffer_size      = buffer_size - 1 - text_offset,
                    destination_buffer_size = utf8_buffer.size ();

        string_converter.Convert (source_buffer, source_buffer_size, destination_buffer, destination_buffer_size);

        if (source_buffer_size || destination_buffer_size < sizeof (unsigned char))
          throw xtl::format_operation_exception (METHOD_NAME, "Can't convert buffer from '%s' to UTF-8 (source_buffer_size=%u, destination_buffer_size=%u)\n", source_encoding,
            source_buffer_size, destination_buffer_size);
          
        *(char*)destination_buffer = '\0';

        buffer = utf8_buffer.data ();
      }
      else
        buffer += text_offset;

      lexer.Reset (buffer);

      const char* prolog_end = strstr (buffer, "?>");

      if (prolog_end)
      {
        size_t prolog_length = prolog_end - buffer + 2;

        stl::string first_line (buffer, prolog_length);

        lexer.Reset (&first_line [0]);

        if (!FindEncoding ())
          lexer.Reset (buffer);
      }

      ParseDocument ();
    }
    
///Рестарт парсинга документа с конвертирование буфера в utf-8
    void ResetLexer (const char* encoding)
    {
      Warning (0, "non utf-8 encoding %s used (this decreases parsing perfomance)", encoding);
      
      size_t buffer_length = xtl::xstrlen (buffer);      

      utf8_buffer.resize (buffer_length * 4 + 1, false);      

      StringConverter string_converter (encoding, "UTF-8");

      const void* source_buffer           = buffer;
      void*       destination_buffer      = utf8_buffer.data ();
      size_t      source_buffer_size      = buffer_length,
                  destination_buffer_size = utf8_buffer.size ();

      string_converter.Convert (source_buffer, source_buffer_size, destination_buffer, destination_buffer_size);

      if (source_buffer_size || destination_buffer_size < sizeof (unsigned char))
        throw xtl::format_operation_exception ("common::XmlParser::RestartParsing", "Can't convert buffer from '%s' to UTF-8 (source_buffer_size=%u, destination_buffer_size=%u)\n", encoding,
          source_buffer_size, destination_buffer_size);
          
      *(char*)destination_buffer = '\0';

      buffer = utf8_buffer.data ();

      lexer.Reset (buffer);
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
        case XmlLexerStatus_InvalidCharacterReference:
          Error ("invalid character reference");
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
      
      if (xtl::xstrcmp ("xml", lexer.Token ()))
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

      for (;; lexer.NextLexem (true))
        switch (lexer.Lexem ())
        {
          case XmlLexem_Token:
          case XmlLexem_Identifier:
          case XmlLexem_String:
          case XmlLexem_CData:
          case XmlLexem_Equal:
          case XmlLexem_Undefined:
            if (lexer.Status () != XmlLexerStatus_NoError)
              ProcessLexError ();
            else
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

        lexer.NextLexem (true);
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
            lexer.NextLexem (true);
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

    bool FindEncoding ()
    {
      lexer.NextLexem ();

      for (;;)
      {
        switch (lexer.Lexem ())
        {
          case XmlLexem_InstructionBeginBracket:
            lexer.NextLexem ();

            if (lexer.Lexem () != XmlLexem_Identifier)
            {
              Error ("syntax error. Expected processing directive identifier");
              return false;
            }

            if (xtl::xstrcmp ("xml", lexer.Token ()))
            {
              Error ("processing directive must start from 'xml' prefix");
              return false;
            }

            if (lexer.NextLexem () == XmlLexem_Identifier)
            {
              const char *name = 0, *value = 0;
              size_t      line = 0;

              for (;;lexer.NextLexem ())
              {
                switch (lexer.Lexem ())
                {
                  case XmlLexem_Identifier:
                    if (ReadAttribute (name, value, line) && !xtl::xstrcmp (name, "encoding"))
                    {
                      switch (encoding)
                      {
                        case XmlEncoding_UTF8:
                        case XmlEncoding_Unknown:
                          encoding = XmlEncoding_FromPI;

                          if (xtl::xstricmp (value, "utf-8")) //не нужно конвертировать из utf8 в utf8
                          {
                            ResetLexer (value);
                            return true;
                          }
                          else
                            return false;
                        case XmlEncoding_UCS4BE:
                          if (xtl::xstrcmp (value, "utf-32be") && xtl::xstrcmp (value, "utf-32"))
                            Error (line, "File encoding is utf-32be but declaration encoding is '%s'", value);

                          return false;
                        case XmlEncoding_UCS4LE:
                          if (xtl::xstrcmp (value, "utf-32le") && xtl::xstrcmp (value, "utf-32"))
                            Error (line, "File encoding is utf-32le but declaration encoding is '%s'", value);

                          return false;
                        case XmlEncoding_UTF16LE:
                          if (xtl::xstrcmp (value, "utf-16le") && xtl::xstrcmp (value, "utf-16"))
                            Error (line, "File encoding is utf-16le but declaration encoding is '%s'", value);

                          return false;
                        case XmlEncoding_UTF16BE:
                          if (xtl::xstrcmp (value, "utf-16be") && xtl::xstrcmp (value, "utf-16"))
                            Error (line, "File encoding is utf-16be but declaration encoding is '%s'", value);

                          return false;
                        case XmlEncoding_FromPI:
                          return false;
                      }
                    }

                    break;
                  default:
                    ProcessError ();
                  case XmlLexem_InstructionEndBracket:
                    Warning (0, "Encoding is not specified, defaulting to utf-8");
                    return false;
                }
              }
            }

            return false;
          default:
            ProcessLexError ();
            return false;
        }

        lexer.NextLexem ();
      }

      Warning (0, "Encoding is not specified, defaulting to utf-8");

      return false;
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
      {
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
    }    

    XmlEncoding DetectEncoding (const unsigned char* buffer, size_t buffer_size, size_t& text_offset)
    {
        if (!buffer)
          return XmlEncoding_Unknown;

        if (buffer_size >= 4)
        {
          if ((buffer [0] == 0x00) && (buffer [1] == 0x00) && (buffer [2] == 0x00) && (buffer [3] == 0x3C))
            return XmlEncoding_UCS4BE;
          if ((buffer [0] == 0x3C) && (buffer [1] == 0x00) && (buffer [2] == 0x00) && (buffer [3] == 0x00))
            return XmlEncoding_UCS4LE;
          if ((buffer [0] == 0x3C) && (buffer [1] == 0x3F) && (buffer [2] == 0x78) && (buffer [3] == 0x6D))
            return XmlEncoding_UTF8;

          /*
           * Although not part of the recommendation, we also
           * attempt an "auto-recognition" of UTF-16LE and
           * UTF-16BE encodings.
           */
          if ((buffer [0] == 0x3C) && (buffer [1] == 0x00) && (buffer [2] == 0x3F) && (buffer [3] == 0x00))
            return XmlEncoding_UTF16LE;
          if ((buffer [0] == 0x00) && (buffer [1] == 0x3C) && (buffer [2] == 0x00) && (buffer [3] == 0x3F))
            return XmlEncoding_UTF16BE;

          /* For UTF-32 we can recognize by the BOM */
          if ((buffer [0] == 0x00) && (buffer [1] == 0x00) && (buffer [2] == 0xFE) && (buffer [3] == 0xFF))
          {
            text_offset = 4;
            return XmlEncoding_UCS4BE;
          }

          if ((buffer [0] == 0x00) && (buffer [1] == 0x00) && (buffer [2] == 0xFF) && (buffer [3] == 0xFE))
          {
            text_offset = 4;
            return XmlEncoding_UCS4LE;
          }
        }

        if (buffer_size >= 3)
        {
          /*
           * Errata on XML-1.0 June 20 2001
           * We now allow an UTF8 encoded BOM
           */
          if ((buffer [0] == 0xEF) && (buffer [1] == 0xBB) && (buffer [2] == 0xBF))
          {
            text_offset = 3;
            return XmlEncoding_UTF8;
          }
        }

        /* For UTF-16 we can recognize by the BOM */
        if (buffer_size >= 2)
        {
          if ((buffer [0] == 0xFE) && (buffer [1] == 0xFF))
          {
            text_offset = 2;
            return XmlEncoding_UTF16BE;
          }

          if ((buffer [0] == 0xFF) && (buffer [1] == 0xFE))
          {
            text_offset = 2;
            return XmlEncoding_UTF16LE;
          }
        }

        return XmlEncoding_Unknown;
    }

  private:
    typedef stl::vector<const char*> AttributeCache;

  private:
    ParseTreeBuilder&                builder;          //построитель дерева грамматического разбора
    ParseLog&                        log;              //протокол грамматического разбора
    XmlLexer                         lexer;            //лексический анализатор
    XmlEncoding                      encoding;         //кодировка разбираемого буфера
    char*                            buffer;           //разбираемый буфер
    const char*                      name;             //имя разбираемого буфера
    AttributeCache                   attributes;       //кеш атрибутов
    size_t                           attributes_count; //количество атрибутов в кеше
    xtl::uninitialized_storage<char> utf8_buffer;      //буфер после конвертации в UTF-8
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

extern "C"
{

ComponentRegistrator<XmlParserComponent> XmlParser (XML_PARSER_COMPONENT_NAME);

}

}

}
