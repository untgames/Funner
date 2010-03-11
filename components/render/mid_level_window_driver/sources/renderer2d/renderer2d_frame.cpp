#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;
using namespace render::low_level;

namespace
{

/*
    Константы
*/

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 2048; //резервируемое количество примитивов

/*
    Контекст рендеринга
*/

struct RenderContext
{
  render::low_level::IDevice*     device;                     //устройство
  render::low_level::ITexture*    current_texture;            //текущая текстура
  render::low_level::IBlendState* current_blend_state;        //текущее состояние уровня смешивания цветов
  render::low_level::Rect*        current_scissor;            //текущая область отсечения
  ShaderMode                      current_shader_mode;        //текущий режим шейдинга
  CommonResources*                resources;                  //ресурсы
  render::low_level::IBuffer*     common_constant_buffer;     //буфер констант
  render::low_level::IBuffer*     dynamic_constant_buffer;    //буфер констант
  DynamicProgramParameters        dynamic_program_parameters; //динамические параметры программы рендеринга
};

/*
    Создание константных буферов
*/

//создание константного буфера общего назначения
BufferPtr create_common_constant_buffer (IDevice& device)
{
  BufferDesc constant_buffer_desc;

  memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

  constant_buffer_desc.size         = sizeof (CommonProgramParameters);
  constant_buffer_desc.usage_mode   = UsageMode_Dynamic;
  constant_buffer_desc.bind_flags   = BindFlag_ConstantBuffer;
  constant_buffer_desc.access_flags = AccessFlag_ReadWrite;

  return BufferPtr (device.CreateBuffer (constant_buffer_desc), false);
}

//создание константного буфера динамичесих параметров
BufferPtr create_dynamic_constant_buffer (IDevice& device)
{
  BufferDesc constant_buffer_desc;

  memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

  constant_buffer_desc.size         = sizeof (DynamicProgramParameters);
  constant_buffer_desc.usage_mode   = UsageMode_Stream;
  constant_buffer_desc.bind_flags   = BindFlag_ConstantBuffer;
  constant_buffer_desc.access_flags = AccessFlag_ReadWrite;

  return BufferPtr (device.CreateBuffer (constant_buffer_desc), false);
}

}

/*
    Конструктор / деструктор
*/

Frame::Frame (CommonResources* in_common_resources, render::low_level::IDevice* device)
  : common_resources (in_common_resources)
{
  common_constant_buffer  = create_common_constant_buffer  (*device);
  dynamic_constant_buffer = create_dynamic_constant_buffer (*device);

  primitives.reserve (PRIMITIVE_ARRAY_RESERVE_SIZE);
}

/*
    Матрица вида / матрица преобразования
*/

void Frame::SetViewPoint (const math::vec3f& point)
{
  common_program_parameters.view_matrix = math::translate (-point);
}

void Frame::SetProjection (const math::mat4f& tm)
{
  common_program_parameters.projection_matrix = tm;
}

void Frame::GetViewPoint (math::vec3f& point)
{
  point = -common_program_parameters.view_matrix.column (3);
}

void Frame::GetProjection (math::mat4f& tm)
{
  tm = common_program_parameters.projection_matrix;
}

/*
    Примитивы
*/

//количество примитивов в кадре
size_t Frame::PrimitivesCount ()
{
  return primitives.size ();
}

//добавление примитива
void Frame::AddPrimitive (IPrimitive* in_primitive)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::renderer2d::Frame::AddPrimitive";

  if (!in_primitive)
    throw xtl::make_null_argument_exception (METHOD_NAME, "primitive");

     //приведение типа примитива

  Primitive* casted_primitive = dynamic_cast<Primitive*> (in_primitive);

  if (!casted_primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "primitive", typeid (in_primitive).name (),
      "Primitive type incompatible with render::mid_level::window_driver::renderer2d::Primitive");

    //добавление примитива в список примитивов кадра (для захвата ресурсов примитива)

  primitives.push_back (casted_primitive);
}

//очистка кадра
void Frame::Clear ()
{
  primitives.clear ();
}

/*
    Обновление вершинных буферов
*/

namespace
{

bool not_blended_sprite_sort_predicate (const RenderableSprite* sprite1, const RenderableSprite* sprite2)
{
  const RenderablePrimitive &primitive1 = *sprite1->primitive,
                            &primitive2 = *sprite2->primitive;

  if (primitive1.blend_mode == primitive2.blend_mode)
    return primitive1.texture < primitive2.texture;

  return primitive1.blend_mode > primitive2.blend_mode;
}

bool blended_sprite_sort_predicate (const RenderableSprite* sprite1, const RenderableSprite* sprite2)
{
    //возможно добавить сортировку по блендингу и текстурам??

//  if (sprite1->vertices [0].position.z != sprite2->vertices [0].position.z)
    return sprite1->vertices [0].position.z > sprite2->vertices [0].position.z;

     //сортировка нужна для визуализации изометрии, в будущем убрать в настройки

//  return sprite1->vertices [0].position.y > sprite2->vertices [0].position.y;
}

}

