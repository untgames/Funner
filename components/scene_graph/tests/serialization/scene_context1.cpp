#include "shared.h"

int main ()
{
  printf ("Results of scene_context1_test:\n");
  
  try
  {
    SceneContext context;
    
    printf ("typed anonymous attachments\n");
    
    int x = 5;
    
    context.Attach (x);
    
    const int* px = context.FindAttachment<const int> ();
    
    if (px) printf ("int attachment: %d\n", *px);
    else    printf ("int attachment not found\n");
    
    try
    {
      context.Attachment<float> ();

      printf ("exception has not thrown\n");
    }
    catch (...)
    {
      printf ("exception thrown\n");
    }
    
    context.DetachAll<int> ();
    
    px = context.FindAttachment<const int> ();
    
    if (px) printf ("int attachment: %d\n", *px);
    else    printf ("int attachment not found\n");    
    
    float y = -1.0f;
    
    context.Attach ("y", y);
    
    const float* py = context.FindAttachment<const float> ("y");

    if (py) printf ("float attachment 'y': %.2f\n", *py);
    else    printf ("float attachment 'y' not found\n");
    
    try
    {
      context.Attachment<float> ("y");

      printf ("exception has not thrown\n");
    }
    catch (...)
    {
      printf ("exception thrown\n");
    }
    
    context.Detach<float> ("y");
    
    py = context.FindAttachment<const float> ("y");

    if (py) printf ("float attachment 'y': %.2f\n", *py);
    else    printf ("float attachment 'y' not found\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
