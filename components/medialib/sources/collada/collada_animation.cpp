#include "h/shared.h"

ColladaAnimation::ColladaAnimation (const ColladaAnimationImpl* source)
  : ColladaEntity(source), impl (source)
  { }

size_t ColladaAnimation::ChannelsCount () const
{
  return impl->anim_channels.size ();
}

ColladaAnimationChannel ColladaAnimation::Channel (size_t index) const
{
  if ((impl->anim_channels.empty()) || (index >= (impl->anim_channels.size())))
  {
    RaiseOutOfRange ("ColladaAnimation::Channel", "index", index, (size_t)0, impl->anim_channels.size());
    return ColladaWrappers::Create <ColladaAnimationChannel> (&ColladaAnimationChannelImpl());
  }
  return ColladaWrappers::Create <ColladaAnimationChannel> (&impl->anim_channels[index]);
}

size_t ColladaAnimation::ChildrenCount () const
{
  return impl->child_animations.size ();
}

ColladaAnimation ColladaAnimation::Child (size_t index) const
{
  if ((impl->child_animations.empty()) || (index >= (impl->child_animations.size())))
  {
    RaiseOutOfRange ("ColladaAnimation::Child", "index", index, (size_t)0, impl->child_animations.size());
    return ColladaWrappers::Create <ColladaAnimation> (&ColladaAnimationImpl());
  }
  return ColladaWrappers::Create <ColladaAnimation> (&impl->child_animations[index]);
}

ColladaAnimationClip::ColladaAnimationClip (const ColladaAnimationClipImpl* source)
  : ColladaEntity(source), impl (source)
  { }

float ColladaAnimationClip::TimeStart () const
{
  return impl->start;
}

float ColladaAnimationClip::TimeEnd () const
{
  return impl->end;
}

size_t ColladaAnimationClip::AnimationsCount () const
{
  return impl->instances_anim.size();
}

ColladaAnimation ColladaAnimationClip::Animation (size_t animation_index) const
{
  if ((impl->instances_anim.empty()) || (animation_index >= (impl->instances_anim.size())))
  {
    RaiseOutOfRange ("ColladaAnimationClip::Animation", "animation_index", animation_index, (size_t)0, impl->instances_anim.size());
    return ColladaWrappers::Create <ColladaAnimation> (&ColladaAnimationImpl());
  }
  for (size_t i = 0; i < impl->collada_impl->animations.size (); i++)
    if (impl->collada_impl->animations[i].id == impl->instances_anim[animation_index])
      return ColladaWrappers::Create <ColladaAnimation> (&impl->collada_impl->animations[i]);;
  _RaiseSearchFailed ("ColladaAnimationClip::Animation");
  return ColladaWrappers::Create <ColladaAnimation> (&ColladaAnimationImpl());
}

template <class SampleType>
ColladaAnimationSamples<SampleType>::ColladaAnimationSamples (size_t count,const Sample* samples)
  : samples (samples), count (count)
  { }

template <class SampleType>
size_t ColladaAnimationSamples<SampleType>::Count () const
{
  return count;
}

template <class SampleType>
const SampleType* ColladaAnimationSamples<SampleType>::Data () const
{
  return samples;
}

template <class SampleType>
const SampleType& ColladaAnimationSamples<SampleType>::At (size_t index) const
{
  if ((!count) || (index >= count))
  {
    RaiseOutOfRange ("ColladaAnimationSamples::At", "index", index, (size_t)0, count);
    return samples[0];
  }
  return samples[index];
}

template <class SampleType>
const SampleType& ColladaAnimationSamples<SampleType>::operator [] (size_t index) const
{
  if ((!count) || (index >= count))
  {
    RaiseOutOfRange ("ColladaAnimationSamples::operator []", "index", index, (size_t)0, count);
    return samples[0];
  }
  return samples[index];
}

ColladaAnimationChannel::ColladaAnimationChannel (const ColladaAnimationChannelImpl* source)
  : ColladaEntity(source), impl (source)
  { }

const char* ColladaAnimationChannel::TargetName () const
{
  return impl->target.c_str();
}

ColladaAnimationParam ColladaAnimationChannel::TargetParam () const
{
  return impl->param;
} 

template <class SampleType>
const ColladaAnimationSamples<SampleType> ColladaAnimationChannel::Samples () const
{
  return ColladaAnimationSamples(impl);
} 

float ColladaAnimationChannel::TimeStart () const
{
  return impl->input.front();
}

float ColladaAnimationChannel::TimeEnd () const
{
  return impl->input.back();
}

