#ifndef COMMONLIB_PARSER_XML_SHARED_HEADER
#define COMMONLIB_PARSER_XML_SHARED_HEADER

#include <cctype>
#include <cstring>

#include <stl/vector>

#include <xtl/function.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/parser.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексема грамматического разбора Xml-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum XmlLexem
{
  XmlLexem_Undefined,               //не определена
  XmlLexem_Equal,                   //равенство
  XmlLexem_Keyword,                 //ключевое слово
  XmlLexem_CData,                   //раздел CDATA
  XmlLexem_SectorBeginBracket,      //открывающая скобка раздела
  XmlLexem_SectorEndBracket,        //закрывающая скобка раздела
  XmlLexem_TagBeginBracket,         //открывающая скобка тега
  XmlLexem_TagEndBracket,           //закрывающая скобка тега
  XmlLexem_TagClose,                //закрытие тега '/>'
  XmlLexem_TagCloseFrame,           //закрытие тега '</'
  XmlLexem_EndOfFile,               //конец файла
  XmlLexem_Token,                   //идентификатор, начинающийся с цифры
  XmlLexem_Identifier,              //идентификатор
  XmlLexem_String,                  //строка
  XmlLexem_InstructionBeginBracket, //открывающая скобка инструкции по обработке
  XmlLexem_InstructionEndBracket,   //закрывающая скобка инструкции по обработке

  XmlLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние лексического анализатора Xml-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum XmlLexerStatus
{
  XmlLexerStatus_NoError,                    //нет ошибки
  XmlLexerStatus_UnclosedComment,            //незакрытый коментарий
  XmlLexerStatus_UnclosedString,             //незакрытая строка
  XmlLexerStatus_UnclosedCData,              //незакрытый раздел CDATA
  XmlLexerStatus_WrongChar,                  //неверный символ
  XmlLexerStatus_WrongIdentifier,            //неверный идентификатор
  XmlLexerStatus_InvalidCharacterReference,  //неверная ссылка на символ

  XmlLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексический анализатор
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlLexer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сканирование следующей лексемы
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexem NextLexem (bool content = false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlLexem       Lexem        () const { return current_lexem; }           //текущая лексема
    XmlLexerStatus Status       () const { return current_status; }          //текущий статус разбора
    size_t         LineNumber   () const { return line_number; }             //текущая строка
    size_t         LinePosition () const { return cursor - line_start + 1; } //смещение от начала строки
    const char*    Token        () const { return current_token; }           //текущий токен
    char           CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //текущий символ

  private:
    void Skip ();
    void SkipBlockComment ();
    void NextLine ();
    void ReadContentString ();
    void ReadString (char border, char* terminators = 0, size_t terminators_count = 0);  //если встречается border - он заменяется нулем и текущая позиция переводится на следующий символ, если встречается терминатор - разбор строки останавливается
    void ReadSymbolReference (char* write_position);
    void ReadCData ();
    void ReadIdentifier (bool identifier);
    void SetError (XmlLexerStatus error, const char* position);
    void ProcessTagBeginBracket (bool process_cdata);
    void ReadTextString ();

  private:
    XmlLexem       current_lexem;
    XmlLexerStatus current_status;
    char           erased_char;
    char*          erased_char_position;
    char*          position;
    const char*    current_token;
    char*          line_start;
    const char*    cursor;
    size_t         line_number;
};

}

#endif
