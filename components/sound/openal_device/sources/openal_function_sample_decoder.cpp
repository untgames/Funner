#include "shared.h"

using namespace sound::low_level;
using namespace sound::low_level::openal;

/*
   ����� ������ � �������� ���������� ����� ��������������� �������
*/

struct FunctionSampleDecoder::Impl : public xtl::reference_counter
{
  IDevice::SampleReadFunction read_function;

  Impl (const IDevice::SampleReadFunction& fn)
    : read_function (fn)
    {}
};

/*
   ������������/����������
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
   ������ ��������������� ����� ����������� samples_count ������� ������� � first sample � data
*/

unsigned int FunctionSampleDecoder::Read (unsigned int first_sample, unsigned int samples_count, void* data)
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
   ������������
*/

FunctionSampleDecoder* FunctionSampleDecoder::Clone ()
{
  return new FunctionSampleDecoder (impl);
}
