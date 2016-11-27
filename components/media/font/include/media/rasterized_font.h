#ifndef MEDIALIB_RASTERIZED_FONT_HEADER
#define MEDIALIB_RASTERIZED_FONT_HEADER

#include <cstdlib>

namespace media
{

//forward declarations
class Font;
class Image;
class IFontRasterizer;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о растеризованном глифе
//////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizedGlyphInfo
{
  unsigned int image_index; //индекс изображения с растеризованным глифом
  unsigned int x_pos;       //X координата нижнего левого угла глифа
  unsigned int y_pos;       //Y координата нижнего левого угла глифа
  unsigned int width;       //Ширина глифа
  unsigned int height;      //Высота глифа
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Растеризованный шрифт
//////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizedFont
{
  friend class Font;

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont  (const RasterizedFont& source);
    ~RasterizedFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont& operator = (const RasterizedFont&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Растеризованные глифы
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RasterizedGlyphInfo* RasterizedGlyphs () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int ImagesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Построение картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildImage (unsigned int image_index, media::Image& out_image) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RasterizedFont&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizedFont (IFontRasterizer*);

  private:
    struct Impl;
    Impl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RasterizedFont&, RasterizedFont&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Растеризатор шрифта
//////////////////////////////////////////////////////////////////////////////////////////////////
class IFontRasterizer
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о растрированных символах
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const RasterizedGlyphInfo* RasterizedGlyphs () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Построение картинок
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int ImagesCount () = 0;
    virtual void         BuildImage  (unsigned int image_index, media::Image& out_image) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчет ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IFontRasterizer () {}
};

}

#endif
