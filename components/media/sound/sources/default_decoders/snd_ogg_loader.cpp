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
    size_t            channels_count;
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

  return LogHolderSingleton::Instance ()->log;
}

void log_exception (const char* source, std::exception& exception)
{
  get_log ().Printf ("Exception at %s: %s", source, exception.what ());
}

void log_exception (const char* source)
{
  get_log ().Printf ("Unknown exception at %s", source);
}

size_t ogg_read_func (void* data, size_t size, size_t count, void* file_ptr)
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
    log_exception ("::ogg_read_func", exception);
  }
  catch (...)
  {
    log_exception ("::ogg_read_func");
  }

  return ret_value / size;
}

int ogg_seek_func (void* file_ptr, ogg_int64_t offset, int origin)
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
    log_exception ("::ogg_seek_func", exception);
  }
  catch (...)
  {
    log_exception ("::ogg_seek_func");
  }
  return 1;
}

long ogg_tell_func (void *file_ptr)
{
  try
  {
    return ((File*)file_ptr)->Tell ();
  }
  catch (std::exception& exception)
  {
    log_exception ("::ogg_tell_func", exception);
  }
  catch (...)
  {
    log_exception ("::ogg_tell_func");
  }

  return 0;
}

void swap_samples (short &sample1, short &sample2)
{
  short temp = sample1;

  sample1 = sample2;
  sample2 = temp;
}

const char* get_vorbis_file_error_name (int error_code)
{
  switch (error_code)
  {
    case OV_EREAD:      return "Read error while fetching compressed data for decode";
    case OV_EFAULT:     return "Internal inconsistency in decode state";
    case OV_EIMPL:      return "Feature not implemented";
    case OV_EINVAL:     return "Either an invalid argument, or incompletely initialized argument";
    case OV_ENOTVORBIS: return "The given file/data was not recognized as Ogg Vorbis data";
    case OV_EBADHEADER: return "The file/data is apparently an Ogg Vorbis stream, but contains a corrupted or undecipherable header";
    case OV_EVERSION:   return "The bitstream format revision of the given stream is not supported";
    case OV_EBADLINK:   return "The given link exists in the Vorbis data stream, but is not decipherable due to garbacge or corruption";
    case OV_ENOSEEK:    return "The given stream is not seekable";
    default:            return "Unknown error";
  }
}

void check_vorbis_file_error (int error_code, const char* source, const char* message)
{
  if (error_code)
    throw xtl::format_operation_exception (source, "%s. Vorbis file error: %s (code %d)", message,
                                           get_vorbis_file_error_name (error_code), error_code);
}

OggInputStream::OggInputStream (const char* file_name, SoundSampleInfo& sound_sample_info)
  : file (file_name)
{
  static const char* METHOD_NAME = "media::sound::OggInputStream::OggInputStream";

  ov_callbacks callbacks = {ogg_read_func, ogg_seek_func, 0, ogg_tell_func};

  check_vorbis_file_error (ov_open_callbacks (&file, &vf, NULL, 0, callbacks), METHOD_NAME, "Can't open vorbis file, error at ::ov_open_callbacks");

  vorbis_info *vi = ov_info (&vf, -1);

  if (!vi)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get vorbis file info, error at ::ov_info");

  sound_sample_info.frequency       = vi->rate;
  sound_sample_info.channels        = channels_count = vi->channels;
  sound_sample_info.bits_per_sample = 16;

  ogg_int64_t samples_count = ov_pcm_total (&vf, -1);

  if (samples_count == OV_EINVAL)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get samples count, the requested bitstream did not exist or the bitstream is unseekable");

  sound_sample_info.samples_count = samples_count;
}

size_t OggInputStream::Read (size_t first_sample, size_t samples_count, void* data)
{
  static const char* METHOD_NAME = "media::sound::OggInputStream::Read";

  size_t ret_value, readed_bytes, decoded_bytes = 0, buffer_size = samples_count * 2 * channels_count;
  int    current_section;

  if (first_sample != ov_pcm_tell (&vf))
    check_vorbis_file_error (ov_pcm_seek (&vf, first_sample), METHOD_NAME, "Can't seek in vorbis file, error at ::ov_pcm_seek");

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
    {
      check_vorbis_file_error (readed_bytes, METHOD_NAME, "Can't read data from vorbis file, error at ::ov_read");
      break;
    }
  }

  if (channels_count == 6)
  {
    short* samples = (short*)data;

    for (size_t current_sample = 0; current_sample < (buffer_size >> 1); current_sample += 6)
    {
      // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
      // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
      swap_samples (samples [current_sample + 1], samples [current_sample + 2]);
      swap_samples (samples [current_sample + 3], samples [current_sample + 5]);
      swap_samples (samples [current_sample + 4], samples [current_sample + 5]);
    }
  }

  ret_value = decoded_bytes / (2 * channels_count);

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
