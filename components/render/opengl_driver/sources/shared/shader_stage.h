#ifndef RENDER_GL_DRIVER_SHADER_STAGE_HEADER
#define RENDER_GL_DRIVER_SHADER_STAGE_HEADER

#include <render/low_level/device.h>

#include <shared/context_manager.h>
#include <shared/context_object.h>
#include <shared/stage_state.h>

#include <stl/auto_ptr.h>

namespace render
{

namespace low_level
{

namespace opengl
{

//forward declarations
class IVertexAttributeDictionary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Shader-stage
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderStage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderStage  (const ContextManager&);
    ~ShaderStage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объекта состояния уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    IStageState* CreateStageState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IProgram*                 CreateProgram                 (unsigned int shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProgram                 (IProgram* program);
    void SetProgramParametersLayout (IProgramParametersLayout* parameters_layout);
    void SetConstantBuffer          (unsigned int buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    IProgramParametersLayout*   GetProgramParametersLayout   () const;
    IProgram*                   GetProgram                   () const;
    IBuffer*                    GetConstantBuffer            (unsigned int buffer_slot) const;
    IVertexAttributeDictionary* GetVertexAttributeDictionary () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строки поддерживаемых контекстом профилей шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////        
    const char* GetShaderProfilesString () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Валидация текущей программы перед отрисовкой
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ValidateBindedProgram ();

  private:
    ShaderStage (const ShaderStage&);             //no impl
    ShaderStage& operator = (const ShaderStage&); //no impl
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
