#ifndef RENDER_MANAGER_RENDER_TARGET_HEADER
#define RENDER_MANAGER_RENDER_TARGET_HEADER

#include <cstddef>

#include <render/manager/bounds.h>

namespace media
{

//forward declarations
class Image;
class CompressedImage;

}

namespace render
{

namespace manager
{

//implementation forwards
class RenderTargetImpl;
class RenderTargetMapImpl;
class ViewportImpl;
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
    unsigned int Width  () const;
    unsigned int Height () const;
    
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Viewport
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Viewport  ();
    Viewport  (const manager::Rect& rect, float min_depth = 0.0f, float max_depth = 1.0f);
    ~Viewport ();
    
    Viewport& operator = (const Viewport&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetRect (const manager::Rect& rect);
    void                 SetRect (int x, int y, unsigned int width, unsigned int height);
    RectArea             Area    () const;
    const manager::Rect& Rect    () const;
    int                  X       () const;
    int                  Y       () const;
    unsigned int         Width   () const;
    unsigned int         Height  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float MinDepth    () const;
    float MaxDepth    () const;
    void  SetMinDepth (float value);
    void  SetMaxDepth (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Viewport&);
  
  private:
    Viewport (ViewportImpl*);
    
  private:
    ViewportImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Viewport&, Viewport&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetMap
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetMap  ();
    RenderTargetMap  (const RenderTargetMap&);
    ~RenderTargetMap ();
    
    RenderTargetMap& operator = (const RenderTargetMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const manager::RenderTarget& target);
    void SetRenderTarget        (const char* name, const manager::RenderTarget& target, const manager::Viewport& viewport);
    void SetRenderTarget        (const char* name, const manager::RenderTarget& target, const manager::Viewport& viewport, const RectArea& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool                  HasRenderTarget (const char* name) const;
    manager::RenderTarget RenderTarget    (const char* name) const;
    manager::Viewport     Viewport        (const char* name) const;
    RectArea              Scissor         (const char* name) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderTargetMap&);

  protected:
    RenderTargetMap (RenderTargetMapImpl*);
    
  private:
    RenderTargetMapImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RenderTargetMap&, RenderTargetMap&);

}

}

#endif
