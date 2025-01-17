#ifndef RENDER_MANAGER_DYNAMIC_TEXTURE_HEADER
#define RENDER_MANAGER_DYNAMIC_TEXTURE_HEADER

namespace render
{

namespace manager
{

//forward declarations
class Frame;
class Texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс динамической текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDynamicTexture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual manager::Texture Texture () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update (Frame& frame) = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Освобождение текстуры (для возможности использования shared dynamic texture)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Release () = 0;

  protected:
    virtual ~IDynamicTexture () {}  
};

}

}

#endif
