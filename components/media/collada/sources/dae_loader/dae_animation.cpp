#include "shared.h"

/*
    –азбор библиотеки анимаций
*/

void DaeParser::ParseLibraryAnimations (Parser::Iterator iter)
{
  if (!iter->First ("animation"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_animations' node. Must be at least one 'animation' sub-tag");
    return;
  }
  
  for_each_child (*iter, "animation", xtl::bind (&DaeParser::ParseAnimation, this, _1, xtl::ref (model.Animations ())));
}

void DaeParser::ParseAnimation (Parser::Iterator iter, AnimationList& collection)
{
  const char *id = get<const char*> (*iter, "id", "");

    //создание анимации
  Animation animation;

  animation.SetId (id);

    //чтение каналов анимаций
  for_each_child (*iter, "channel", xtl::bind (&DaeParser::ParseAnimationChannel, this, _1, iter, xtl::ref (animation)));

    //чтение дочерних анимаций
  for_each_child (*iter, "animation", xtl::bind (&DaeParser::ParseAnimation, this, _1, xtl::ref (animation.Animations ())));

  if (animation.Animations ().IsEmpty () && animation.Channels ().IsEmpty ())
    raise_parser_exception (*iter, "Incorrect 'animation' tag, must contain at least one 'animation' sub-tag or 'sampler'/'channel' pair");

    //добавление анимации в коллекцию в библиотеку
  collection.Add (animation);
}

void DaeParser::ParseAnimationChannel (Parser::Iterator channel_iter, Parser::Iterator animation_iter, Animation& animation)
{
  const char *source       = get<const char*> (*channel_iter, "source"),
             *target       = get<const char*> (*channel_iter, "target");
  size_t     target_length = xtl::xstrlen (target);

    //проверка корректности формата ссылок
  if (!target_length)
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag, 'target' attribute is empty");

  if (target [0] == '.')
    raise_parser_exception (*channel_iter, "Unsupported 'channel' tag target attribute, relative pathes not supported");

  const char* path_end = strchr (target, '/');

  if (path_end == target)
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag target attribute, empty target name");

  if (!path_end || path_end - target == target_length - 1)
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag target attribute, target name not contains property name");

  const char* subparameter_start = strchr (target, '.');

  if (subparameter_start)
    raise_parser_exception (*channel_iter, "Unsupported 'channel' tag 'target' attribute, property components animation not supported yet");

  if (subparameter_start && subparameter_start < path_end)
    raise_parser_exception (*channel_iter, "Unsupported 'channel' tag 'target' attribute, '.' symbol founded before property name start");

  size_t parameter_name_length = subparameter_start ? (subparameter_start - path_end) - 1 : target_length - (path_end - target) - 1;

  stl::string target_name      (target, path_end - target),
              parameter_name   (path_end + 1, parameter_name_length),
              semantic_map_key (target, subparameter_start ? subparameter_start - target : target_length);

  AnimationSemanticsMap::iterator semantic_iter = animation_semantics.find (semantic_map_key.c_str ());

  if (semantic_iter == animation_semantics.end ())
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag, can't determine semantic for this animation");

    //проверка наличи€ соответсвующего сэмплера
  Parser::Iterator sampler;

  source++; //избавл€емс€ от префиксного '#'

  for (Parser::NamesakeIterator sampler_iter = animation_iter->First ("sampler"); sampler_iter; ++sampler_iter)
    if (!xtl::xstrcmp (get <const char*> (*sampler_iter, "id"), source))
    {
      sampler = sampler_iter;
      break;
    }

  if (!sampler)
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag 'source' attribute, sampler with this name not found");

    //чтение данных канала анимации
  int samples_count = -1;

  stl::vector <float> samples_times, samples_data;

  for (Parser::NamesakeIterator input_iter = sampler->First ("input"); input_iter; ++input_iter)
  {
    const char *semantic     = get <const char*> (*input_iter, "semantic"),
               *input_source = get <const char*> (*input_iter, "source");

    input_source++; //избавл€емс€ от префиксного '#'

    bool is_source_found = false;

    int source_size;

    for (Parser::NamesakeIterator animation_source_iter = animation_iter->First ("source"); animation_source_iter; ++animation_source_iter)
    {
      const char* animation_source_id = get <const char*> (*animation_source_iter, "id");

      if (xtl::xstrcmp (input_source, animation_source_id))
        continue;

      if (!xtl::xstrcmp (semantic, "INTERPOLATION"))
      {
        stl::vector <stl::string> interpolation_types_array;

        ParseNameArray (animation_source_iter, interpolation_types_array);

        source_size = interpolation_types_array.size ();

        for (size_t i = 0, count = interpolation_types_array.size (); i < count; i++)
          if (interpolation_types_array [i] != "LINEAR")
            raise_parser_exception (*animation_source_iter, "Unsupported interpolation 'source' tag value '%s', only 'LINEAR' interpolation supported", interpolation_types_array [i].c_str ());
      }
      else if (!xtl::xstrcmp (semantic, "INPUT"))
      {
        ParseFloatArray (animation_source_iter, samples_times);

        source_size = samples_times.size ();
      }
      else if (!xtl::xstrcmp (semantic, "OUTPUT"))
      {
        ParseFloatArray (animation_source_iter, samples_data);

        switch (semantic_iter->second)
        {
          case AnimationChannelSemantic_Transform:
            if (samples_data.size () % 16)
              raise_parser_exception (*animation_source_iter, "Incorrect 'source' data array size, must contain float 4x4 for transform animation semantic");

            source_size = samples_data.size () / 16;
            break;
          default:
            raise_parser_exception (*animation_source_iter, "Can't process animation data, unsupported animation semantic");
        }
      }
      else
        raise_parser_exception (*sampler, "Unsupported 'sampler' tag 'input' semantic '%s'", semantic);

      is_source_found = true;
      break;
    }

    if (!is_source_found)
      raise_parser_exception (*sampler, "Incorrect 'sampler', can't find 'source' for id '%s'", input_source);

    if (samples_count < 0)
      samples_count = source_size;

    if (source_size != samples_count)
      raise_parser_exception (*sampler, "Incorrect 'sampler', 'source' for id '%s' has different size from previous source", input_source);
  }

  if (samples_times.empty ())
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag, INPUT semantic empty");

  if (samples_data.empty ())
    raise_parser_exception (*channel_iter, "Incorrect 'channel' tag, OUTPUT semantic empty");

  AnimationChannel animation_channel (semantic_iter->second);

  animation_channel.SetTargetName    (target_name.c_str ());
  animation_channel.SetParameterName (parameter_name.c_str ());

  animation_channel.SetSamplesCount (samples_count);

  switch (semantic_iter->second)
  {
    case AnimationChannelSemantic_Transform:
    {
      AnimationSampleTransform *animation_sample = animation_channel.Samples <AnimationSampleTransform> ();

      for (int i = 0; i < samples_count; i++, animation_sample++)
      {
        animation_sample->time  = samples_times [i];
        animation_sample->value = math::mat4f (&samples_data [i * 16]);
      }

      break;
    }
    default:
      raise_parser_exception (*channel_iter, "Can't process animation data, unsupported animation semantic");
  }

  animation.Channels ().Add (animation_channel);
}
