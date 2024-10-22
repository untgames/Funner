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
///Выходной уровень конвейера OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputStage  (const ContextManager&);
    ~OutputStage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объекта состояния уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    IStageState* CreateStageState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание состояний выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);
    IRasterizerState*   CreateRasterizerState   (const RasterizerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка подуровня растеризации
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetRasterizerState (IRasterizerState* state);
    IRasterizerState* GetRasterizerState () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка подуровня смешивания цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetBlendState (IBlendState*);
    IBlendState* GetBlendState () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка подуровня попиксельного отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void                SetDepthStencilState (IDepthStencilState* state);
    void                SetStencilReference  (unsigned int reference);
    IDepthStencilState* GetDepthStencilState () const;
    unsigned int        GetStencilReference  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния уровня в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (const bool has_render_target [DEVICE_RENDER_TARGET_SLOTS_COUNT], bool has_depth_stencil);

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
