#ifndef RENDER_DX11_DRIVER_SHADER_STAGE_HEADER
#define RENDER_DX11_DRIVER_SHADER_STAGE_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/device.h>

#include <shared/common.h>
#include <shared/default_resources.h>
#include <shared/device_manager.h>
#include <shared/input_layout.h>

namespace render
{

namespace low_level
{

namespace dx11
{

//forwards
class ShaderLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние контекста менеджера шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManagerContextState  (const DeviceManager&);
    ~ShaderManagerContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление конфигурацией входных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetInputLayout (InputLayout* state);
    InputLayout* GetInputLayout () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProgram                 (IProgram* program);
    void SetProgramParametersLayout (IProgramParametersLayout* parameters_layout);
    void SetConstantBuffer          (size_t buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* GetProgramParametersLayout () const;
    IProgram*                 GetProgram                 () const;
    IBuffer*                  GetConstantBuffer          (size_t buffer_slot) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование состояния
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (const StateBlockMask&, ShaderManagerContextState& dst_state) const;

  protected:
    struct Impl;

    Impl& GetImpl () const;

    ShaderManagerContextState (Impl*);

  private:
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст менеджера шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManagerContext: public ShaderManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManagerContext  (ShaderLibrary&, const DxContextPtr&, const DefaultResources&);
    ~ShaderManagerContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  private:
    struct Impl;

    Impl& GetImpl () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManager  (const DeviceManager&);
    ~ShaderManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    IBuffer*                  CreateConstantBuffer          (const BufferDesc&, const void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение библиотеки шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderLibrary& GetShaderLibrary () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строки поддерживаемых контекстом профилей шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////        
    const char* GetShaderProfilesString () const;
    
  private:
    ShaderManager (const ShaderManager&);             //no impl
    ShaderManager& operator = (const ShaderManager&); //no impl
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
