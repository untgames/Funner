#include "shared.h"

struct CustomLoader: public ISceneFactory
{
  CustomLoader ()
  {
    printf ("CustomLoader::CustomLoader\n");
  }
  
  ~CustomLoader ()
  {
    printf ("CustomLoader::~CustomLoader\n");
  }

  bool GetSceneInfo (const char* name, ResourceGroup* resources)
  {
    if (!strcmp (name, "my_scene"))
      return true;
      
    return false;
  }
    
  void CreateScene (const char* name, Node& parent, SceneContext& scene_context)
  {
    printf ("CreateScene(%s)\n", name);
  }

  static ISceneFactory* LoadScene (const char* file_name, const SceneManager::LogHandler&)
  {
    return new CustomLoader;
  }
};

void register_attachments (SceneContext& context)
{
  printf ("register_attachments\n");
}

int main ()
{
  printf ("Results of scene_manager_test:\n");
  
  try
  {
    SceneManager manager;
                                                                                     
    manager.RegisterSceneContextCreator (&register_attachments);

    SceneSerializationManager::RegisterLoader ("test_scn", &CustomLoader::LoadScene);
    
    manager.LoadScene ("test.test_scn", "prefix");
    
    Node::Pointer node = manager.CreateScene ("prefix.my_scene");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
