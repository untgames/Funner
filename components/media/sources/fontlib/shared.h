#ifndef FONTLIB_SHARED_HEADER
#define FONTLIB_SHARED_HEADER

#include <common/singleton.h>
#include <stl/string>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/memory>
#include <media/font.h>
#include <xtl/function.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация гарнитуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class FontFaceImpl
{
  public:
    FontFaceImpl () {glyphs_count = 0;}
    FontFaceImpl (size_t first_char, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning, const char* font_file_name);

    size_t                     first_char_index;  //char-код первого глифа
    size_t                     glyphs_count;      //количество глифов
    stl::auto_ptr<GlyphInfo>   glyphs;            //глифы
    stl::auto_ptr<KerningInfo> kerning_table;     //таблица межзнаковых интервалов
    stl::string                file_name;         //имя файла с содержимым гарнитуры
    stl::string                str_name;          //имя гарнитуры
};

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация системы шрифтов
///////////////////////////////////////////////////////////////////////////////////////////////////
class FontSystemImpl
{
  public:
    typedef media::FontSystem::CodecLoadFunc CodecLoadFunc;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    FontSystemImpl ();
    ~FontSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация открытых гарнитур / закрытие всех открытых гарнитур
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFontFace   (media::FontFace&);
    void UnregisterFontFace (media::FontFace&);
    void CloseAllFontFaces  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация и получение пользовательских функций загрузки гарнитур
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool           RegisterLoadFunc   (const char* extension, const CodecLoadFunc& codec);
    void           UnRegisterLoadFunc (const char* extension);
    void           UnRegisterAllFuncs ();
    CodecLoadFunc* GetLoadFunc        (const char* extension);

  private:
    typedef stl::hash_set<media::FontFace*>           OpenFontFacesSet;
    typedef stl::hash_map<stl::string, CodecLoadFunc> LoadCodecs;

  private:    
    OpenFontFacesSet open_font_faces;  //список открытых гарнитур
    LoadCodecs       load_codecs;      //список пользовательских функций загрузки
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон системы шрифтов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<FontSystemImpl> FontSystemSingleton;

#endif
