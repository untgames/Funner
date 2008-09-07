#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_HEADER

#include <stl/auto_ptr.h>
#include <shared/context_manager.h>
#include <shared/stage_state.h>

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
    OutputStage  (const ContextManager&);
    ~OutputStage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IStageState* CreateStageState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);
    IRasterizerState*   CreateRasterizerState   (const RasterizerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetRasterizerState (IRasterizerState* state);
    IRasterizerState* GetRasterizerState () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetBlendState (IBlendState*);
    IBlendState* GetBlendState () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void                SetDepthStencilState (IDepthStencilState* state);
    void                SetStencilReference  (size_t reference);
    IDepthStencilState* GetDepthStencilState () const;
    size_t              GetStencilReference  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

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
