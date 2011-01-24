#include "shared.h"

using namespace common;
using namespace media::animation;

/*
 * array <> format: unsigned int elements_count; elements
 *
 * File format: int header; int version; data
 *
 * data format: array<animation>
 *
 * animation format: array<char> id; array<char> name; array<event> event_track; array<channel> channel
 *
 * event format: array<char> event; float delay; float period
 * channel format: array<char> parameter_name; array<char> target_name; array<char> track_type; array<channel_key> keys;
 */

namespace
{

/*
    Константы
*/

const char HEADER [4] = {'B', 'A', 'N', 'M'};
const int  VERSION    = 1;

void file_write (OutputFile& file, const void* data, size_t size)
{
  if (file.Write (data, size) != size)
    throw xtl::format_operation_exception ("media::animation::BinAnimationLibrarySaver", "Can't write data to file");
}

void file_write (OutputFile& file, const char* string)
{
  unsigned int string_length = xtl::xstrlen (string);

  file_write (file, &string_length, sizeof (string_length));
  file_write (file, string, string_length);
}

/*
    Класс, сохраняющий анимации в бинарном формате
*/

class BinAnimationLibrarySaver
{
  private:
    //сохранение трека событий
    void SaveEventTrack (const EventTrack& track)
    {
      unsigned int  events_count = track.Size ();

      file_write (result_file, &events_count, sizeof (events_count));

      for (size_t i = 0; i < events_count; i++)
      {
        file_write (result_file, track.GetEvent (i));

        float delay  = track.GetDelay (i),
              period = track.GetPeriod (i);

        file_write (result_file, &delay, sizeof (delay));
        file_write (result_file, &period, sizeof (period));
      }
    }

    //сохранение канала анимации
    template <class T> void SaveSpecificKeyInfo (const math::spline_tcb_key<T>& key)
    {
      typedef typename math::spline_tcb_key<T>::scalar_type scalar_type;

      scalar_type tension    = key.tension,
                  continuity = key.continuity,
                  bias       = key.bias;

      file_write (result_file, &tension, sizeof (tension));
      file_write (result_file, &continuity, sizeof (continuity));
      file_write (result_file, &bias, sizeof (bias));
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_bezier_key<T>& key)
    {
      typedef typename math::spline_bezier_key<T>::value_type value_type;

      value_type inner_value = key.inner_value,
                 outer_value = key.outer_value;

      file_write (result_file, &inner_value, sizeof (inner_value));
      file_write (result_file, &outer_value, sizeof (outer_value));
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_step_key<T>& key)
    {
    }

    template <class T> void SaveSpecificKeyInfo (const math::spline_linear_key<T>& key)
    {
    }

    template <class T> void SaveSpline (const char* spline_id, const T* spline)
    {
      if (!spline)
        throw xtl::make_null_argument_exception ("media::animation::BinAnimationLibrarySaver::SaveSpline", "spline");

      file_write (result_file, spline_id);

      unsigned int keys_count = spline->keys_count ();

      file_write (result_file, &keys_count, sizeof (keys_count));

      for (size_t i = 0; i < keys_count; i++)
      {
        const typename T::key_type& key = spline->get_key (i);

        typename T::time_type  time  = key.time;
        typename T::value_type value = key.value;

        file_write (result_file, &time, sizeof (time));
        file_write (result_file, &value, sizeof (value));

        SaveSpecificKeyInfo (key);
      }
    }

