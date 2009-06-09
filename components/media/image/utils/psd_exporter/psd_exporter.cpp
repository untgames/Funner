#include <cstdio>
#include <cstdlib>
#include <exception>

#include <libpsd.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/image.h>

#pragma pack (1)

struct bgra_t
{
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char alpha;
};

struct rgba_t
{
  unsigned char red; 
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;  
};

void check_status (psd_status status, const char* source)
{
  if (status == psd_status_done)
    return;
    
  fprintf (stderr, "Function %s return error status %d\n", source, status);
  exit (1);  
}

/*const char* get_layer_type_string (psd_layer_type type)
{
  switch (type)
  {
    case psd_layer_type_normal:                return "psd_layer_type_normal";
    case psd_layer_type_hidden:                return "psd_layer_type_hidden";
    case psd_layer_type_folder:                return "psd_layer_type_folder";
    case psd_layer_type_solid_color:           return "psd_layer_type_solid_color";
    case psd_layer_type_gradient_fill:         return "psd_layer_type_gradient_fill";
    case psd_layer_type_pattern_fill:          return "psd_layer_type_pattern_fill";
    case psd_layer_type_levels:                return "psd_layer_type_levels";
    case psd_layer_type_curves:                return "psd_layer_type_curves";
    case psd_layer_type_brightness_contrast:   return "psd_layer_type_brightness_contrast";
    case psd_layer_type_color_balance:         return "psd_layer_type_color_balance";
    case psd_layer_type_hue_saturation:        return "psd_layer_type_hue_saturation";
    case psd_layer_type_selective_color:       return "psd_layer_type_selective_color";
    case psd_layer_type_threshold:             return "psd_layer_type_threshold";
    case psd_layer_type_invert:                return "psd_layer_type_invert";
    case psd_layer_type_posterize:             return "psd_layer_type_posterize";
    case psd_layer_type_channel_mixer:         return "psd_layer_type_channel_mixer";
    case psd_layer_type_gradient_map:          return "psd_layer_type_gradient_map";
    case psd_layer_type_photo_filter:          return "psd_layer_type_photo_filter";
    default:                                   return "unknown";    
  }
} */

void convert_image_data (size_t width, size_t height, const psd_argb_color* src_image, void* dst_image)
{
  for (size_t i=0; i<height; i++)
  {
    const bgra_t* src = (const bgra_t*)src_image + i * width;
    rgba_t*       dst = (rgba_t*)dst_image + width * (height - 1) - i * width;

    for (size_t j=0; j<width; j++, src++, dst++)
    {
      dst->red   = src->red;
      dst->green = src->green;
      dst->blue  = src->blue;
      dst->alpha = src->alpha;
    }    
  }
}

int main (int argc, char* argv [])
{
  if (argc < 2)
  {
    fprintf (stderr, "psd-exporter <image.psd> [<out_description.xml>] [<layers_dir>]\n");
    return 1;
  }
  
  try
  {
    const char*  src_file_name      = argv [1];
    const char*  desc_file_name_opt = argc > 2 ? argv [2] : "";
    const char*  dst_dir_name_opt   = argc > 3 ? argv [3] : "";
    psd_context* context            = 0;
    
    check_status (psd_image_load (&context, (psd_char*)src_file_name), "::psd_image_load");
    
    stl::string dst_dir_name   = *dst_dir_name_opt ? stl::string (dst_dir_name_opt) : common::notdir (common::basename (src_file_name));
    stl::string desc_file_name = *desc_file_name_opt ? stl::string (desc_file_name_opt) : (common::notdir (common::basename (src_file_name)) + ".xlayermap");

    if (!common::FileSystem::IsFileExist (dst_dir_name.c_str ()))
      common::FileSystem::Mkdir (dst_dir_name.c_str ());

    common::XmlWriter xml_writer (desc_file_name.c_str ());
    
    common::XmlWriter::Scope image_list (xml_writer, "LayerMap");

    xml_writer.WriteAttribute ("TotalWidth",  context->width);
    xml_writer.WriteAttribute ("TotalHeight", context->height);
    
    size_t image_index = 1;

    for (int i=0; i<context->layer_count; i++)
    {
      psd_layer_record& layer = context->layer_records [i];
      stl::string       name ((char*)layer.layer_name);
      stl::string       dst_image_name = common::format ("%s/image%03u.png", dst_dir_name.c_str (), image_index);

      if (!strncmp (name.c_str (), "</", 2))
        continue;        
      
      switch (layer.layer_type)
      {
        case psd_layer_type_normal:
          break;
        default:
          continue;
      }
      
      image_index++;

      common::XmlWriter::Scope layer_scope (xml_writer, "Layer");

      xml_writer.WriteAttribute ("Name",   name.c_str ());
      xml_writer.WriteAttribute ("Image",  dst_image_name.c_str ());
      xml_writer.WriteAttribute ("Left",   layer.left);
      xml_writer.WriteAttribute ("Top",    layer.top);
      xml_writer.WriteAttribute ("Width",  layer.width);
      xml_writer.WriteAttribute ("Height", layer.height);      
      
      printf ("Save layer image '%s'...\n", dst_image_name.c_str ());

      media::Image image (layer.width, layer.height, 1, media::PixelFormat_RGBA8, 0);

      convert_image_data (layer.width, layer.height, layer.image_data, image.Bitmap ()); 

      image.Save (dst_image_name.c_str ());
    }

    psd_image_free (context);
  }
  catch (std::exception& e)
  {
    fprintf (stderr, "%s\n", e.what ());
    return 1;
  }

  return 0;
}
