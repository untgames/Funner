#include "scene_graph.h"

using namespace scene_graph;

namespace
{

const char* SCENE_PAGE_CURL_LIBRARY                   = "Scene.PageCurl";
const char* SCENE_STATIC_PAGE_CURL_CORNER_LIBRARY     = "Scene.PageCurlCorner";
const char* SCENE_STATIC_PAGE_CURL_PAGE_TYPE_LIBRARY  = "Scene.PageCurlPageType";
const char* SCENE_STATIC_PAGE_CURL_MODE_LIBRARY       = "Scene.PageCurlMode";

}

template class engine::decl_sg_cast<PageCurl, VisualModel>;
template class engine::decl_sg_cast<PageCurl, Entity>;
template class engine::decl_sg_cast<PageCurl, Node>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    —оздание листани€
*/

PageCurl::Pointer create_page_curl ()
{
  return PageCurl::Create ();
}

/*
   –егистраци€ библиотеки работы с листанием
*/

void bind_static_page_curl_library (Environment& environment)
{
  InvokerRegistry page_curl_corner_lib    = environment.CreateLibrary (SCENE_STATIC_PAGE_CURL_CORNER_LIBRARY);
  InvokerRegistry page_curl_page_type_lib = environment.CreateLibrary (SCENE_STATIC_PAGE_CURL_PAGE_TYPE_LIBRARY);
  InvokerRegistry page_curl_mode_lib      = environment.CreateLibrary (SCENE_STATIC_PAGE_CURL_MODE_LIBRARY);

  page_curl_corner_lib.Register    ("get_LeftTop",                  make_const (PageCurlCorner_LeftTop));
  page_curl_corner_lib.Register    ("get_LeftBottom",               make_const (PageCurlCorner_LeftBottom));
  page_curl_corner_lib.Register    ("get_RightTop",                 make_const (PageCurlCorner_RightTop));
  page_curl_corner_lib.Register    ("get_RightBottom",              make_const (PageCurlCorner_RightBottom));
  page_curl_page_type_lib.Register ("get_FrontLeft",                make_const (PageCurlPageType_FrontLeft));
  page_curl_page_type_lib.Register ("get_FrontRight",               make_const (PageCurlPageType_FrontRight));
  page_curl_page_type_lib.Register ("get_BackLeft",                 make_const (PageCurlPageType_BackLeft));
  page_curl_page_type_lib.Register ("get_BackRight",                make_const (PageCurlPageType_BackRight));
  page_curl_page_type_lib.Register ("get_Front",                    make_const (PageCurlPageType_Front));
  page_curl_page_type_lib.Register ("get_Back",                     make_const (PageCurlPageType_Back));
  page_curl_mode_lib.Register      ("get_SinglePage",               make_const (PageCurlMode_SinglePage));
  page_curl_mode_lib.Register      ("get_DoublePageSingleMaterial", make_const (PageCurlMode_DoublePageSingleMaterial));
  page_curl_mode_lib.Register      ("get_DoublePageDoubleMaterial", make_const (PageCurlMode_DoublePageDoubleMaterial));

  environment.RegisterType<PageCurlCorner>   (SCENE_STATIC_PAGE_CURL_CORNER_LIBRARY);
  environment.RegisterType<PageCurlPageType> (SCENE_STATIC_PAGE_CURL_PAGE_TYPE_LIBRARY);
  environment.RegisterType<PageCurlMode>     (SCENE_STATIC_PAGE_CURL_MODE_LIBRARY);
}

math::vec2f get_grid_size (PageCurl& page_curl)
{
  return math::vec2f ((float)page_curl.GridSize ().x, (float)page_curl.GridSize ().y);
}

