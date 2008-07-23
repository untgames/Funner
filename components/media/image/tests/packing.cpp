#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <stl/vector>
#include <stl/algorithm>

#include <media/image.h>

using namespace media;

#pragma pack (1)

const size_t IMAGES_COUNT             = 200;
const size_t MAX_IMAGE_WIDTH          = 1000;
const size_t MAX_IMAGE_HEIGHT         = 1000;
const size_t START_LARGE_IMAGE_WIDTH  = 128;
const size_t START_LARGE_IMAGE_HEIGHT = 128;
const char*  RESULT_IMAGE_NAME        = "results/atlas.tga";

struct Rect
{
  size_t left, top, width, height;
  
  Rect () : left (0), top (0), width (0), height (0) {}

  Rect (size_t in_left, size_t in_top, size_t in_width, size_t in_height) :
    left (in_left), top (in_top), width (in_width), height (in_height) {}    
};

struct Color
{
  unsigned char red, green, blue;
};

struct ImageDesc
{
  size_t width, height;
  int    id;
  Color  color;  
  
  ImageDesc (int in_id, size_t in_width, size_t in_height) : width (in_width), height (in_height), id (in_id)
  {
    color.red = color.green = color.blue;
  }
};

struct Node
{
  Node* child [2];
  Rect  rect;
  int   image_id;
  Color color;
  
  Node () : image_id (0) { child [0] = child [1] = 0; }
  ~Node ()
  {
    if (child [0])
    {
      delete child [0];
      delete child [1];
    }
  }

  Node* Insert (const ImageDesc&);
  
  void PrintImages ();
  void FillImages (Image& image);
};

Node* Node::Insert (const ImageDesc& image)
{
    //если узел не является листом - вставляем картинку в одного из его детей

  if (child [0])
  {
    Node* new_node = child [0]->Insert (image);
    
    if (new_node)
      return new_node;
    
    return child [1]->Insert (image);
  }

    //если в узел уже вставлена картинка - добавление невозможно

  if (image_id)
    return 0;  
  
    //если нет места для вставки картинки - добавление невозможно  
    
  if (rect.width < image.width || rect.height < image.height)
    return 0;
    
    //если размер картинки точно подходит - добавляем её
    
  if (rect.width == image.width && rect.height == image.height)
  {
    image_id = image.id;
    color    = image.color;

    return this;
  }
  
    //разрезаем узел на 2
    
  child [0] = new Node;
  child [1] = new Node;
  
    //выбор границы разреза
    
  size_t free_width  = rect.width - image.width,
         free_height = rect.height - image.height;
         
  if (free_width > free_height)
  {
    child [0]->rect = Rect (rect.left, rect.top, image.width, rect.height);
    child [1]->rect = Rect (rect.left + image.width, rect.top, free_width, rect.height);
  }
  else
  {
    child [0]->rect = Rect (rect.left, rect.top, rect.width, image.height);    
    child [1]->rect = Rect (rect.left, rect.top + image.height, rect.width, free_height);
  }
  
    //вставляем картинку в первого ребёнка
    
  return child [0]->Insert (image);
}

void Node::PrintImages ()
{
  if (child [0])
    child [0]->PrintImages ();

  if (image_id)
  {  
    printf ("image %d width=%u height=%u\n", image_id, rect.width, rect.height);
  }
  
  if (child [1])
    child [1]->PrintImages ();  
}

void Node::FillImages (Image& image)
{
  if (child [0])
    child [0]->FillImages (image);

  if (image_id)
  {  
    Color* dst = reinterpret_cast<Color*> (image.Bitmap ()) + rect.top * image.Width () + rect.left;
    Color  src = color;
    
    size_t step = image.Width () - rect.width;
    
    for (size_t height=rect.height; height--; dst += step)
    {
      for (size_t width=rect.width; width--; dst++)
        *dst = src;
    }
  }

  if (child [1])
    child [1]->FillImages (image);    
}

