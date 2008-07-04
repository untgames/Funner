#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

/*
     онстанты
*/

namespace
{

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 4096;  //резервируемое количество примитивов
const size_t DEFAULT_VERTEX_BUFFER_SIZE   = 10000; //размер вершинного буффера по умолчанию (количество вершин)

}

/*
     онструктор / деструктор
*/

Frame::Frame (CommonResources* in_common_resources, render::low_level::IDevice* device)
  : common_resources (in_common_resources), current_vertex_buffer_size (DEFAULT_VERTEX_BUFFER_SIZE)
{
  using namespace render::low_level;

  BufferDesc vertex_buffer_desc;

  memset (&vertex_buffer_desc, 0, sizeof (vertex_buffer_desc));

  vertex_buffer_desc.size         = sizeof (SpriteVertex) * current_vertex_buffer_size;
  vertex_buffer_desc.usage_mode   = UsageMode_Default;
  vertex_buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  vertex_buffer_desc.access_flags = AccessFlag_ReadWrite;

  vertex_buffer = device->CreateBuffer (vertex_buffer_desc);

  primitives.reserve (PRIMITIVE_ARRAY_RESERVE_SIZE);
}

/*
    ћатрица вида / матрица преобразовани€
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
    ѕримитивы
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

  primitives.push_back (casted_primitive);
}

//очистка кадра
void Frame::Clear ()
{
  primitives.clear ();
}

/*
    –еализаци€ визуализации
*/

void Frame::DrawCore (render::low_level::IDevice* device)
{
  using namespace render::low_level;

  common_resources->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, view_matrix), sizeof (view_tm), &view_tm);
  common_resources->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, projection_matrix), sizeof (proj_tm), &proj_tm);

  /*
    в нормальной реализации в этом методе должна быть реализована сортировка спрайтов по удалению от наблюдател€
  */

  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive& primitive = **iter;
    
    device->SSSetTexture (0, primitive.GetLowLevelTexture ());
    
    device->OSSetBlendState (common_resources->GetBlendState (primitive.GetBlendMode ()));

    size_t sprites_count=primitive.GetSpritesCount ();

    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite s;
      
      primitive.GetSprite (i, s);    

      vertex_buffer->SetData (0, sizeof (SpriteVertex) * 4 , primitive.GetVertexBuffer () + i * 4);

      device->OSSetDepthStencilState (0);
      device->ISSetVertexBuffer      (0, vertex_buffer.get ());

      device->Draw (PrimitiveType_TriangleStrip, 0, 4);
    }
  }
}
