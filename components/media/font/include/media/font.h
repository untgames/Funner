#ifndef MEDIALIB_FONT_HEADER
#define MEDIALIB_FONT_HEADER

#include <cstdlib>

namespace media
{

//forward declarations
class FontImpl;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
//////////////////////////////////////////////////////////////////////////////////////////////////
struct GlyphInfo
{
  size_t width;      //������ �����
  size_t height;     //������ �����
  float  bearing_x;  //���������� �� ��������� ���� �� ����� ������� �����
  float  bearing_y;  //���������� �� ��������� ���� �� ������� ������� �����
  float  advance_x;  //������ �� ��� X �� �������� ��������� ���� �� ���������� ��������� ����
  float  advance_y;  //������ �� ��� Y �� �������� ��������� ���� �� ���������� ��������� ����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ����������� ����������� ����������
//////////////////////////////////////////////////////////////////////////////////////////////////
struct KerningInfo
{
  float x_kerning;   //������ �� ��� X �� ��������� ���� ��� ����������� ������ ����� �� ��������� ���� ��� ����������� ������� �����
  float y_kerning;   //������ �� ��� Y �� ��������� ���� ��� ����������� ������ ����� �� ��������� ���� ��� ����������� ������� �����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizedFontCreationParams
{
  size_t             max_image_size;
  bool               pot;
  size_t             glyph_margin;
  media::PixelFormat image_format;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������
//////////////////////////////////////////////////////////////////////////////////////////////////
class Font
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Font  (const Font& source);
    ~Font ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Font& operator = (const Font&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Source () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� / ��� ��������� / ��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name       () const;
    const char* FamilyName () const;
    const char* StyleName  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GlyphsCount () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FirstGlyphCode () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FontSize () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const GlyphInfo* Glyphs () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    KerningInfo Kerning    (size_t left_glyph_index, size_t right_glyph_index) const;
    bool        HasKerning (size_t left_glyph_index, size_t right_glyph_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::RasterizedFont CreateRasterizedFont (const RasterizedFontCreationParams&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Font&);

  private:
    Font (FontImpl*);

  private:
    FontImpl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Font&, Font&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
class FontBuilder
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
             FontBuilder  ();
             FontBuilder  (const FontBuilder& source);
    explicit FontBuilder  (const char* file_name);
             ~FontBuilder ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FontBuilder& operator = (const FontBuilder&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� ������� ������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetGlyphsCount (size_t new_glyphs_count);
    size_t GlyphsCount    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ���� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFirstGlyphCode (size_t new_first_glyph_code);
    size_t FirstGlyphCode    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFontSize (size_t new_font_size);
    size_t FontSize    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const GlyphInfo* Glyphs () const;
          GlyphInfo* Glyphs ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/���������/�������� ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        InsertKerning     (size_t left_glyph_index, size_t right_glyph_index, const KerningInfo& kerning_info);
    void        RemoveKerning     (size_t left_glyph_index, size_t right_glyph_index);
    void        RemoveAllKernings ();
    KerningInfo Kerning           (size_t left_glyph_index, size_t right_glyph_index) const;
    bool        HasKerning        (size_t left_glyph_index, size_t right_glyph_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<IFontRasterizer* (const Font&, const RasterizedFontCreationParams&)> RasterizerHandler;

    void                     SetRasterizer (const RasterizerHandler&);
    const RasterizerHandler& Rasterizer    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Font BuildFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (FontBuilder&);

  private:
    struct Impl;
    Impl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (FontBuilder&, FontBuilder&);

}

#endif
