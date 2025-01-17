#include "shared.h"

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());
  printf ("]");
}

void dump_bounds (const PageCurl& light)
{
  printf ("    local=");

  dump (light.BoundBox ());
  printf ("\n    world=");
  dump (light.WorldBoundBox ());
  printf ("\n");
}

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

void dump (const PageCurl& curl)
{
  printf ("  mode                              %s\n",                     get_mode_name (curl.Mode ()));
  printf ("  front left material               '%s'\n",                   curl.PageMaterial (PageCurlPageType_FrontLeft));
  printf ("  front right material              '%s'\n",                   curl.PageMaterial (PageCurlPageType_FrontRight));
  printf ("  back left material                '%s'\n",                   curl.PageMaterial (PageCurlPageType_BackLeft));
  printf ("  back right material               '%s'\n",                   curl.PageMaterial (PageCurlPageType_BackRight));
  printf ("  size                              %.2fx%.2f\n",              curl.Size ().x, curl.Size ().y);
  printf ("  curl point                        %.2f; %.2f\n",             curl.CurlPoint ().x, curl.CurlPoint ().y);
  printf ("  curl point position               %.2f; %.2f\n",             curl.CurlPointPosition ().x, curl.CurlPointPosition ().y);
  printf ("  curl radius                       %.2f\n",                   curl.CurlRadius ());
  printf ("  minimum curl radius               %.2f\n",                   curl.MinimumCurlRadius ());
  printf ("  grid size                         %ux%u\n",                  curl.GridSize ().x, curl.GridSize ().y);
  printf ("  page color                        %.3f; %.3f; %.3f; %.3f\n", curl.PageColor ().x, curl.PageColor ().y, curl.PageColor ().z, curl.PageColor ().w);
  printf ("  corner shadow offset              %.2f\n",                   curl.CornerShadowOffset ());
  printf ("  shadow width                      %.2f\n",                   curl.ShadowWidth ());
  printf ("  shadow log base                   %.2f\n",                   curl.ShadowLogBase ());
  printf ("  shadow min log value              %.2f\n",                   curl.ShadowMinLogValue ());
  printf ("  find best curl steps              %u\n",                     curl.FindBestCurlSteps ());
  printf ("  binding mismatch weight           %.2f\n",                   curl.BindingMismatchWeight ());
  printf ("  is rigid page                     %c\n",                     curl.IsRigidPage () ? 'y' : 'n');
  printf ("  rigid page perspective factor     %.2f\n",                   curl.RigidPagePerspectiveFactor ());
  printf ("  bounding box:\n");
  dump_bounds (curl);
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
  curl->SetSize                          (2.f, 4.f);
  curl->SetCurlPoint                     (PageCurlCorner_RightBottom);
  curl->SetCurlPointPosition             (1.f, 2.f);
  curl->SetCurlRadius                    (4.f);
  curl->SetMinimumCurlRadius             (2.f);
  curl->SetGridSize                      (10, 20);
  curl->SetPageColor                     (0.5f, 0.25f, 0.125f, 0.25f);
  curl->SetCornerShadowOffset            (16.f);
  curl->SetShadowWidth                   (8.f);
  curl->SetShadowLogBase                 (32.f);
  curl->SetShadowMinLogValue             (0.f);
  curl->SetFindBestCurlSteps             (500);
  curl->SetBindingMismatchWeight         (2.f);
  curl->SetRigidPage                     (true);
  curl->SetRigidPagePerspectiveFactor    (2.f);

  printf ("Changed curl state:\n");

  dump (*curl);

  return 0;
}
