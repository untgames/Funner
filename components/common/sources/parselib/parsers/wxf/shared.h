#ifndef COMMONLIB_PARSER_WXF_SHARED_HEADER
#define COMMONLIB_PARSER_WXF_SHARED_HEADER

#include <cctype>
#include <cstddef>

#include <stl/string>
#include <stl/vector>

#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/parser.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексемы
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WxfLexem
{
  WxfLexem_Undefined,    //не определена
  WxfLexem_EndOfFile,    //конец файла
  WxfLexem_NewLine,      //новая строка
  WxfLexem_Value,        //значение (число)
  WxfLexem_Identifier,   //идентификатор
  WxfLexem_String,       //строка
  WxfLexem_BeginBracket, //открывающая скобка '('
  WxfLexem_EndBracket,   //закрывающая скобка ')'
  WxfLexem_BeginFrame,   //начало фрейма '{'
  WxfLexem_EndFrame,     //конец фрейма '}'
  WxfLexem_Directive,    //директива '#...'

  WxfLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Статус лексического анализатора wxf-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WxfLexerStatus
{
  WxfLexerStatus_NoError,          //нет ошибки
  WxfLexerStatus_UnclosedComment,  //незакрытый коментарий
  WxfLexerStatus_UnclosedString,   //незакрытая строка
  WxfLexerStatus_WrongChar,        //неверный символ
  WxfLexerStatus_WrongIdentifier,  //неверный идентификатор
  WxfLexerStatus_WrongValue,       //неверное значение
  WxfLexerStatus_WrongDirective,   //неверная директива

  WxfLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексический анализатор
///////////////////////////////////////////////////////////////////////////////////////////////////
class WxfLexer
{
  public:      
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сканирование следующей лексемы
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexem NextLexem  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfLexem       Lexem        () const { return current_lexem; }               //текущая лексема
    WxfLexerStatus Status       () const { return current_status; }              //текущая ошибка
    size_t         LineNumber   () const { return line_number; }                 //текущая строка
    size_t         LinePosition () const { return cursor - line_start + 1; }     //смещение от начала строки
    const char*    Token        () const { return current_token; }               //текущий токен
    char           CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //текущий символ

  private:
    WxfLexer (const WxfLexer&); //no impl
    WxfLexer& operator = (const WxfLexer&); //no impl
  
    void Skip ();
    void SkipBlockComment ();
    void SkipRestOfLine ();
    void SkipWrongToken ();
    void NextLine ();
    void ReadString (char border);
    void ReadIdentifier ();
    void ReadValue ();
    void ReadDirective ();
    void SetError (WxfLexerStatus status, char* position = 0);
    void SetErrorAndSkip (WxfLexerStatus status, char* position = 0);

  private:
    WxfLexem       current_lexem;
    WxfLexerStatus current_status;
    char           erased_char;
    char*          erased_char_position;
    char*          position;
    char*          current_token;
    char*          line_start;
    char*          cursor;
    size_t         line_number;
};

}

#endif
