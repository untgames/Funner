#include <vorbis/vorbisfile.h>
#include "shared.h"

using namespace media;
using namespace common;

namespace
{

class OggCodec : public media::SoundCodec
{
  public:
    OggCodec  (const char* file_name, media::SoundSampleInfo& sound_sample_info);
    ~OggCodec () {ov_clear (&vf);}

    size_t Read (size_t first_sample, size_t samples_count, void* data);

    OggCodec* Clone ();

    common::StdFile* file;
    OggVorbis_File   vf;
};

size_t OggReadFunc (void* data, size_t size, size_t count, void* file_ptr)
{
  size_t ret_value = 0;

  if (size == 0 || count == 0)
    return 0;

  try
  {
    ret_value = ((File*)file_ptr)->Read (data, size * count);
  }
  catch (std::exception& exception)
  {
    SoundSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    SoundSystemSingleton::Instance ().DebugLog ("Unknown exception at OggReadFunc.");
  }

  return ret_value / size;
}

int OggSeekFunc (void* file_ptr, ogg_int64_t offset, int origin)
{
  FileSeekMode seek_mode[3] = {FILE_SEEK_SET, FILE_SEEK_CUR, FILE_SEEK_END};

  try
  {
     filepos_t seek_pos;

     switch (seek_mode[origin])
     {
       case FILE_SEEK_SET: seek_pos = (filepos_t)offset; break;
       case FILE_SEEK_CUR: seek_pos = ((StdFile*)file_ptr)->Tell () + (filepos_t)offset; break;
       case FILE_SEEK_END: seek_pos = ((StdFile*)file_ptr)->Size () + (filepos_t)offset; break;
     } 
     return seek_pos != ((StdFile*)file_ptr)->Seek ((filepos_t)offset, seek_mode[origin]);  
  }
  catch (std::exception& exception)
  {
    SoundSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    SoundSystemSingleton::Instance ().DebugLog ("Unknown exception at OggSeekFunc.");
  }
  return 1;
}

int OggCloseFunc (void* file_ptr)
{
  ((File*)file_ptr)->Close ();
  return 0;
}

long OggTellFunc (void *file_ptr)
{
  int ret_value;

  try
  {
     ret_value = ((File*)file_ptr)->Tell ();
  }
  catch (std::exception& exception)
  {
    SoundSystemSingleton::Instance ().DebugLog (exception.what ());
  }
  catch (...)
  {
    SoundSystemSingleton::Instance ().DebugLog ("Unknown exception at OggTellFunc.");
  }

  return ret_value;
}

void SwapSamples (short &s1, short &s2)
{
  short sTemp = s1;
  s1 = s2;
  s2 = sTemp;
}

}

OggCodec::OggCodec (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  file = new StdFile (file_name, FILE_MODE_READ_ONLY);

  if (!file)
    Raise <Exception> ("OggCodec::OggCodec", "Can't open file '%s'.", file_name);

  ov_callbacks callbacks = {OggReadFunc, OggSeekFunc, OggCloseFunc, OggTellFunc};
  int          ret_code;

  ret_code = ov_open_callbacks (file, &vf, NULL, 0, callbacks);

  if (ret_code < 0) 
    Raise <Exception> ("OggCodec::OggCodec", "Can't open ogg file, seems to be not ogg bitstream (return code %d)", ret_code);

  vorbis_info *vi = ov_info (&vf, -1);
  sound_sample_info.frequency       = vi->rate;
  sound_sample_info.channels        = vi->channels;
  sound_sample_info.samples_count   = (size_t) ov_pcm_total (&vf, -1);
  sound_sample_info.bits_per_sample = 16;
}

size_t OggCodec::Read (size_t first_sample, size_t samples_count, void* data)
{
  size_t ret_value;

  vorbis_info* vi = ov_info (&vf, -1);
  size_t       decoded_bytes = 0, readed_bytes;
  int          current_section;       
  size_t       buffer_size = samples_count * 2 * vi->channels;

  ov_pcm_seek (&vf, first_sample);

  while (1)
  {
    readed_bytes = ov_read (&vf, (char*)data + decoded_bytes, buffer_size - decoded_bytes, 0, 2, 1, &current_section);
    if (readed_bytes > 0)
    {
      decoded_bytes += readed_bytes;
      if (decoded_bytes >= buffer_size)
        break;
    }
    else 
      break;
  }

  if (vi->channels == 6)
  {   
    short* pSamples = (short*)data;
    for (size_t ulSamples = 0; ulSamples < (buffer_size>>1); ulSamples+=6)
    {
      // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
      // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
      SwapSamples(pSamples[ulSamples+1], pSamples[ulSamples+2]);
      SwapSamples(pSamples[ulSamples+3], pSamples[ulSamples+5]);
      SwapSamples(pSamples[ulSamples+4], pSamples[ulSamples+5]);
    }
  }

  ret_value = decoded_bytes / (2 * vi->channels);

  return ret_value;
}

OggCodec* OggCodec::Clone ()
{
  OggCodec* ret_value = new OggCodec (*this);

  ret_value->file = new StdFile (*file);

  ov_callbacks callbacks = {OggReadFunc, OggSeekFunc, OggCloseFunc, OggTellFunc};

  ret_value->file->Seek (0, FILE_SEEK_SET);

  ov_open_callbacks (ret_value->file, &(ret_value->vf), NULL, 0, callbacks);

  return ret_value;
}

namespace media
{

SoundCodec* SoundSample::DefaultOggLoader (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  return new OggCodec (file_name, sound_sample_info);
}

}
