#include "shared.h"

typedef com_ptr<PerspectiveCamera> PerspectiveCameraPtr;

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

int main ()
{
  printf ("Results of camera_perspective_test:\n");
  
  PerspectiveCameraPtr camera (PerspectiveCamera::Create (), false);

  camera->SetFovX   (16);
  camera->SetFovY   (9);
  camera->SetZNear  (1);
  camera->SetZFar   (4);

  printf ("Perspective camera params:\n fovx = %f, fovy = %f, znear = %f, zfar = %f\n", camera->FovX (), camera->FovY (), camera->ZNear (), camera->ZFar ());

  printf ("Projection matrix:\n");
  dump_matrix (camera->ProjectionMatrix ());
  printf ("\n");

  return 0;
}
