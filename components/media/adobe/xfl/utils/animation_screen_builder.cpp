#include <cstdio>

#include <stl/map>
#include <stl/hash_set>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <math/vector.h>

#include <common/file.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/adobe/xfl.h>
#include <media/collection.h>
#include <media/image.h>

using namespace math;
using namespace common;
using namespace media;
using namespace media::adobe::xfl;

const char* CROPPED_ANIMATIONS_DIR = "cropped_animations";

const int LEFT_X        = 0;
const int TOP_Y         = 0;
const int TARGET_WIDTH  = 1024;
const int TARGET_HEIGHT = -768;

const size_t CROP_ALPHA = 1;
const size_t RESIZE_WIDTH = 640;
const size_t RESIZE_HEIGHT = 480;

const bool FORCE_ANIMATION_PIVOT = false;

const float EPSILON   = 0.001;
const float FADE_TIME = 0.1;

//прямоугольная область
struct Rect
{
  int    x, y;
  size_t width, height;
};

struct Vec2fKeyframe
{
  float time;
  vec2f value;
};

struct rgba_t
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
};

typedef stl::hash_set<const char*>                                 UsedResourcesSet;
typedef stl::multimap<float, stl::string, stl::less<float> >       ActivateSpritesMap;
typedef CollectionImpl<Vec2fKeyframe, ICollection<Vec2fKeyframe> > Vec2fKeyframes;

void save_materials (const Document& document)
{
  stl::string xmtl_name = common::format ("%s.xmtl", document.Name ());

  XmlWriter writer (xmtl_name.c_str ());

  XmlWriter::Scope scope (writer, "material_library");

  for (Document::ResourceList::ConstIterator iter = document.Resources ().CreateIterator (); iter; ++iter)
  {
    XmlWriter::Scope material_scope (writer, "material");

    writer.WriteAttribute ("id", iter->Name ());

    XmlWriter::Scope sprite_profile (writer, "sprite_profile");

    writer.WriteAttribute ("image", iter->Path ());
    writer.WriteAttribute ("blend_mode", "translucent");
  }
}

void write_track_key (XmlWriter& writer, float time, const char* value, const char* value_attribute = "Value")
{
  XmlWriter::Scope key_scope (writer, "Key");

  writer.WriteAttribute ("Time",  time);
  writer.WriteAttribute (value_attribute, value);
}

void write_track_key (XmlWriter& writer, float time, vec2f value)
{
  stl::string value_string = common::format ("%f;%f", value.x, value.y);

  write_track_key (writer, time, value_string.c_str ());
}

void write_track_key (XmlWriter& writer, float time, float value)
{
  stl::string value_string = common::format ("%f", value);

  write_track_key (writer, time, value_string.c_str ());
}

