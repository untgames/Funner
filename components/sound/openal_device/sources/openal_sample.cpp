#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;

namespace
{

const size_t MAX_SAMPLE_BUFFER_SIZE = 1024 * 64;

}

/*
   Базовый класс работы с сэмплами
*/

/*
   Конструкторы
*/

OpenALSample::OpenALSample (const char* in_name)
{
  try
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("", "name");

    name = in_name;

    media::SoundSample sample (in_name);

    memset (&desc, 0, sizeof (desc));

    desc.bits_per_sample = sample.BitsPerSample ();
    desc.channels        = sample.Channels ();
    desc.frequency       = sample.Frequency ();
    desc.samples_count   = sample.SamplesCount ();

    if (sample.SamplesToBytes (sample.SamplesCount ()) < MAX_SAMPLE_BUFFER_SIZE)
      decoder = SampleDecoderPtr (new BufferedSampleDecoder (in_name), false);
    else
      decoder = SampleDecoderPtr (new MediaSampleDecoder (in_name), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::OpenALSample::OpenALSample");
    throw;
  }
}

OpenALSample::OpenALSample (const SampleDesc& in_desc, const IDevice::SampleReadFunction& fn)
{
  try
  {
    desc = in_desc;

    if (SamplesToBytes (SamplesCount ()) < MAX_SAMPLE_BUFFER_SIZE)
      decoder = SampleDecoderPtr (new BufferedSampleDecoder (in_desc, fn), false);
    else
      decoder = SampleDecoderPtr (new FunctionSampleDecoder (fn), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::OpenALSample::OpenALSample");
    throw;
  }
}

/*
   Получение имени сэмпла
*/

const char* OpenALSample::GetName ()
{
  return name.c_str ();
}

/*
   Получение дескриптора сэмпла
*/

void OpenALSample::GetDesc (SampleDesc& out_desc)
{
  out_desc = desc;
}

/*
   Функции расчёта одних характеристик через другие (всегда происходит округление в меньшую сторону)
*/

size_t OpenALSample::SamplesToBytes (size_t sample_count)
{
  return desc.bits_per_sample / 8 * sample_count * desc.channels;
}

size_t OpenALSample::BytesToSamples (size_t byte_count)
{
  if (!desc.bits_per_sample || !desc.channels)
    return 0;

  return byte_count / (desc.bits_per_sample / 8 * desc.channels);

}

double OpenALSample::SamplesToSeconds (size_t sample_count)
{
  if (!desc.frequency)
    return 0;

  return (double)sample_count / (double)desc.frequency;

}

size_t OpenALSample::SecondsToSamples (double second_count)
{
  return size_t(desc.frequency * second_count);
}

/*
   Получение параметров
*/

size_t OpenALSample::Channels ()
{
  return desc.channels;
}

size_t OpenALSample::Frequency ()
{
  return desc.frequency;
}

size_t OpenALSample::SamplesCount ()
{
  return desc.samples_count;
}

double OpenALSample::Duration ()
{
  return SamplesToSeconds (SamplesCount ());
}

/*
   Создание декодера
*/

ISampleDecoder* OpenALSample::CreateDecoder ()
{
  return decoder->Clone ();
}
