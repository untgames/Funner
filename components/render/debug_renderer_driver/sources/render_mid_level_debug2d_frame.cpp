#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

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
  
  log.Printf ("Create frame2d (id=%u)", Id ());
}

Frame::~Frame ()
{
  try
  {
    log.Printf ("Destroy frame2d (id=%u)", Id ());
  }
  catch (...)
  {
    //подавление исключений
  }
}

/*
    ћатрица вида / матрица преобразовани€
*/

void Frame::SetViewPoint (const math::vec3f& point)
{
  view_point = point;
}

void Frame::SetProjection (const math::mat4f& tm)
{
  proj_tm = tm;
}

void Frame::GetViewPoint (math::vec3f& point)
{
  point = view_point;
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
  static const char* METHOD_NAME = "render::mid_level::debug::renderer2d::Frame::AddPrimitive";

  if (!in_primitive)
    throw xtl::make_null_argument_exception (METHOD_NAME, "primitive");
    
  Primitive* casted_primitive = dynamic_cast<Primitive*> (in_primitive);
  
  if (!casted_primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "primitive", typeid (in_primitive).name (),
      "Primitive type incompatible with render::mid_level::debug::renderer2d::Primitive");

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

void Frame::DrawCore ()
{
    //установка области вывода

  BasicFrame::BindViewport ();

  /*
    в нормальной реализации в этом методе должна быть реализована сортировка спрайтов по удалению от наблюдател€
  */

  log.Printf ("Projection matrix=[[%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f]]",
    proj_tm [0][0], proj_tm [0][1], proj_tm [0][2], proj_tm [0][3],
    proj_tm [1][0], proj_tm [1][1], proj_tm [1][2], proj_tm [1][3],
    proj_tm [2][0], proj_tm [2][1], proj_tm [2][2], proj_tm [2][3],
    proj_tm [3][0], proj_tm [3][1], proj_tm [3][2], proj_tm [3][3]);

  log.Printf ("View point=[%.2f %.2f %.2f]", view_point.x, view_point.y, view_point.z);
  
  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive& primitive = **iter;
   
    log.Printf ("Draw primitive (id=%u)", primitive.Id ());
    
    if (primitive.GetTexture ()) log.Printf ("  texture_id=%u", primitive.GetTextureId ());
    else                         log.Printf ("  texture_id=none");
    
    switch (primitive.GetBlendMode ())
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
      case BlendMode_AlphaClamp:
        log.Printf ("  blend=alpha_clamp");
        log.Printf ("  alpha_reference=%.2f", primitive.GetAlphaReference ());
        break;
      default:
        break;
    }
    
    log.Printf ("  scissor_state=%s", primitive.GetScissorState () ? "true" : "false");
    
    if (primitive.GetScissorState ())
    {
      Viewport viewport;

      primitive.GetScissor (viewport);

      log.Printf ("  scissor=(%d, %d)-(%d, %d)", viewport.x, viewport.y, viewport.x+viewport.width, viewport.y+viewport.height);
    }

    math::mat4f object_tm;

    primitive.GetTransform (object_tm);

    log.Printf ("  transform=[[%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f]]",
      object_tm [0][0], object_tm [0][1], object_tm [0][2], object_tm [0][3],
      object_tm [1][0], object_tm [1][1], object_tm [1][2], object_tm [1][3],
      object_tm [2][0], object_tm [2][1], object_tm [2][2], object_tm [2][3],
      object_tm [3][0], object_tm [3][1], object_tm [3][2], object_tm [3][3]);
      
    size_t sprites_count = primitive.GetSpritesCount ();

    log.Printf ("  sprites (count=%u):", sprites_count);
    
    const SpriteVertex* verts = primitive.Vertices ();

    for (size_t i=0; i<sprites_count*4; i++)
    {
      const SpriteVertex& v = verts [i];

      log.Printf ("    vertex #%u: position=[%.2f %.2f %.2f] normal=[%.2f %.2f %.2f] color=[%.2f %.2f %.2f %.2f] tex_coord=[%.2f %.2f]",
        i, v.position.x, v.position.y, v.position.z, v.normal.x, v.normal.y, v.normal.z, v.color.x, v.color.y, v.color.z, v.color.w, 
        v.tex_coord.x, v.tex_coord.y);
    }
  }
}
