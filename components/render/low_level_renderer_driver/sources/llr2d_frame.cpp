#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

/*
    Константы
*/

namespace
{

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 2048;                             //резервируемое количество примитивов
const size_t DEFAULT_VERTEX_BUFFER_SIZE   = PRIMITIVE_ARRAY_RESERVE_SIZE * 4; //размер вершинного буффера по умолчанию (количество вершин)

}

/*
    Конструктор / деструктор
*/

Frame::Frame (CommonResources* in_common_resources, render::low_level::IDevice* device)
  : common_resources (in_common_resources), current_not_blended_sprites_vertex_buffer_size (DEFAULT_VERTEX_BUFFER_SIZE), current_blended_sprites_vertex_buffer_size (DEFAULT_VERTEX_BUFFER_SIZE)
{
  using namespace render::low_level;

  memset (&vertex_buffer_desc, 0, sizeof (vertex_buffer_desc));

//  vertex_buffer_desc.usage_mode   = UsageMode_Default;
  vertex_buffer_desc.usage_mode   = UsageMode_Stream;
  vertex_buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  vertex_buffer_desc.access_flags = AccessFlag_ReadWrite;

  ReserveNotBlendedSpritesVertexBuffer (device, current_not_blended_sprites_vertex_buffer_size);
  ReserveBlendedSpritesVertexBuffer    (device, current_blended_sprites_vertex_buffer_size);
  primitives.reserve                   (PRIMITIVE_ARRAY_RESERVE_SIZE);
}

/*
    Матрица вида / матрица преобразования
*/

void Frame::SetView (const math::mat4f& tm)
{
  view_tm = tm;  
}

void Frame::SetProjection (const math::mat4f& tm)
{
  proj_tm = tm;
}

void Frame::GetView (math::mat4f& tm)
{
  tm = view_tm;
}

