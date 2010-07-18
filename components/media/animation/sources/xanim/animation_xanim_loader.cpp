#include "shared.h"

using namespace common;
using namespace media::animation;

namespace
{

/*
     онстанты
*/

const char* LOG_NAME = "media.animation.XmlAnimationLibraryLoader"; //им€ потока протоколировани€

template <unsigned int Size>
void read (Parser::Iterator iter, const char* str, math::vector <float, Size>& value)
{
  StringArray components = common::split (str);

  if (components.Size () != Size)
    raise_parser_exception (*iter, "Invalid vector format");

  for (size_t i = 0; i < Size; i++)
    value [i] = (float)atof (components [i]);
}

template <unsigned int Size>
void read (Parser::Iterator iter, const char* str, math::matrix <float, Size>& value)
{
  StringArray components = common::split (str);

  if (components.Size () != Size * Size)
    raise_parser_exception (*iter, "Invalid matrix format");

  for (size_t i = 0; i < Size; i++)
    for (size_t j = 0; j < Size; j++)
      value [i][j] = (float)atof (components [i * Size + j]);
}

void read (Parser::Iterator iter, const char* str, float& value)
{
  value = (float)atof (str);
}

/*
    «агрузчик анимаций в формате Xml
*/

class XmlAnimationLibraryLoader
{
  private:
    //разбор трека событий
    void ParseEvent (Parser::Iterator event_iter, EventTrack& track)
    {
      const char* event  = get<const char*> (*event_iter, "event");
      float       delay  = get<float> (*event_iter, "delay"),
                  period = get<float> (*event_iter, "period");

      track.AddEvent (delay, period, event);
    }

    //разбор канала анимации
    template <class T> void ParseSpecificKeyInfo (Parser::Iterator key_iter, math::spline_tcb_key<T>& key)
    {
      typedef typename math::spline_tcb_key<T>::scalar_type scalar_type;

      key.tension    = get<scalar_type> (*key_iter, "tension");
      key.continuity = get<scalar_type> (*key_iter, "continuity");
      key.bias       = get<scalar_type> (*key_iter, "bias");
    }

    template <class T> void ParseSpecificKeyInfo (Parser::Iterator key_iter, math::spline_bezier_key<T>& key)
    {
      typedef typename math::spline_bezier_key<T>::value_type value_type;

      const char *inner_value_string = get<const char*> (*key_iter, "inner_value"),
                 *outer_value_string = get<const char*> (*key_iter, "outer_value");

      read (key_iter, inner_value_string, key.inner_value);
      read (key_iter, outer_value_string, key.outer_value);
    }

    template <class T> void ParseSpecificKeyInfo (Parser::Iterator key_iter, math::spline_step_key<T>& key)
    {
    }

    template <class T> void ParseSpecificKeyInfo (Parser::Iterator key_iter, math::spline_linear_key<T>& key)
    {
    }

    template <class T> void ParseSplineKey (Parser::Iterator key_iter, T& spline)
    {
        typename T::time_type time         = get<typename T::time_type> (*key_iter, "time");
        const char*           value_string = get<const char*> (*key_iter, "value");

        typename T::value_type value;

        read (key_iter, value_string, value);

        typename T::key_type key (time, value);

        ParseSpecificKeyInfo (key_iter, key);

        spline.add_key (key);
    }

    template <class T> void ParseSpline (Parser::Iterator spline_iter, Channel& parent)
    {
        T spline;

        for_each_child (*spline_iter, "key",
                        xtl::bind (&XmlAnimationLibraryLoader::ParseSplineKey<T>, this, _1, xtl::ref (spline)));

        parent.SetTrack (spline);
    }

    void ParseAnimationChannel (Parser::Iterator channel_iter, Animation& parent)
    {
      const char *parameter_name = get<const char*> (*channel_iter, "parameter_name"),
                 *track_type     = get<const char*> (*channel_iter, "track_type"),
                 *target_name    = get<const char*> (*channel_iter, "target_name");

      Channel channel;

      channel.SetParameterName (parameter_name);

      if (!xtl::xstrcmp (track_type, "basic_spline<spline_tcb_key<float>>"))
        ParseSpline<math::tcb_splinef> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_tcb_key<vec2f>>"))
        ParseSpline<math::tcb_spline2f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_tcb_key<vec3f>>"))
        ParseSpline<math::tcb_spline3f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_tcb_key<vec4f>>"))
        ParseSpline<math::tcb_spline4f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_bezier_key<float>>"))
        ParseSpline<math::bezier_splinef> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_bezier_key<vec2f>>"))
        ParseSpline<math::bezier_spline2f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_bezier_key<vec3f>>"))
        ParseSpline<math::bezier_spline3f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_bezier_key<vec4f>>"))
        ParseSpline<math::bezier_spline4f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_linear_key<float>>"))
        ParseSpline<math::linear_splinef> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_linear_key<vec2f>>"))
        ParseSpline<math::linear_spline2f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_linear_key<vec3f>>"))
        ParseSpline<math::linear_spline3f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_linear_key<vec4f>>"))
        ParseSpline<math::linear_spline4f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_linear_key<mat4f>>"))
        ParseSpline<math::linear_spline_mat4f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_step_key<float>>"))
        ParseSpline<math::step_splinef> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_step_key<vec2f>>"))
        ParseSpline<math::step_spline2f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_step_key<vec3f>>"))
        ParseSpline<math::step_spline3f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_step_key<vec4f>>"))
        ParseSpline<math::step_spline4f> (channel_iter, channel);
      else if (!xtl::xstrcmp (track_type, "basic_spline<spline_step_key<mat4f>>"))
        ParseSpline<math::step_spline_mat4f> (channel_iter, channel);
      else
        throw xtl::format_operation_exception ("media::animation::XmlAnimationLibraryLoader::ParseAnimationChannel",
                                               "Unsupported channel track type '%s'", track_type);

      parent.AddChannel (target_name, channel);
    }

    //разбор анимации
    void ParseAnimation (Parser::Iterator animation_iter, AnimationLibrary& library)
    {
      const char *id   = get<const char*> (*animation_iter, "id"),
                 *name = get<const char*> (*animation_iter, "name");

      Animation animation;

      animation.Rename (name);



      for_each_child (*animation_iter, "event_track.event",
                      xtl::bind (&XmlAnimationLibraryLoader::ParseEvent, this, _1, animation.Events ()));

      for_each_child (*animation_iter, "channels.channel",
                      xtl::bind (&XmlAnimationLibraryLoader::ParseAnimationChannel, this, _1, xtl::ref (animation)));

      library.Attach (id, animation);
    }

  public:
    XmlAnimationLibraryLoader (const char* file_name, AnimationLibrary& library) : parser (file_name, "xml")
    {
      for_each_child (parser.Root ().First ("animation_library"), "animation",
                      xtl::bind (&XmlAnimationLibraryLoader::ParseAnimation, this, _1, xtl::ref (library)));

        //протоколирование

      Log log (LOG_NAME);

      parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));
    }
    
  private:
    Parser parser;   //парсер
};

/*
    јвтоматическа€ регистраци€ компонента
*/

class XAnimLoaderComponent
{
  public:
    XAnimLoaderComponent ()
    {
      AnimationLibraryManager::RegisterLoader ("xanim", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, AnimationLibrary& library)
    {
      XmlAnimationLibraryLoader (file_name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<XAnimLoaderComponent> XAnimLoader ("media.animation.loaders.XAnimLoader");

}
