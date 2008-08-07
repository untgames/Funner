#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;
using namespace render::low_level;

/*
    Константы
*/

namespace
{

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 2048; //резервируемое количество примитивов

}

/*
    Конструктор / деструктор
*/

Frame::Frame (CommonResources* in_common_resources, render::low_level::IDevice* device)
  : common_resources (in_common_resources)
{
  program_parameters.alpha_reference = 0.0f;

  primitives.reserve (PRIMITIVE_ARRAY_RESERVE_SIZE);
}

/*
    Матрица вида / матрица преобразования
*/

void Frame::SetViewPoint (const math::vec3f& point)
{
  program_parameters.view_matrix = math::translate (-point);
}

void Frame::SetProjection (const math::mat4f& tm)
{
  program_parameters.projection_matrix = tm;
}

void Frame::GetViewPoint (math::vec3f& point)
{
  point = -program_parameters.view_matrix.column (3);
}

void Frame::GetProjection (math::mat4f& tm)
{
  tm = program_parameters.projection_matrix;
}

/*
    Установка параметра для работы альфа-теста
*/

void Frame::SetAlphaReference (float ref)
{
  program_parameters.alpha_reference = ref;
}

float Frame::GetAlphaReference ()
{
  return program_parameters.alpha_reference;
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
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::renderer2d::Frame::AddPrimitive";

  if (!in_primitive)
    throw xtl::make_null_argument_exception (METHOD_NAME, "primitive");
    
  Primitive* casted_primitive = dynamic_cast<Primitive*> (in_primitive);
  
  if (!casted_primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "primitive", typeid (in_primitive).name (),
      "Primitive type incompatible with render::mid_level::low_level_driver::renderer2d::Primitive");

  if (primitives.size () == primitives.capacity ())
    primitives.reserve (primitives.capacity () + PRIMITIVE_ARRAY_RESERVE_SIZE);

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

bool not_blended_sprite_sort_predicate (const SpriteVertexData* data1, const SpriteVertexData* data2)
{
  if (data1->blend_mode == data2->blend_mode)
    return data1->texture < data2->texture;

  return data1->blend_mode > data2->blend_mode;
}

bool blended_sprite_sort_predicate (const SpriteVertexData* data1, const SpriteVertexData* data2)
{
  return data1->vertices[0].position.z > data2->vertices[0].position.z; //возможно добавить сортировку по блендингу и текстурам
}

}

void Frame::UpdateVertexBuffers (render::low_level::IDevice& device)
{
    //очистка буферов спрайтов

  not_blended_sprites.Clear ();
  blended_sprites.Clear ();
  
    //заполнение буферов спрайтов  

  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive&    primitive = **iter;
    SpriteBuffer* buffer;
    
    switch (primitive.GetBlendMode ())
    {
      case BlendMode_None:
      case BlendMode_AlphaClamp:
        buffer = &not_blended_sprites;
        break;
      default:
        buffer = &blended_sprites;
        break;
    }

    buffer->Add (primitive.GetSpritesCount (), primitive.GetSpriteVertexBuffer ());
  }

    //сортировка геометрии для ускорения вывода

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
void draw_sprites (IDevice& device, const SpriteVertexData** first_sprite, const SpriteVertexData** last_sprite)
{
  for (const SpriteVertexData** sprite=first_sprite; sprite != last_sprite;)
  {
    size_t base_sprite_index = sprite - first_sprite;

    render::low_level::ITexture* texture = (*sprite)->texture;

    for (; sprite != last_sprite && (*sprite)->texture == texture; sprite++);

    size_t sprites_count = sprite - first_sprite - base_sprite_index + 1;

    device.SSSetTexture (0, texture);    
    device.Draw         (render::low_level::PrimitiveType_TriangleList, base_sprite_index * 6, sprites_count * 6);
  }  
}

//рисование спрайтов с прозрачностью
void draw_sprites (IDevice& device, const SpriteVertexData** first_sprite, const SpriteVertexData** last_sprite, CommonResources& resources)
{
  render::low_level::ITexture*     current_texture     = device.SSGetTexture (0);
  render::low_level::IBlendState*  current_blend_state = device.OSGetBlendState ();

  for (const SpriteVertexData** sprite=first_sprite; sprite != last_sprite;)
  {
    size_t base_sprite_index = sprite - first_sprite;

    render::low_level::ITexture*             texture    = (*sprite)->texture;
    render::mid_level::renderer2d::BlendMode blend_mode = (*sprite)->blend_mode;

    for (; sprite != last_sprite && (*sprite)->texture == texture && (*sprite)->blend_mode == blend_mode; sprite++);

    size_t sprites_count = sprite - first_sprite - base_sprite_index + 1;

    device.SSSetTexture (0, texture);

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

    device.Draw (render::low_level::PrimitiveType_TriangleList, base_sprite_index * 6, sprites_count * 6);
  }
}

}

void Frame::DrawCore (IDevice* device)
{
    //установка области вывода

  BasicFrame::BindViewport (device);
  
    //обновление парметров шейдинга    

  IBuffer* constant_buffer = common_resources->GetConstantBuffer ();

  constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);

    //обновление вершинных буферов
    
  UpdateVertexBuffers (*device);

    //установка общих ресурсов

  device->ISSetInputLayout             (common_resources->GetInputLayout ());
  device->SSSetConstantBuffer          (0, constant_buffer);
  device->SSSetProgramParametersLayout (common_resources->GetProgramParametersLayout ());
  device->SSSetSampler                 (0, common_resources->GetSamplerState ());

    //отрисовка спрайтов без блендинга

  device->ISSetVertexBuffer      (0, not_blended_sprites.GetVertexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (true));
  device->OSSetBlendState        (common_resources->GetBlendState (BlendMode_None));

  const SpriteVertexData **first_sprite = not_blended_sprites.GetSprites (),
                         **last_sprite  = first_sprite + not_blended_sprites.GetSpritesCount (),
                         **first_solid_sprite;

    //поиск первого спрайта без альфа-отсечения
    
  device->SSSetProgram (common_resources->GetAlphaClampProgram ());

  for (first_solid_sprite=first_sprite; first_solid_sprite != last_sprite && (*first_solid_sprite)->blend_mode == BlendMode_AlphaClamp; ++first_solid_sprite);

    //отрисовка спрайтов с альфа-отсечением

  draw_sprites (*device, first_sprite, first_solid_sprite);

    //отрисовка спрайтов без альфа-отсечения

  device->SSSetProgram (common_resources->GetDefaultProgram ());

  draw_sprites (*device, first_solid_sprite, last_sprite);

    //отрисовка спрайтов с блендингом

  device->ISSetVertexBuffer      (0, blended_sprites.GetVertexBuffer ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (false));

  first_sprite = blended_sprites.GetSprites ();
  last_sprite  = first_sprite + blended_sprites.GetSpritesCount ();

  draw_sprites (*device, first_sprite, last_sprite, *common_resources);
}
