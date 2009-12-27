#include <cstdio>

#include <xtl/iterator.h>

#include <common/file.h>

#include <media/adobe/xfl.h>

using namespace media::adobe::xfl;

const char* MOUNT_POINT = "/mount/test.xfl";
const char* FILE_NAME   = "data/test.xfl";

template <class Item>
void dump (const char* collection_name, const ICollection<Item>& collection, int level);

//печать строки пробелов (отступ)
void print_space (int count)
{
  count *= 2;

  for (int i = 0; i < count; i++) printf(" ");
}

//печать числа с плавающей точкой
void print (float value)
{
  static float EPS = 0.001f;

  if (fabs (value) < EPS && value < 0.0f)
    value = 0.0f;

  printf ("%+.3f", value);
}

//печать вектора
template <class T, unsigned int N>
void print (const math::vector<T, N>& v)
{
  printf ("[");

  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }

  printf (" ]");
}

//печать кейфремов
void dump (const PropertyAnimationKeyframe& keyframe, int level)
{
  print_space (level++);
  printf      ("Keyframe\n");
  print_space (level);
  printf      ("Value: %.3f\n", keyframe.value);
  print_space (level);
  printf      ("Time:  %.3f\n", keyframe.time);
}

//печать анимируемого параметра
void dump (const PropertyAnimation& property, int level)
{
  print_space (level++);
  printf      ("Property '%s'\n", property.Name ());
  print_space (level);
  printf      ("Enabled: %s\n", property.Enabled () ? "yes" : "no");

  dump ("keyframes", property.Keyframes (), level);
}

//печать анимации кадра анимации
void dump (const AnimationCore& animation, int level)
{
  print_space (++level);
  printf      ("Duration:       %.3f\n", animation.Duration ());
  print_space (level);
  printf      ("Orient to path: %s\n", animation.OrientToPath () ? "yes" : "no");

  dump ("animation properties", animation.Properties (), level);
}

const char* get_frame_element_type (FrameElementType type)
{
  switch (type)
  {
    case FrameElementType_ResourceInstance: return "ResourceInstance";
    case FrameElementType_SymbolInstance:   return "SymbolInstance";
    default:                                return "Unknown";
  }
}

//печать элемента кадра анимации
void dump (const FrameElement& element, int level)
{
  print_space (level++);
  printf      ("Frame element '%s'\n", element.Name ());
  print_space (level);
  printf      ("Type:                 %s\n", get_frame_element_type (element.Type ()));
  print_space (level);
  printf      ("First frame:          %u\n", element.FirstFrame ());
  print_space (level);
  printf      ("Translation:          ");
  print       (element.Translation ());
  printf      ("\n");
  print_space (level);
  printf      ("Transformation point: ");
  print       (element.TransformationPoint ());
  printf      ("\n");
}

//печать кадра анимации
void dump (const Frame& frame, int level)
{
  print_space (level++);
  printf      ("Frame:\n");
  print_space (level);
  printf      ("First frame: %u\n", frame.FirstFrame ());
  print_space (level);
  printf      ("Duration:    %u\n", frame.Duration ());
  print_space (level);
  printf      ("Animation:\n");
  dump        (frame.Animation (), level);

  dump ("frame elements", frame.Elements (), level);
}

//печать слоя анимации
void dump (const Layer& layer, int level)
{
  print_space (level++);
  printf      ("Layer '%s'\n", layer.Name ());

  dump ("frames", layer.Frames (), level);
}

//печать анимации
void dump (const Timeline& timeline, int level)
{
  print_space (level++);
  printf      ("Timeline '%s'\n", timeline.Name ());

  dump ("layers", timeline.Layers (), level);
}

//печать ресурса
void dump (const Resource& resource, int level)
{
  print_space (level++);
  printf      ("Resource '%s'\n", resource.Name ());
  print_space (level);
  printf      ("Path: '%s'\n", resource.Path ());
}

//печать элемента анимации
void dump (const Symbol& symbol, int level)
{
  print_space (level++);
  printf      ("Symbol '%s'\n", symbol.Name ());

  dump (symbol.Timeline (), level);
}

//печать библиотеки
template <class Item>
void dump (const char* collection_name, const ICollection<Item>& collection, int level)
{
  print_space (level++);
  printf      ("Collection '%s' (%u items)\n", collection_name, collection.Size ());

  for (typename ICollection<Item>::ConstIterator i=collection.CreateIterator (); i; ++i)
    dump (*i, level);
}

int main ()
{
  printf ("Results of xfl_load_test:\n");

  try
  {
    common::FileSystem::Mount (MOUNT_POINT, FILE_NAME, "zip");

    printf ("--- Parse xfl document '%s' ---\n", FILE_NAME);

    Document document (MOUNT_POINT);

    printf ("---  Xfl document dump ---\n");
    printf ("Document '%s'\n", document.Name ());
    print_space (1);
    printf ("framerate:        %.3f\n", document.FrameRate ());
    print_space (1);
    printf ("width:            %u\n", document.Width ());
    print_space (1);
    printf ("height:           %u\n", document.Height ());
    print_space (1);
    printf ("background color: ");
    print (document.BackgroundColor ());
    printf ("\n");

    dump ("resources", document.Resources (), 1);
    dump ("symbols",   document.Symbols   (), 1);
    dump ("timelines", document.Timelines (), 1);

  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