void Frame::UpdateVertexBuffers (render::low_level::IDevice& device)
{
    //очистка списков вывода

  not_blended_sprites.Clear ();
  blended_sprites.Clear ();

    //добавление спрайтов

  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive& primitive = **iter;

    switch (primitive.GetBlendMode ())
    {
      case BlendMode_None:
        primitive.AddSprites (not_blended_sprites);
        break;
      default:
        primitive.AddSprites (blended_sprites);
        break;
    }
  }

    //сортировка геометрии

  not_blended_sprites.Sort (not_blended_sprite_sort_predicate);
  blended_sprites.Sort     (blended_sprite_sort_predicate);

    //обновление вершинных буферов

  not_blended_sprites.UpdateVertexBuffer (device);
  blended_sprites.UpdateVertexBuffer     (device);
}

/*
    Реализация визуализации
*/

namespace
{

//рисование непрозрачных спрайтов
void draw_solid_sprites (RenderContext& context, const RenderableSprite** sprites, size_t first_sprite, size_t sprites_count)
{
  for (const RenderableSprite **sprite=sprites+first_sprite, **last_sprite=sprite+sprites_count; sprite != last_sprite;)
  {
    render::low_level::ITexture* texture         = (*sprite)->primitive->texture;
    render::low_level::Rect*     scissor         = (*sprite)->primitive->scissor;
    float                        alpha_reference = (*sprite)->primitive->alpha_reference;
    ShaderMode                   shader_mode     = (*sprite)->primitive->shader_mode;

    size_t base_sprite_index = sprite++ - sprites;

    for (; sprite != last_sprite && (*sprite)->primitive->texture == texture && (*sprite)->primitive->alpha_reference == alpha_reference &&
      scissor == (*sprite)->primitive->scissor && (*sprite)->primitive->shader_mode == shader_mode; sprite++);

    size_t draw_sprites_count = sprite - sprites - base_sprite_index;

    if (context.dynamic_program_parameters.alpha_reference != alpha_reference)
    {
      context.dynamic_program_parameters.alpha_reference = alpha_reference;

      context.dynamic_constant_buffer->SetData (0, sizeof (context.dynamic_program_parameters), &context.dynamic_program_parameters);
    }
    
    if (shader_mode != context.current_shader_mode)
    {
      switch (shader_mode)
      {
        case ShaderMode_Normal:
          context.device->SSSetProgram (context.resources->GetDefaultProgram ());
          break;
        case ShaderMode_AlphaClamp:
          context.device->SSSetProgram (context.resources->GetAlphaClampProgram ());
          break;
        case ShaderMode_Reflection:
          context.device->SSSetProgram (context.resources->GetReflectionProgram ());
          break;
        default:
          break;
      }
      
      context.current_shader_mode = shader_mode;
    }
    
    if (context.current_scissor != scissor)
    {
      if (scissor)
      {
        context.device->RSSetScissor (*scissor);
        context.device->RSSetState   (context.resources->GetRasterizerState (true));
      }
      else
      {
        context.device->RSSetState (context.resources->GetRasterizerState (false));
      }

      context.current_scissor = scissor;
    }

    if (context.current_texture != texture)
    {
      context.device->SSSetTexture (0, texture);

      context.current_texture = texture;
    }

    context.device->DrawIndexed (render::low_level::PrimitiveType_TriangleList, base_sprite_index * SPRITE_INDICES_COUNT,
      draw_sprites_count * SPRITE_INDICES_COUNT, 0);
  }
}

//рисование спрайтов с прозрачностью
void draw_blend_sprites (RenderContext& context, const RenderableSprite** sprites, size_t first_sprite, size_t sprites_count)
{
  for (const RenderableSprite **sprite=sprites+first_sprite, **last_sprite=sprite+sprites_count; sprite != last_sprite;)
  {
    render::low_level::ITexture*             texture         = (*sprite)->primitive->texture;
    render::mid_level::renderer2d::BlendMode blend_mode      = (*sprite)->primitive->blend_mode;
    render::low_level::Rect*                 scissor         = (*sprite)->primitive->scissor;
    float                                    alpha_reference = (*sprite)->primitive->alpha_reference;
    ShaderMode                               shader_mode     = (*sprite)->primitive->shader_mode;

    size_t base_sprite_index = sprite++ - sprites;

    for (; sprite != last_sprite && (*sprite)->primitive->texture == texture && (*sprite)->primitive->blend_mode == blend_mode &&
      (*sprite)->primitive->scissor == scissor && (*sprite)->primitive->shader_mode == shader_mode && (*sprite)->primitive->alpha_reference == alpha_reference; sprite++);

    size_t draw_sprites_count = sprite - sprites - base_sprite_index;

    if (context.current_scissor != scissor)
    {
      if (scissor)
      {
        context.device->RSSetScissor (*scissor);
        context.device->RSSetState   (context.resources->GetRasterizerState (true));
      }
      else
      {
        context.device->RSSetState (context.resources->GetRasterizerState (false));
      }

      context.current_scissor = scissor;
    }
    
    if (shader_mode != context.current_shader_mode)
    {
      switch (shader_mode)
      {
        case ShaderMode_Normal:
          context.device->SSSetProgram (context.resources->GetDefaultProgram ());
          break;
        case ShaderMode_AlphaClamp:
          context.device->SSSetProgram (context.resources->GetAlphaClampProgram ());
          break;
        case ShaderMode_Reflection:
          context.device->SSSetProgram (context.resources->GetReflectionProgram ());
          break;
        default:
          break;
      }
      
      context.current_shader_mode = shader_mode;
    }
    
    if (context.dynamic_program_parameters.alpha_reference != alpha_reference)
    {
      context.dynamic_program_parameters.alpha_reference = alpha_reference;

      context.dynamic_constant_buffer->SetData (0, sizeof (context.dynamic_program_parameters), &context.dynamic_program_parameters);
    }

    if (context.current_texture != texture)
    {
      context.device->SSSetTexture (0, texture);

      context.current_texture = texture;
    }

    render::low_level::IBlendState* blend_state = context.resources->GetBlendState (blend_mode);

    if (context.current_blend_state != blend_state)
    {
      context.device->OSSetBlendState (blend_state);

      context.current_blend_state = blend_state;
    }

    context.device->DrawIndexed (render::low_level::PrimitiveType_TriangleList, base_sprite_index * SPRITE_INDICES_COUNT,
      draw_sprites_count * SPRITE_INDICES_COUNT, 0);
  }
}

}

