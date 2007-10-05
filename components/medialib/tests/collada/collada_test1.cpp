#include "shared.h"

const char* file_name = "data/ape.dae";

struct TexmapName
{
  TextureMap  map;
  const char* name;
};

static TexmapName texmaps [] = {
  {TextureMap_Diffuse,     "diffuse"},
  {TextureMap_Ambient,     "ambient"},
  {TextureMap_Specular,    "specular"},
  {TextureMap_Reflective,  "reflective"},
  {TextureMap_Transparent, "transparent"},
  {TextureMap_Bump,        "bump"},
  {(TextureMap)0, 0}
};

//печать строки пробелов (отступ)
void print_space (int count)
{
  count *= 2;

  for (int i = 0; i < count; i++) printf(" ");
}

//печать числа с плавающей точкой
void print (float value)
{
  printf ("%+.3f", value);
}

//печать вектора
template <class T, size_t N>
void print (const math::vec<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }
  
  printf (" ]");
}

//печать матрицы
template <class T, size_t N>
void print (const math::matrix<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }

  printf (" ]");
}

//печать текстуры
void dump (const char* name, Texture& texture, int level, Model& model)
{
  print_space (level++);  
  printf      ("Texture '%s'\n", name);
  print_space (level);
  printf      ("image: '%s'\n", texture.Image ());
  print_space (level);
  printf      ("texcoord: '%s'\n", texture.TexcoordChannel ());
  print_space (level);
  printf      ("amount: %g\n", texture.Amount ());
  print_space (level);
  printf      ("transform: ");
  print       (texture.Transform ());
  printf      ("\n");
}

//печать эффекта
void dump (Effect& effect, int level, Model& model)
{
  print_space (level++);
  
  printf      ("Effect: '%s'\n", effect.Id ());  
  print_space (level);
  printf      ("shader type: ");
  
  switch (effect.ShaderType ())
  {
    case ShaderType_Constant: printf ("constant\n"); break;
    case ShaderType_Lambert:  printf ("lambert\n");  break;
    case ShaderType_Phong:    printf ("phong\n");    break;
    case ShaderType_Blinn:    printf ("blinn\n");    break;
    default:                  printf ("unknown\n");  break;
  }

  print_space (level);  
  printf      ("reflectivity: %g\n", effect.Param (EffectParam_Reflectivity));  
  print_space (level);
  printf      ("transparency: %g\n", effect.Param (EffectParam_Transparency));
  print_space (level);
  printf      ("refraction index: %g\n", effect.Param (EffectParam_RefractionIndex));  
  print_space (level);
  printf      ("shininess: %g\n", effect.Param (EffectParam_Shininess));  
      
  for (const TexmapName* texmap = texmaps; texmap->name; texmap++)
    if (*effect.Texture (texmap->map).Image ())
    {
      dump (texmap->name, effect.Texture (texmap->map), level, model);
    }
    else
    {
      print_space (level);
      printf      ("%s color: ", texmap->name);
      print       (effect.MapColor (texmap->map));
      printf      ("\n");
    }    
}

//печать материала
void dump (Material& material, int level, Model& model)
{
  print_space (level++);
  printf      ("Material '%s'\n", material.Id ());
  print_space (level);
  printf      ("effect: '%s'\n", material.Effect ());
}

//печать вершины
void print (const Vertex& v)
{
  printf ("coord=");
  print  (v.coord);
  printf (" normal=");
  print  (v.normal);
}

//печать текстурированной вершины
void print (const TexVertex& tv)
{
  printf ("coord=");
  print  (tv.coord);
  printf (" tangent=");
  print  (tv.tangent);
  printf (" binormal=");
  print  (tv.binormal);  
}

//печать вершинного веса
void print (const VertexInfluence& influence)
{
  printf ("first=%u count=%u", influence.first_weight, influence.weights_count);
}

