#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/buffer.h>
#include <render/low_level/state.h>
#include <render/low_level/view.h>
#include <render/low_level/shader.h>
#include <render/low_level/query.h>
#include <render/low_level/state_block.h>

namespace render
{

namespace low_level
{

//forward declarations
class ISwapChain;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги очистки буфера кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ClearFlag
{
  ClearFlag_RenderTarget = 1, //очищать буферы цвета (render-target)
  ClearFlag_Depth        = 2, //очищать буфер глубины
  ClearFlag_Stencil      = 4, //очищать буфер трафарета
  ClearFlag_ViewportOnly = 8, //очищать только область вывода

  ClearFlag_DepthStencil = ClearFlag_Depth | ClearFlag_Stencil,
  ClearFlag_All          = ClearFlag_RenderTarget | ClearFlag_DepthStencil
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType
{
  PrimitiveType_PointList,     //список точек
  PrimitiveType_LineList,      //список отрезков
  PrimitiveType_LineStrip,     //цепочка отрезков
  PrimitiveType_TriangleList,  //список треугольников
  PrimitiveType_TriangleStrip, //цепочка треугольников
  PrimitiveType_TriangleFan,   //вейер треугольников

  PrimitiveType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DeviceCaps
{
  size_t max_texture_1d_size;            //максимальная длина одномерной текстуры
  size_t max_texture_2d_size;            //максимальный размер ребра двумерной текстуры
  size_t max_texture_3d_size;            //максимальный размер ребра трёхмерной текстуры
  size_t max_texture_cubemap_size;       //максимальный размер ребра кубической текстуры
  size_t max_texture_anisotropy;         //максимальный уровень анизотропии
  size_t samplers_count;                 //количество текстурных сэмплеров
  bool   has_advanced_blend;             //поддерживаются сложные режимы смешивания цветов
  bool   has_depth_texture;              //поддерживаются текстуры с информацией о глубине
  bool   has_multisample;                //поддерживается мультисэмплинг
  bool   has_occlusion_queries;          //поддерживаются запросы видимости геометрии
  bool   has_shadow_maps;                //поддерживаются карты теней
  bool   has_texture_anisotropic_filter; //поддерживается анизотропная фильтрация
  bool   has_texture_cube_map;           //поддерживаются кубические текстуры
  bool   has_texture_lod_bias;           //поддерживаются уровни детализации текстур
  bool   has_texture_mirrored_repeat;    //поддерживается отражение текстурных координат
  bool   has_texture_non_power_of_two;   //поддерживаются текстуры с размерами не кратными степени двойки
  bool   has_texture1d;                  //поддерживаются одномерные текстуры
  bool   has_texture3d;                  //поддерживаются трёхмерные текстуры
  bool   has_two_side_stencil;           //поддерживается двусторонний буфер трафарета
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы строк возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
enum DeviceCapString
{
  DeviceCapString_TextureCompressionFormats, //строка со списком внутренних форматов сжатия текстур
  DeviceCapString_ShaderProfiles,            //строка со списком профилей шейдеров
  
  DeviceCapString_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Устройство отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        GetCaps      (DeviceCaps&) = 0;
    virtual const char* GetCapString (DeviceCapString) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение номера внутреннего формата сжатия текстур по имени (-1 в случае отсутствия формата)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int GetTextureCompressionFormat (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayout*             CreateInputLayout             (const InputLayoutDesc&) = 0;
    virtual IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&) = 0;
    virtual IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&) = 0;
    virtual IBlendState*              CreateBlendState              (const BlendDesc&) = 0;
    virtual IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&) = 0;
    virtual ISamplerState*            CreateSamplerState            (const SamplerDesc&) = 0;
    virtual IBuffer*                  CreateBuffer                  (const BufferDesc&) = 0;
    virtual IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&) = 0;
    virtual ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain) = 0;
    virtual IView*                    CreateView                    (ITexture* texture, const ViewDesc& desc) = 0;
    virtual IPredicate*               CreatePredicate               () = 0;
    virtual IStatisticsQuery*         CreateStatisticsQuery         () = 0;
    virtual IStateBlock*              CreateStateBlock              (const StateBlockMask& mask) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление входным уровнем (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void          ISSetInputLayout  (IInputLayout* state) = 0;
    virtual void          ISSetVertexBuffer (size_t vertex_buffer_slot,  IBuffer* buffer) = 0;
    virtual void          ISSetIndexBuffer  (IBuffer* buffer) = 0;
    virtual IInputLayout* ISGetInputLayout  () = 0;
    virtual IBuffer*      ISGetVertexBuffer (size_t vertex_buffer_slot) = 0;
    virtual IBuffer*      ISGetIndexBuffer  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление шейдерными уровнями (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                      SSSetProgram                 (IProgram* program) = 0;
    virtual void                      SSSetProgramParametersLayout (IProgramParametersLayout* parameters_layout) = 0;
    virtual void                      SSSetSampler                 (size_t sampler_slot, ISamplerState* state) = 0;
    virtual void                      SSSetTexture                 (size_t sampler_slot, ITexture* texture) = 0;
    virtual void                      SSSetConstantBuffer          (size_t buffer_slot, IBuffer* buffer) = 0;
    virtual IProgramParametersLayout* SSGetProgramParametersLayout () = 0;
    virtual IProgram*                 SSGetProgram                 () = 0;
    virtual ISamplerState*            SSGetSampler                 (size_t sampler_slot) = 0;
    virtual ITexture*                 SSGetTexture                 (size_t sampler_slot) = 0;
    virtual IBuffer*                  SSGetConstantBuffer          (size_t buffer_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление растеризатором (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void              RSSetState    (IRasterizerState* state) = 0;
    virtual void              RSSetViewport (const Viewport& viewport) = 0;
    virtual void              RSSetScissor  (const Rect& scissor_rect) = 0;
    virtual IRasterizerState* RSGetState    () = 0;
    virtual const Viewport&   RSGetViewport () = 0;
    virtual const Rect&       RSGetScissor  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление выходным уровнем (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OSSetBlendState        (IBlendState* state) = 0;
    virtual void                OSSetDepthStencilState (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (size_t reference) = 0;
    virtual void                OSSetRenderTargets     (IView* render_target_view, IView* depth_stencil_view) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual size_t              OSGetStencilReference  () = 0;
    virtual IView*              OSGetRenderTargetView  () = 0;
    virtual IView*              OSGetDepthStencilView  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetView (const Color4f& color) = 0;
    virtual void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil) = 0;
    virtual void ClearViews            (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* GetPredicate      () = 0;
    virtual bool        GetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Draw        (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count) = 0;
    virtual void DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание завершения выполнения буфера команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush () = 0;
};

}

}

#endif
