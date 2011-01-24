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

void file_read (InputFile& file, void* data, size_t size)
{
  if (file.Read (data, size) != size)
    throw xtl::format_operation_exception ("media::animation::BinAnimationLibraryLoader", "Can't read data from file at %u, file size is %u",
                                           file.Tell (), file.Size ());
}

void file_read (InputFile& file, xtl::uninitialized_storage<char>& str)
{
  unsigned int str_length;

  file_read (file, &str_length, sizeof (str_length));

  str.resize (str_length + 1);

  file_read (file, str.data (), sizeof (char) * str_length);

  str.data ()[str_length] = '\0';
}

/*
    Загрузчик анимаций в бинарном формате
*/

class BinAnimationLibraryLoader
{
  private:
    //чтение трека событий
    void ReadEvents (EventTrack track)
    {
      unsigned int events_count;

      file_read (input_file, &events_count, sizeof (events_count));

      for (size_t i = 0; i < events_count; i++)
      {
        xtl::uninitialized_storage<char> event;

        file_read (input_file, event);

        float delay, period;

        file_read (input_file, &delay, sizeof (delay));
        file_read (input_file, &period, sizeof (period));

        track.AddEvent (delay, period, event.data ());
      }
    }

    //разбор канала анимации
    template <class T> void ReadSpecificKeyInfo (math::spline_tcb_key<T>& key)
    {
      file_read (input_file, &key.tension,    sizeof (key.tension));
      file_read (input_file, &key.continuity, sizeof (key.continuity));
      file_read (input_file, &key.bias,       sizeof (key.bias));
    }

    template <class T> void ReadSpecificKeyInfo (math::spline_bezier_key<T>& key)
    {
      file_read (input_file, &key.inner_value, sizeof (key.inner_value));
      file_read (input_file, &key.outer_value, sizeof (key.outer_value));
    }

    template <class T> void ReadSpecificKeyInfo (math::spline_step_key<T>& key)
    {
    }

    template <class T> void ReadSpecificKeyInfo (math::spline_linear_key<T>& key)
    {
    }

    template <class T> void ReadSplineKey (T& spline)
    {
        typename T::key_type key;

        file_read (input_file, &key.time, sizeof (key.time));
        file_read (input_file, &key.value, sizeof (key.value));

        ReadSpecificKeyInfo (key);

        spline.add_key (key);
    }

    template <class T> void ReadSpline (Channel& parent)
    {
      T spline;

      unsigned int keys_count;

      file_read (input_file, &keys_count, sizeof (keys_count));

      spline.reserve (keys_count);

      for (size_t i = 0; i < keys_count; i++)
        ReadSplineKey<T> (spline);

      parent.SetTrack (spline);
    }

    void ReadAnimationChannel (Animation& parent)
    {
      xtl::uninitialized_storage<char> parameter_name, target_name, track_type;

      file_read (input_file, parameter_name);
      file_read (input_file, target_name);
      file_read (input_file, track_type);

      Channel channel;

      channel.SetParameterName (parameter_name.data ());

      if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_tcb_key<float>>"))
        ReadSpline<math::tcb_splinef> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_tcb_key<vec2f>>"))
        ReadSpline<math::tcb_spline2f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_tcb_key<vec3f>>"))
        ReadSpline<math::tcb_spline3f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_tcb_key<vec4f>>"))
        ReadSpline<math::tcb_spline4f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_bezier_key<float>>"))
        ReadSpline<math::bezier_splinef> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_bezier_key<vec2f>>"))
        ReadSpline<math::bezier_spline2f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_bezier_key<vec3f>>"))
        ReadSpline<math::bezier_spline3f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_bezier_key<vec4f>>"))
        ReadSpline<math::bezier_spline4f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_linear_key<float>>"))
        ReadSpline<math::linear_splinef> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_linear_key<vec2f>>"))
        ReadSpline<math::linear_spline2f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_linear_key<vec3f>>"))
        ReadSpline<math::linear_spline3f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_linear_key<vec4f>>"))
        ReadSpline<math::linear_spline4f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_linear_key<mat4f>>"))
        ReadSpline<math::linear_spline_mat4f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_step_key<float>>"))
        ReadSpline<math::step_splinef> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_step_key<vec2f>>"))
        ReadSpline<math::step_spline2f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_step_key<vec3f>>"))
        ReadSpline<math::step_spline3f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_step_key<vec4f>>"))
        ReadSpline<math::step_spline4f> (channel);
      else if (!xtl::xstrcmp (track_type.data (), "basic_spline<spline_step_key<mat4f>>"))
        ReadSpline<math::step_spline_mat4f> (channel);
      else
        throw xtl::format_operation_exception ("media::animation::BinAnimationLibraryLoader::ReadAnimationChannel",
                                               "Unsupported channel track type '%s'", track_type.data ());

      parent.AddChannel (target_name.data (), channel);
    }

    void ReadChannels (Animation& animation)
    {
      unsigned int channels_count;

      file_read (input_file, &channels_count, sizeof (channels_count));

      for (size_t i = 0; i < channels_count; i++)
        ReadAnimationChannel (animation);
    }

    //чтение анимации
    void ReadAnimation (AnimationLibrary& library)
    {
      xtl::uninitialized_storage<char> id, name;

      file_read (input_file, id);
      file_read (input_file, name);

      Animation animation;

      animation.Rename (name.data ());

      ReadEvents (animation.Events ());
      ReadChannels (animation);

      library.Attach (id.data (), animation);
    }

  public:
    BinAnimationLibraryLoader (const char* file_name, AnimationLibrary& library)
      : input_file (file_name)
    {
      static const char* METHOD_NAME = "media::animation::BinAnimationLibraryLoader::BinAnimationLibraryLoader";

        //проверка заголовка файла

      char header [4] = {0, 0, 0, 0};

      file_read (input_file, header, sizeof (header));

      if (memcmp (&header, HEADER, 4))
        throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has invalid header '%c%c%c%c', must be '%c%c%c%c'", file_name,
          header [0], header [1], header [2], header [3], HEADER [0], HEADER [1], HEADER [2], HEADER [3]);

      int version;

      file_read (input_file, &version, sizeof (version));

      if (version != VERSION)
        throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has unsupported version %d, supported version - %d", file_name,
                                               version, VERSION);

        //чтение данных

      unsigned int animations_count;

      file_read (input_file, &animations_count, sizeof (animations_count));

      for (size_t i = 0; i < animations_count; i++)
        ReadAnimation (library);
    }
    
  private:
    InputFile input_file; //исходный файл
};

/*
    Автоматическая регистрация компонента
*/

class BinAnimLoaderComponent
{
  public:
    BinAnimLoaderComponent ()
    {
      AnimationLibraryManager::RegisterLoader ("binanim", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, AnimationLibrary& library)
    {
      BinAnimationLibraryLoader (file_name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<BinAnimLoaderComponent> BinAnimLoader ("media.animation.loaders.BinAnimLoader");

}
