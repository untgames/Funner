#include "shared.h"

using namespace media;
using namespace stl;
using namespace common;

const size_t SAMPLE_ARRAY_RESERVE_SIZE = 4;    //резервируемое количество сэмплов
const float  INFINITY_DISTANCE         = 1e6f; //расстояние "бесконечности"

/*
    Описание реализации SoundDeclaration
*/

struct SoundDeclaration::Impl: public xtl::reference_counter
{
  string          name;                    //имя
  string          type;                    //тип звука
  bool            looping;                 //цикличность
  vector <string> samples;                 //список сэмплов
  float           params [SoundParam_Num]; //параметры

  
  Impl ();
};

/*
    Impl::Impl
*/

SoundDeclaration::Impl::Impl ()
{
  samples.reserve (SAMPLE_ARRAY_RESERVE_SIZE);
  
  params [SoundParam_Gain]              = 1.0f;
  params [SoundParam_MinimumGain]       = 0.0f;
  params [SoundParam_MaximumGain]       = 1.0f;
  params [SoundParam_InnerAngle]        = 360.0f;
  params [SoundParam_OuterAngle]        = 360.0f;
  params [SoundParam_OuterGain]         = 1.0f;
  params [SoundParam_ReferenceDistance] = INFINITY_DISTANCE;
  params [SoundParam_MaximumDistance]   = INFINITY_DISTANCE;
  params [SoundParam_CullDistance]      = INFINITY_DISTANCE;
  
  looping = false;
}

/*
    Конструкторы / деструктор
*/

SoundDeclaration::SoundDeclaration ()
  : impl (new Impl, false)
  {}

SoundDeclaration::SoundDeclaration (const SoundDeclaration& sound_decl, CloneMode mode)
  : impl (clone (sound_decl.impl, mode, "media::SoundDeclaration::SoundDeclaration"))
  {}

SoundDeclaration::~SoundDeclaration ()
{
}

/*
    Присваивание
*/

SoundDeclaration& SoundDeclaration::operator = (const SoundDeclaration& sound_decl)
{
  impl = sound_decl.impl;

  return *this;
}

/*
    Имя
*/

const char* SoundDeclaration::Name () const
{
  return impl->name.c_str ();
}

void SoundDeclaration::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::SoundDeclaration::Rename", "name");
    
  impl->name = name;
}

/*
    Тип звука
*/

const char* SoundDeclaration::Type () const
{
  return impl->type.c_str ();
}

void SoundDeclaration::SetType (const char* type)
{
  if (!type)
    RaiseNullArgument ("media::SoundDeclaration::SetType", "type");

  impl->type = type;
}
  
/*
    Цикличность воспроизведения
*/

void SoundDeclaration::SetLooping (bool looping)
{
  impl->looping = looping;
}

bool SoundDeclaration::Looping () const
{
  return impl->looping;
}
  
/*
    Работа с сэмплами
*/

size_t SoundDeclaration::SamplesCount () const
{
  return impl->samples.size ();
}

const char* SoundDeclaration::Sample (size_t sample_index) const
{
  if (sample_index >= impl->samples.size ())
    RaiseOutOfRange ("media::SoundDeclaration::Sample", "sample_index", sample_index, impl->samples.size ());

  return impl->samples [sample_index].c_str ();
}

size_t SoundDeclaration::AddSample (const char* sample_name)
{
  if (!sample_name)
    RaiseNullArgument ("media::SoundDeclaration::AddSample", "sample_name");

  impl->samples.push_back (sample_name);

  return impl->samples.size () - 1;
}

void SoundDeclaration::RemoveSample (size_t sample_index)
{
  if (sample_index >= impl->samples.size ())
    return;

  impl->samples.erase (impl->samples.begin () + sample_index);
}

void SoundDeclaration::RemoveAllSamples ()
{
  impl->samples.clear ();
}

/*
    Параметры
*/

void SoundDeclaration::SetParam (SoundParam param, float value)
{
  if (param < 0 || param >= SoundParam_Num)
    RaiseInvalidArgument ("media::SoundDeclaration::SetParam", "param", param);
    
  impl->params [param] = value;
}

float SoundDeclaration::Param (SoundParam param) const
{
  if (param < 0 || param >= SoundParam_Num)
    RaiseInvalidArgument ("media::SoundDeclaration::Param", "param", param);

  return impl->params [param];
}
     
/*
    Обмен
*/

void SoundDeclaration::Swap (SoundDeclaration& sound_decl)
{
  swap (impl, sound_decl.impl);
}

namespace media
{

void swap (SoundDeclaration& sound_decl1, SoundDeclaration& sound_decl2)
{
  sound_decl1.Swap (sound_decl2);
}

}
