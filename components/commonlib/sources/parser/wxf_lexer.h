#include <stddef.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексический анализатор
///////////////////////////////////////////////////////////////////////////////////////////////////
class WXFLexer
{
  public:      
    enum Lexem {     //лексема
      UNDEFINED,     //не определена
      END_OF_FILE,   //конец файла
      NEW_LINE,      //новая строка
      VALUE,         //значение (число)
      IDENTIFIER,    //идентификатор
      STRING,        //строка
      BEGIN_BRACKET, //открывающая скобка '('
      END_BRACKET,   //закрывающая скобка ')'
      BEGIN_FRAME,   //начало фрейма '{'
      END_FRAME,     //конец фрейма '}'
      DIRECTIVE      //директива '#...'
    };
      
    enum Error {        //код ошибки
      NO_ERROR,         //нет ошибки
      UNCLOSED_COMMENT, //незакрытый коментарий
      UNCLOSED_STRING,  //незакрытая строка
      WRONG_CHAR,       //неверный символ
      WRONG_IDENTIFIER, //неверный идентификатор
      WRONG_VALUE,      //неверное значение
      WRONG_DIRECTIVE   //неверная директива
    };

    WXFLexer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация. Установка буфера разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void reset (char* buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сканироване следующей лексемы
///////////////////////////////////////////////////////////////////////////////////////////////////
    Lexem next  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Lexem  lexem () const { return cur_lexem; }               //текущая лексема
    Error  error () const { return cur_error; }               //текущая ошибка
    size_t line  () const { return line_number; }             //текущая строка
    size_t offs  () const { return cursor - line_start + 1; } //смещение от начала строки

    const char* token   () const { return cur_token; }    //текущий токен
    char        curchar () const { return cursor == erased_char_pos ? erased_char : *cursor; } //текущий символ

  private:
    void skip ();
    void skip_block_comment ();
    void skip_rest_of_line ();
    void skip_wrong_token ();
    void nextline ();
    void read_string (char border);
    void read_identifier ();
    void read_value ();
    void read_directive ();

    void SetError        (Error error,char* position = NULL);
    void SetErrorAndSkip (Error error,char* position = NULL);

  private:    
    static char* null_ptr;
    static char  null_char;

    Lexem  cur_lexem;
    Error  cur_error;
    char   erased_char, *erased_char_pos;
    char*  pos;
    char*  buf;
    char*  cur_token;
    char*  line_start;
    char*  cursor;
    size_t line_number;
};

}
