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
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDynamicTexture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual manager::Texture Texture () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update (Frame& frame) = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������� (��� ����������� ������������� shared dynamic texture)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Release () = 0;

  protected:
    virtual ~IDynamicTexture () {}  
};

}

}

#endif