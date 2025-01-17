#ifndef RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_SHADER_STAGE_SHARED_HEADER

#include <stl/vector>
#include <stl/algorithm>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/common_exceptions.h>

#include <common/strlib.h>

#include <render/low_level/utils.h>

#include <render/low_level/helpers/program_parameters_layout.h>

#include <shared/shader_stage.h>
#include <shared/input_stage.h>

namespace render
{

namespace low_level
{

namespace opengl
{

using helpers::ProgramParameterGroup;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Элементы таблицы локальных данных контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ShadingStageCacheEntry
{
  CacheEntry_UsedProgram = CacheEntry_ShadingStagePrivateFirst, //ID текущей используемой программы
  CacheEntry_FfpViewerStateHash,             //хэш параметров наблюдателя
  CacheEntry_FfpObjectStateHash,             //хэш параметров объекта
  CacheEntry_FfpRasterizationStateHash,      //хэш параметров растеризации
  CacheEntry_FfpMaterialStateHash,           //хэш параметров материала
  CacheEntry_FfpLightingStateHash,           //хэш параметров освещения
  CacheEntry_FfpTexmapsStateHash,            //хэш параметров текстурирования
  CacheEntry_FfpModesStateHash,              //хэш режимов визуализации
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание расположения параметров программы шейдинга
///////////////////////////////////////////////////////////////////////////////////////////////////
class ProgramParametersLayout: public helpers::ProgramParametersLayout, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramParametersLayout (const ContextManager& manager, const ProgramParametersLayoutDesc& desc)
      : helpers::ProgramParametersLayout (desc)
      , ContextObject (manager)
    {
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Шейдер
///////////////////////////////////////////////////////////////////////////////////////////////////
class IShader: virtual public IObject
{
  protected:
    virtual ~IShader () {}
};

typedef xtl::trackable_ptr<IBindableBuffer> ConstantBufferPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Программа шейдинга с возможностью установки в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class IBindableProgram: virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind (ConstantBufferPtr* constant_buffers) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка корректности контекста выполнения программы
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Validate () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Скомпилированная программа шейдинга
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICompiledProgram: virtual public IProgram
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание программы с возможностью установки в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IBindableProgram* CreateBindableProgram (ProgramParametersLayout* layout) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение словаря атрибутов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IVertexAttributeDictionary* GetVertexAttributeDictionary () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class IShaderManager : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество поддерживаемых профилей
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual size_t GetProfilesCount () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имена поддерживаемых профилей
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual const char* GetProfile (size_t index) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдера / программы
///////////////////////////////////////////////////////////////////////////////////////////////////
   virtual IShader*          CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log) = 0;
   virtual ICompiledProgram* CreateProgram (unsigned int shaders_count, IShader** shaders, const LogFunction& error_log) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдер менеджеров
///////////////////////////////////////////////////////////////////////////////////////////////////
IShaderManager* create_ffp_shader_manager  (const ContextManager& manager);
IShaderManager* create_glsl_shader_manager (const ContextManager& manager);

}

}

}

#endif