//печать поверхности
void dump (Surface& surface, int level, Model& model)
{
  print_space (level++);
  printf      ("Surface:\n");
  print_space (level);
  printf      ("primitive_type: ");
  
  switch (surface.PrimitiveType ())
  {
    case PrimitiveType_LineList:      printf ("line-list\n"); break;
    case PrimitiveType_LineStrip:     printf ("line-strip\n"); break;
    case PrimitiveType_TriangleList:  printf ("triangle-list\n"); break;    
    case PrimitiveType_TriangleStrip: printf ("triangle-strip\n"); break;    
    case PrimitiveType_TriangleFan:   printf ("triangle-fan\n"); break;
    default:                          printf ("unknown\n"); break;
  }
  
  print_space (level);
  printf      ("material_name: '%s'\n", surface.Material ());  

  print_space (level);
  printf      ("vertices_count: %u\n", surface.VerticesCount ());
  print_space (level);
  printf      ("indices_count:  %u\n", surface.IndicesCount ());  
  print_space (level);
  printf      ("vertices\n");
  
  for (size_t i=0; i<surface.VerticesCount (); i++)
  {
    print_space (level+1);
    print       (surface.Vertices () [i]);
    printf      ("\n");
  }
  
  for (size_t i=0; i<surface.TexVertexChannels ().Size (); i++)
  {
    print_space (level);
    printf      ("texture_channel '%s'\n", surface.TexVertexChannels ().Name (i));
    
    const TexVertex* tv = surface.TexVertexChannels ().Data (i);
    
    for (size_t j=0; j<surface.VerticesCount (); j++, tv++)
    {
      print_space (level+1);
      print       (*tv);
      printf      ("\n");
    }
  }
  
  for (size_t i=0; i<surface.ColorChannels ().Size (); i++)
  {
    print_space (level);
    printf      ("color_channel '%s'\n", surface.ColorChannels ().Name (i));
    
    const math::vec3f* color = surface.ColorChannels ().Data (i);
    
    for (size_t j=0; j<surface.VerticesCount (); j++, color++)
    {
      print_space (level+1);
      print       (*color);
      printf      ("\n");
    }
  }  
  
  for (size_t i = 0; i < surface.InfluenceChannels ().Size (); i++)
  {
    print_space (level);
    printf ("influence_channel: '%s'\n", surface.InfluenceChannels ().Name (i));
    
    const VertexInfluence* influence = surface.InfluenceChannels ().Data (i);
    
    for (size_t j=0; j < surface.VerticesCount (); j++, influence++)
    {
      print_space (level+1);
      print       (*influence);
      printf      ("\n");
    }
  }  

  print_space (level);
  printf      ("indices: ");

  for (size_t i=0; i<surface.IndicesCount (); i++)
    printf ("%u ", surface.Indices () [i]);

  printf ("\n");
}

//печать меша
void dump (Mesh& mesh, int level, Model& model)
{
  print_space (level++);
  printf      ("Mesh '%s' (%u surfaces)\n", mesh.Id (), mesh.Surfaces ().Size ());

  for (size_t i=0; i<mesh.Surfaces ().Size (); i++)
    dump (mesh.Surfaces () [i], level, model);
}

//печать цели морфа
void dump (MorphTarget& morph_target, int level)
{
  print_space (level);
  printf      ("Mesh '%s', weight = %f\n", morph_target.Mesh (), morph_target.Weight ());
}

//печать морфа
void dump (Morph& morph, int level, Model& model)
{
  print_space (level++);
  printf      ("Morph '%s' (%u targets)\n", morph.Id (), morph.Targets ().Size ());
  print_space (level);
  printf      ("base mesh: '%s'\n", morph.BaseMesh ());
  print_space (level);
  printf      ("method: ");
  switch (morph.Method ())
  {
    case MorphMethod_Normalized: printf ("normalized\n"); break;
    case MorphMethod_Relative:   printf ("relative\n");   break;
    default:                     printf ("unknown\n");    break;
  }
  print_space (level++);
  printf      ("targets:\n");
  for (size_t i=0; i<morph.Targets ().Size (); i++)
    dump (morph.Targets () [i], level);
}

//печать скина
void dump (Skin& skin, int level, Model& model)
{
  print_space (level++);
  printf      ("Skin '%s'\n", skin.Id ());
  print_space (level);
  printf      ("base mesh '%s'\n", skin.BaseMesh ());
  print_space (level);
  printf      ("bind shape matrix: ");
  print       (skin.BindShapeMatrix ());
  printf      ("\n");
  print_space (level);
  printf      ("joints count: '%u'\n", skin.JointsCount ());
  print_space (level++);
  printf      ("joints' inv matrixes:\n");
  for (size_t i=0; i<skin.JointsCount (); i++)
  {
    print_space (level);
    printf      ("joint '%s' inv_matrix: ", skin.JointName (i));
    print (skin.JointInvMatrix (i));
    printf      ("\n");
  }
  print_space ((--level)++);
  printf      ("joint weights:\n");
  for (size_t i = 0; i < skin.WeightsCount (); i++)
  {
    print_space (level);
    printf ("joint %d weight %f\n", skin.Weights ()[i].joint, skin.Weights ()[i].weight);
  }
}

