#include "shared.h"

using namespace common;
using namespace media;

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации звукового сэмпла
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SoundSample::Impl
{
  public:
    Impl () {}
    Impl (const stl::string& in_name, const SoundSampleInfo& in_info) : str_name (in_name), info (in_info) {}

    stl::string                     str_name;     //SoundSample name
    SoundSampleInfo                 info;         //Информация о файле
    xtl::com_ptr<ISoundInputStream> input_stream; //Кодек
};

}

SoundSample::SoundSample ()
  : impl (new Impl)
{
}

SoundSample::SoundSample (const SoundSample& source)
  : impl (new Impl (source.impl->str_name, source.impl->info))
{                  
  impl->input_stream = source.impl->input_stream;
}

SoundSample::SoundSample (const char* file_name)
  : impl (new Impl)
{
  if (!file_name)
    raise_null_argument ("media::SoundSample::SoundSample", "file_name");

  try
  {
    static ComponentLoader loader ("media.sound.loaders.*");

    impl->input_stream    = SoundSampleManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, impl->info);
    impl->str_name = file_name;
  }
  catch (Exception& exception)
  {
    exception.Touch ("media::SoundSample::SoundSample");
    throw;
  }
}

SoundSample::SoundSample (ISoundInputStream* istream)
  : impl (new Impl)
{
  if (!istream)
    raise_null_argument ("media::SoundSample::SoundSample", "istream");

  impl->input_stream = istream;
}

SoundSample::~SoundSample ()
{
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
  SoundSample (file_name).Swap (*this);
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

  return impl->input_stream->Read (first_sample, samples_count, data);
}

/*
   Сохранение
*/

void SoundSample::Save (const char* file_name) const
{
  if (!file_name)
    raise_null_argument ("media::SoundSample::Save", "file_name");
    
  try
  {
    static ComponentLoader loader ("media.sound.savers.*");

    SoundSampleManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::SoundSample::Save");
    throw;
  }
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
