#include "shared.h"
#include <common/file.h>

using namespace medialib;

namespace
{

void ResetSoundSample (SoundSample* sound)
{
  SoundSample().Swap (*sound);
}

void DefaultLogFunction (const char* log_message)
{
  printf ("%s\n", log_message);
}

}

SoundSampleSystemImpl::SoundSampleSystemImpl ()
{
  log_function = DefaultLogFunction;
  RegisterLoadFunc ("ogg", &SoundSample::DefaultOggLoader);
  RegisterLoadFunc ("wav", &SoundSample::DefaultWavLoader);
}

SoundSampleSystemImpl::~SoundSampleSystemImpl ()
{
  CloseAllSoundSamples ();
}

void SoundSampleSystemImpl::RegisterSoundSample (SoundSample& sound)
{
  open_sound_samples.insert (&sound);
}

void SoundSampleSystemImpl::UnregisterSoundSample (SoundSample& sound)
{
  open_sound_samples.erase (&sound);
}

void SoundSampleSystemImpl::CloseAllSoundSamples ()
{
  for_each (open_sound_samples.begin (),open_sound_samples.end (), ResetSoundSample);
}

void SoundSampleSystemImpl::SetDebugLog (const design::function<void (const char*)>& new_log_function)
{
  log_function = new_log_function;
}

void SoundSampleSystemImpl::DebugLog (const char* debug_message)
{
  log_function (debug_message);
}

bool SoundSampleSystemImpl::RegisterLoadFunc (const char* extension, const SoundSampleSystem::CodecLoadFunc& load_codec)
{
  return (load_codecs.insert_pair (extension, load_codec)).second;
}

void SoundSampleSystemImpl::UnregisterLoadFunc (const char* extension)
{
  load_codecs.erase (stl::string (extension));
}

void SoundSampleSystemImpl::UnregisterAllFuncs ()
{
  load_codecs.clear ();
}

SoundSampleSystem::CodecLoadFunc* SoundSampleSystemImpl::GetLoadFunc (const char* extension)
{
  stl::hash_map<stl::string, SoundSampleSystem::CodecLoadFunc>::iterator load_func = load_codecs.find (stl::string (extension));

  if (load_func == load_codecs.end ())
    return NULL;
  return &load_func->second;
}

namespace medialib
{

void SoundSampleSystem::SetDebugLog (const design::function<void (const char*)>& log_function)
{
  SoundSystemSingleton::Instance ().SetDebugLog (log_function);
}

bool SoundSampleSystem::RegisterCodec (const char* extension, const CodecLoadFunc& load_codec)
{
  return SoundSystemSingleton::Instance ().RegisterLoadFunc (extension, load_codec);
}

void SoundSampleSystem::UnregisterCodec (const char* extension)
{
  SoundSystemSingleton::Instance ().UnregisterLoadFunc (extension);
}

void SoundSampleSystem::UnregisterAllCodecs ()
{
  SoundSystemSingleton::Instance ().UnregisterAllFuncs ();
}

}