//вывод источника света
void dump (Light& light, int level, Model& model)
{
  print_space (level++);
  printf      ("Light '%s'\n", light.Id ());
  print_space (level);
  printf      ("type: ");
  
  switch (light.Type ())
  {
    case LightType_Ambient: printf ("ambient\n"); break;
    case LightType_Point:   printf ("point\n");   break;
    case LightType_Spot:    printf ("spot\n");    break;
    case LightType_Direct:  printf ("direct\n");  break;
    default:                printf ("unknown\n"); break;
  }
  
  print_space (level);
  printf      ("color: ");
  print       (light.Color ());
  printf      ("\n");
  print_space (level);
  printf      ("attenuation_constant: %g\n", light.Param (LightParam_AttenuationConstant));
  print_space (level);
  printf      ("attenuation_linear: %g\n", light.Param (LightParam_AttenuationLinear));
  print_space (level);
  printf      ("attenuation_quadratic: %g\n", light.Param (LightParam_AttenuationQuadratic));
  print_space (level);
  printf      ("falloff_angle: %g\n", light.Param (LightParam_FalloffAngle));
  print_space (level);
  printf      ("falloff_exponent: %g\n", light.Param (LightParam_FalloffExponent));
}

void dump (Camera& camera, int level, Model& model)
{
  print_space (level++);
  printf      ("Camera '%s'\n", camera.Id ());
  print_space (level);
  printf      ("type: ");
  
  switch (camera.Type ())
  {
    case CameraType_Perspective:  printf ("perspective\n"); break;
    case CameraType_Orthographic: printf ("ortho\n");   break;
    default:                      printf ("unknown\n"); break;
  }
  
  print_space (level);
  printf      ("znear: %g\n", camera.Param (CameraParam_ZNear));
  print_space (level);
  printf      ("zfar: %g\n", camera.Param (CameraParam_ZFar));
  print_space (level);
  printf      ("aspect_ratio: %g\n", camera.Param (CameraParam_AspectRatio));
  print_space (level);
  printf      ("xfov: %g\n", camera.Param (CameraParam_XFov));
  print_space (level);
  printf      ("yfov: %g\n", camera.Param (CameraParam_YFov));
  print_space (level);
  printf      ("xmag: %g\n", camera.Param (CameraParam_XMagnitude));
  print_space (level);
  printf      ("ymag: %g\n", camera.Param (CameraParam_YMagnitude));
}

//вывод инстанцированного меша
void dump (InstanceMesh& imesh, int level, Model& model)
{
  Mesh* mesh = model.Meshes ().Find (imesh.Mesh ());

  if (!mesh)
    return;
           
  Mesh::SurfaceList& surfaces = mesh->Surfaces ();
  MaterialBinds&     binds    = imesh.MaterialBinds ();

  print_space (level++);
  printf      ("Instance mesh '%s'\n", imesh.Mesh ());

  for (size_t i=0; i<surfaces.Size (); i++)
  {
    Surface&  surface  = surfaces [i];
    Material* material = model.Materials ().Find (binds.FindMaterial (surface));
    
    if (!material)
    {
      print_space (level);
      printf      ("No bind material with name '%s' detected\n", surface.Material ());
      continue;
    }
    
    print_space (level);
    printf      ("surface #%u material: '%s'\n", i, material->Id ());
    
    Effect* effect = model.Effects ().Find (material->Effect ());
    
    if (!effect)
    {
      print_space (level);
      printf      ("No effect with name '%s' detected\n", material->Effect ());
      continue;
    }
    
    for (const TexmapName* texmap = texmaps; texmap->name; texmap++)
      if (effect->HasTexture (texmap->map))
      {
        print_space (level+1);
        printf      ("map %s channel #%d\n", texmap->name, binds.FindTexcoordChannel (surface, effect->Texture (texmap->map)));
      }
  }
}

