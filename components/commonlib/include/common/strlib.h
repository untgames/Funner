#ifndef COMMONLIB_STRING_LIBRARY_HEADER
#define COMMONLIB_STRING_LIBRARY_HEADER

#include <common/strwrap.h>
#include <stl/string>
#include <stl/vector>

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

stl::string  format  (const char*,...);
stl::string  vformat (const char*,va_list);
stl::wstring format  (const wchar_t*,...);
stl::wstring vformat (const wchar_t*,va_list);

/*
    Замена управляющих символов их обозначениями (\n, \r, etc.)
*/

stl::string compress   (const char*);
stl::string decompress (const char* str,const char* exception = NULL);

/*
    Разбиение строки
*/

//strip/trim/...

stl::vector<stl::string> split (const char* str,const char* delimiters=" ",const char* spaces=" \t");
stl::vector<stl::string> split (const stl::string& str,const char* delimiters=" ",const char* spaces=" \t");

/*
    Работа с регулярными выражениями
*/

stl::vector<stl::string> parse   (const char* string,const char* re_pattern);
stl::string              replace (const char* string,const char* re_pattern,const char* replacement);

bool rematch  (const char* string,const char* re_pattern);
bool wcmatch  (const char* string,const char* wildcard);
bool wcimatch (const char* string,const char* wildcard);

/*
    Хэширование
*/

size_t strhash  (const stl::string&,size_t init_hash=0xFFFFFFFF);
size_t strihash (const stl::string&,size_t init_hash=0xFFFFFFFF);

#include <common/detail/strlib.inl>

}

#endif
