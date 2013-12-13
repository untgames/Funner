#include "shared.h"

int main ()
{
  printf ("Results of event_batcher2_test:\n");

  try
  {   
    MyServer server;

    ServerGroupAttachment attachment ("my", server);

    Group group_a, group_b, group_c, group_1, group_2, group_3;

    group_a.Add ("A1");
    group_a.Add ("A3");

    group_c.Add ("C1");
    group_c.Add ("C3");

    group_1.Add ("A1");
    group_1.Add ("C1");

    group_3.Add ("A3");
    group_3.Add ("C3");

    Binding binding_a = ResourceManager::CreateBinding (group_a),
            binding_c = ResourceManager::CreateBinding (group_c),
            binding_1 = ResourceManager::CreateBinding (group_1),
            binding_3 = ResourceManager::CreateBinding (group_3);

    printf ("start recording...\n");
    
    attachment.DisableNotifications ();

    binding_a.Load ();
    binding_c.Unload ();
    binding_1.Load ();
    binding_3.Unload ();

    printf ("notifications:\n");

    attachment.EnableNotifications ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