void compose_vec2f_keyframes (const PropertyAnimation::KeyframeList& x_keyframes, const PropertyAnimation::KeyframeList& y_keyframes, Vec2fKeyframes& keyframes)
{
  keyframes.Reserve (stl::max (x_keyframes.Size (), y_keyframes.Size ()));

  size_t x_keyframes_count        = x_keyframes.Size (),
         y_keyframes_count        = y_keyframes.Size (),
         current_x_keyframe_index = 0,
         current_y_keyframe_index = 0;

  PropertyAnimation::KeyframeList::ConstIterator current_x_keyframe = x_keyframes.CreateIterator (),
                                                 current_y_keyframe = y_keyframes.CreateIterator ();

  float last_exported_time = -1;

  while (current_x_keyframe_index < x_keyframes_count || current_y_keyframe_index < y_keyframes_count)
  {
    Vec2fKeyframe new_keyframe;

    if ((current_x_keyframe_index >= x_keyframes_count) || (current_x_keyframe->time > current_y_keyframe->time && current_y_keyframe_index < y_keyframes_count))
    {
      if (current_y_keyframe->time > last_exported_time + EPSILON)
      {
        last_exported_time = current_y_keyframe->time;

        new_keyframe.time = current_y_keyframe->time;

        new_keyframe.value.y = current_y_keyframe->value;

        if (!current_x_keyframe_index || (current_x_keyframe_index >= x_keyframes_count))
          new_keyframe.value.x = current_x_keyframe->value;
        else
        {
          const PropertyAnimationKeyframe& previous_x_keyframe = x_keyframes [current_x_keyframe_index - 1];

          new_keyframe.value.x = (current_x_keyframe->value - previous_x_keyframe.value) / (current_x_keyframe->time - previous_x_keyframe.time) * (current_y_keyframe->time - previous_x_keyframe.time);
        }

        keyframes.Add (new_keyframe);
      }

      current_y_keyframe_index++;

      if (current_y_keyframe_index < y_keyframes_count)
        ++current_y_keyframe;
    }
    else
    {
      if (current_x_keyframe->time > last_exported_time + EPSILON)
      {
        last_exported_time = current_x_keyframe->time;

        new_keyframe.time = current_x_keyframe->time;

        new_keyframe.value.x = current_x_keyframe->value;

        if (!current_y_keyframe_index || (current_y_keyframe_index >= y_keyframes_count))
          new_keyframe.value.y = current_y_keyframe->value;
        else
        {
          const PropertyAnimationKeyframe& previous_y_keyframe = y_keyframes [current_y_keyframe_index - 1];

          new_keyframe.value.y = (current_y_keyframe->value - previous_y_keyframe.value) / (current_y_keyframe->time - previous_y_keyframe.time) * (current_x_keyframe->time - previous_y_keyframe.time);
        }

        keyframes.Add (new_keyframe);
      }

      current_x_keyframe_index++;

      if (current_x_keyframe_index < x_keyframes_count)
        ++current_x_keyframe;
    }
  }
}

//обрезание
void crop_by_alpha (size_t width, size_t height, const rgba_t* image, size_t crop_alpha, Rect& cropped_rect)
{
  int  min_x = 0, min_y = 0, max_x = 0, max_y = 0;
  bool first_point_found = false;

  for (int y = 0; y < (int)height; y++)
  {
    const rgba_t* data = image + y * width;

    for (int x = 0; x < (int)width; x++, data++)
    {
      if (data->alpha < crop_alpha)
        continue;

      if (!first_point_found)
      {
        min_x             = max_x = x;
        min_y             = max_y = y;
        first_point_found = true;

        continue;
      }

      if (x < min_x) min_x = x;
      if (y < min_y) min_y = y;
      if (x > max_x) max_x = x;
      if (y > max_y) max_y = y;
    }
  }

  if (!first_point_found)
  {
    cropped_rect.x      = 0;
    cropped_rect.y      = 0;
    cropped_rect.width  = 0;
    cropped_rect.height = 0;

    return;
  }

  if (!cropped_rect.width || !cropped_rect.height)
  {
    cropped_rect.x      = min_x;
    cropped_rect.y      = min_y;
    cropped_rect.width  = max_x - min_x + 1;
    cropped_rect.height = max_y - min_y + 1;
  }
  else
  {
    if (min_x < cropped_rect.x) cropped_rect.x = min_x;
    if (min_y < cropped_rect.y) cropped_rect.y = min_y;

    if (max_x > (int)(cropped_rect.x + cropped_rect.width))
      cropped_rect.width = max_x - cropped_rect.x + 1;

    if (max_y > (int)(cropped_rect.y + cropped_rect.height))
      cropped_rect.height = max_y - cropped_rect.y + 1;
  }
}

void union_rect (const Rect& rect1, Rect& target_rect)
{
  if (target_rect.width && target_rect.height)
  {
    size_t left_side = stl::max (rect1.x + rect1.width, target_rect.x + target_rect.width),
           top_side  = stl::max (rect1.y + rect1.height, target_rect.y + target_rect.height);

    target_rect.x      = stl::min (rect1.x, target_rect.x);
    target_rect.y      = stl::min (rect1.y, target_rect.y);
    target_rect.width  = left_side - target_rect.x;
    target_rect.height = top_side - target_rect.y;
  }
  else
    target_rect = rect1;
}