    void SaveAnimationChannel (const Channel& channel, const char* target_name)
    {
      file_write (result_file, channel.ParameterName ());
      file_write (result_file, target_name);

      const std::type_info& track_type = channel.TrackType ();

      if (track_type == typeid (math::tcb_splinef))
        SaveSpline ("basic_spline<spline_tcb_key<float>>", channel.Track<math::tcb_splinef> ());
      else if (track_type == typeid (math::tcb_spline2f))
        SaveSpline ("basic_spline<spline_tcb_key<vec2f>>", channel.Track<math::tcb_spline2f> ());
      else if (track_type == typeid (math::tcb_spline3f))
        SaveSpline ("basic_spline<spline_tcb_key<vec3f>>", channel.Track<math::tcb_spline3f> ());
      else if (track_type == typeid (math::tcb_spline4f))
        SaveSpline ("basic_spline<spline_tcb_key<vec4f>>", channel.Track<math::tcb_spline4f> ());
      else if (track_type == typeid (math::bezier_splinef))
        SaveSpline ("basic_spline<spline_bezier_key<float>>", channel.Track<math::bezier_splinef> ());
      else if (track_type == typeid (math::bezier_spline2f))
        SaveSpline ("basic_spline<spline_bezier_key<vec2f>>", channel.Track<math::bezier_spline2f> ());
      else if (track_type == typeid (math::bezier_spline3f))
        SaveSpline ("basic_spline<spline_bezier_key<vec3f>>", channel.Track<math::bezier_spline3f> ());
      else if (track_type == typeid (math::bezier_spline4f))
        SaveSpline ("basic_spline<spline_bezier_key<vec4f>>", channel.Track<math::bezier_spline4f> ());
      else if (track_type == typeid (math::linear_splinef))
        SaveSpline ("basic_spline<spline_linear_key<float>>", channel.Track<math::linear_splinef> ());
      else if (track_type == typeid (math::linear_spline2f))
        SaveSpline ("basic_spline<spline_linear_key<vec2f>>", channel.Track<math::linear_spline2f> ());
      else if (track_type == typeid (math::linear_spline3f))
        SaveSpline ("basic_spline<spline_linear_key<vec3f>>", channel.Track<math::linear_spline3f> ());
      else if (track_type == typeid (math::linear_spline4f))
        SaveSpline ("basic_spline<spline_linear_key<vec4f>>", channel.Track<math::linear_spline4f> ());
      else if (track_type == typeid (math::linear_spline_mat4f))
        SaveSpline ("basic_spline<spline_linear_key<mat4f>>", channel.Track<math::linear_spline_mat4f> ());
      else if (track_type == typeid (math::step_splinef))
        SaveSpline ("basic_spline<spline_step_key<float>>", channel.Track<math::step_splinef> ());
      else if (track_type == typeid (math::step_spline2f))
        SaveSpline ("basic_spline<spline_step_key<vec2f>>", channel.Track<math::step_spline2f> ());
      else if (track_type == typeid (math::step_spline3f))
        SaveSpline ("basic_spline<spline_step_key<vec3f>>", channel.Track<math::step_spline3f> ());
      else if (track_type == typeid (math::step_spline4f))
        SaveSpline ("basic_spline<spline_step_key<vec4f>>", channel.Track<math::step_spline4f> ());
      else if (track_type == typeid (math::step_spline_mat4f))
        SaveSpline ("basic_spline<spline_step_key<mat4f>>", channel.Track<math::step_spline_mat4f> ());
      else
        throw xtl::format_operation_exception ("media::animation::BinAnimationLibrarySaver::SaveAnimationChannel",
                                               "Unsupported channel track type '%s'", track_type.name ());
    }

    //сохранение анимации
    void SaveAnimation (const Animation& animation, const char* id)
    {
      file_write (result_file, id);
      file_write (result_file, animation.Name ());

      SaveEventTrack (animation.Events ());

      unsigned int channels_count = 0;

      for (size_t i = 0, targets_count = animation.TargetsCount (); i < targets_count; i++)
        channels_count += animation.ChannelsCount (i);

      file_write (result_file, &channels_count, sizeof (channels_count));

      for (size_t i = 0, targets_count = animation.TargetsCount (); i < targets_count; i++)
      {
        const char* target_name = animation.TargetName (i);

        for (size_t j = 0, channels_count = animation.ChannelsCount (i); j < channels_count; j++)
          SaveAnimationChannel (animation.Channel (i, j), target_name);
      }
    }

    void SaveHeader ()
    {
      file_write (result_file, &HEADER, sizeof (HEADER));
      file_write (result_file, &VERSION, sizeof (VERSION));
    }
    
  public:
      //конструктор
    BinAnimationLibrarySaver (const char* file_name, const AnimationLibrary& library)
      : result_file (file_name)
    {
      SaveHeader ();

      unsigned int animations_count = library.Size ();

      file_write (result_file, &animations_count, sizeof (animations_count));

      for (AnimationLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
        SaveAnimation (*iter, library.ItemId (iter));
    }

  private:
    OutputFile result_file; //результирующий файл
};

/*
    Автоматическая регистрация компонента
*/

class BinAnimSaverComponent
{
  public:
    BinAnimSaverComponent ()
    {
      AnimationLibraryManager::RegisterSaver ("binanim", &SaveLibrary);
    }

  private:
    static void SaveLibrary (const char* file_name, const AnimationLibrary& library)
    {
      BinAnimationLibrarySaver (file_name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<BinAnimSaverComponent> BinAnimSaver ("media.animation.savers.BinAnimSaver");

}
