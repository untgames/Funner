#ifndef RENDER_GL_DRIVER_DRIVER_SHARED_HEADER
#define RENDER_GL_DRIVER_DRIVER_SHARED_HEADER

#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <shared/context_manager.h>
#include <shared/input_stage.h>
#include <shared/object.h>
#include <shared/output_stage.h>
#include <shared/platform.h>
#include <shared/property_list.h>
#include <shared/query_manager.h>
#include <shared/render_target_manager.h>
#include <shared/shader_stage.h>
#include <shared/texture_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация драйвера для OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных адаптеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    GetAdaptersCount ();
    IAdapter* GetAdapter       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание адаптера
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain* swap_chain, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка функции отладочного протоколирования драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetDebugLog (const LogFunction&);
    const LogFunction& GetDebugLog ();

  private:
    void RegisterAdapter   (IAdapter*);
    void UnregisterAdapter (IAdapter*);

  private:
    typedef stl::vector<IAdapter*> AdapterArray;

  private:
    AdapterArray adapters; //адаптеры отрисовки
    LogFunction  log_fn;   //функция отладочного протоколирования
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Устройство отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class Device: virtual public IDevice, public Object
{
  friend class StateBlock;
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device  (ISwapChain* swap_chain, const char* init_string);
    ~Device ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Указатель на устройство отрисовки, которому принадлежит ресурс
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* GetDevice () { return this; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        GetCaps      (DeviceCaps&);
    const char* GetCapString (DeviceCapString);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение номера внутреннего формата сжатия текстур по имени (-1 в случае отсутствия формата)
///////////////////////////////////////////////////////////////////////////////////////////////////
    int GetTextureCompressionFormat (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    IInputLayout*             CreateInputLayout             (const InputLayoutDesc&);
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&);
    IBlendState*              CreateBlendState              (const BlendDesc&);
    IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&);
    ISamplerState*            CreateSamplerState            (const SamplerDesc&);
    IBuffer*                  CreateBuffer                  (const BufferDesc&);
    IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    ITexture*                 CreateTexture                 (const TextureDesc&);
    ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&);
    ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, size_t buffer_index);
    ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain);
    IView*                    CreateView                    (ITexture* texture, const ViewDesc&);
    IPredicate*               CreatePredicate               ();
    IStatisticsQuery*         CreateStatisticsQuery         ();
    IStateBlock*              CreateStateBlock              (const StateBlockMask& mask);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление входным уровнем (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          ISSetInputLayout  (IInputLayout* state);
    void          ISSetVertexBuffer (size_t vertex_buffer_slot,  IBuffer* buffer);
    void          ISSetIndexBuffer  (IBuffer* buffer);
    IInputLayout* ISGetInputLayout  ();
    IBuffer*      ISGetVertexBuffer (size_t vertex_buffer_slot);
    IBuffer*      ISGetIndexBuffer  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление шейдерными уровнями (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                      SSSetProgram                 (IProgram* program);
    void                      SSSetProgramParametersLayout (IProgramParametersLayout* parameters_layout);
    void                      SSSetSampler                 (size_t sampler_slot, ISamplerState* state);
    void                      SSSetTexture                 (size_t sampler_slot, ITexture* texture);
    void                      SSSetConstantBuffer          (size_t buffer_slot, IBuffer* buffer);
    IProgramParametersLayout* SSGetProgramParametersLayout ();
    IProgram*                 SSGetProgram                 ();
    ISamplerState*            SSGetSampler                 (size_t sampler_slot);
    ITexture*                 SSGetTexture                 (size_t sampler_slot);
    IBuffer*                  SSGetConstantBuffer          (size_t buffer_slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление растеризатором (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              RSSetState    (IRasterizerState* state);
    void              RSSetViewport (const Viewport& viewport);
    void              RSSetScissor  (const Rect& scissor_rect);
    IRasterizerState* RSGetState    ();
    const Viewport&   RSGetViewport ();
    const Rect&       RSGetScissor  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление выходным уровнем (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                OSSetBlendState        (IBlendState* state);
    void                OSSetDepthStencilState (IDepthStencilState* state);
    void                OSSetStencilReference  (size_t reference);    
    void                OSSetRenderTargets     (IView* render_target_view, IView* depth_stencil_view);
    IBlendState*        OSGetBlendState        ();
    IDepthStencilState* OSGetDepthStencilState ();
    size_t              OSGetStencilReference  ();
    IView*              OSGetRenderTargetView  ();
    IView*              OSGetDepthStencilView  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearRenderTargetView (const Color4f& color);
    void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil);
    void ClearViews            (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPredication    (IPredicate* predicate, bool predicate_value);
    IPredicate* GetPredicate      ();
    bool        GetPredicateValue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw        (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count);
    void DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание завершения выполнения буфера команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список свойств устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

  private:
    ITexture* CreateTexture (const TextureDesc&, const TextureData*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния устройства в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (size_t base_vertex, size_t base_index, IndicesLayout* out_indices_layout = 0);

  private:
    ContextManager      context_manager;        //менеджер контекстов OpenGL
    PropertyList        properties;             //свойства устройства
    RenderTargetManager render_target_manager;  //менеджер целевых буферов рендеринга
    OutputStage         output_stage;           //выходной уровень
    InputStage          input_stage;            //входной уровень
    TextureManager      texture_manager;        //менеджер текстур
    ShaderStage         shader_stage;           //уровень шейдинга
    QueryManager        query_manager;          //менеджер запросов
    size_t              cached_base_vertex;     //кэшированный индекс базовой вершины
    size_t              cached_base_index;      //кэшированный индекс базового индекса
    IndicesLayout       cached_indices_layout;  //кэшированное расположение индексов
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блок состояний устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class StateBlock: virtual public IStateBlock, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StateBlock (Device& device, const StateBlockMask&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение маски блока
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetMask (StateBlockMask&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Захват настроек устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Применение настроек устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Apply ();

  private:
    typedef stl::auto_ptr<IStageState> StageStatePtr;

  private:
    StateBlockMask mask;                        //маска состояний
    StageStatePtr  render_target_manager_state; //состояние менеджера целевых буферов рендеринга
    StageStatePtr  output_stage_state;          //состояние выходного уровня
    StageStatePtr  input_stage_state;           //состояние входного уровня
    StageStatePtr  texture_manager_state;       //состояние менеджера текстур
    StageStatePtr  shader_stage_state;          //состояние шейдерного уровня
    StageStatePtr  query_manager_state;         //состояние менеджера запросов
};

}

}

}

#endif