void preprocess_symbols (Document& document)
{
  static const char* METHOD_NAME = "preprocess_symbols";

  UsedResourcesSet used_resources;

  if (!FileSystem::IsDir (CROPPED_ANIMATIONS_DIR))
    FileSystem::Mkdir (CROPPED_ANIMATIONS_DIR);

  for (Document::SymbolList::Iterator symbol_iter = document.Symbols ().CreateIterator (); symbol_iter; ++symbol_iter)
  {
    const Timeline& symbol_timeline = symbol_iter->Timeline ();

    if (symbol_timeline.Layers ().Size () > 1)
      xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', it has more than one layer", symbol_iter->Name ());

    Layer&        symbol_layer       = ((ICollection<Layer>&)symbol_timeline.Layers ()) [0];
    Frame&        first_symbol_frame = ((ICollection<Frame>&)symbol_layer.Frames ()) [0];
    FrameElement& resource_element   = ((ICollection<FrameElement>&)first_symbol_frame.Elements ()) [0];
    Resource&     resource           = document.Resources () [resource_element.Name ()];

    size_t bitmaps_count = 0;

    Image image (resource.Path ());

    size_t image_width  = image.Width (),
           image_height = image.Height ();

    Rect crop_rect;

    memset (&crop_rect, 0, sizeof (crop_rect));

    for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
      for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, bitmaps_count++)
      {
        if (symbol_element_iter->Type () != FrameElementType_ResourceInstance)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains elements other than resource instance", symbol_iter->Name ());

        const Resource& frame_resource = document.Resources () [symbol_element_iter->Name ()];

        Image frame (frame_resource.Path ());

        size_t frame_width  = frame.Width (),
               frame_height = frame.Height ();

        if (frame_width != image_width || frame_height != image_height)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains animations with different frame size", symbol_iter->Name ());

        if (frame.Format () != PixelFormat_RGBA8)
          continue;

        Rect frame_crop_rect;

        memset (&frame_crop_rect, 0, sizeof (frame_crop_rect));

        crop_by_alpha (frame_width, frame_height, (rgba_t*)frame.Bitmap (), CROP_ALPHA, frame_crop_rect);
        union_rect    (frame_crop_rect, crop_rect);
      }

    if (!bitmaps_count)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', referenced symbol elements has no resource instance", symbol_iter->Name ());

    if (bitmaps_count == 1)
      used_resources.insert (resource.Name ());
    else if (bitmaps_count > 1)  //анимация
    {
      if (!crop_rect.width || !crop_rect.height)
      {
        printf ("Can't process symbol '%s', referenced frame animation has empty image\n", symbol_iter->Name ());

        Document::SymbolList::Iterator remove_iter = symbol_iter;

        --symbol_iter;

        document.Symbols ().Remove (stl::distance (document.Symbols ().CreateIterator (), remove_iter));

        continue;
      }

      stl::string resource_base_name = common::dir (resource.Name ()),
                  resource_extension = common::suffix (resource.Path ());

      size_t check_frame = 1;

      xtl::uninitialized_storage<char> image_copy_buffer (get_bytes_per_pixel (image.Format ()) * crop_rect.width * crop_rect.height);

      Layer::FrameList &layer_frames = symbol_layer.Frames ();

      for (Layer::FrameList::Iterator symbol_frame_iter = layer_frames.CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
        for (Frame::FrameElementList::Iterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, check_frame++)
        {
          if (symbol_frame_iter->FirstFrame () != check_frame - 1)
          {
            if (check_frame > 1)
            {
              bitmaps_count = check_frame - 2;
              printf ("Can't fully process symbol '%s', referenced frame animation has unsupported framerate\n", symbol_iter->Name ());

              size_t invalid_frame_index = stl::distance (layer_frames.CreateIterator (), symbol_frame_iter);

              ICollection<Resource>& resources = document.Resources ();

              for (size_t remove_frame_index = layer_frames.Size () - 1; remove_frame_index >= invalid_frame_index; remove_frame_index--)
              {
                Frame& remove_frame = layer_frames [remove_frame_index];

                for (Frame::FrameElementList::Iterator remove_element_iter = remove_frame.Elements ().CreateIterator (); remove_element_iter; ++remove_element_iter)
                  for (size_t resource_index = 0, count = resources.Size (); resource_index < count; resource_index++)
                  {
                    if (!xtl::xstrcmp (resources [resource_index].Name (), remove_element_iter->Name ()))
                    {
                      resources.Remove (resource_index);
                      break;
                    }
                  }

                layer_frames.Remove (remove_frame_index);
              }

              symbol_frame_iter.clear ();
              break;
            }
            else
              throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', referenced frame animation has unsupported framerate", symbol_iter->Name ());
          }

          stl::string save_folder_name     = common::format ("%s/%s", CROPPED_ANIMATIONS_DIR, resource_base_name.c_str ()),
                      correct_element_name = common::format ("%s%u%s", save_folder_name.c_str (), check_frame, resource_extension.c_str ());

          if (!FileSystem::IsDir (save_folder_name.c_str ()))
            FileSystem::Mkdir (save_folder_name.c_str ());

          Resource& frame_resource = document.Resources () [symbol_element_iter->Name ()];

          Image frame (frame_resource.Path ());

          if (frame.Format () != PixelFormat_RGBA8)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't process symbol '%s', symbol contains frames with not RGBA8 pixel format", symbol_iter->Name ());

          frame.GetImage (crop_rect.x, crop_rect.y, 0, crop_rect.width, crop_rect.height, 1, PixelFormat_RGBA8, image_copy_buffer.data ());

          Image cropped_frame (crop_rect.width, crop_rect.height, 1, PixelFormat_RGBA8, image_copy_buffer.data ());

          cropped_frame.Save (correct_element_name.c_str ());

          symbol_element_iter->SetName (correct_element_name.c_str ());

          Resource new_resource;

          new_resource.SetName (correct_element_name.c_str ());
          new_resource.SetPath (correct_element_name.c_str ());

          document.Resources ().Add (new_resource);

          used_resources.insert (new_resource.Name ());

            //update transformation and translation
          vec2f current_transformation_point = symbol_element_iter->TransformationPoint (),
                current_translation          = symbol_element_iter->Translation ();

          if (FORCE_ANIMATION_PIVOT)
          {
            symbol_element_iter->SetTransformationPoint (vec2f (image_width / 2.f + current_transformation_point.x - crop_rect.x, image_height / 2.f + current_transformation_point.y - crop_rect.y));
            symbol_element_iter->SetTranslation         (vec2f (current_translation.x + crop_rect.x, current_translation.y + crop_rect.y));
//            symbol_element_iter->SetTransformationPoint (vec2f (image_width / 2.f + current_transformation_point.x - crop_rect.x, image_height / 2.f + current_transformation_point.y - crop_rect.y));
//            symbol_element_iter->SetTranslation         (vec2f (current_translation.x + crop_rect.x, current_translation.y + crop_rect.y));
          }
          else
          {
            symbol_element_iter->SetTransformationPoint (vec2f (current_transformation_point.x - crop_rect.x, current_transformation_point.y - crop_rect.y));
            symbol_element_iter->SetTranslation         (vec2f (current_translation.x + crop_rect.x, current_translation.y + crop_rect.y));
          }
        }
    }
  }

  ICollection<Resource>& resources = document.Resources ();

  for (size_t i = 0; i < resources.Size ();)
  {
    if (used_resources.find (resources [i].Name ()) == used_resources.end ())
      resources.Remove (i);
    else
      i++;
  }
}

