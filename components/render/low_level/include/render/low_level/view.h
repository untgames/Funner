#ifndef RENDER_LOW_LEVEL_VIEW_HEADER
#define RENDER_LOW_LEVEL_VIEW_HEADER

#include <render/low_level/texture.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор отображения текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ViewDesc
{
  unsigned int layer;     //слой текстуры, на который делается отображение
  unsigned int mip_level; //mip-уровень текстуры, на который делается отображение
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отображение текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class IView: virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура, на которую строится отображение
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITexture* GetTexture () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (ViewDesc&) = 0;
};

}

}

#endif
