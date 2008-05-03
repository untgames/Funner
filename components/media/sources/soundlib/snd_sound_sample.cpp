#include "shared.h"

namespace
{

const char* GetExtension (const char* file_name)
{
  const char*  ext;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return file_name;

  ext = file_name + len;

  for (; len && (*ext != '.'); len--, ext--);

  if (!len)
    return file_name;

  return ext + 1;
}

}

using namespace common;
using namespace media;

SoundSample::SoundSample ()
  : impl (new SoundSampleImpl)
{
  SoundSystemSingleton::Instance ().RegisterSoundSample (*this);
}

SoundSample::SoundSample (const SoundSample& source)
  : impl (new SoundSampleImpl)
{                  
  impl->str_name = source.impl->str_name;
  impl->info     = source.impl->info;
  if (source.impl->codec.get ())
    impl->codec  = source.impl->codec->Clone ();

  SoundSystemSingleton::Instance ().RegisterSoundSample (*this);
}

SoundSample::SoundSample (const char* file_name)
  : impl (new SoundSampleImpl)
{
  SoundSampleSystem::CodecLoadFunc* load_func = SoundSystemSingleton::Instance ().GetLoadFunc(GetExtension (file_name));

  if (!load_func)
    Raise <Exception> ("SoundSample::SoundSample", "Can't create sound from file '%s'. Format not supported.", file_name);

  impl->str_name = file_name;
  impl->codec    = (*load_func) (file_name, impl->info);

  SoundSystemSingleton::Instance ().RegisterSoundSample (*this);
}

SoundSample::SoundSample (SoundSampleImpl* source)
  : impl (source)
{
  SoundSystemSingleton::Instance ().RegisterSoundSample (*this);
}

SoundSample::~SoundSample ()
{
  SoundSystemSingleton::Instance ().UnregisterSoundSample (*this);
}

SoundSample& SoundSample::operator = (const SoundSample& source)
{
  if (this == &source)
    return *this;

  SoundSample(source).Swap (*this);

  return *this; 
}

void SoundSample::Load (const char* file_name)
{  
  SoundSampleSystem::CodecLoadFunc* load_func = SoundSystemSingleton::Instance ().GetLoadFunc(GetExtension (file_name));

  if (!load_func)
    Raise <Exception> ("SoundSample::Load", "Can't create sound from file '%s'. Format not supported.", file_name);

  impl->str_name = file_name;
  impl->codec    = (*load_func) (file_name, impl->info);
}

const char* SoundSample::Name () const
{
  return impl->str_name.c_str ();
}

void SoundSample::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

size_t SoundSample::SamplesToBytes (size_t sample_count) const
{
  return impl->info.bits_per_sample / 8 * sample_count * impl->info.channels;
}

size_t SoundSample::BytesToSamples (size_t byte_count) const
{
  if (!impl->info.bits_per_sample || !impl->info.channels)
    return 0;

  return byte_count / (impl->info.bits_per_sample / 8 * impl->info.channels);
}

double SoundSample::SamplesToSeconds (size_t sample_count) const
{
  if (!impl->info.frequency)
    return 0;

  return (double)sample_count / (double)impl->info.frequency;
}

size_t SoundSample::SecondsToSamples (double second_count) const
{
  return size_t(impl->info.frequency * second_count);
}

size_t SoundSample::SamplesCount () const
{
  return impl->info.samples_count;
}

size_t SoundSample::SizeInBytes () const
{
  return impl->info.samples_count * impl->info.bits_per_sample / 8 * impl->info.channels;
}

size_t SoundSample::Frequency () const
{
  return impl->info.frequency;
}

size_t SoundSample::Channels () const
{
  return impl->info.channels;
}

size_t SoundSample::BitsPerSample () const
{
  return impl->info.bits_per_sample;
}

size_t SoundSample::Read (size_t first_sample, size_t samples_count, void* data)
{
  if (first_sample >= impl->info.samples_count)
    return 0;

  return impl->codec->Read (first_sample, samples_count, data);
}

void SoundSample::Swap (SoundSample& sound)
{
  swap (impl, sound.impl);
}

namespace media
{

void swap (SoundSample& sound1,SoundSample& sound2)
{
  sound1.Swap (sound2);
}

}
