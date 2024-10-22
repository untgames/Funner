#ifndef RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER
#define RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER

#include <cmath>
#include <cfloat>

#include <stl/algorithm>
#include <stl/auto_ptr.h>

#include <xtl/array>
#include <xtl/common_exceptions.h>
#include <xtl/stat_counter.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/hash.h>

#include <render/low_level/utils.h>

#include <shared/context_object.h>
#include <shared/texture_manager.h>

#include "nv_dxt/blockdxt.h"
#include "nv_dxt/colorblock.h"

#undef min
#undef max

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы элементов кэша контекстной таблицы
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureManagerCacheEntry
{
  CacheEntry_TextureId0 = CacheEntry_TextureManagerPrivateFirst,
  CacheEntry_TextureIdLast = CacheEntry_TextureId0 + DEVICE_SAMPLER_SLOTS_COUNT - 1,
  CacheEntry_TextureTarget0,
  CacheEntry_TextureTargetLast = CacheEntry_TextureTarget0 + DEVICE_SAMPLER_SLOTS_COUNT - 1,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор слоя текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LayerDesc
{
  GLenum       target;    //тип текстуры
  unsigned int new_index; //индекс слоя в образе получаемом посредством glGetTexImage
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор текстуры устанавливамой в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BindableTextureDesc
{
  unsigned int target; //целевой тип текстуры
  unsigned int id;     //идентификатор текстуры
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные мип-уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureLevelData
{
  unsigned int size; //размер буфера
  const void*  data; //буфер с данными
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртка для выборки текстурных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureDataSelector
{
  public: 
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureDataSelector (const TextureDesc& desc, const TextureData* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetLevelData (unsigned int width, unsigned int height, unsigned int depth, TextureLevelData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переход к следующему блоку
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Next ();
    
  private:
    PixelFormat        format;
    const TextureData* data;
    unsigned int       data_size;
    unsigned int       index;
    unsigned int       images_count;
    unsigned int       offset;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура устанавливаемая в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class BindableTexture: virtual public ITexture, virtual public IRenderTargetTexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескрипторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    using ITexture::GetDesc;
    using IRenderTargetTexture::GetDesc;
    
    virtual void GetDesc (BindableTextureDesc&) = 0;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текстуры в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение формата пикселей
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual PixelFormat GetFormat () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка / получение хэша дескриптора прикрепленного сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void   SetSamplerHash (size_t hash) = 0;
    virtual size_t GetSamplerHash () = 0;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    BindableTexture (const ContextManager& context_manager) : ContextObject (context_manager) {}      
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс реализации текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture: public BindableTexture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc         (TextureDesc&);
    void GetDesc         (RenderTargetTextureDesc&);
    void GetDesc         (BindableTextureDesc&); 
    void GetMipLevelDesc (unsigned int level, MipLevelDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о текстуре
///////////////////////////////////////////////////////////////////////////////////////////////////
    GLenum       GetTarget    () { return target; }      //получение типа текстуры
    GLuint       GetTextureId () { return texture_id; }  //получение идентификатора текстуры
    unsigned int GetMipsCount () { return mips_count; }  //получение количества mip-уровней
    PixelFormat  GetFormat    () { return desc.format; } //получение формата текстуры

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка / получение хэша дескриптора прикрепленного сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerHash (size_t hash);
    size_t GetSamplerHash ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текстуры в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с данными
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat source_format, const void* buffer, IDeviceContext* = 0);
    void GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext* = 0);
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture  (const ContextManager&, const TextureDesc& desc, GLenum target, unsigned int mips_count);
    ~Texture ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение формата текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFormat (PixelFormat);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора слоя текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetLayerDesc (unsigned int layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                                      GLenum format, GLenum type, const void* buffer) = 0;
    virtual void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                                    GLenum format, unsigned int buffer_size, const void* buffer) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Генерация mip-уровней
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildMipmaps (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int unclamped_width, unsigned int height, PixelFormat format, const void* data);
    
  private:
    TextureDesc   desc;                //дескриптор текстуры
    GLenum        target;              //тип текстуры
    GLuint        texture_id;          //идентификатор текстуры
    unsigned int  mips_count;          //количество мип-уровней
    size_t        binded_sampler_hash; //хэш дескриптора прикрепленного сэмплера
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Одномерная текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1D: public Texture, public xtl::instance_counter<Texture1D>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1D (const ContextManager&, const TextureDesc& texture_desc, const TextureData* data, bool ignore_null_data = false);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Одномерная текстура без возможности изменения части изображения
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1DNoSubimage: public Texture1D, public xtl::instance_counter<Texture1DNoSubimage>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1DNoSubimage (const ContextManager&, const TextureDesc&, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);

  private:
    GLenum gl_internal_format;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Двумерная текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D: public Texture, public xtl::instance_counter<Texture2D>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2D (const ContextManager&, const TextureDesc&, const TextureData* data, bool ignore_null_data = false);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Двумерная текстура без возможности изменения части изображения
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2DNoSubimage: public Texture2D, public xtl::instance_counter<Texture2DNoSubimage>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2DNoSubimage (const ContextManager&, const TextureDesc&, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);

  private:
    GLenum gl_internal_format;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура со сторонами не степени 2, работающая через расширение GL_EXT_texture_rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureNpot: public Texture, public xtl::instance_counter<TextureNpot>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureNpot (const ContextManager&, const TextureDesc& texture_desc, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Трёхмерная текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture3D : public Texture, public xtl::instance_counter<Texture3D>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture3D (const ContextManager&, const TextureDesc&, const TextureData* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с данными
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext* context);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кубмап текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureCubemap : public Texture, public xtl::instance_counter<TextureCubemap>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureCubemap (const ContextManager&, const TextureDesc&, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора слоя текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetLayerDesc (unsigned int layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                            GLenum format, unsigned int buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура со сторонами, масштабированными до ближайшей степени двойки
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScaledTexture: public BindableTexture, public xtl::instance_counter<ScaledTexture>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScaledTexture (const ContextManager&, TextureManager&, const TextureDesc& original_desc, const TextureData* data, unsigned int scaled_width=0, unsigned int scaled_height=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        GetDesc         (TextureDesc&);
    void        GetDesc         (RenderTargetTextureDesc&);
    void        GetDesc         (BindableTextureDesc&);
    void        GetMipLevelDesc (unsigned int level, MipLevelDesc& desc);
    PixelFormat GetFormat       ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка / получение хэша дескриптора прикрепленного сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerHash (size_t hash);
    size_t GetSamplerHash ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текстуры в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с данными
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat source_format, const void* buffer, IDeviceContext* = 0);
    void GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext* = 0);
    
  private:
    typedef xtl::com_ptr<BindableTexture>    TexturePtr;    
    typedef xtl::uninitialized_storage<char> Buffer;

    void ScaleImage (unsigned int width, unsigned int height, PixelFormat source_format, const void* src_buffer, Buffer& scaled_buffer, PixelFormat& scaled_format);

  private:
    TexturePtr    shadow_texture;   //теневая текстура, со сторонами кратными степени двойки
    unsigned int  original_width;   //ширина оригинальной текстуры
    unsigned int  original_height;  //высота оригинальной текстуры
    float         horisontal_scale; //коэффициент растяжения по горизонтали
    float         vertical_scale;   //коэффициент растяжения по вертикали
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сэмплер
///////////////////////////////////////////////////////////////////////////////////////////////////
class SamplerState : virtual public ISamplerState, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SamplerState  (const ContextManager& manager, const SamplerDesc& desc);
    ~SamplerState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выбор сэмплера в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (GLenum texture_target, bool is_depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение/получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const SamplerDesc&);
    void GetDesc (SamplerDesc&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение хэша дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t GetDescHash ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Утилиты
///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int get_image_size                      (PixelFormat format, unsigned int width, unsigned int height, unsigned int depth); //получение размера битового образа
unsigned int get_next_higher_power_of_two        (unsigned int size);        //получение ближайшей сверху степени двойки
GLint        get_gl_internal_format              (PixelFormat format); //внутренний формат OpenGL
GLenum       get_gl_format                       (PixelFormat format); //формат OpenGL
GLenum       get_gl_type                         (PixelFormat format); //тип OpenGL
GLenum       get_uncompressed_gl_internal_format (PixelFormat format); //распакованный внутренний формат OpenGL
GLenum       get_uncompressed_gl_format          (PixelFormat format); //распакованный формат OpenGL
GLenum       get_uncompressed_gl_type            (PixelFormat format); //распакованный тип OpenGL
PixelFormat  get_pixel_format                    (GLenum gl_format);   //эквивалент внутреннего формата OpenGL

///////////////////////////////////////////////////////////////////////////////////////////////////
///Масштабирование текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
void scale_image_2x_down (PixelFormat format, unsigned int width, unsigned int height, const void* src, void* dest);
void scale_image         (PixelFormat format, unsigned int width, unsigned int height, unsigned int new_width, unsigned int new_height, const void* src, void* dest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с DXT форматом при остутствии аппаратной поддержки
///////////////////////////////////////////////////////////////////////////////////////////////////
void unpack_dxt (PixelFormat format, unsigned int width, unsigned int height, const void* dxt_data, void* unpacked_data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование изображений различных форматов
///////////////////////////////////////////////////////////////////////////////////////////////////
void copy_image (unsigned int pixels_count, PixelFormat src_format, const void* src_buffer, PixelFormat dst_format, void* dst_buffer);

}

}

}

#endif
