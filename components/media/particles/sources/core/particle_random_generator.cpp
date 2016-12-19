#include "shared.h"

using namespace media::particles;

/*
    Constants
*/

const size_t VALUES_BATCH_SIZE = 256;

/*
    Implementation details
*/

namespace
{

struct DefaultRandomGenerator
{
  void operator () (size_t count, float* values) const
  {
    for (float* value = values; count--; value++)
    {
      *value = (float)(rand () / double (RAND_MAX));
    }
  }
};

}

typedef stl::vector<float> ValueArray;

struct RandomGenerator::Impl: public xtl::reference_counter
{
  ValueArray     values;
  RandomFunction generator_fn;
  float*         current_value;
  float*         finish_value;         

  Impl (const RandomFunction& in_generator_fn)
    : generator_fn (in_generator_fn)
  {
    values.resize (VALUES_BATCH_SIZE);

    FillValues ();

    //current value is filled inside FillValues call above
    finish_value = &values [values.size ()];
  }

  float Generate ()
  {
    if (current_value != finish_value)
      return *current_value++;

    FillValues ();

    return *current_value++;
  }

  void FillValues ()
  {
    generator_fn (VALUES_BATCH_SIZE, &values [0]);

    current_value = &values [0];
  }
};

/*
    Constructors / destructor / assignment
*/

RandomGenerator::RandomGenerator ()
  : impl (new Impl (DefaultRandomGenerator ()))
{
}

RandomGenerator::RandomGenerator (const RandomFunction& fn)
  : impl (new Impl (fn))
{
}

RandomGenerator::RandomGenerator (const RandomGenerator& generator)
  : impl (generator.impl)
{
  addref (impl);
}

RandomGenerator::~RandomGenerator ()
{
  release (impl);
}

RandomGenerator& RandomGenerator::operator = (const RandomGenerator& generator)
{
  RandomGenerator (generator).Swap (*this);
  return *this;
}

/*
    Generate single float in range [0; 1]
*/

float RandomGenerator::Generate () const
{
  return impl->Generate ();
}

/*
    Swap
*/

void RandomGenerator::Swap (RandomGenerator& generator)
{
  stl::swap (impl, generator.impl);
}

namespace media {
namespace particles {

void swap (RandomGenerator& generator1, RandomGenerator& generator2)
{
  generator1.Swap (generator2);
}

}}