//вывод инстанцированного контроллера
void dump (InstanceController& icontroller, int level, Model& model)
{  
  Mesh* mesh = model.Meshes ().Find (icontroller.FindBaseMesh (model));
  
  if (!mesh)
    return;

  Mesh::SurfaceList& surfaces = mesh->Surfaces ();
  MaterialBinds&     binds    = icontroller.MaterialBinds ();

  print_space (level++);
  printf      ("Instance controller '%s'\n", icontroller.Controller ());

  for (size_t i=0; i<surfaces.Size (); i++)
  {
    Surface&  surface  = surfaces [i];
    Material* material = model.Materials ().Find (binds.FindMaterial (surface));
    
    if (!material)
    {
      print_space (level);
      printf      ("No bind material with name '%s' detected\n", surface.Material ());
      continue;
    }
    
    print_space (level);
    printf      ("surface #%u material: '%s'\n", i, material->Id ());
    
    Effect* effect = model.Effects ().Find (material->Effect ());
    
    if (!effect)
    {
      print_space (level);
      printf      ("No effect with name '%s' detected\n", material->Effect ());
      continue;
    }
    
    for (const TexmapName* texmap = texmaps; texmap->name; texmap++)
      if (effect->HasTexture (texmap->map))
      {
        print_space (level+1);
        printf      ("map %s channel #%d\n", texmap->name, binds.FindTexcoordChannel (surface, effect->Texture (texmap->map)));
      }
  }
  
  print_space (level++);
  printf      ("search roots:\n");
  
  for (size_t i=0; i<icontroller.JointSearchRootsCount (); i++)
  {
    print_space (level);
    printf      ("node '%s'\n", icontroller.JointSearchRoot (i));
  }
}

//вывод коллекции
template <class Item> void dump (const char* name, ICollection<Item>& collection, int level)
{
  print_space (level++);
  printf      ("Collection '%s' (%u items)\n", name, collection.Size ());    

  for (size_t i=0; i<collection.Size (); i++)
  {
    print_space (level);
    printf      ("'%s'\n", collection [i].Id ());
  }
}

//печать узла
void dump (Node& node, int level, Model& model)
{
  print_space (level++);
  printf      ("Node '%s'\n", node.Id ());
  print_space (level);
  printf      ("sid: '%s'\n", node.SubId ());
  print_space (level);
  printf      ("name: '%s'\n", node.Name ());
  print_space (level);
  printf      ("transform: ");
  print       (node.Transform ());
  printf      ("\n");
  
  print_space (level);
  printf      ("Collection 'instance_meshes' (%u items)\n", node.Meshes ().Size ());    
   
  for (size_t i=0; i<node.Meshes ().Size (); i++)
    dump (node.Meshes () [i], level + 1, model);
    
  print_space (level);
  printf      ("Collection 'instance_controllers' (%u items)\n", node.Controllers ().Size ());        
    
  for (size_t i=0; i<node.Controllers ().Size (); i++)
    dump (node.Controllers () [i], level + 1, model);

  dump ("lights", node.Lights (), level);
  dump ("cameras", node.Cameras (), level);
  
  for (size_t i=0; i<node.Nodes ().Size (); i++)
    dump (node.Nodes () [i], level, model);
}

//печать элемента библиотеки
template <class Item> void dump_item (Item& item, int level, Model& model)
{
  dump (item, level, model);
}

//печать библиотеки
template <class Item> void dump (const char* library_name, ILibrary<Item>& library, int level, Model& model)
{
  print_space (level++);
  printf      ("Library '%s' (%u items)\n", library_name, library.Size ());
  
  for (ILibrary<Item>::Iterator i=library.CreateIterator (); i; ++i)
    dump (*i, level, model);
}

//функция протоколирования ошибок разбора модели
void log_print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of collada_test1:\n");

  try
  {
    printf ("--- Parse collada model '%s' ---\n", file_name);

    Model model (file_name, &log_print);

    printf ("---  Collada model dump ---\n");
    printf ("Model '%s'\n", file_name);
    print_space (1);
    printf ("Active scene: '%s'\n", model.ActiveSceneName ());

    dump ("library_effects", model.Effects (), 1, model);
    dump ("library_materials", model.Materials (), 1, model);
    dump ("library_meshes", model.Meshes (), 1, model);
    dump ("library_skins", model.Skins (), 1, model);
    dump ("library_morphs", model.Morphs (), 1, model);
    dump ("library_lights", model.Lights (), 1, model);
    dump ("library_cameras", model.Cameras (), 1, model);
    dump ("library_scenes", model.Scenes (), 1, model);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