void save_timeline (Document& document, const Timeline& timeline)
{
  static const char* METHOD_NAME = "save_timeline";

  preprocess_symbols (document);

  stl::string xml_name = common::format ("%s.xml", timeline.Name ());

  XmlWriter writer (xml_name.c_str ());

  XmlWriter::Scope screen_scope (writer, "AnimationScreenPart");

  int   target_width_abs  = abs (TARGET_WIDTH),
        target_height_abs = abs (TARGET_HEIGHT);
  float x_scale           = (float)TARGET_WIDTH / document.Width (),
        y_scale           = (float)TARGET_HEIGHT / document.Height ();
  vec2f screen_center ((LEFT_X + TARGET_WIDTH) / 2.f, (TOP_Y + TARGET_HEIGHT) / 2.f);

  ActivateSpritesMap activate_sprites_info;

  for (int i = timeline.Layers ().Size () - 1; i >= 0; i--)
  {
    const Layer& layer = ((ICollection<Layer>&)timeline.Layers ()) [i];

    for (Layer::FrameList::ConstIterator frame_iter = layer.Frames ().CreateIterator (); frame_iter; ++frame_iter)
      for (Frame::FrameElementList::ConstIterator element_iter = frame_iter->Elements ().CreateIterator (); element_iter; ++element_iter)
      {
        if (element_iter->Type () != FrameElementType_SymbolInstance)
          continue;

          //Поиск и верификация картинок
        const Symbol* symbol = document.Symbols ().Find (element_iter->Name ());

        if (!symbol)
        {
          printf ("Can't process animation for frame element '%s' of layer '%s', references symbol not found\n", element_iter->Name (), layer.Name ());
          continue;
        }

        float begin_time = frame_iter->FirstFrame () / document.FrameRate (),
              duration   = frame_iter->Duration () / document.FrameRate ();

        XmlWriter::Scope sprite_scope (writer, "Sprite");

        const Timeline&     symbol_timeline    = symbol->Timeline ();
        const Layer&        symbol_layer       = ((const ICollection<Layer>&)symbol_timeline.Layers ()) [0];
        const Frame&        first_symbol_frame = ((const ICollection<Frame>&)symbol_layer.Frames ()) [0];
        const FrameElement& resource_element   = ((const ICollection<FrameElement>&)first_symbol_frame.Elements ()) [0];
        const Resource&     resource           = document.Resources () [resource_element.Name ()];

        size_t bitmaps_count = 0;

        Image image (resource.Path ());

        size_t image_width  = image.Width (),
               image_height = image.Height ();

        writer.WriteAttribute ("Active", "false");
        writer.WriteAttribute ("Name", resource.Name ());

        activate_sprites_info.insert_pair (begin_time, common::format ("ActivateSprite('%s')", resource.Name ()));

        for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
          for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, bitmaps_count++);

        if (bitmaps_count == 1)
          writer.WriteAttribute ("Material", resource.Name ());
        else
        {
          stl::string resource_base_name        = common::dir (resource.Name ()),
                      resource_extension        = common::suffix (resource.Path ()),
                      material_animation_string = common::format ("%s%%u%s; 1; %u", resource_base_name.c_str (), resource_extension.c_str (), bitmaps_count),
                      fps_string                = common::format ("%f", document.FrameRate ());

          writer.WriteAttribute ("MaterialAnimation", material_animation_string);
          writer.WriteAttribute ("Fps",               fps_string);
          writer.WriteAttribute ("RepeatMode",        "clamp_to_end");
        }

        vec2f transformation_point (element_iter->TransformationPoint ().x + resource_element.TransformationPoint ().x,
                                    element_iter->TransformationPoint ().y + resource_element.TransformationPoint ().y);

        stl::string pivot_value_string = common::format ("%f;%f;0", transformation_point.x / image_width - 0.5f,
                                                                    transformation_point.y / image_height - 0.5f);

        writer.WriteAttribute ("PivotPosition", pivot_value_string.c_str());

          //Сохранение треков
        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "size");

          write_track_key (writer, 0, vec2f (image.Width () * target_width_abs / (float)document.Width (), image.Height () * target_height_abs / (float)document.Height ()));
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "alpha");

          const PropertyAnimation&               alpha_track = frame_iter->Animation ().Properties () ["headContainer.Colors.Alpha_Color.Alpha_Amount"];
          const PropertyAnimation::KeyframeList& keyframes   = alpha_track.Keyframes ();

          if (alpha_track.Enabled ())
          {
            for (PropertyAnimation::KeyframeList::ConstIterator keyframe_iter = keyframes.CreateIterator (); keyframe_iter; ++keyframe_iter)
              write_track_key (writer, keyframe_iter->time, keyframe_iter->value / 100.f);
          }
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "position");

          const PropertyAnimation               &motion_x_track = frame_iter->Animation ().Properties () ["headContainer.Basic_Motion.Motion_X"],
                                                &motion_y_track = frame_iter->Animation ().Properties () ["headContainer.Basic_Motion.Motion_Y"];
          const PropertyAnimation::KeyframeList &x_keyframes    = motion_x_track.Keyframes (),
                                                &y_keyframes    = motion_y_track.Keyframes ();

          vec2f base_point ((transformation_point.x + element_iter->Translation ().x + resource_element.Translation ().x) / document.Width () * TARGET_WIDTH,
                            (transformation_point.y + element_iter->Translation ().y + resource_element.Translation ().y) / document.Height () * TARGET_HEIGHT);
