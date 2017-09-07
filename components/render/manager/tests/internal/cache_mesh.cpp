#include "shared.h"

using namespace media::geometry;

typedef Vertex<Position3f, Normalf, Color4ub, TexChannel<0>::Coord2f> CustomVertex;

void on_application_initialized ()
{
  try
  {
    printf ("--- Create device ---\n");
    
    Test test (L"Mesh cache test", true);            
    
    RenderManager render_manager = test.RenderManager ();    
    
      //создание материала и примитива
      
    printf ("--- Load resources ---\n");

    render_manager.LoadResource ("data/test.xmesh");
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource ("data/render.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/pic1.dds");
    
    printf ("--- Create primitive & entity ---\n");
    
    render::manager::Primitive primitive = render_manager.CreateSharedPrimitive ("mesh2");

    primitive.UpdateCache ();
    
    Entity entity = render_manager.CreateEntity ();
    
    entity.SetPrimitive ("mesh2");
    
    entity.UpdateCache ();    
    
    printf ("--- Update primitive ---\n");    
    
    Mesh mesh1;    
    
    VertexStream vs (get_vertex_format<CustomVertex> ());
    media::geometry::VertexBuffer vb;
    
    vb.Attach (vs);  
    
    mesh1.Attach (vb);
      
    media::geometry::IndexBuffer ib (12);
    
    mesh1.Attach (ib);
    mesh1.Rename ("my mesh");
    
    vs.Resize (5);        
    
    mesh1.AddPrimitive (PrimitiveType_TriangleList, 0, 12, 5, "canyon.Material__195");
    mesh1.AddPrimitive (PrimitiveType_TriangleStrip, 0, 14, 8, "canyon.Material__195");    
    
    primitive.AddMesh (mesh1);
    
    entity.UpdateCache ();
    
    printf ("--- Unload materials ---\n");
    
    render_manager.UnloadResource ("data/test.xmtl");
    
    entity.UpdateCache ();    
      
    printf ("--- Exiting... ---\n");          
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of cache_mesh_test:\n");

  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
