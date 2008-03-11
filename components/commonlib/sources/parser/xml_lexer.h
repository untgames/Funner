#include <stddef.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексический анализатор
///////////////////////////////////////////////////////////////////////////////////////////////////
class XMLLexer
{
  public:      
    enum Lexem {                 //лексема
      UNDEFINED,                 //не определена
      EQUAL,                     //равенство
      KEYWORD,                   //ключевое слово
      CDATA,                     //раздел CDATA
      SECTOR_BEGIN_BRACKET,      //открывающая скобка раздела
      SECTOR_END_BRACKET,        //закрывающая скобка раздела
      TAG_BEGIN_BRACKET,         //открывающая скобка тега
      TAG_END_BRACKET,           //закрывающая скобка тега
      TAG_CLOSE,                 //закрытие тега
      END_OF_FILE,               //конец файла
      TOKEN,                     //идентификатор, начинающийся с цифры
      IDENTIFIER,                //идентификатор
      STRING,                    //строка
      INSTRUCTION_BEGIN_BRACKET, //открывающая скобка инструкции по обработке
      INSTRUCTION_END_BRACKET    //закрывающая скобка инструкции по обработке
    };
      
    enum Error {        //код ошибки
      NO_ERROR,         //нет ошибки
      UNCLOSED_COMMENT, //незакрытый коментарий
      UNCLOSED_STRING,  //незакрытая строка
      UNCLOSED_CDATA,   //незакрытый раздел CDATA
      WRONG_CHAR,       //неверный символ
      WRONG_IDENTIFIER, //неверный идентификатор
    };

    XMLLexer ();

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
    void nextline ();
    void read_string (char border);
    void read_shifted_string (char border);
    void read_cdata ();
    void read_identifier (bool identifier);

    void SetError (Error,char*);

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
    bool   name_char_map [256];
};

}