//          vec2f base_point (transformation_point.x / document.Width () * TARGET_WIDTH + (element_iter->Translation ().x + resource_element.Translation ().x),
//                            transformation_point.y / document.Height () * TARGET_HEIGHT - (element_iter->Translation ().y + resource_element.Translation ().y));

          if (!motion_x_track.Enabled () || !motion_y_track.Enabled ())
            write_track_key (writer, 0, base_point);
          else
          {
            Vec2fKeyframes keyframes;

            compose_vec2f_keyframes (x_keyframes, y_keyframes, keyframes);

            for (Vec2fKeyframes::ConstIterator iter = keyframes.CreateIterator (); iter; ++iter)
              write_track_key (writer, iter->time, base_point + vec2f (iter->value.x * x_scale, iter->value.y * y_scale));
          }
        }

        //TODO export rotation!

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "scale");

          const PropertyAnimation               &scale_x_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Scale_X"],
                                                &scale_y_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Scale_Y"];
          const PropertyAnimation::KeyframeList &x_keyframes   = scale_x_track.Keyframes (),
                                                &y_keyframes   = scale_y_track.Keyframes ();

          if (scale_x_track.Enabled () && scale_y_track.Enabled ())
          {
            Vec2fKeyframes keyframes;

            compose_vec2f_keyframes (x_keyframes, y_keyframes, keyframes);

            for (Vec2fKeyframes::ConstIterator iter = keyframes.CreateIterator (); iter; ++iter)
              write_track_key (writer, iter->time, vec2f (iter->value.x / 100.f, iter->value.y / 100.f));
          }
        }

        {
          XmlWriter::Scope track_scope (writer, "Track");

          writer.WriteAttribute ("Name", "events");

          stl::string event_string = common::format ("DeactivateSprite ('%s')", resource.Name ());

          write_track_key (writer, duration, event_string.c_str (), "Event");
        }

        {
          const PropertyAnimation               &skew_x_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Skew_X"],
                                                &skew_y_track = frame_iter->Animation ().Properties () ["headContainer.Transformation.Skew_Y"];
          const PropertyAnimation::KeyframeList &x_keyframes  = skew_x_track.Keyframes (),
                                                &y_keyframes  = skew_y_track.Keyframes ();

          if (x_keyframes.Size () != y_keyframes.Size ())
            throw xtl::format_operation_exception (METHOD_NAME, "Invalid skew track for frame element '%s' of layer '%s', x and y skew has different count of keyframes",
                                                   element_iter->Name (), layer.Name ());

          if (skew_x_track.Enabled () && skew_y_track.Enabled ())
          {
            for (size_t j = 0, count = x_keyframes.Size (); j < count; j++)
            {
              const PropertyAnimationKeyframe &x_keyframe = x_keyframes [j],
                                              &y_keyframe = y_keyframes [j];

              if (x_keyframe.value || y_keyframe.value)
                throw xtl::format_operation_exception (METHOD_NAME, "Unsupported animation for frame element '%s' of layer '%s', skew not supported",
                                                   element_iter->Name (), layer.Name ());
            }
          }
        }
      }
  }

  XmlWriter::Scope sprite_scope (writer, "Sprite");

  writer.WriteAttribute ("Name", "_____SpriteManager_____");
  writer.WriteAttribute ("Material", "transparent");

  XmlWriter::Scope track_scope (writer, "Track");

  writer.WriteAttribute ("Name", "events");

  for (ActivateSpritesMap::const_iterator iter = activate_sprites_info.begin (), end = activate_sprites_info.end (); iter != end; ++iter)
    write_track_key (writer, iter->first, iter->second.c_str (), "Event");
}

int main (int argc, char *argv[])
{
  if (argc != 2)
  {
    printf ("Usage: animation_screen_builder xfl_folder_path\n");
    return 1;
  }

  try
  {
    Document document (argv [1]);

    for (Document::TimelineList::ConstIterator iter = document.Timelines ().CreateIterator (); iter; ++iter)
      save_timeline (document, *iter);

    save_materials (document);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
