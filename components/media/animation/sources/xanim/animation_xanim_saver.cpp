#include "shared.h"

using namespace common;
using namespace media::animation;

namespace
{

/*
    Константы
*/

const char* FLOAT_FORMAT = ".###"; //количество знаков после запятой при выводе вещественных чисел

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

      for (size_t i = 0, count = track.Size (); i < count; i++)
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

    void SaveAnimationChannel (const Channel& channel)
    {
      XmlWriter::Scope scope (writer, "channel");

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
      else
        throw xtl::format_operation_exception ("media::animation::XmlAnimationLibrarySaver::SaveAnimationChannel",
                                               "Unsupported channel track type '%s'", track_type.name ());
    }

    //сохранение анимации
    void SaveAnimation (const Animation& animation, const char* id = 0)
    {
      XmlWriter::Scope scope (writer, "animation");

      if (id)
        writer.WriteAttribute ("id", id);

      writer.WriteAttribute ("name", animation.Name ());
      writer.WriteAttribute ("target_name", animation.TargetName ());

      SaveEventTrack (animation.Events ());

      if (animation.ChannelsCount ())
      {
        XmlWriter::Scope scope (writer, "channels");

        for (size_t i = 0, count = animation.ChannelsCount (); i < count; i++)
          SaveAnimationChannel (animation.Channel (i));
      }

      if (animation.SubAnimationsCount ())
      {
        XmlWriter::Scope scope (writer, "animations");

        for (size_t i = 0, count = animation.SubAnimationsCount (); i < count; i++)
          SaveAnimation (animation.SubAnimation (i));
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

}

extern "C"
{

ComponentRegistrator<XAnimSaverComponent> XAnimSaver ("media.animation.savers.XAnimSaver");

}
