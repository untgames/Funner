#include "shared.h"

const char* get_mode_name (PageCurlMode mode)
{
  switch (mode)
  {
    case PageCurlMode_SinglePage:
      return "SinglePage";
    case PageCurlMode_DoublePageSingleMaterial:
      return "DoublePageSingleMaterial";
    case PageCurlMode_DoublePageDoubleMaterial:
      return "DoublePageDoubleMaterial";
    default:
      return "Unknown";
  }
}

const char* get_corner_name (PageCurlCorner corner)
{
  switch (corner)
  {
    case PageCurlCorner_LeftTop:
      return "LeftTop";
    case PageCurlCorner_LeftBottom:
      return "LeftBottom";
    case PageCurlCorner_RightTop:
      return "RightTop";
    case PageCurlCorner_RightBottom:
      return "RightBottom";
    default:
      return "Unknown";
  }
}

void dump (const PageCurl& curl)
{
  printf ("  mode                              %s\n",                     get_mode_name (curl.Mode ()));
  printf ("  front left material               '%s'\n",                   curl.PageMaterial (PageCurlPageType_FrontLeft));
  printf ("  front right material              '%s'\n",                   curl.PageMaterial (PageCurlPageType_FrontRight));
  printf ("  back left material                '%s'\n",                   curl.PageMaterial (PageCurlPageType_BackLeft));
  printf ("  back right material               '%s'\n",                   curl.PageMaterial (PageCurlPageType_BackRight));
  printf ("  shadow material                   '%s'\n",                   curl.ShadowMaterial ());
  printf ("  size                              %.2fx%.2f\n",              curl.Size ().x, curl.Size ().y);
  printf ("  corner                            '%s'\n",                   get_corner_name (curl.CurlCorner ()));
  printf ("  corner position                   %.2f; %.2f\n",             curl.CornerPosition ().x, curl.CornerPosition ().y);
  printf ("  curl radius                       %.2f\n",                   curl.CurlRadius ());
  printf ("  minimum curl radius               %.2f\n",                   curl.MinimumCurlRadius ());
  printf ("  grid size                         %ux%u\n",                  curl.GridSize ().x, curl.GridSize ().y);
  printf ("  page color                        %.3f; %.3f; %.3f; %.3f\n", curl.PageColor ().x, curl.PageColor ().y, curl.PageColor ().z, curl.PageColor ().w);
  printf ("  corner shadow offset              %.2f\n",                   curl.CornerShadowOffset ());
  printf ("  shadow width                      %.2f\n",                   curl.ShadowWidth ());
  printf ("  shadow density                    %.2f\n",                   curl.ShadowDensity ());
  printf ("  shadow grow power                 %.2f\n",                   curl.ShadowGrowPower ());
  printf ("  opposite corner shadow grow power %.2f\n",                   curl.OppositeCornerShadowGrowPower ());
  printf ("  find best curl steps              %u\n",                     curl.FindBestCurlSteps ());
  printf ("  binding mismatch weight           %.2f\n",                   curl.BindingMismatchWeight ());
}

int main ()
{
  printf ("Results of page_curl_test:\n");
  
  PageCurl::Pointer curl (PageCurl::Create ());

  printf ("Default curl state:\n");

  dump (*curl);

  curl->SetMode                          (PageCurlMode_DoublePageDoubleMaterial);
  curl->SetPageMaterial                  (PageCurlPageType_Front,     "front_material");
  curl->SetPageMaterial                  (PageCurlPageType_BackLeft,  "back_left_material");
  curl->SetPageMaterial                  (PageCurlPageType_BackRight, "back_right_material");
  curl->SetShadowMaterial                ("shadow_material");
  curl->SetSize                          (2.f, 4.f);
  curl->SetCurlCorner                    (PageCurlCorner_RightBottom);
  curl->SetCornerPosition                (1.f, 2.f);
  curl->SetCurlRadius                    (4.f);
  curl->SetMinimumCurlRadius             (2.f);
  curl->SetGridSize                      (10, 20);
  curl->SetPageColor                     (0.5f, 0.25f, 0.125f, 0.25f);
  curl->SetCornerShadowOffset            (16.f);
  curl->SetShadowWidth                   (8.f);
  curl->SetShadowDensity                 (0.5f);
  curl->SetShadowGrowPower               (0.5f);
  curl->SetOppositeCornerShadowGrowPower (2.f);
  curl->SetFindBestCurlSteps             (500);
  curl->SetBindingMismatchWeight         (2.f);

  printf ("Changed curl state:\n");

  dump (*curl);

  return 0;
}
