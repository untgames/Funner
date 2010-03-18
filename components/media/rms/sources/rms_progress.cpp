#include "shared.h"

using namespace media::rms;

/*
    Конструкторы / деструктор / присваивание
*/

Progress::Progress (AsyncOperation* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::rms::Progress::Progress", "impl");

  impl->AddRef ();
}

Progress::Progress (const Progress& progress)
  : impl (progress.impl)
{
  impl->AddRef ();
}

Progress::~Progress ()
{
  impl->Release ();
}

Progress& Progress::operator = (const Progress& progress)
{
  Progress (progress).Swap (*this);
  return *this;
}

/*
    Состояние загрузки
*/

void Progress::SetState (ProgressState state)
{
  try
  {
    impl->SetState (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Progress::SetState");
    throw;
  }
}

ProgressState Progress::State () const
{
  return impl->State ();
}

float Progress::Value () const
{
  return impl->Value ();
}

const char* Progress::Resource () const
{
  return impl->Resource ();
}

const char* Progress::Stage () const
{
  return impl->Stage ();
}

/*
    Обмен
*/

void Progress::Swap (Progress& progress)
{
  stl::swap (impl, progress.impl);
}

namespace media
{

namespace rms
{

void swap (Progress& progress1, Progress& progress2)
{
  progress1.Swap (progress2);
}

}

}
