#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;

/*
   Класс работы с сэмплами работающий через предоставленную функцию
*/

struct FunctionSampleDecoder::Impl : public xtl::reference_counter
{
  IDevice::SampleReadFunction read_function;

  Impl (const IDevice::SampleReadFunction& fn)
    : read_function (fn)
    {}
};

/*
   Конструкторы/деструктор
*/

FunctionSampleDecoder::FunctionSampleDecoder (const IDevice::SampleReadFunction& fn)
  : impl (new Impl (fn))
  {}

FunctionSampleDecoder::FunctionSampleDecoder (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

FunctionSampleDecoder::~FunctionSampleDecoder ()
{
  release (impl);
}

/*
   Чтение декодированного звука количеством samples_count сэмплов начиная с first sample в data
*/

size_t FunctionSampleDecoder::Read (size_t first_sample, size_t samples_count, void* data)
{
  try
  {
    return impl->read_function (first_sample, samples_count, data);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("sound::low_level::openal::FunctionSampleDecoder::Read");
    throw;
  }
}

/*
   Клонирование
*/

FunctionSampleDecoder* FunctionSampleDecoder::Clone ()
{
  return new FunctionSampleDecoder (impl);
}
