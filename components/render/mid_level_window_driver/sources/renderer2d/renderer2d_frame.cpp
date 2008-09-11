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
      case BlendMode_AlphaClamp:
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

//рисование спрайтов без блендинга
void draw_solid_sprites
 (IDevice&                 device,
  const RenderableSprite** base_sprite,
  const RenderableSprite** first_sprite,
  const RenderableSprite** last_sprite)
{
  for (const RenderableSprite** sprite=first_sprite; sprite != last_sprite;)
  {
    render::low_level::ITexture* texture = (*sprite)->primitive->texture;    

    size_t base_sprite_index = sprite++ - base_sprite;

    for (; sprite != last_sprite && (*sprite)->primitive->texture == texture; sprite++);

    size_t sprites_count = sprite - base_sprite - base_sprite_index;

    device.SSSetTexture (0, texture);
    device.DrawIndexed  (render::low_level::PrimitiveType_TriangleList, base_sprite_index * SPRITE_INDICES_COUNT,
                         sprites_count * SPRITE_INDICES_COUNT, 0);
  }
}

//рисование спрайтов с альфа-отсечением
void draw_alpha_clamped_sprites
 (IDevice&                 device,
  const RenderableSprite** base_sprite,
  const RenderableSprite** first_sprite,
  const RenderableSprite** last_sprite,
  IBuffer&                 constant_buffer)
{
  render::low_level::ITexture* current_texture         = device.SSGetTexture (0);
  DynamicProgramParameters     dynamic_program_parameters; //динамические параметры программы рендеринга  

  dynamic_program_parameters.alpha_reference = 0.0f;

  for (const RenderableSprite** sprite=first_sprite; sprite != last_sprite;)
  {
    render::low_level::ITexture* texture         = (*sprite)->primitive->texture;
    float                        alpha_reference = (*sprite)->primitive->alpha_reference;

    size_t base_sprite_index = sprite++ - base_sprite;

    for (; sprite != last_sprite && (*sprite)->primitive->texture == texture && (*sprite)->primitive->alpha_reference == alpha_reference; sprite++);

    size_t sprites_count = sprite - base_sprite - base_sprite_index;
    
    if (dynamic_program_parameters.alpha_reference != alpha_reference)
    {
      dynamic_program_parameters.alpha_reference = alpha_reference;

      constant_buffer.SetData (0, sizeof (dynamic_program_parameters), &dynamic_program_parameters);
    }

    if (current_texture != texture)
    {
      device.SSSetTexture (0, texture);

      current_texture = texture;
    }

    device.DrawIndexed (render::low_level::PrimitiveType_TriangleList, base_sprite_index * SPRITE_INDICES_COUNT,
      sprites_count * SPRITE_INDICES_COUNT, 0);
  }  
}

//рисование спрайтов с прозрачностью
void draw_blend_sprites
 (IDevice&                 device,
  const RenderableSprite** base_sprite,
  const RenderableSprite** first_sprite,
  const RenderableSprite** last_sprite,
  CommonResources&         resources)
{
  render::low_level::ITexture*     current_texture     = device.SSGetTexture (0);
  render::low_level::IBlendState*  current_blend_state = device.OSGetBlendState ();

  for (const RenderableSprite** sprite=first_sprite; sprite != last_sprite;)
  {
    render::low_level::ITexture*             texture    = (*sprite)->primitive->texture;
    render::mid_level::renderer2d::BlendMode blend_mode = (*sprite)->primitive->blend_mode;    
    
    size_t base_sprite_index = sprite++ - base_sprite;

    for (; sprite != last_sprite && (*sprite)->primitive->texture == texture && (*sprite)->primitive->blend_mode == blend_mode; sprite++);

    size_t sprites_count = sprite - base_sprite - base_sprite_index;

    if (current_texture != texture)
    {
      device.SSSetTexture (0, texture);

      current_texture = texture;
    }

    render::low_level::IBlendState* blend_state = resources.GetBlendState (blend_mode);

    if (current_blend_state != blend_state)
    {
      device.OSSetBlendState (blend_state);

      current_blend_state = blend_state;
    }

    device.DrawIndexed (render::low_level::PrimitiveType_TriangleList, base_sprite_index * SPRITE_INDICES_COUNT,
      sprites_count * SPRITE_INDICES_COUNT, 0);
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

    //отрисовка спрайтов без блендинга

  device->ISSetVertexBuffer      (0, not_blended_sprites.GetVertexBuffer ());
  device->ISSetIndexBuffer       (not_blended_sprites.GetIndexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (true));
  device->OSSetBlendState        (common_resources->GetBlendState (BlendMode_None));

  const RenderableSprite **first_sprite = not_blended_sprites.GetSprites (),
                         **last_sprite  = first_sprite + not_blended_sprites.Size (),
                         **first_solid_sprite;

    //поиск первого спрайта без альфа-отсечения
    
  device->SSSetProgram (common_resources->GetAlphaClampProgram ());

  for (first_solid_sprite=first_sprite; first_solid_sprite != last_sprite && (*first_solid_sprite)->primitive->blend_mode == BlendMode_AlphaClamp; ++first_solid_sprite);

    //отрисовка спрайтов с альфа-отсечением

  draw_alpha_clamped_sprites (*device, first_sprite, first_sprite, first_solid_sprite, *dynamic_constant_buffer);  

    //отрисовка спрайтов без альфа-отсечения

  device->SSSetProgram (common_resources->GetDefaultProgram ());

  draw_solid_sprites (*device, first_sprite, first_solid_sprite, last_sprite);

    //отрисовка спрайтов с блендингом    

  device->ISSetVertexBuffer      (0, blended_sprites.GetVertexBuffer ());
  device->ISSetIndexBuffer       (blended_sprites.GetIndexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (false));

  first_sprite = blended_sprites.GetSprites ();
  last_sprite  = first_sprite + blended_sprites.Size ();

  draw_blend_sprites (*device, first_sprite, first_sprite, last_sprite, *common_resources);
}
