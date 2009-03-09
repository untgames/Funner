#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "media.sound.ogg"; //имя потока протоколирования

//поток декодированияogg
class OggInputStream : public media::ISoundInputStream, public xtl::reference_counter
{
  public:
    OggInputStream  (const char* file_name, media::SoundSampleInfo& sound_sample_info);
    ~OggInputStream () {ov_clear (&vf);}

    size_t Read (size_t first_sample, size_t samples_count, void* data);

    void AddRef () { addref (this); }
    void Release () { release (this); }

    common::InputFile file;
    OggVorbis_File    vf;
};

/*
    Протоколирование исключения
*/

struct LogHolder
{
  LogHolder () : log (LOG_NAME) {}

  Log log;
};

Log& get_log ()
{
  typedef common::Singleton<LogHolder> LogHolderSingleton;

  return LogHolderSingleton::Instance ().log;
}

void log_exception (const char* source, std::exception& exception)
{
  get_log ().Printf ("Exception at %s: %s", source, exception.what ());
}

void log_exception (const char* source)
{
  get_log ().Printf ("Unknown exception at %s", source);
}

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
    log_exception ("::OggReadFunc", exception);
  }
  catch (...)
  {
    log_exception ("::OggReadFunc");
  }

  return ret_value / size;
}

int OggSeekFunc (void* file_ptr, ogg_int64_t offset, int origin)
{
  FileSeekMode seek_mode[3] = {FileSeekMode_Set, FileSeekMode_Current, FileSeekMode_End};

  try
  {
     filepos_t seek_pos;

     switch (seek_mode[origin])
     {
       default:
       case FileSeekMode_Set:     seek_pos = (filepos_t)offset; break;
       case FileSeekMode_Current: seek_pos = ((StdFile*)file_ptr)->Tell () + (filepos_t)offset; break;
       case FileSeekMode_End:     seek_pos = ((StdFile*)file_ptr)->Size () + (filepos_t)offset; break;
     }
     return seek_pos != ((StdFile*)file_ptr)->Seek ((filepos_t)offset, seek_mode[origin]);
  }
  catch (std::exception& exception)
  {
    log_exception ("::OggSeekFunc", exception);
  }
  catch (...)
  {
    log_exception ("::OggSeekFunc");
  }
  return 1;
}

long OggTellFunc (void *file_ptr)
{
  try
  {
    return ((File*)file_ptr)->Tell ();
  }
  catch (std::exception& exception)
  {
    log_exception ("::OggTellFunc", exception);
  }
  catch (...)
  {
    log_exception ("::OggTellFunc");
  }

  return 0;
}

void SwapSamples (short &s1, short &s2)
{
  short sTemp = s1;
  s1 = s2;
  s2 = sTemp;
}

OggInputStream::OggInputStream (const char* file_name, SoundSampleInfo& sound_sample_info)
  : file (file_name, FileMode_ReadOnly)
{
  ov_callbacks callbacks = {OggReadFunc, OggSeekFunc, 0, OggTellFunc};
  int          ret_code;

  ret_code = ov_open_callbacks (&file, &vf, NULL, 0, callbacks);

  if (ret_code < 0)
    throw xtl::format_operation_exception ("OggCodec::OggCodec", "Can't open ogg file, seems to be not ogg bitstream (return code %d)", ret_code);

  vorbis_info *vi = ov_info (&vf, -1);
  sound_sample_info.frequency       = vi->rate;
  sound_sample_info.channels        = vi->channels;
  sound_sample_info.samples_count   = (size_t) ov_pcm_total (&vf, -1);
  sound_sample_info.bits_per_sample = 16;
}

size_t OggInputStream::Read (size_t first_sample, size_t samples_count, void* data)
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

ISoundInputStream* default_ogg_loader (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  return new OggInputStream (file_name, sound_sample_info);
}

/*
   Компонент работы с звуками
*/

class OggLoaderComponent
{
  public:
    //загрузка компонента
    OggLoaderComponent ()
    {
      SoundSampleManager::RegisterLoader ("ogg", &default_ogg_loader);
    }
};

extern "C"
{

ComponentRegistrator<OggLoaderComponent> OggLoader ("media.sound.loaders.Ogg");

}

}
