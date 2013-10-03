#ifndef RENDER_SCENE_SERVER_RENDER_TARGET_MAP_SHARED_HEADER
#define RENDER_SCENE_SERVER_RENDER_TARGET_MAP_SHARED_HEADER

#include <render/manager/common.h>

namespace render
{

namespace scene
{

namespace server
{

using manager::Rect;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetDesc
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetDesc  (const char* name, const manager::RenderTarget& target, const Rect& rect);
    RenderTargetDesc  (const RenderTargetDesc&);
    ~RenderTargetDesc ();  

    RenderTargetDesc& operator = (const RenderTargetDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                  Name   () const;
    const Rect&                  Rect   () const;
    const manager::RenderTarget& Target () const;
          manager::RenderTarget  Target ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IRenderTargetMapListener
{
  public:
    virtual void OnRenderTargetAdded   (const RenderTargetDesc& desc) = 0;
    virtual void OnRenderTargetRemoved (const char* name) = 0;

  protected:
    virtual ~IRenderTargetMapListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetMap
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetMap  ();
    RenderTargetMap  (const RenderTargetMap&);
    ~RenderTargetMap ();

    RenderTargetMap& operator = (const RenderTargetMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RenderTargetDesc* Find (const char* name) const;
          RenderTargetDesc* Find (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� 
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const char* name, const manager::RenderTarget& target, const Rect& rect);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachListener     (IRenderTargetMapListener*);
    void DetachListener     (IRenderTargetMapListener*);
    void DetachAllListeners ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
