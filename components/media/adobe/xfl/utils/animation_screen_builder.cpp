#include <cstdio>

#include <stl/map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <math/vector.h>

#include <common/strlib.h>
#include <common/xml_writer.h>

#include <media/adobe/xfl.h>
#include <media/image.h>

using namespace math;
using namespace common;
using namespace media;
using namespace media::adobe::xfl;

const int LEFT_X        = 0;
const int TOP_Y         = 0;
const int TARGET_WIDTH  = 1024;
const int TARGET_HEIGHT = -768;

const float FADE_TIME = 0.1;

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

const FrameElement& get_first_resource_frame_for_symbol (const Document& document, const Symbol& symbol)
{
  for (Timeline::LayerList::ConstIterator layer_iter = symbol.Timeline ().Layers ().CreateIterator (); layer_iter; ++layer_iter)
    for (Layer::FrameList::ConstIterator frame_iter = layer_iter->Frames ().CreateIterator (); frame_iter; ++frame_iter)
      for (Frame::FrameElementList::ConstIterator element_iter = frame_iter->Elements ().CreateIterator (); element_iter; ++element_iter)
        if (element_iter->Type () == FrameElementType_ResourceInstance)
          return *element_iter;

  throw xtl::format_operation_exception ("get_resource_for_symbol", "Symbol '%s' has no resources", symbol.Name ());
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

stl::string get_base_path (const char* path)
{
  const char* base_path_end = strrchr (path, '/');

  if (!base_path_end)
    return stl::string ();

  return stl::string (path, base_path_end - path + 1);
}

stl::string get_extension (const char* path)
{
  const char* extension_begin = strrchr (path, '.');

  if (!extension_begin)
    return stl::string ();

  return stl::string (extension_begin);
}

typedef stl::map<float, stl::string, stl::less<float> > ActivateSpritesMap;

void save_timeline (const Document& document, const Timeline& timeline)
{
  static const char* METHOD_NAME = "save_timeline";

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

        float begin_time = frame_iter->FirstFrame () / document.FrameRate (),
              duration   = frame_iter->Duration () / document.FrameRate ();

        XmlWriter::Scope sprite_scope (writer, "Sprite");

          //Поиск и верификация картинок
        const Timeline& symbol_timeline = document.Symbols () [element_iter->Name ()].Timeline ();

        if (symbol_timeline.Layers ().Size () > 1)
          xtl::format_operation_exception (METHOD_NAME, "Can't process frame element '%s' of layer '%s', referenced symbol has more than one layer",
                                           element_iter->Name (), layer.Name ());

        const Layer& symbol_layer = ((const ICollection<Layer>&)symbol_timeline.Layers ()) [0];

        size_t bitmaps_count = 0;

        const Frame&        first_symbol_frame = ((const ICollection<Frame>&)symbol_layer.Frames ()) [0];
        const FrameElement& resource_element   = ((const ICollection<FrameElement>&)first_symbol_frame.Elements ()) [0];
        const Resource&     resource           = document.Resources () [resource_element.Name ()];

        Image image (resource.Path ());

        size_t image_width  = image.Width (),
               image_height = image.Height ();

        writer.WriteAttribute ("Active", "false");
        writer.WriteAttribute ("Name", resource.Name ());

        ActivateSpritesMap::iterator activate_iter = activate_sprites_info.find (begin_time);

        if (activate_iter == activate_sprites_info.end ())
          activate_sprites_info.insert_pair (begin_time, common::format ("ActivateSprite('%s');", resource.Name ()));
        else
          activate_iter->second.append (common::format ("ActivateSprite('%s');", resource.Name ()));

        for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
          for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, bitmaps_count++)
          {
            if (symbol_element_iter->Type () != FrameElementType_ResourceInstance)
              throw xtl::format_operation_exception (METHOD_NAME, "Can't process frame element '%s' of layer '%s', referenced symbol elements other than resource instance",
                                                     element_iter->Name (), layer.Name ());
          }

        if (!bitmaps_count)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't process frame element '%s' of layer '%s', referenced symbol elements has no resource instance",
                                                 element_iter->Name (), layer.Name ());

        if (bitmaps_count == 1)
          writer.WriteAttribute ("Material", resource.Name ());
        else
        {
          stl::string resource_base_name = get_base_path (resource.Name ()),
                      resource_extension = get_extension (resource.Path ());

          size_t check_frame = 1;

          for (Layer::FrameList::ConstIterator symbol_frame_iter = symbol_layer.Frames ().CreateIterator (); symbol_frame_iter; ++symbol_frame_iter)
            for (Frame::FrameElementList::ConstIterator symbol_element_iter = symbol_frame_iter->Elements ().CreateIterator (); symbol_element_iter; ++symbol_element_iter, check_frame++)
            {
              stl::string correct_element_name = common::format ("%s/%u%s", resource_base_name.c_str (), check_frame, resource_extension.c_str ());

              if (!xtl::xstrcmp (correct_element_name.c_str (), symbol_element_iter->Name ()))
                throw xtl::format_operation_exception (METHOD_NAME, "Can't process frame element '%s' of layer '%s', referenced symbol elements named not sequentially",
                                                       element_iter->Name (), layer.Name ());

              if (symbol_frame_iter->FirstFrame () != check_frame - 1)
                throw xtl::format_operation_exception (METHOD_NAME, "Can't process frame element '%s' of layer '%s', referenced frame animation has unsupported framerate",
                                                       element_iter->Name (), layer.Name ());
            }

          stl::string material_animation_string = common::format ("%s%%u%s; 1; %u", resource_base_name.c_str (), resource_extension.c_str (), bitmaps_count),
                      fps_string                = common::format ("%f", document.FrameRate ());

          writer.WriteAttribute ("MaterialAnimation", material_animation_string);
          writer.WriteAttribute ("Fps",               fps_string);
          writer.WriteAttribute ("RepeatMode",        "clamp_to_end");
        }

        stl::string pivot_value_string = common::format ("%f;%f;0", (element_iter->TransformationPoint ().x + resource_element.TransformationPoint ().x) / image_width - 0.5f,
                                                                    (element_iter->TransformationPoint ().y + resource_element.TransformationPoint ().y) / image_height - 0.5f);

        if (bitmaps_count == 1)
          writer.WriteAttribute ("PivotPosition", pivot_value_string);

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

          if (x_keyframes.Size () != y_keyframes.Size ())
            throw xtl::format_operation_exception (METHOD_NAME, "Can't save motion track for frame element '%s' of layer '%s', x and y motion has different count of keyframes",
                                                   element_iter->Name (), layer.Name ());

          vec2f base_point (TARGET_WIDTH / 2.f + (element_iter->Translation ().x + resource_element.Translation ().x) / TARGET_WIDTH,
                            TARGET_HEIGHT / 2.f + (element_iter->Translation ().y + resource_element.Translation ().y) / TARGET_HEIGHT);

          if (!motion_x_track.Enabled () || !motion_y_track.Enabled ())
          {
            write_track_key (writer, 0, base_point);
          }
          else
          {
            for (size_t j = 0, count = x_keyframes.Size (); j < count; j++)
            {
              const PropertyAnimationKeyframe &x_keyframe = x_keyframes [j],
                                              &y_keyframe = y_keyframes [j];

              write_track_key (writer, x_keyframe.time, base_point + vec2f (x_keyframe.value * x_scale, y_keyframe.value * y_scale));
            }
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

          if (x_keyframes.Size () != y_keyframes.Size ())
            throw xtl::format_operation_exception (METHOD_NAME, "Can't save scale track for frame element '%s' of layer '%s', x and y scale has different count of keyframes",
                                                   element_iter->Name (), layer.Name ());

          if (scale_x_track.Enabled () && scale_y_track.Enabled ())
          {
            for (size_t j = 0, count = x_keyframes.Size (); j < count; j++)
            {
              const PropertyAnimationKeyframe &x_keyframe = x_keyframes [j],
                                              &y_keyframe = y_keyframes [j];

              write_track_key (writer, x_keyframe.time, vec2f (x_keyframe.value / 100.f, y_keyframe.value / 100.f));
            }
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

    save_materials (document);

    for (Document::TimelineList::ConstIterator iter = document.Timelines ().CreateIterator (); iter; ++iter)
      save_timeline (document, *iter);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
