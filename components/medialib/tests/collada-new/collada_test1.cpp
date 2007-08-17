#include <media/collada.h>
#include <xtl/functional>
#include <stdio.h>

using namespace medialib::collada;
using namespace xtl;

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
void dump (const char* name, Texture& texture, int level)
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
void dump (Effect& effect, int level)
{
  print_space (level++);
  
  printf      ("Effect: '%s'\n", effect.EntityId ());  
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
    if (effect.HasTexture (texmap->map))
    {
      dump (texmap->name, effect.Texture (texmap->map), level);
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
void dump (Material& material, int level)
{
  print_space (level++);
  printf      ("Material '%s'\n", material.EntityId ());
  print_space (level);
  printf      ("effect: '%s'\n", material.Effect ().EntityId ());
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

//печать поверхности
void dump (Surface& surface, int level)
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
  printf      ("material_name: '%s'\n", surface.MaterialName ());  
  for (size_t i = 0; i < surface.InfluenceChannels ().Size (); i++)
  {
    print_space (level);
    printf ("skin: '%s'\n", surface.InfluenceChannels ().Name (i));
  }
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

  print_space (level);
  printf      ("indices: ");

  for (size_t i=0; i<surface.IndicesCount (); i++)
    printf ("%u ", surface.Indices () [i]);

  printf ("\n");
}

//печать меша
void dump (Mesh& mesh, int level)
{
  print_space (level++);
  printf      ("Mesh '%s' (%u surfaces)\n", mesh.EntityId (), mesh.Surfaces ().Size ());

  for (size_t i=0; i<mesh.Surfaces ().Size (); i++)
    dump (mesh.Surfaces () [i], level);
}

//печать цели морфа
void dump (MorphTarget& morph_target, int level)
{
  print_space (level);
  printf      ("Mesh '%s', weight = %f\n", morph_target.Mesh ().EntityId (), morph_target.Weight ());
}

//печать морфа
void dump (Morph& morph, int level)
{
  print_space (level++);
  printf      ("Morph '%s' (%u targets)\n", morph.EntityId (), morph.Targets ().Size ());
  print_space (level);
  printf      ("base mesh: '%s'\n", morph.BaseMesh ().EntityId ());
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
void dump (Skin& skin, int level)
{
  print_space (level++);
  printf      ("Skin '%s'\n", skin.EntityId ());
  print_space (level);
  if (skin.BaseMorph ())
    printf ("base morph '%s'\n", skin.BaseMorph ()->EntityId ());
  else
    printf ("no base morph\n");
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
void dump (Light& light, int level)
{
  print_space (level++);
  printf      ("Light '%s'\n", light.EntityId ());
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

void dump (Camera& camera, int level)
{
  print_space (level++);
  printf      ("Camera '%s'\n", camera.EntityId ());
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
void dump (InstanceMesh& imesh, int level)
{
  Mesh::SurfaceList& surfaces = imesh.Mesh ().Surfaces ();
  MaterialBinds&     binds    = imesh.MaterialBinds ();

  print_space (level++);
  printf      ("Instance mesh '%s'\n", imesh.Mesh ().EntityId ());  

  for (size_t i=0; i<surfaces.Size (); i++)
  {
    Surface&  surface  = surfaces [i];
    Material* material = binds.FindMaterial (surface);
    
    if (!material)
    {
      print_space (level);
      printf      ("No bind material with name '%s' detected", surface.MaterialName ());
      continue;
    }
    
    print_space (level);
    printf      ("surface #%u material: '%s'\n", i, material->EntityId ());
    
    Effect& effect = material->Effect ();
    
    for (const TexmapName* texmap = texmaps; texmap->name; texmap++)
      if (effect.HasTexture (texmap->map))
      {
        print_space (level+1);
        printf      ("map %s channel #%d\n", texmap->name, binds.FindTexcoordChannel (surface, effect.Texture (texmap->map)));
      }
  }
}

//вывод коллекции
template <class Item> void dump (ICollection<Item>& collection, int level)
{
  print_space (level++);
  printf      ("Collection '%s' (%u items)\n", collection.EntityId (), collection.Size ());    

  for (size_t i=0; i<collection.Size (); i++)
  {
    print_space (level);
    printf      ("'%s'\n", collection [i].EntityId ());
  }
}

//печать узла
void dump (Node& node, int level)
{
  print_space (level++);
  printf      ("Node '%s'\n", node.EntityId ());
  print_space (level);
  printf      ("sid: '%s'\n", node.SubId ());
  print_space (level);
  printf      ("name: '%s'\n", node.Name ());
  print_space (level);
  printf      ("transform: ");
  print       (node.Transform ());
  printf      ("\n");
  
  for (size_t i=0; i<node.Meshes ().Size (); i++)
    dump (node.Meshes () [i], level);

  dump (node.Lights (), level);
  dump (node.Cameras (), level);
  
  for (size_t i=0; i<node.Nodes ().Size (); i++)
    dump (node.Nodes () [i], level);
}

//печать элемента библиотеки
template <class Item> void dump_item (Item& item, int level)
{
  dump (item, level);
}

//печать библиотеки
template <class Item> void dump (ILibrary<Item>& library, int level)
{
  print_space (level++);
  printf      ("Library '%s' (%u items)\n", library.EntityId (), library.Size ());

  library.ForEach (bind (&dump_item<Item>, _1, level));
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

    dump (model.Effects (), 1);
    dump (model.Materials (), 1);
    dump (model.Meshes (), 1);    
    dump (model.Skins (), 1);
    dump (model.Morphs (), 1);
    dump (model.Lights (), 1);
    dump (model.Cameras (), 1);
    dump (model.Scenes (), 1);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
