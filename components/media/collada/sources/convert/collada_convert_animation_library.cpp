#include "shared.h"

using namespace media::collada;

namespace
{

/*
    Преобразователь данных коллады
*/

class Converter
{
  private:
    void ConvertAnimationChannel (const AnimationChannel& collada_channel, media::animation::Animation& parent_animation, bool as_animation)
    {
      media::animation::Channel channel;

      channel.SetParameterName (collada_channel.ParameterName ());

      switch (collada_channel.Semantic ())
      {
        case AnimationChannelSemantic_Transform:
        {
          math::step_spline_mat4f        spline;
          const AnimationSampleTransform *current_sample = collada_channel.Samples<AnimationSampleTransform> ();

          for (size_t i = 0, samples_count = collada_channel.SamplesCount (); i < samples_count; i++, current_sample++)
            spline.add_key (current_sample->time, current_sample->value);

          channel.SetTrack (spline);
          break;
        }
        default:
          throw xtl::format_operation_exception ("::convert_animation_channel", "Unsupported collada animation channel semantic");
      }

      if (as_animation)
      {
        media::animation::Animation sub_animation;

        sub_animation.SetTargetName (collada_channel.TargetName ());

        sub_animation.AddChannel (channel);

        parent_animation.AddSubAnimation (sub_animation);
      }
      else
        parent_animation.AddChannel (channel);
    }

    void ConvertAnimation (const Animation& collada_animation, media::animation::Animation& animation)
    {
      animation.Rename (collada_animation.Id ());

      const Animation::AnimationChannelList& channels = collada_animation.Channels ();

      if (!channels.IsEmpty ())
      {
        const char* target_name = channels [0].TargetName ();

        animation.SetTargetName (target_name);

        for (Animation::AnimationChannelList::ConstIterator channel_iter = channels.CreateIterator (); channel_iter; ++channel_iter)
          ConvertAnimationChannel (*channel_iter, animation, xtl::xstrcmp (target_name, channel_iter->TargetName ()) != 0);
      }

      for (AnimationList::ConstIterator sub_animation_iter = collada_animation.Animations ().CreateIterator (); sub_animation_iter; ++sub_animation_iter)
      {
        media::animation::Animation sub_animation;

        ConvertAnimation (*sub_animation_iter, sub_animation);

        animation.AddSubAnimation (sub_animation);
      }
    }

    void AddAnimationToLibrary (const Animation& collada_animation, media::animation::AnimationLibrary& library)
    {
      media::animation::Animation animation;

      ConvertAnimation (collada_animation, animation);

      stl::string id (collada_animation.Id ());

      //копируем имя объекта в id анимации, если при экспорте id не был задан
      if (id.empty ())
        id = animation.TargetName ();

      //если для одного объекта существует несколько анимаций, добавляем символ '_' чтобы не затереть предыдущие
      while (library.Find (id.c_str ()))
        id += '_';

      library.Attach (id.c_str (), animation);
    }

  public:
    /*
        Конструктор
    */

    Converter (const media::collada::Model& model, media::animation::AnimationLibrary& library)
    {
      for (AnimationList::ConstIterator i = model.Animations ().CreateIterator (); i; ++i)
        AddAnimationToLibrary (*i, library);
    }
};

}

namespace media
{

namespace collada
{

//преобразование коллада-модели в библиотеку анимаций
void convert (const media::collada::Model& src_model, animation::AnimationLibrary& dst_library)
{
  Converter (src_model, dst_library);
}

}

}
