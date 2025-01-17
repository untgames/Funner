#ifndef MEDIALIB_CHARSET_MANAGER_HEADER
#define MEDIALIB_CHARSET_MANAGER_HEADER

namespace media
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер наборов символов
//////////////////////////////////////////////////////////////////////////////////////////////////
class CharsetManager
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация наборов символов
//////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterCharset       (const char* name, const char* chars);
    static void UnregisterCharset     (const char* name);
    static void UnregisterAllCharsets ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск набора символов
//////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* FindCharset (const char* name);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск отсортированного набора символов без дубликатов в utf32 кодировке
//////////////////////////////////////////////////////////////////////////////////////////////////
    static const unsigned int* FindSortedUtf32Charset (const char* name);
};

}

#endif
