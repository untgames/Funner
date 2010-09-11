#ifndef COMMONLIB_STRING_LIBRARY_HEADER
#define COMMONLIB_STRING_LIBRARY_HEADER

#include <stl/string>
#include <xtl/functional_fwd>

#include <common/hash.h>
#include <common/string.h>

namespace common
{

//forward declarations
class PropertyMap;

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
    Отсечение пробелов
*/

stl::string strip (const char* str, const char* spaces=" \t");
stl::string trim  (const char* str, const char* spaces=" \t");

/*
    Разбиение строки
*/

stl::string word (const char* str, size_t word_index, const char* delimiters=" ", const char* spaces=" \t", const char* brackets="");
stl::string word (const stl::string& str, size_t word_index, const char* delimiters=" ", const char* spaces=" \t", const char* brackets="");

StringArray split (const char* str,const char* delimiters=" ",const char* spaces=" \t", const char* brackets="");
StringArray split (const stl::string& str,const char* delimiters=" ",const char* spaces=" \t", const char* brackets="");

/*
    Разбор строк инициализации (property1=value property2='string value')
*/

void        parse_init_string (const char* init_string, const xtl::function<void (const char* property, const char* value)>& fn);
PropertyMap parse_init_string (const char* init_string);
void        parse_format_string (const char* format_string, const xtl::function<void (const char* prefix, const char* replacement_tag)>& fn);

/*
    Работа с регулярными выражениями
*/

StringArray parse   (const char* string,const char* re_pattern, const char* flags="");
stl::string replace (const char* string,const char* re_pattern,const char* replacement, const char* flags="");

bool rematch  (const char* string,const char* re_pattern, const char* flags="");
bool wcmatch  (const char* string,const char* wildcard);
bool wcimatch (const char* string,const char* wildcard);

bool is_wildcard (const char* string);

/*
    Получение строки с информацией об ошибке (портируемый аналог strerror)
*/

const char* strerror (int code);

/*
    Компрессия / декомпрессия последовательности байт в символы
*/

void decompress_buffer (size_t src_buffer_size, const void* src_buffer, char* dst_buffer); //dst_buffer_size = buffer_size * 2 + 1
void compress_buffer   (size_t src_buffer_size, const char* src_buffer, void* dst_buffer); //dst_buffer_size = src_buffer_size / 2

#include <common/detail/strlib.inl>

}

#endif
