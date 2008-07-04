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

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 4096; //резервируемое количество примитивов

}

/*
     онструктор / деструктор
*/

Frame::Frame (Renderer* in_renderer, render::low_level::IDevice* device)
  : renderer (in_renderer)
{
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

  renderer->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, view_matrix), sizeof (view_tm), &view_tm);
  renderer->GetConstantBuffer ()->SetData (offsetof (ProgramParameters, projection_matrix), sizeof (proj_tm), &proj_tm);

  /*
    в нормальной реализации в этом методе должна быть реализована сортировка спрайтов по удалению от наблюдател€
  */

  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive& primitive = **iter;
    
    device->SSSetTexture (0, primitive.GetLowLevelTexture ());
    
/*    switch (primitive.GetBlendMode ())
    {
      case BlendMode_None:
        log.Printf ("  blend=none");
        break;
      case BlendMode_Translucent:
        log.Printf ("  blend=translucent");
        break;
      case BlendMode_Mask:
        log.Printf ("  blend=mask");
        break;
      case BlendMode_Additive:
        log.Printf ("  blend=additive");
        break;
      default:
        break;
    }
*/    

    device->OSSetBlendState (renderer->GetBlendState (primitive.GetBlendMode ()));

    size_t sprites_count=primitive.GetSpritesCount ();

    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite s;
      
      primitive.GetSprite (i, s);    

      BufferDesc vertex_buffer_desc;

      memset (&vertex_buffer_desc, 0, sizeof (vertex_buffer_desc));

      vertex_buffer_desc.size         = sizeof (SpriteVertex) * 4;
      vertex_buffer_desc.usage_mode   = UsageMode_Default;
      vertex_buffer_desc.bind_flags   = BindFlag_VertexBuffer;
      vertex_buffer_desc.access_flags = AccessFlag_ReadWrite;

      xtl::com_ptr<IBuffer> vertex_buffer (device->CreateBuffer (vertex_buffer_desc), false);

      vertex_buffer->SetData (0, sizeof (SpriteVertex) * 4 , primitive.GetVertexBuffer () + i * 4);

      static VertexAttribute attributes [3];

      memset (attributes, 0, sizeof (attributes));      

      attributes[0].semantic = VertexAttributeSemantic_Position;
      attributes[0].format   = InputDataFormat_Vector3;
      attributes[0].type     = InputDataType_Float;
      attributes[0].slot     = 0;
      attributes[0].offset   = offsetof (SpriteVertex, position);
      attributes[0].stride   = sizeof (SpriteVertex);

      attributes[1].semantic = VertexAttributeSemantic_TexCoord0;
      attributes[1].format   = InputDataFormat_Vector2;
      attributes[1].type     = InputDataType_Float;
      attributes[1].slot     = 0;
      attributes[1].offset   = offsetof (SpriteVertex, texcoord);
      attributes[1].stride   = sizeof (SpriteVertex);

      attributes[2].semantic = VertexAttributeSemantic_Color;
      attributes[2].format   = InputDataFormat_Vector4;
      attributes[2].type     = InputDataType_Float;
      attributes[2].slot     = 0;
      attributes[2].offset   = offsetof (SpriteVertex, color);
      attributes[2].stride   = sizeof (SpriteVertex);

      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UInt;
      layout_desc.index_buffer_offset     = 0;            

      xtl::com_ptr<IInputLayout> input_layout (device->CreateInputLayout (layout_desc), false);      

      device->OSSetDepthStencilState (0);
      device->ISSetInputLayout       (input_layout.get ());
      device->ISSetVertexBuffer      (0, vertex_buffer.get ());

      device->Draw (PrimitiveType_TriangleStrip, 0, 4);
    }
  }
}
