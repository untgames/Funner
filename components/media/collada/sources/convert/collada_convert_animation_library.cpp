#include "shared.h"

using namespace media::collada;

namespace
{

const float TRANSFORM_EQUAL_EPSILON = 0.001f;

/*
    Преобразователь данных коллады
*/

class Converter
{
  private:
    void ConvertAnimationChannel (const AnimationChannel& collada_channel, media::animation::Animation& parent_animation)
    {
      switch (collada_channel.Semantic ())
      {
        case AnimationChannelSemantic_Transform:
        {
          //we do not support setting transform directly to node, it should be decomposed to position, rotation and scale in order to use
          media::animation::Channel position_channel, orientation_channel, scale_channel;

          position_channel.SetParameterName    ("position");
          orientation_channel.SetParameterName ("orientation");
          scale_channel.SetParameterName       ("scale");

          math::linear_spline3f          position_spline, scale_spline;
          math::step_spline_quatf        orientation_spline;
          const AnimationSampleTransform *current_sample = collada_channel.Samples<AnimationSampleTransform> (),
                                         *previous_sample = 0;
          size_t                         samples_count = collada_channel.SamplesCount ();

          position_spline.reserve    (samples_count);
          scale_spline.reserve       (samples_count);
          orientation_spline.reserve (samples_count);

          math::vec3f current_position, current_scale;
          math::quatf current_orientation;

          for (size_t i = 0; i < samples_count; i++, current_sample++)
          {
            const AnimationSampleTransform *next_sample = i < samples_count - 1 ? current_sample + 1 : 0;

            if (previous_sample && next_sample && math::equal (previous_sample->value, current_sample->value, TRANSFORM_EQUAL_EPSILON) &&
                math::equal (next_sample->value, current_sample->value, TRANSFORM_EQUAL_EPSILON))
              continue;

            math::affine_decompose (current_sample->value, current_position, current_orientation, current_scale);

            position_spline.add_key    (current_sample->time, current_position);
            orientation_spline.add_key (current_sample->time, current_orientation);
            scale_spline.add_key       (current_sample->time, current_scale);

            previous_sample = current_sample;
          }

          position_channel.SetTrack    (position_spline);
          scale_channel.SetTrack       (scale_spline);
          orientation_channel.SetTrack (orientation_spline);

          parent_animation.AddChannel (collada_channel.TargetName (), position_channel);
          parent_animation.AddChannel (collada_channel.TargetName (), scale_channel);
          parent_animation.AddChannel (collada_channel.TargetName (), orientation_channel);

          break;
        }
        default:
          throw xtl::format_operation_exception ("::convert_animation_channel", "Unsupported collada animation channel semantic");
      }
    }

    void ConvertAnimation (const Animation& collada_animation, media::animation::Animation& animation, bool sub_animation)
    {
      if (!sub_animation)
        animation.Rename (collada_animation.Id ());

      const Animation::AnimationChannelList& channels = collada_animation.Channels ();

      for (Animation::AnimationChannelList::ConstIterator channel_iter = channels.CreateIterator (); channel_iter; ++channel_iter)
        ConvertAnimationChannel (*channel_iter, animation);

      for (AnimationList::ConstIterator sub_animation_iter = collada_animation.Animations ().CreateIterator (); sub_animation_iter; ++sub_animation_iter)
        ConvertAnimation (*sub_animation_iter, animation, true);
    }

    void AddAnimationToLibrary (const Animation& collada_animation, media::animation::AnimationLibrary& library)
    {
      media::animation::Animation animation;

      ConvertAnimation (collada_animation, animation, false);

      stl::string id (collada_animation.Id ());

      //копируем имя объекта в id анимации, если при экспорте id не был задан
      if (id.empty () && animation.TargetsCount ())
        id = animation.TargetName (0);

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

    Converter (const media::collada::Model& model, media::animation::AnimationLibrary& library, const char* merge_animation)
    {
      media::animation::Animation root_animation;

      for (AnimationList::ConstIterator i = model.Animations ().CreateIterator (); i; ++i)
        ConvertAnimation (*i, root_animation, true);

      library.Attach (merge_animation, root_animation);
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

void convert (const collada::Model& source, animation::AnimationLibrary& destination, const char* merge_animation)
{
  if (!merge_animation)
    throw xtl::make_null_argument_exception ("media::collada::convert (const collada::Model&, AnimationLibrary&, const char*)", "merge_animation");

  Converter (source, destination, merge_animation);
}

}

}
