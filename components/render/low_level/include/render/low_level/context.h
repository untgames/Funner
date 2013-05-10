#ifndef RENDER_LOW_LEVEL_CONTEXT_HEADER
#define RENDER_LOW_LEVEL_CONTEXT_HEADER

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
///Список команд
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICommandList: virtual public IObject {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDeviceContext: virtual public IObject
{
  public:  
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
/// в случае несовпадения расположения переменных в программе и лэйауте - генерируется исключение
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
    virtual void              RSSetViewport (size_t render_target_slot, const Viewport& viewport) = 0;
    virtual void              RSSetScissor  (size_t render_target_slot, const Rect& scissor_rect) = 0;
    virtual IRasterizerState* RSGetState    () = 0;
    virtual const Viewport&   RSGetViewport (size_t render_target_slot) = 0;
    virtual const Rect&       RSGetScissor  (size_t render_target_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление выходным уровнем (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OSSetBlendState        (IBlendState* state) = 0;
    virtual void                OSSetDepthStencilState (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (size_t reference) = 0;
    virtual void                OSSetRenderTargets     (size_t views_count, IView** render_target_view, IView* depth_stencil_view) = 0;
    virtual void                OSSetRenderTargetView  (size_t render_target_slot, IView* view) = 0;
    virtual void                OSSetDepthStencilView  (IView* view) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual size_t              OSGetStencilReference  () = 0;
    virtual IView*              OSGetRenderTargetView  (size_t render_target_slot) = 0;
    virtual IView*              OSGetDepthStencilView  () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление уровнем вывода вершин
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void     SOSetTargets      (size_t buffers_count, IBuffer** buffers, const size_t* offsets) = 0;
    virtual void     SOSetTarget       (size_t stream_output_slot, IBuffer* buffer, size_t offset) = 0;
    virtual IBuffer* SOGetTarget       (size_t stream_output_slot) = 0;
    virtual size_t   SOGetTargetOffset (size_t stream_output_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetView (size_t render_target_slot, const Color4f& color) = 0;
    virtual void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil) = 0;
    virtual void ClearViews            (size_t clear_flags, size_t render_targets_count, const size_t* render_target_indices, const Color4f* colors, float depth, unsigned char stencil) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Генерация мип-уровней текстуры (необходимо для текстур в которые ведется рендеринг)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GenerateMips (ITexture* texture) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* GetPredicate      () = 0;
    virtual bool        GetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawAuto             (PrimitiveType primitive_type) = 0;
    virtual void Draw                 (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count) = 0;
    virtual void DrawIndexed          (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex) = 0;
    virtual void DrawInstanced        (PrimitiveType primitive_type, size_t vertex_count_per_instance, size_t instance_count, size_t first_vertex, size_t first_instance_location) = 0;
    virtual void DrawIndexedInstanced (PrimitiveType primitive_type, size_t index_count_per_instance, size_t instance_count, size_t first_index, size_t base_vertex, size_t first_instance_location) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание завершения выполнения буфера команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение списка команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ICommandList* FinishCommandList  (bool restore_state = 0) = 0;
    virtual void          ExecuteCommandList (ICommandList* list, bool restore_state = false) = 0;
};

}

}

#endif
