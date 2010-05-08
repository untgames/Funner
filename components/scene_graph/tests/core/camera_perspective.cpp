#include "shared.h"

void print (float value)
{
  printf ("%+.3f", value);
}

template <class T, unsigned int N>
void dump_vector (const math::vector<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }
  
  printf (" ]");
}

template <class T, unsigned int N>
void dump_matrix (const math::matrix<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    dump_vector (v [i]);
  }

  printf (" ]");
}

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());  
  printf ("]");
}

void dump_bounds (Camera& camera)
{
  if (camera.IsInfiniteBounds ()) printf ("camera '%s' has infinite bounds:\n  local=", camera.Name ());
  else                            printf ("camera '%s' bound:\n  local=", camera.Name ());

  dump (camera.BoundBox ());
  printf ("\n  world=");
  dump (camera.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of camera_perspective_test:\n");
  
  PerspectiveCamera::Pointer camera (PerspectiveCamera::Create ());

  camera->SetFovX   (degree (16.0f));
  camera->SetFovY   (degree (9.0f));
  camera->SetZNear  (1);
  camera->SetZFar   (4);

  printf ("Perspective camera params:\n fovx = %f, fovy = %f, znear = %f, zfar = %f\n", degree (camera->FovX ()), degree (camera->FovY ()), camera->ZNear (), camera->ZFar ());

  printf ("Projection matrix:\n");
  dump_matrix (camera->ProjectionMatrix ());
  printf ("\n");

  dump_bounds (*camera);

  camera->SetFovX (degree (32.0f));
  printf ("Projection matrix:\n");
  dump_matrix (camera->ProjectionMatrix ());
  printf ("\n");

  return 0;
}
