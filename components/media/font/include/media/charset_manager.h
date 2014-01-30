#ifndef MEDIALIB_CHARSET_MANAGER_HEADER
#define MEDIALIB_CHARSET_MANAGER_HEADER

namespace media
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер раскладок символов
//////////////////////////////////////////////////////////////////////////////////////////////////
class CharsetManager
{
  public:
    static void RegisterCharset       (const char* name, const char* chars);
    static void UnregisterCharset     (const char* name);
    static void UnregisterAllCharsets ();

    static const char* FindCharset    (const char* name);
};

}

#endif