bool image_order (const ImageDesc& image1, const ImageDesc& image2)
{
//  return sqrtf (image1.width * image1.width + image1.height * image1.height) >
//         sqrtf (image2.width * image2.width + image2.height * image2.height);
//  return image1.width * image1.height > image2.width * image2.height;
  if (image1.width > image2.width)
    return true;
    
  if (image1.width < image2.height)
    return false;
    
  return image1.height > image2.height;
}

float get_usage (const Image& image)
{
  size_t               non_zero_pixels = 0,
                       total_pixels    = image.Width () * image.Height () * get_bytes_per_pixel (image.Format ());
  const unsigned char* pixel           = reinterpret_cast<const unsigned char*> (image.Bitmap ());
  
  for (size_t i=total_pixels; i--; pixel++)
    if (*pixel)
      non_zero_pixels++;
      
  return float (non_zero_pixels) / total_pixels;
}

int main ()
{
  try
  {
    srand (0);
    
      //генерация картинок
      
    typedef stl::vector<ImageDesc> ImageArray;
    
    ImageArray images;
    
    images.reserve (IMAGES_COUNT);
    
    for (size_t i=0; i<IMAGES_COUNT; i++)
    {
      ImageDesc image (i, rand () % MAX_IMAGE_WIDTH + 1, rand () % MAX_IMAGE_HEIGHT + 1);
//      ImageDesc image (i, (sinf (i) + 1.0) * MAX_IMAGE_WIDTH, (cosf (i) + 1.0f) * MAX_IMAGE_HEIGHT);
//      ImageDesc image (i, i + 1, i + 1);

      unsigned char* colors = &image.color.red;

      colors [i%3] = unsigned char (rand () % 128 + 127);

      images.push_back (image);
    }  
    
      //сортировка картинок
      
    stl::sort (images.begin (), images.end (), &image_order);

      //заполнение дерева картинками        
      
    size_t bad_inserts;
    Node* root = 0;
    
    size_t large_image_width = START_LARGE_IMAGE_WIDTH,
           large_image_height = START_LARGE_IMAGE_HEIGHT;
           
    clock_t start_time = clock ();
    
    int next_edge = 0;
    size_t iterations_count = 0;
    
    for (;;)
    {
      iterations_count++;
      
        //инициализация корневого узла

      root = new Node;

      root->rect = Rect (0, 0, large_image_width, large_image_height);

      bad_inserts = 0;

      for (size_t i=0; i<images.size (); i++)
      {
        if (!root->Insert (images [i]))
        {
          bad_inserts++;
          break;
    //      printf ("failed image insert %d width=%u height=%u\n", images [i].id, images [i].width, images [i].height);
        }
      }
    
  //    printf ("Bad inserts: %u (%.2f%%)\n", bad_inserts, 100 * float (bad_inserts) / images.size ());    

      if (!bad_inserts)
        break;      

      delete root;

      if (next_edge) large_image_width  *= 2;
      else           large_image_height *= 2;
      
      next_edge = !next_edge;
    }

      //вывод результатов

  //  root->PrintImages ();

      //сохранение картинки
      
    printf ("Create large image (%ux%u)...\n", large_image_width, large_image_height);
      
    Image result (large_image_width, large_image_height, 1, PixelFormat_RGB8);

    memset (result.Bitmap (), 0, large_image_width * large_image_height * sizeof (Color));

    root->FillImages (result);
    
    clock_t end_time = clock ();  

    result.Save (RESULT_IMAGE_NAME);    
    
      //вывод статистики

    printf ("At time:          %.2f seconds\n", float (end_time - start_time) / CLOCKS_PER_SEC);
    printf ("Iterations count: %u\n", iterations_count);
    printf ("Image size:       %ux%u\n", large_image_width, large_image_height);
    printf ("Usage:            %.2f%%\n", get_usage (result) * 100);

    return 0;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}