void Frame::DrawCore (IDevice* device)
{
    //установка области вывода

  BasicFrame::BindViewport (device);     

    //обновление парметров шейдинга

  common_constant_buffer->SetData (0, sizeof (common_program_parameters), &common_program_parameters);

    //обновление вершинных буферов

  UpdateVertexBuffers (*device);

    //установка общих ресурсов

  device->ISSetInputLayout             (common_resources->GetInputLayout ());
  device->SSSetConstantBuffer          (0, common_constant_buffer.get ());
  device->SSSetConstantBuffer          (1, dynamic_constant_buffer.get ());
  device->SSSetProgramParametersLayout (common_resources->GetProgramParametersLayout ());
  device->SSSetSampler                 (0, common_resources->GetSamplerState ());
  device->RSSetState                   (common_resources->GetRasterizerState (0));

    //отрисовка спрайтов без блендинга

  device->ISSetVertexBuffer      (0, not_blended_sprites.GetVertexBuffer ());
  device->ISSetIndexBuffer       (not_blended_sprites.GetIndexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (true));
  device->OSSetBlendState        (common_resources->GetBlendState (BlendMode_None));
  
    //заполнение контекста отрисовки
  
  RenderContext context;
  
  memset (&context, 0, sizeof (context));
  
  context.device                                     = device;
  context.current_texture                            = device->SSGetTexture (0);
  context.current_scissor                            = 0;
  context.current_blend_state                        = device->OSGetBlendState ();
  context.current_shader_mode                        = ShaderMode_Normal;
  context.resources                                  = &*common_resources;
  context.common_constant_buffer                     = &*common_constant_buffer;
  context.dynamic_constant_buffer                    = &*dynamic_constant_buffer;
  context.dynamic_program_parameters.alpha_reference = 0.0f;
  
  device->SSSetProgram (common_resources->GetDefaultProgram ());
  
    //поиск первого спрайта без полупрозрачности
    
  const RenderableSprite **first_sprite = not_blended_sprites.GetSprites (),
                         **last_sprite  = first_sprite + not_blended_sprites.Size ();

  draw_solid_sprites (context, first_sprite, 0, last_sprite - first_sprite);

    //отрисовка спрайтов с блендингом

  device->ISSetVertexBuffer      (0, blended_sprites.GetVertexBuffer ());
  device->ISSetIndexBuffer       (blended_sprites.GetIndexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (false));

  first_sprite = blended_sprites.GetSprites ();
  last_sprite  = first_sprite + blended_sprites.Size ();

  draw_blend_sprites (context, first_sprite, 0, last_sprite - first_sprite);
}
