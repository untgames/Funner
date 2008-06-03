#ifndef COMMONLIB_STRING_LIBRARY_HEADER
#define COMMONLIB_STRING_LIBRARY_HEADER

#include <stl/string>
#include <stl/vector>
#include <xtl/functional_fwd>

#include <common/hash.h>

namespace common
{

/*
    Функции работы с именами путей
*/

stl::string basename (const char*);
stl::string suffix   (const char*);
stl::string dir      (const char*);
stl::string notdir   (const char*);
stl::string basename (const stl::string&);
stl::string suffix   (const stl::string&);
stl::string dir      (const stl::string&);
stl::string notdir   (const stl::string&);

/*
    Форматирование режима файла и флагов поиска файлов
*/

typedef size_t filemode_t;

stl::string strfilemode    (filemode_t mode);
stl::string strsearchflags (size_t search_flags);

/*
    Форматированный вывод в строку
*/

stl::string format  (const char*,...);
stl::string vformat (const char*,va_list);

/*
    Замена управляющих символов их обозначениями (\n, \r, etc.)
*/

stl::string compress   (const char*);
stl::string decompress (const char* str,const char* exception = NULL);

/*
    Разбиение строки
*/

//strip/trim/...

stl::string word (const char* str, size_t word_index, const char* delimiters=" ", const char* spaces=" \t", const char* brackets="");
stl::string word (const stl::string& str, size_t word_index, const char* delimiters=" ", const char* spaces=" \t", const char* brackets="");

stl::vector<stl::string> split (const char* str,const char* delimiters=" ",const char* spaces=" \t", const char* brackets="");
stl::vector<stl::string> split (const stl::string& str,const char* delimiters=" ",const char* spaces=" \t", const char* brackets="");

/*
    Разбор строк инициализации (property1=value property2='string value')
*/

void parse_init_string (const char* init_string, const xtl::function<void (const char* property, const char* value)>& fn);

/*
    Работа с регулярными выражениями
*/

stl::vector<stl::string> parse   (const char* string,const char* re_pattern, const char* flags="");
stl::string              replace (const char* string,const char* re_pattern,const char* replacement, const char* flags="");

bool rematch  (const char* string,const char* re_pattern, const char* flags="");
bool wcmatch  (const char* string,const char* wildcard);
bool wcimatch (const char* string,const char* wildcard);

#include <common/detail/strlib.inl>

}

#endif
