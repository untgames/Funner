#include "shared.h"

void print (float value)
{
  printf ("%+.3f", value);
}

template <class T, unsigned int N>
void dump_vector (const math::vector<T, N>& v)
{
  printf ("[");
  
  for (unsigned int i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }
  
  printf (" ]");
}

void test (const char* name, const Camera& camera, const math::vec4f& p)
{
  math::vec4f np = camera.ProjectionMatrix () * p;

  np /= np.w;

  printf ("at %s: ", name);

  dump_vector (np);

  printf ("\n");
}

int main ()
{
  printf ("Results of camera_projections_test:\n");

  {
    printf ("perspective camera tests:\n");

    PerspectiveCamera::Pointer camera (PerspectiveCamera::Create ());

    camera->SetFovX   (degree (90.0f));
    camera->SetFovY   (degree (90.0f));
    camera->SetZNear  (1);
    camera->SetZFar   (4);

    test ("zfar",   *camera, math::vec4f (0,       0, 4.0f, 1.0f));
    test ("znear",  *camera, math::vec4f (0,       0, 1.0f, 1.0f));
    test ("left",   *camera, math::vec4f (1.0f,    0, 1.0f, 1.0f));
    test ("right",  *camera, math::vec4f (-1.0f,   0, 1.0f, 1.0f));
    test ("top",    *camera, math::vec4f (0,    1.0f, 1.0f, 1.0f));
    test ("bottom", *camera, math::vec4f (0,   -1.0f, 1.0f, 1.0f));
  }

  {
    printf ("ortho camera tests:\n");

    OrthoCamera::Pointer camera (OrthoCamera::Create ());

    camera->SetLeft   (-1.0);
    camera->SetRight  ( 1.0);
    camera->SetTop    ( 1.0);
    camera->SetBottom (-1.0);
    camera->SetZNear  (1);
    camera->SetZFar   (4);

    test ("zfar",   *camera, math::vec4f (0,       0, 4.0f, 1.0f));
    test ("znear",  *camera, math::vec4f (0,       0, 1.0f, 1.0f));
    test ("left",   *camera, math::vec4f (1.0f,    0, 1.0f, 1.0f));
    test ("right",  *camera, math::vec4f (-1.0f,   0, 1.0f, 1.0f));
    test ("top",    *camera, math::vec4f (0,    1.0f, 1.0f, 1.0f));
    test ("bottom", *camera, math::vec4f (0,   -1.0f, 1.0f, 1.0f));
  }

  return 0;
}
