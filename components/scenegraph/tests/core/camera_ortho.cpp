#include "shared.h"

typedef com_ptr<OrthoCamera> OrthoCameraPtr;

void print (float value)
{
  printf ("%+.3f", value);
}

template <class T, size_t N>
void dump_vector (const math::vec<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }
  
  printf (" ]");
}

template <class T, size_t N>
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
  if (camera.IsInfiniteBounds ()) printf ("camera has infinite bounds:\n  local=", camera.Name ());
  else                            printf ("camera bound:\n  local=", camera.Name ());

  dump (camera.BoundBox ());
  printf ("\n  world=");
  dump (camera.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of camera_ortho_test:\n");
  
  OrthoCameraPtr camera (OrthoCamera::Create (), false);

  camera->SetLeft   (-2);
  camera->SetRight  (2);
  camera->SetBottom (-2);
  camera->SetTop    (2);
  camera->SetZNear  (0);
  camera->SetZFar   (4);

  printf ("Ortho camera params:\n left = %f, right = %f, bottom = %f, top = %f, znear = %f, zfar = %f\n", camera->Left (), camera->Right (), 
           camera->Bottom (), camera->Top (), camera->ZNear (), camera->ZFar ());

  printf ("Projection matrix:\n");
  dump_matrix (camera->ProjectionMatrix ());
  printf ("\n");

  dump_bounds (*camera);

  return 0;
}