void ColladaImpl::parse_library_animations (Parser::Iterator p)
{
  if(!p->Present("animation"))
  {
    log->Error(p, "Uncorrect 'library_animations' tag. Must be at least one 'animation' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("animation"); i; i++)
  {
    animations.resize(animations.size() + 1);
    parse_animation (i, &animations.back());
  }
}

void ColladaImpl::parse_animation (Parser::Iterator p, ColladaAnimationImpl* destination)
{
  const char *sampler;
  
  destination->line = p->LineNumber();
  p->Read ("id", destination->id, "No id");

  for(Parser::NamesakeIterator i = p->First("channel"); i; i++)
  {
    if(!i->Present("target") || !i->Present("source"))
    {
      log->Error(i, "One of required sub-tag 'target' or 'source' not detected.");
      continue;
    }

    destination->anim_channels.resize(destination->anim_channels.size() + 1);
    destination->anim_channels.back().target = i->ReadString("target");
    destination->anim_channels.back().line = i->LineNumber();
    sampler = &(i->ReadString("source", "##")[1]);

    for(Parser::NamesakeIterator j = p->First("sampler"); j; j++)
      if(j->Test("id", sampler))
        for(Parser::NamesakeIterator k = j->First("input"); k; k++)
        {
          if(k->Test("semantic", "INPUT"))
            parse_source(p, &(destination->anim_channels.back().input), &(k->ReadString("source", "##")[1]));
          if(k->Test("semantic", "INTERPOLATION"))
            parse_interpolation_source (p, &(k->ReadString("source", "##")[1]));
          if(k->Test("semantic", "IN_TANGENT"))
            parse_source(p, &(destination->anim_channels.back().in_tangent), &(k->ReadString("source", "##")[1]));
          if(k->Test("semantic", "OUT_TANGENT"))
            parse_source(p, &(destination->anim_channels.back().out_tangent), &(k->ReadString("source", "##")[1]));
          if(k->Test ("semantic", "OUTPUT"))
          {
            bool param_set = false;

            for (Parser::NamesakeIterator m = p->First("source"); m; m++)
              if(m->Test("id", k->ReadString("source", "##") + 1))
              {
                if(m->Present("technique_common"))
                  if(m->First("technique_common")->Present("accessor"))
                    if(m->First("technique_common")->First("accessor")->Present("param"))
                    {
                      if (m->Test("technique_common.accessor.param.name", "TRANSFORM"))
                        destination->anim_channels.back().param = COLLADA_ANIMATION_NODE_TRANSFORM;      
                      else
                      {
                        log->Error(m, "Unknown animation param, please call to developers");
                        destination->anim_channels.back().param = COLLADA_ANIMATION_NODE_TRANSFORM;      
                      }
                      param_set = true;
                    }
              }
            if(!param_set)
              log->Error (m, "Can't retreive animation param");
            parse_source(p, &(destination->anim_channels.back().output), &(k->ReadString("source", "##")[1]));
          }
        }
  }

  for(Parser::NamesakeIterator i = p->First("animation"); i; i++)
  {
    destination->child_animations.resize(destination->child_animations.size() + 1);
    parse_animation(i, &destination->child_animations.back());
  }
}

void ColladaImpl::parse_library_animation_clips (Parser::Iterator p)
{
  if(!p->Present("animation_clip"))
  {
    log->Error(p, "Uncorrect 'library_animation_clips' tag. Must be at least one 'animation_clip' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("animation_clip"); i; i++)
    parse_animation_clip (i);
}

void ColladaImpl::parse_animation_clip (Parser::Iterator p)
{
  animation_clips.resize(animation_clips.size() + 1);

  p->Read ("id", animation_clips.back().id, "No id");
  p->Read ("start", animation_clips.back().start, 0.0f);
  p->Read ("end",   animation_clips.back().end,   -1.0f);
  animation_clips.back().line = p->LineNumber();

  for(Parser::NamesakeIterator i = p->First("instance_animation"); i; i++)
  {
    if(!i->Present("url"))
    {
      log->Error(i, "Required sub-tag 'url' not detected.");
      continue;
    }
    animation_clips.back().instances_anim.resize(animation_clips.back().instances_anim.size() + 1);
    animation_clips.back().instances_anim.back() = &(i->ReadString("url", "##")[1]);
  }
}

void ColladaImpl::parse_interpolation_source (Parser::Iterator p, const char *source)
{
  for (Parser::NamesakeIterator i = p->First("source"); i; i++)
    if (i->Test("id", source))
    {
      for (Parser::NamesakeIterator j = i->First("Name_array"); j; j++)
        parse_interpolations (j);
      break;
    }
}

void ColladaImpl::parse_interpolations (Parser::Iterator n_array)
{
  size_t count;

  n_array->Read("count", count, (size_t)0);

  for (size_t i = 0; i < count; i++)
    if(!n_array->Test("#text", i, "LINEAR"))
      log->Warning (n_array, "Using of non-Linear interpolation type not supported. Use exporter options for forced animation converting");
}
