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

Frame::Frame ()
{
  primitives.reserve (PRIMITIVE_ARRAY_RESERVE_SIZE);
}

Frame::~Frame ()
{
  try
  {
  }
  catch (...)
  {
    //подавление исключений
  }
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

namespace
{

#pragma pack (1)

struct Vertex
{
  math::vec3f position;
  math::vec2f texcoord;      //color
};

}

void Frame::DrawCore (render::low_level::IDevice* device)
{
  using namespace render::low_level;

  if (!constant_buffer)
  {
    BufferDesc constant_buffer_desc = {sizeof (ProgramParameters), UsageMode_Default, BindFlag_ConstantBuffer, AccessFlag_ReadWrite};

    constant_buffer = device->CreateBuffer (constant_buffer_desc);

    device->SSSetConstantBuffer (0, constant_buffer.get ());
  }

  view_tm = math::translatef (0, 0, -15.5); //??????????

  constant_buffer->SetData (offsetof (ProgramParameters, view_matrix), sizeof (view_tm), &view_tm);
  constant_buffer->SetData (offsetof (ProgramParameters, projection_matrix), sizeof (proj_tm), &proj_tm);

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
    math::mat4f object_tm;
    
    primitive.GetTransform (object_tm);

    constant_buffer->SetData (offsetof (ProgramParameters, object_matrix), sizeof (object_tm), &object_tm);

    size_t sprites_count=primitive.GetSpritesCount ();

    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite s;
      
      primitive.GetSprite (i, s);    

      BufferDesc vertex_buffer_desc = {sizeof (Vertex) * 4, UsageMode_Default, BindFlag_VertexBuffer, AccessFlag_ReadWrite};

      xtl::com_ptr<IBuffer> vertex_buffer (device->CreateBuffer (vertex_buffer_desc), false);

      Vertex vertices[4] = { { math::vec3f (s.position.x - s.size.x,       s.position.y - s.size.y, s.position.z),
                               math::vec2f (0.f, 0.f) },
//                               math::vec2f (s.tex_offset.x,                s.tex_offset.y + s.tex_size.y) },
                             { math::vec3f (s.position.x + s.size.x,       s.position.y - s.size.y, s.position.z), 
                               math::vec2f (1.f, 0.f) },
//                               math::vec2f (s.tex_offset.x + s.tex_size.x, s.tex_offset.y + s.tex_size.y) },
                             { math::vec3f (s.position.x - s.size.x,       s.position.y + s.size.y, s.position.z), 
                               math::vec2f (0.f, 1.f) },
//                               math::vec2f (s.tex_offset.x,                s.tex_offset.y) },
                             { math::vec3f (s.position.x + s.size.x,       s.position.y + s.size.y, s.position.z),
                               math::vec2f (1.f, 1.f) } };
//                               math::vec2f (s.tex_offset.x + s.tex_size.x, s.tex_offset.y) } };

      vertex_buffer->SetData (0, sizeof (vertices), vertices);

      static VertexAttribute attributes [] = {
        {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (Vertex, position), sizeof (Vertex)},
        {VertexAttributeSemantic_TexCoord0, InputDataFormat_Vector2, InputDataType_Float, 0, offsetof (Vertex, texcoord), sizeof (Vertex)},
      };
      
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