void Frame::GetProjection (math::mat4f& tm)
{
  tm = proj_tm;
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
   Резервирование места под вершинные буферы
*/

void Frame::ReserveNotBlendedSpritesVertexBuffer (render::low_level::IDevice* device, size_t sprites_count)
{
  vertex_buffer_desc.size = sizeof (RenderedSpriteVertex) * sprites_count * 6;

  not_blended_sprites_vertex_buffer = BufferPtr (device->CreateBuffer (vertex_buffer_desc), false);

  not_blended_sprites_vertex_data_buffer.reserve (sprites_count);
}

void Frame::ReserveBlendedSpritesVertexBuffer (render::low_level::IDevice* device, size_t sprites_count)
{
  vertex_buffer_desc.size = sizeof (RenderedSpriteVertex) * sprites_count * 6;

  blended_sprites_vertex_buffer = BufferPtr (device->CreateBuffer (vertex_buffer_desc), false);

  blended_sprites_vertex_data_buffer.reserve (sprites_count);
}

/*
   Формирование вершинного буффера
*/

void Frame::MakeVertexBuffer (SpriteVertexArray& vertex_data_array, BufferPtr& buffer)
{
  size_t base_offset = 0;
  
  RenderedSpriteVertex new_vertex;

  for (SpriteVertexArray::iterator iter = vertex_data_array.begin (), end = vertex_data_array.end (); iter < end; ++iter, base_offset += sizeof (RenderedSpriteVertex) * 6)
  {     
    new_vertex.color = (*iter)->color;

    new_vertex.position = (*iter)->vertices[0].position;
    new_vertex.texcoord = (*iter)->vertices[0].texcoord;
    buffer->SetData (base_offset,                                     sizeof (RenderedSpriteVertex), &new_vertex);

    new_vertex.position = (*iter)->vertices[1].position;
    new_vertex.texcoord = (*iter)->vertices[1].texcoord;
    buffer->SetData (base_offset + sizeof (RenderedSpriteVertex),     sizeof (RenderedSpriteVertex), &new_vertex);
    
    new_vertex.position = (*iter)->vertices[2].position;
    new_vertex.texcoord = (*iter)->vertices[2].texcoord;
    buffer->SetData (base_offset + sizeof (RenderedSpriteVertex) * 2, sizeof (RenderedSpriteVertex), &new_vertex);
    
    new_vertex.position = (*iter)->vertices[3].position;
    new_vertex.texcoord = (*iter)->vertices[3].texcoord;
    buffer->SetData (base_offset + sizeof (RenderedSpriteVertex) * 3, sizeof (RenderedSpriteVertex), &new_vertex);
    
    new_vertex.position = (*iter)->vertices[2].position;
    new_vertex.texcoord = (*iter)->vertices[2].texcoord;
    buffer->SetData (base_offset + sizeof (RenderedSpriteVertex) * 4, sizeof (RenderedSpriteVertex), &new_vertex);
    
    new_vertex.position = (*iter)->vertices[1].position;
    new_vertex.texcoord = (*iter)->vertices[1].texcoord;
    buffer->SetData (base_offset + sizeof (RenderedSpriteVertex) * 5, sizeof (RenderedSpriteVertex), &new_vertex);
  }
}

/*
    Реализация визуализации
*/

namespace
{

bool not_blended_sprite_sort_predicate (SpriteVertexData* data1, SpriteVertexData* data2)
{
  return data1->texture < data2->texture;  //возможно добавить сортировку по глубине
}

bool blended_sprite_sort_predicate (SpriteVertexData* data1, SpriteVertexData* data2)
{
  return data1->vertices[0].position.z > data2->vertices[0].position.z; //возможно добавить сортировку по блендингу и текстурам
}

}

void Frame::DrawCore (render::low_level::IDevice* device)
{
  common_resources->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, view_matrix), sizeof (view_tm), &view_tm);
  common_resources->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, projection_matrix), sizeof (proj_tm), &proj_tm);

  //Подготовка вершинных буфферов

  not_blended_sprites_vertex_data_buffer.clear ();
  blended_sprites_vertex_data_buffer.clear ();

  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    if ((*iter)->GetBlendMode () == BlendMode_None)
      for (size_t i = 0; i < (*iter)->GetSpritesCount (); i++)
        not_blended_sprites_vertex_data_buffer.push_back (&((*iter)->GetSpriteVertexBuffer ()[i]));
    else
      for (size_t i = 0; i < (*iter)->GetSpritesCount (); i++)
        blended_sprites_vertex_data_buffer.push_back (&((*iter)->GetSpriteVertexBuffer ()[i]));
  }

  stl::sort (not_blended_sprites_vertex_data_buffer.begin (), not_blended_sprites_vertex_data_buffer.end (), not_blended_sprite_sort_predicate);
  stl::sort (blended_sprites_vertex_data_buffer.begin (), blended_sprites_vertex_data_buffer.end (), blended_sprite_sort_predicate);

  if (not_blended_sprites_vertex_data_buffer.size () > current_not_blended_sprites_vertex_buffer_size)
  {
    current_not_blended_sprites_vertex_buffer_size = not_blended_sprites_vertex_data_buffer.size () + DEFAULT_VERTEX_BUFFER_SIZE;
    ReserveNotBlendedSpritesVertexBuffer (device, current_not_blended_sprites_vertex_buffer_size);
  }

  if (blended_sprites_vertex_data_buffer.size () > current_blended_sprites_vertex_buffer_size)
  {
    current_blended_sprites_vertex_buffer_size = blended_sprites_vertex_data_buffer.size () + DEFAULT_VERTEX_BUFFER_SIZE;
    ReserveBlendedSpritesVertexBuffer (device, current_blended_sprites_vertex_buffer_size);
  }

  MakeVertexBuffer (not_blended_sprites_vertex_data_buffer, not_blended_sprites_vertex_buffer);
  MakeVertexBuffer (blended_sprites_vertex_data_buffer,     blended_sprites_vertex_buffer);

  //Отрисовка спрайтов без блендинга

  device->ISSetVertexBuffer      (0, not_blended_sprites_vertex_buffer.get ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (true));
  device->OSSetBlendState        (common_resources->GetBlendState (BlendMode_None));

  for (size_t i = 0; i < not_blended_sprites_vertex_data_buffer.size ();)
  {
    size_t j = i + 1;
    render::low_level::ITexture* texture = not_blended_sprites_vertex_data_buffer[i]->texture;

    for (; (j < not_blended_sprites_vertex_data_buffer.size ()) && (not_blended_sprites_vertex_data_buffer[j]->texture == texture); j++);

    device->SSSetTexture (0, texture);
    
    device->Draw (render::low_level::PrimitiveType_TriangleList, i * 6, (j - i) * 6);

    i = j;
  }

  //Отрисовка спрайтов с блендингом

  device->ISSetVertexBuffer      (0, blended_sprites_vertex_buffer.get ());
  device->OSSetDepthStencilState (common_resources->GetDepthStencilState (false));
  
  render::low_level::ITexture*     current_texture     = device->SSGetTexture (0);
  render::low_level::IBlendState*  current_blend_state = device->OSGetBlendState ();
  
  for (size_t i = 0; i < blended_sprites_vertex_data_buffer.size (); i++)
  {
    if (current_texture != blended_sprites_vertex_data_buffer[i]->texture)
    {
      device->SSSetTexture (0, blended_sprites_vertex_data_buffer[i]->texture);
      
      current_texture = blended_sprites_vertex_data_buffer[i]->texture;
    }
    
    render::low_level::IBlendState* blend_state = common_resources->GetBlendState (blended_sprites_vertex_data_buffer[i]->blend_mode);

    if (current_blend_state != blend_state)
    {
      device->OSSetBlendState (blend_state);
      
      current_blend_state = blend_state;
    }

    device->Draw (render::low_level::PrimitiveType_TriangleList, i * 6, 6);
  }
}
