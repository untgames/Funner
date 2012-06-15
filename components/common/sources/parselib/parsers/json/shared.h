#ifndef COMMONLIB_PARSER_JSON_SHARED_HEADER
#define COMMONLIB_PARSER_JSON_SHARED_HEADER

#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/strconv.h>
#include <common/utf_converter.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексема грамматического разбора Json-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum JsonLexem
{
  JsonLexem_Undefined,               //не определена
  JsonLexem_Colon,                   //двоеточие
  JsonLexem_Comma,                   //запятая
  JsonLexem_ObjectBeginBracket,      //открывающая скобка объекта
  JsonLexem_ObjectEndBracket,        //закрывающая скобка объекта
  JsonLexem_ArrayBeginBracket,       //открывающая скобка массива
  JsonLexem_ArrayEndBracket,         //закрывающая скобка массива
  JsonLexem_EndOfFile,               //конец файла
  JsonLexem_String,                  //строка
  JsonLexem_Number,                  //числовое значение
  JsonLexem_Boolean,                 //булевое значение
  JsonLexem_Null,                    //null-значение поля

  JsonLexem_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние лексического анализатора Json-файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum JsonLexerStatus
{
  JsonLexerStatus_NoError,                    //нет ошибки
  JsonLexerStatus_UnclosedString,             //незакрытая строка
  JsonLexerStatus_WrongChar,                  //неверный символ
  JsonLexerStatus_WrongNumber,                //неверное числовое значение
  JsonLexerStatus_WrongKeyword,               //неверное числовое значение
  JsonLexerStatus_InvalidCharacterReference,  //неверная ссылка на символ

  JsonLexerStatus_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексический анализатор
///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonLexer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сканирование следующей лексемы
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexem NextLexem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка есть ли в текущем массиве вложенные массивы или объекты
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ArrayHasObjects ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    JsonLexem       Lexem        () const { return current_lexem; }           //текущая лексема
    JsonLexerStatus Status       () const { return current_status; }          //текущий статус разбора
    size_t          LineNumber   () const { return line_number; }             //текущая строка
    size_t          LinePosition () const { return cursor - line_start + 1; } //смещение от начала строки
    const char*     Token        () const { return current_token; }           //текущий токен
    char            CurrentChar  () const { return cursor == erased_char_position ? erased_char : *cursor; } //текущий символ

  private:
    void Skip ();
    void NextLine ();
    void ReadString ();
    void ReadSymbolReference (char*& write_position);
    void ReadNumber ();
    void ReadKeyword (JsonLexem lexem, const char* keyword);
    void SetError (JsonLexerStatus error, const char* position);

  private:
    JsonLexem       current_lexem;
    JsonLexerStatus current_status;
    char            erased_char;
    char*           erased_char_position;
    char*           position;
    const char*     current_token;
    char*           line_start;
    const char*     cursor;
    size_t          line_number;
};

}

#endif
