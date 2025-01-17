#ifndef RENDER_DX11_DRIVER_OUTPUT_MANAGER_HEADER
#define RENDER_DX11_DRIVER_OUTPUT_MANAGER_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/device.h>

#include <shared/common.h>
#include <shared/default_resources.h>
#include <shared/device_manager.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние контекста менеджера выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputManagerContextState  (const DeviceManager&);
    ~OutputManagerContextState ();

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
    void                SetStencilReference  (size_t reference);
    IDepthStencilState* GetDepthStencilState () const;
    size_t              GetStencilReference  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование состояния
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (const StateBlockMask&, OutputManagerContextState& dst_state) const;

  protected:
    struct Impl;

    Impl& GetImpl () const;

    OutputManagerContextState (Impl*);

  private:
    OutputManagerContextState (const OutputManagerContextState&); //no impl
    OutputManagerContextState& operator = (const OutputManagerContextState&); //no impl

  private:
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст менеджера выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputManagerContext: public OutputManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputManagerContext  (const DeviceManager&, const DxContextPtr&, const DefaultResources&);
    ~OutputManagerContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния уровня в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  protected:
    struct Impl;

    Impl& GetImpl () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputManager  (const DeviceManager&);
    ~OutputManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание состояний выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);
    IRasterizerState*   CreateRasterizerState   (const RasterizerDesc&);

  private:
    OutputManager (const OutputManager&); //no impl
    OutputManager& operator = (const OutputManager&); //no impl

  private:  
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
