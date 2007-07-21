#include <media/collada.h>
#include <xtl/functional>
#include <stdio.h>

using namespace medialib::collada;
using namespace xtl;

const char* file_name = "data/cube.dae";

//печать строки пробелов (отступ)
void print_space (int count)
{
  count *= 2;

  for (int i = 0; i < count; i++) printf(" ");
}

//печать числа с плавающей точкой
void print (float value)
{
  printf ("%g", value);
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
  
  printf      ("Effect: '%s'\n", effect.EntityID ());  
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
  printf      ("Material '%s'\n", material.EntityID ());
  print_space (level);
  printf      ("effect: '%s'\n", material.Effect ().EntityID ());
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
  printf      ("Library '%s' (%u items)\n", library.EntityID (), library.Size ());

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
    dump   (model.Effects (), 0);
    dump   (model.Materials (), 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