void bind_page_curl_library (Environment& environment)
{
    //регистраци€ статических переменных

  bind_static_page_curl_library (environment);

  InvokerRegistry lib = environment.CreateLibrary (SCENE_PAGE_CURL_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистраци€ функций создани€

  lib.Register ("Create", make_invoker (&create_page_curl));

    //регистраци€ операций

  lib.Register ("set_BindingMismatchWeight",         make_invoker (&PageCurl::SetBindingMismatchWeight));
  lib.Register ("set_CurlPointPosition",             make_invoker (xtl::implicit_cast<void (PageCurl::*) (const math::vec2f&)> (&PageCurl::SetCurlPointPosition)));
  lib.Register ("set_CornerShadowOffset",            make_invoker (&PageCurl::SetCornerShadowOffset));
  lib.Register ("set_CurlPoint",                     make_invoker (xtl::implicit_cast<void (PageCurl::*) (PageCurlCorner)> (&PageCurl::SetCurlPoint)));
  lib.Register ("set_CurlRadius",                    make_invoker (&PageCurl::SetCurlRadius));
  lib.Register ("set_FindBestCurlSteps",             make_invoker (&PageCurl::SetFindBestCurlSteps));
  lib.Register ("set_MinimumCurlRadius",             make_invoker (&PageCurl::SetMinimumCurlRadius));
  lib.Register ("set_Mode",                          make_invoker (&PageCurl::SetMode));
  lib.Register ("set_PageColor",                     make_invoker (xtl::implicit_cast<void (PageCurl::*) (const math::vec4f&)> (&PageCurl::SetPageColor)));
  lib.Register ("set_RigidPage",                     make_invoker (&PageCurl::SetRigidPage));
  lib.Register ("set_RigidPagePerspectiveFactor",    make_invoker (&PageCurl::SetRigidPagePerspectiveFactor));
  lib.Register ("set_ShadowLogBase",                 make_invoker (&PageCurl::SetShadowLogBase));
  lib.Register ("set_ShadowMinLogValue",             make_invoker (&PageCurl::SetShadowMinLogValue));
  lib.Register ("set_ShadowWidth",                   make_invoker (&PageCurl::SetShadowWidth));
  lib.Register ("set_Size",                          make_invoker (xtl::implicit_cast<void (PageCurl::*) (const math::vec2f&)> (&PageCurl::SetSize)));
  lib.Register ("get_BindingMismatchWeight",         make_invoker (&PageCurl::BindingMismatchWeight));
  lib.Register ("get_CurlPointPosition",             make_invoker (&PageCurl::CurlPointPosition));
  lib.Register ("get_CornerShadowOffset",            make_invoker (&PageCurl::CornerShadowOffset));
  lib.Register ("get_CurlPoint",                     make_invoker (&PageCurl::CurlPoint));
  lib.Register ("get_CurlRadius",                    make_invoker (&PageCurl::CurlRadius));
  lib.Register ("get_FindBestCurlSteps",             make_invoker (&PageCurl::FindBestCurlSteps));
  lib.Register ("get_GridSize",                      make_invoker (&get_grid_size));
  lib.Register ("get_MinimumCurlRadius",             make_invoker (&PageCurl::MinimumCurlRadius));
  lib.Register ("get_Mode",                          make_invoker (&PageCurl::Mode));
  lib.Register ("get_PageColor",                     make_invoker (&PageCurl::PageColor));
  lib.Register ("get_IsRigidPage",                   make_invoker (&PageCurl::IsRigidPage));
  lib.Register ("get_RigidPagePerspectiveFactor",    make_invoker (&PageCurl::RigidPagePerspectiveFactor));
  lib.Register ("get_ShadowLogBase",                 make_invoker (&PageCurl::ShadowLogBase));
  lib.Register ("get_ShadowMinLogValue",             make_invoker (&PageCurl::ShadowMinLogValue));
  lib.Register ("get_ShadowWidth",                   make_invoker (&PageCurl::ShadowWidth));
  lib.Register ("get_Size",                          make_invoker (&PageCurl::Size));

  lib.Register ("SetCurlPointPosition", make_invoker (xtl::implicit_cast<void (PageCurl::*) (float, float)> (&PageCurl::SetCurlPointPosition)));
  lib.Register ("SetCurlPoint", make_invoker (
      make_invoker (xtl::implicit_cast<void (PageCurl::*) (float, float)>       (&PageCurl::SetCurlPoint)),
      make_invoker (xtl::implicit_cast<void (PageCurl::*) (const math::vec2f&)> (&PageCurl::SetCurlPoint))
  ));
  lib.Register ("SetPageMaterial",      make_invoker (&PageCurl::SetPageMaterial));
  lib.Register ("PageMaterial",         make_invoker (&PageCurl::PageMaterial));
  lib.Register ("SetPageColor",         make_invoker (xtl::implicit_cast<void (PageCurl::*) (float, float, float, float)> (&PageCurl::SetPageColor)));
  lib.Register ("SetSize",              make_invoker (xtl::implicit_cast<void (PageCurl::*) (float, float)> (&PageCurl::SetSize)));
  lib.Register ("SetGridSize",          make_invoker (xtl::implicit_cast<void (PageCurl::*) (size_t, size_t)> (&PageCurl::SetGridSize)));

    //регистраци€ типов данных

  environment.RegisterType<PageCurl> (SCENE_PAGE_CURL_LIBRARY);
}

}

}

