#ifndef RENDER_MANAGER_RENDER_TARGET_HEADER
#define RENDER_MANAGER_RENDER_TARGET_HEADER

#include <cstddef>

namespace media
{

//forward declarations
class Image;
class CompressedImage;

}

namespace render
{

//implementation forwards
class RenderTargetImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTarget
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget  (const RenderTarget&);
    ~RenderTarget ();
    
    RenderTarget& operator = (const RenderTarget&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () const;
    size_t Height () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture (media::Image&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderTarget&);
    
  private:
    RenderTarget (RenderTargetImpl*);

  private:
    RenderTargetImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RenderTarget&, RenderTarget&);

}

#endif
