#include "shared.h"

using namespace common;
using namespace media;

namespace media
{

typedef xtl::com_ptr<ISoundInputStream> SoundInputStreamPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации звукового сэмпла
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SoundSample::Impl : public xtl::reference_counter
{
  public:
    Impl () {}
    Impl (const Impl& source) : str_name (source.str_name), info (source.info), input_stream (source.input_stream) {}

    Impl* Clone ()
    {
      return new Impl (*this);
    }

    stl::string         str_name;     //SoundSample name
    SoundSampleInfo     info;         //Информация о файле
    SoundInputStreamPtr input_stream; //Кодек
};

}

/*
   Конструкторы / деструктор
*/

SoundSample::SoundSample (Impl* in_impl)
  : impl (in_impl)
{
}

SoundSample::SoundSample ()
  : impl (new Impl)
{
}

SoundSample::SoundSample (const SoundSample& source)
  : impl (source.impl)
{
  addref (impl);
}

SoundSample::SoundSample (const char* file_name)
  : impl (new Impl)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    static ComponentLoader loader ("media.sound.loaders.*");

    impl->input_stream = SoundInputStreamPtr (SoundSampleManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, impl->info), false);
    impl->str_name     = file_name;
  }
  catch (xtl::exception& exception)
  {
    release (impl);
    exception.touch ("media::SoundSample::SoundSample (const char*)");
    throw;
  }
}

SoundSample::SoundSample (ISoundInputStream* istream)
  : impl (new Impl)
{
  try
  {
    if (!istream)
      throw xtl::make_null_argument_exception ("", "istream");

    impl->input_stream = SoundInputStreamPtr (istream);
  }
  catch (xtl::exception& exception)
  {
    release (impl);
    exception.touch ("media::SoundSample::SoundSample (ISoundInputStream*)");
    throw;
  }
}

SoundSample::~SoundSample ()
{
  release (impl);
}

/*
   Присваивание
*/

SoundSample& SoundSample::operator = (const SoundSample& source)
{
  SoundSample (source).Swap (*this);

  return *this;
}

/*
   Клонирование
*/

SoundSample SoundSample::Clone () const
{
  try
  {
    return SoundSample (impl->Clone ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::SoundSample::Clone");
    throw;
  }
}

/*
   Загрузка
*/

void SoundSample::Load (const char* file_name)
{
  SoundSample (file_name).Swap (*this);
}

/*
   Имя сэмпла
*/

const char* SoundSample::Name () const
{
  return impl->str_name.c_str ();
}

void SoundSample::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

/*
   Функции расчёта одних характеристик через другие (всегда происходит округление в меньшую сторону)
*/

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

/*
   Характеристики
*/

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

/*
   Работа с данными
*/

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
    throw xtl::make_null_argument_exception ("media::SoundSample::Save", "file_name");

  try
  {
    static ComponentLoader loader ("media.sound.savers.*");

    SoundSampleManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::SoundSample::Save");
    throw;
  }
}

/*
   Обмен
*/

void SoundSample::Swap (SoundSample& sound)
{
  stl::swap (impl, sound.impl);
}

namespace media
{

void swap (SoundSample& sound1,SoundSample& sound2)
{
  sound1.Swap (sound2);
}

}
