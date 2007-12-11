#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER

#include <stl/auto_ptr.h>
#include <shared/context_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputStage  (ContextManager&, ISwapChain*);
    ~OutputStage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IFrameBuffer* CreateFrameBuffer (const FrameBufferDesc&);
    IFrameBuffer* CreateFrameBuffer (ISwapChain*);
    IFrameBuffer* CreateFrameBuffer (ITexture* render_target);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetFrameBuffer (IFrameBuffer*);
    IFrameBuffer* GetFrameBuffer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetBlendState (IBlendState*);
    IBlendState* GetBlendState () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void                SetDepthStencil      (IDepthStencilState* state);
    void                SetStencilReference  (size_t reference);
    IDepthStencilState* GetDepthStencilState () const;
    size_t              GetStencilReference  () const;

  private:
    OutputStage (const OutputStage&); //no impl
    OutputStage& operator = (const OutputStage&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
