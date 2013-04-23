#ifndef RENDER_DX11_DRIVER_SHADER_STAGE_HEADER
#define RENDER_DX11_DRIVER_SHADER_STAGE_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/device.h>

#include <shared/common.h>
#include <shared/input_layout.h>
#include <shared/shader_code.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManagerContextState  (const DeviceManager&);
    ~ShaderManagerContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetInputLayout (InputLayout* state);
    InputLayout* GetInputLayout () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProgram                 (IProgram* program);
    void SetProgramParametersLayout (IProgramParametersLayout* parameters_layout);
    void SetConstantBuffer          (size_t buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* GetProgramParametersLayout () const;
    IProgram*                 GetProgram                 () const;
    IBuffer*                  GetConstantBuffer          (size_t buffer_slot) const;

  protected:
    struct Impl;

    Impl& GetImpl () const;

    ShaderManagerContextState (Impl*);

  private:
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManagerContext: public ShaderManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManagerContext  (const DeviceManager&, const DxContextPtr&);
    ~ShaderManagerContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������, �������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  private:
    struct Impl;

    Impl& GetImpl () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderManager  (const DeviceManager&);
    ~ShaderManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������������� ���������� �������� ��������
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