#include "shared.h"

using namespace media;

namespace
{

//Реализация менеджера наборов символов
class CharsetManagerImpl
{
  public:
    //Регистрация наборов символов
    void RegisterCharset (const char* name, const char* chars)
    {
      CharsetsMap::iterator iter = charsets.find (name);

      CharsetPtr charset (new Charset (chars), false);

      if (iter != charsets.end ())
        iter->second = charset;
      else
        charsets.insert_pair (name, charset);
    }

    void UnregisterCharset (const char* name)
    {
      charsets.erase (name);
    }

    void UnregisterAllCharsets ()
    {
      charsets.clear ();
    }

    ///Поиск набора символов
    const char* FindCharset (const char* name)
    {
      CharsetsMap::iterator iter = charsets.find (name);

      if (iter == charsets.end ())
        return 0;

      return iter->second->utf8_charset.c_str ();
    }

    ///Поиск отсортированного набора символов без дубликатов в utf32 кодировке
    const unsigned int* FindSortedUtf32Charset (const char* name)
    {
      CharsetsMap::iterator iter = charsets.find (name);

      if (iter == charsets.end ())
        return 0;

      return &iter->second->utf32_charset.front ();
    }

  private:
    //Данные набора символов
    struct Charset : public  xtl::reference_counter
    {
      stl::string               utf8_charset;  //исходный набор символов
      stl::vector<unsigned int> utf32_charset; //отсортированный набор символов без дубликатов в utf32 кодировке

      Charset (const char* in_utf8_charset)
        : utf8_charset (in_utf8_charset)
      {
        size_t utf8_charset_length = xtl::xstrlen (in_utf8_charset);

        utf32_charset.resize (utf8_charset_length);

        const void* source_ptr  = in_utf8_charset;
        size_t      source_size = utf8_charset_length;
        void*       dst_ptr     = &utf32_charset.front ();
        size_t      dst_size    = utf8_charset_length * sizeof (int);

        common::convert_encoding (common::Encoding_UTF8, source_ptr, source_size, common::Encoding_UTF32LE, dst_ptr, dst_size);

        if (source_size)
          throw xtl::format_operation_exception ("", "Can't convert charset to utf32");

        size_t charset_size = utf32_charset.size () - dst_size / sizeof (int);

        utf32_charset.resize (charset_size);

        stl::sort (utf32_charset.begin (), utf32_charset.end ());

        utf32_charset.erase (stl::unique (utf32_charset.begin (), utf32_charset.end ()), utf32_charset.end ());

        utf32_charset.push_back (0);
      }
    };

    typedef xtl::intrusive_ptr<Charset>                           CharsetPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, CharsetPtr> CharsetsMap;

  private:
    CharsetsMap charsets;
};

typedef common::Singleton<CharsetManagerImpl> CharsetManagerSingleton;

}

/*
   Менеджер наборов символов
*/

/*
   Регистрация наборов символов
*/

void CharsetManager::RegisterCharset (const char* name, const char* chars)
{
  static const char* METHOD_NAME = "media::CharsetManager::RegisterCharset";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!chars)
    throw xtl::make_null_argument_exception (METHOD_NAME, "chars");

  CharsetManagerSingleton::Instance ()->RegisterCharset (name, chars);
}

void CharsetManager::UnregisterCharset (const char* name)
{
  if (!name)
    return;

  CharsetManagerSingleton::Instance ()->UnregisterCharset (name);
}

void CharsetManager::UnregisterAllCharsets ()
{
  CharsetManagerSingleton::Instance ()->UnregisterAllCharsets ();
}

/*
   Поиск набора символов
*/

const char* CharsetManager::FindCharset (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::CharsetManager::FindCharset", "name");

  return CharsetManagerSingleton::Instance ()->FindCharset (name);
}

/*
   Поиск отсортированного набора символов без дубликатов в utf32 кодировке
*/

const unsigned int* CharsetManager::FindSortedUtf32Charset (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::CharsetManager::FindCharset", "name");

  return CharsetManagerSingleton::Instance ()->FindSortedUtf32Charset (name);
}
