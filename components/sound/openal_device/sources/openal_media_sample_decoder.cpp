#include "shared.h"

using namespace sound::low_level::openal;

/*
   Класс работы с сэмплами на основе media::Sample
*/

/*
   Конструкторы
*/

MediaSampleDecoder::MediaSampleDecoder (const char* name)
{
  try
  {
    sample.Load (name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::MediaSampleDecoder (const char*)");
    throw;
  }
}

MediaSampleDecoder::MediaSampleDecoder (const media::SoundSample& in_sample)
{
  try
  {
    sample = in_sample.Clone ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::MediaSampleDecoder (const media::SoundSample&)");
    throw;
  }
}

/*
   Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
*/

size_t MediaSampleDecoder::Read (size_t first_sample, size_t samples_count, void* data)
{
  try
  {
    return sample.Read (first_sample, samples_count, data);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::MediaSampleDecoder::Read");
    throw;
  }
}

/*
   Клонирование
*/

MediaSampleDecoder* MediaSampleDecoder::Clone ()
{
  return new MediaSampleDecoder (sample);
}
