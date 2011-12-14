#include "shared.h"

int main ()
{
  printf ("Results of cache_material_test:\n");
  
  try
  {
    printf ("--- Create device ---\n");
    
    Test test (L"Material cache test", true);            
    
    RenderManager render_manager = test.RenderManager ();    
    
      //создание материала и примитива
      
    printf ("--- Load resources ---\n");

    render_manager.LoadResource ("data/test.xmesh");
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource ("data/render.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/pic1.dds");
    
    printf ("--- Create primitive & material ---\n");
    
    Primitive primitive = render_manager.CreateSharedPrimitive ("mesh2");
    Material  material  = render_manager.CreateSharedMaterial ("canyon.stn_2");
    
    printf ("--- Update primitive cache ---\n");    

    primitive.UpdateCache ();

      //обновление полного состояния материала
      
    printf ("--- Update full material state ---\n");        
    
    media::rfx::Material src_mtl;
    
    src_mtl.Properties ().SetProperty ("A", 1.0f);
    src_mtl.Properties ().SetProperty ("B", 2);
    
    src_mtl.SetTags ("solid");
    
    src_mtl.SetProgram ("phong");
    
    media::rfx::Texmap diffuse_map;
    
    diffuse_map.SetSemantic ("diffuse");
    diffuse_map.SetImage    ("data/bottom.jpg");
    diffuse_map.SetSampler  ("default");    
    
    src_mtl.AddTexmap (diffuse_map);
    
    material.Update (src_mtl);
    
    primitive.UpdateCache ();

      //обновление свойств материала с сохранение их структуры
      
    printf ("--- Update material properties (with same layout) ---\n");
      
    src_mtl.Properties ().SetProperty ("A", 2.0f);
    
    primitive.UpdateCache ();
    
      //обновление свойств материала с изменением их структуры        
    
    printf ("--- Update material properties (with new layout) ---\n");
      
    src_mtl.Properties ().SetProperty ("C", math::mat4f (3.0f));
    src_mtl.Properties ().SetProperty ("D", math::mat4f (4.0f));    
    src_mtl.Properties ().SetProperty ("E", math::mat4f (5.0f));    
    
    primitive.UpdateCache ();

      //обновление текстур
      
    printf ("--- Unload texture ---\n");      
      
    render_manager.UnloadResource ("data/bottom.jpg");
    
    try
    {
      primitive.UpdateCache ();    
    }
    catch (std::exception& e)
    {
      printf ("%s\n", e.what ());
    }

    printf ("--- Load texture ---\n");      

    render_manager.LoadResource ("data/bottom.jpg");        

    primitive.UpdateCache ();        

      //обновление программы
      
    printf ("--- Update program ---\n");            
      
    src_mtl.SetProgram ("helper");
    
    material.Update (src_mtl);
    
    primitive.UpdateCache ();
      
    printf ("--- Exiting... ---\n");          
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
