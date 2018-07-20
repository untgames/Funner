#include "shared.h"

using namespace common;
using namespace media::animation;

namespace components
{

namespace xanim_saver
{

/*
    Константы
*/

const char* FLOAT_FORMAT = ".000"; //количество знаков после запятой при выводе вещественных чисел

/*
    Класс, сохраняющий анимации в Xml-формате
*/

class XmlAnimationLibrarySaver
{
  private:
    //сохранение трека событий
    void SaveEventTrack (const EventTrack& track)
    {
      if (!track.Size ())
        return;

      XmlWriter::Scope scope (writer, "event_track");

      for (unsigned int i = 0, count = track.Size (); i < count; i++)
      {
        XmlWriter::Scope scope (writer, "event");

        writer.WriteAttribute ("event", track.GetEvent (i));
        writer.WriteAttribute ("delay", track.GetDelay (i), FLOAT_FORMAT);
        writer.WriteAttribute ("period", track.GetPeriod (i), FLOAT_FORMAT);
      }
    }

    //сохранение канала анимации
    template <class T> void SaveSpecificKeyInfo (const math::spline_tcb_key<T>& key)
    {
      writer.WriteAttribute ("tension", key.tension, FLOAT_FORMAT);
      writer.WriteAttribute ("continuity", key.continuity, FLOAT_FORMAT);
      writer.WriteAttribute ("bias", key.bias, FLOAT_FORMAT);
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_bezier_key<T>& key)
    {
      writer.WriteAttribute ("inner_value", key.inner_value, FLOAT_FORMAT);
      writer.WriteAttribute ("outer_value", key.outer_value, FLOAT_FORMAT);
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_step_key<T>& key)
    {
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_linear_key<T>& key)
    {
    }

    template <class T> void SaveSpline (const T* spline)
    {
      if (!spline)
        throw xtl::make_null_argument_exception ("media::animation::XmlAnimationLibrarySaver::SaveSpline", "spline");

      for (size_t i = 0, count = spline->keys_count (); i < count; i++)
      {
        XmlWriter::Scope scope (writer, "key");

        const typename T::key_type& key = spline->get_key (i);

        writer.WriteAttribute ("time", key.time, FLOAT_FORMAT);
        writer.WriteAttribute ("value", key.value, FLOAT_FORMAT);

        SaveSpecificKeyInfo (key);
      }
    }

    void SaveAnimationChannel (const Channel& channel, const char* target_name)
    {
      XmlWriter::Scope scope (writer, "channel");

      writer.WriteAttribute ("target_name", target_name);
      writer.WriteAttribute ("parameter_name", channel.ParameterName ());

      const std::type_info& track_type = channel.TrackType ();

      if (track_type == typeid (math::tcb_splinef))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_tcb_key<float>>");

        SaveSpline (channel.Track<math::tcb_splinef> ());
      }
      else if (track_type == typeid (math::tcb_spline2f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_tcb_key<vec2f>>");

        SaveSpline (channel.Track<math::tcb_spline2f> ());
      }
      else if (track_type == typeid (math::tcb_spline3f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_tcb_key<vec3f>>");

        SaveSpline (channel.Track<math::tcb_spline3f> ());
      }
      else if (track_type == typeid (math::tcb_spline4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_tcb_key<vec4f>>");

        SaveSpline (channel.Track<math::tcb_spline4f> ());
      }
      else if (track_type == typeid (math::bezier_splinef))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_bezier_key<float>>");

        SaveSpline (channel.Track<math::bezier_splinef> ());
      }
      else if (track_type == typeid (math::bezier_spline2f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_bezier_key<vec2f>>");

        SaveSpline (channel.Track<math::bezier_spline2f> ());
      }
      else if (track_type == typeid (math::bezier_spline3f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_bezier_key<vec3f>>");

        SaveSpline (channel.Track<math::bezier_spline3f> ());
      }
      else if (track_type == typeid (math::bezier_spline4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_bezier_key<vec4f>>");

        SaveSpline (channel.Track<math::bezier_spline4f> ());
      }
      else if (track_type == typeid (math::linear_splinef))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_linear_key<float>>");

        SaveSpline (channel.Track<math::linear_splinef> ());
      }
      else if (track_type == typeid (math::linear_spline2f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_linear_key<vec2f>>");

        SaveSpline (channel.Track<math::linear_spline2f> ());
      }
      else if (track_type == typeid (math::linear_spline3f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_linear_key<vec3f>>");

        SaveSpline (channel.Track<math::linear_spline3f> ());
      }
      else if (track_type == typeid (math::linear_spline4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_linear_key<vec4f>>");

        SaveSpline (channel.Track<math::linear_spline4f> ());
      }
      else if (track_type == typeid (math::linear_spline_mat4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_linear_key<mat4f>>");

        SaveSpline (channel.Track<math::linear_spline_mat4f> ());
      }
      else if (track_type == typeid (math::step_splinef))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<float>>");

        SaveSpline (channel.Track<math::step_splinef> ());
      }
      else if (track_type == typeid (math::step_spline2f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<vec2f>>");

        SaveSpline (channel.Track<math::step_spline2f> ());
      }
      else if (track_type == typeid (math::step_spline3f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<vec3f>>");

        SaveSpline (channel.Track<math::step_spline3f> ());
      }
      else if (track_type == typeid (math::step_spline4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<vec4f>>");

        SaveSpline (channel.Track<math::step_spline4f> ());
      }
      else if (track_type == typeid (math::step_spline_mat4f))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<mat4f>>");

        SaveSpline (channel.Track<math::step_spline_mat4f> ());
      }
      else if (track_type == typeid (math::step_spline_quatf))
      {
        writer.WriteAttribute ("track_type", "basic_spline<spline_step_key<quatf>>");

        SaveSpline (channel.Track<math::step_spline_quatf> ());
      }
      else
        throw xtl::format_operation_exception ("media::animation::XmlAnimationLibrarySaver::SaveAnimationChannel",
                                               "Unsupported channel track type '%s'", track_type.name ());
    }

    //сохранение анимации
    void SaveAnimation (const Animation& animation, const char* id)
    {
      XmlWriter::Scope scope (writer, "animation");

      writer.WriteAttribute ("id", id);
      writer.WriteAttribute ("name", animation.Name ());

      SaveEventTrack (animation.Events ());

      if (animation.TargetsCount ())
      {
        XmlWriter::Scope scope (writer, "channels");

        for (unsigned int i = 0, targets_count = animation.TargetsCount (); i < targets_count; i++)
        {
          const char* target_name = animation.TargetName (i);

          for (unsigned int j = 0, channels_count = animation.ChannelsCount (i); j < channels_count; j++)
            SaveAnimationChannel (animation.Channel (i, j), target_name);
        }
      }
    }
    
  public:
      //конструктор
    XmlAnimationLibrarySaver (const char* file_name, const AnimationLibrary& library) : writer (file_name)
    {
      XmlWriter::Scope library_scope (writer, "animation_library");

      for (AnimationLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
        SaveAnimation (*iter, library.ItemId (iter));
    }

  private:
    XmlWriter writer; //сериализатор Xml
};

/*
    Автоматическая регистрация компонента
*/

class XAnimSaverComponent
{
  public:
    XAnimSaverComponent ()
    {
      AnimationLibraryManager::RegisterSaver ("xanim", &SaveLibrary);
    }

  private:
    static void SaveLibrary (const char* file_name, const AnimationLibrary& library)
    {
      XmlAnimationLibrarySaver (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<XAnimSaverComponent> XAnimSaver ("media.animation.savers.XAnimSaver");

}

}

}
