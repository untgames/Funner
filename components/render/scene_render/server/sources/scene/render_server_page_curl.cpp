#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ������������ ������
*/

struct PageCurl::Impl
{
  PageCurlParams   params;               //���������
  stl::string      front_left_material;  //�������� ��������
  stl::string      front_right_material; //�������� ��������
  stl::string      back_left_material;   //�������� ��������
  stl::string      back_right_material;  //�������� ��������
  manager::Entity& entity;               //��������

  /// �����������
    Impl (manager::Entity& in_entity) : entity (in_entity) {}
};

/*
    ������������ / ���������� / ������������
*/

PageCurl::PageCurl (RenderManager& render_manager)
  : VisualModel (render_manager)
  , impl (new Impl (Entity ()))
{
}

PageCurl::~PageCurl ()
{
}

/*
    ���������
*/

void PageCurl::SetParams (const PageCurlParams& params, const char* front_left_material, const char* front_right_material,
                          const char* back_left_material, const char* back_right_material)
{
  static const char* METHOD_NAME = "render::scene::server::PageCurl::SetParams";

  if (!front_left_material)
    throw xtl::make_null_argument_exception (METHOD_NAME, "front_left_material");

  if (!front_right_material)
    throw xtl::make_null_argument_exception (METHOD_NAME, "front_right_material");

  if (!back_left_material)
    throw xtl::make_null_argument_exception (METHOD_NAME, "back_left_material");

  if (!back_right_material)
    throw xtl::make_null_argument_exception (METHOD_NAME, "back_right_material");

  impl->params               = params;
  impl->front_left_material  = front_left_material;
  impl->front_right_material = front_right_material;
  impl->back_left_material   = back_left_material;
  impl->back_right_material  = back_right_material;
}

const PageCurlParams& PageCurl::Params () const
{
  return impl->params;
}

/*
   ���������
*/

void PageCurl::DrawCore (RenderingContext& context, void* user_data)
{


  VisualModel::DrawCore (context, user_data);
}