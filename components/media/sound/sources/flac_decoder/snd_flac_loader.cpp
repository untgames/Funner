#include <FLAC/stream_decoder.h>

#include <xtl/reference_counter.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>

#include <media/sound.h>

using namespace media;
using namespace common;

namespace components
{

namespace flac_loader
{

/*
    Constants
*/

const char* LOG_NAME = "media.sound.flac"; //log stream name

//Decoding stream
class FlacInputStream : public media::ISoundInputStream, public xtl::reference_counter
{
  public:
    FlacInputStream  (const char* file_name, media::SoundSampleInfo& sound_sample_info);
    ~FlacInputStream ();

    size_t Read (size_t first_sample, size_t samples_count, void* data);

    void AddRef () { addref (this); }
    void Release () { release (this); }

    common::InputFile                file;                         //source file
    FLAC__StreamDecoder              *decoder;                     //flac decoder
    xtl::uninitialized_storage<char> excessive_data;               //buffer for excessive decoded data
    size_t                           excessive_data_first_sample;  //index of first decoded sample in buffer
    size_t                           excessive_data_samples_count; //count of decoded samples in buffer
    size_t                           current_sample_index;         //current stream sample index
    char*                            output_buffer;                //target decoding buffer
    size_t                           output_buffer_size;           //size of out_buffer
    size_t                           sample_size;                  //size of one sample
    size_t                           frequency;                    //stream frequency
    size_t                           channels_count;               //stream channels count
    size_t                           bits_per_sample;              //stream bits per sample
    size_t                           samples_count;                //total stream samples count
};

/*
    Exceptions logging
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

FLAC__StreamDecoderReadStatus flac_read_func (const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
  if (*bytes == 0)
    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;

  try
  {
    *bytes = ((FlacInputStream*)client_data)->file.Read (buffer, sizeof (FLAC__byte) * *bytes);

    if (*bytes)
      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    else
      return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_read_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_read_func");
  }

  return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
}

FLAC__StreamDecoderSeekStatus flac_seek_func (const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
  try
  {
     if (absolute_byte_offset == (size_t)((FlacInputStream*)client_data)->file.Seek ((filepos_t)absolute_byte_offset))
       return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
     else
       return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_seek_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_seek_func");
  }

  return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
}

FLAC__StreamDecoderTellStatus flac_tell_func (const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
  try
  {
    *absolute_byte_offset = ((FlacInputStream*)client_data)->file.Tell ();

    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_tell_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_tell_func");
  }

  return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
}

FLAC__StreamDecoderLengthStatus flac_length_func (const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data)
{
  try
  {
    *stream_length = ((FlacInputStream*)client_data)->file.Size ();

    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_length_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_length_func");
  }

  return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
}

FLAC__bool flac_eof_func (const FLAC__StreamDecoder *decoder, void *client_data)
{
  try
  {
    return ((FlacInputStream*)client_data)->file.Eof ();
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_eof_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_eof_func");
  }

  return true;
}

void copy_flac_decoded_data (const FLAC__int32 * const buffer[], char* destination, size_t channels_count, size_t sample_size, size_t first_sample, size_t samples_count)
{
  for (size_t i = first_sample, last_sample = first_sample + samples_count; i < last_sample; i++)
  {
    for (size_t j = 0; j < channels_count; j++)
    {
      memcpy (destination, buffer [j] + i, sample_size); //endianness????
      destination += sample_size;
    }
  }
}

FLAC__StreamDecoderWriteStatus flac_write_func (const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
  try
  {
    if (frame->header.number_type != FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER)
      throw xtl::format_operation_exception ("", "Unsupported frame number type %d", frame->header.number_type);

    FlacInputStream* stream = (FlacInputStream*)client_data;

    if (!stream->sample_size)
    {
      stream->frequency       = frame->header.sample_rate;
      stream->channels_count  = frame->header.channels;
      stream->bits_per_sample = frame->header.bits_per_sample;
      stream->sample_size     = stream->bits_per_sample / 8;
    }
    else
    {
      if (stream->frequency != frame->header.sample_rate)
        throw xtl::format_operation_exception ("", "Sample rate changed to %u", frame->header.sample_rate);
      if (stream->channels_count != frame->header.channels)
        throw xtl::format_operation_exception ("", "Channels count changed to %u", frame->header.channels);
      if (stream->bits_per_sample != frame->header.bits_per_sample)
        throw xtl::format_operation_exception ("", "Bits per sample changed to %u", frame->header.bits_per_sample);
    }

    size_t decoded_samples_count = frame->header.blocksize;
    size_t copied_samples        = 0;

    if (stream->output_buffer && stream->output_buffer_size)
    {
      copied_samples = stl::min (stream->output_buffer_size, decoded_samples_count);

      copy_flac_decoded_data (buffer, stream->output_buffer, stream->channels_count, stream->sample_size, 0, copied_samples);

      stream->output_buffer += stream->sample_size * copied_samples * stream->channels_count;

      stream->output_buffer_size -= copied_samples;
    }

    if (copied_samples < decoded_samples_count)
    {
      size_t excessive_samples_count = decoded_samples_count - copied_samples,
             excessive_data_size     = excessive_samples_count * stream->channels_count * stream->sample_size;

      if (excessive_data_size > stream->excessive_data.size ())
        stream->excessive_data.resize (excessive_data_size, false);

      copy_flac_decoded_data (buffer, stream->excessive_data.data (), stream->channels_count, stream->sample_size, copied_samples, excessive_samples_count);

      stream->excessive_data_samples_count = excessive_samples_count;
      stream->excessive_data_first_sample  = (size_t)frame->header.number.sample_number + copied_samples;
    }

    stream->current_sample_index = (size_t)frame->header.number.sample_number + decoded_samples_count;

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
  }
  catch (std::exception& exception)
  {
    log_exception ("::flac_write_func", exception);
  }
  catch (...)
  {
    log_exception ("::flac_write_func");
  }

  return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

void flac_error_func (const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
  get_log ().Printf ("flac_error_func caled with error '%s'", FLAC__StreamDecoderErrorStatusString [status]);
}

FlacInputStream::FlacInputStream (const char* file_name, SoundSampleInfo& sound_sample_info)
  : file (file_name)
  , excessive_data_first_sample (0)
  , excessive_data_samples_count (0)
  , current_sample_index (0)
  , output_buffer (0)
  , output_buffer_size (0)
  , sample_size (0)
  , frequency (0)
  , channels_count (0)
  , bits_per_sample (0)
  , samples_count (0)
{
  try
  {
    decoder = FLAC__stream_decoder_new ();

    if (!decoder)
      throw xtl::format_operation_exception ("::FLAC__stream_decoder_new", "Can't create new decoder");

    FLAC__StreamDecoderInitStatus init_status = FLAC__stream_decoder_init_stream (decoder, flac_read_func,
                                                  flac_seek_func, flac_tell_func, flac_length_func,
                                                  flac_eof_func, flac_write_func, 0, flac_error_func, this);

    if (init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
      throw xtl::format_operation_exception ("::FLAC__stream_decoder_init_stream", FLAC__StreamDecoderInitStatusString [init_status]);

    for (;;)
    {
      if (!FLAC__stream_decoder_process_single (decoder)) //decode first frame to get stream info
        throw xtl::format_operation_exception ("::FLAC__stream_decoder_process_single", "Can't decode initial frames, error '%s'", FLAC__StreamDecoderStateString [FLAC__stream_decoder_get_state (decoder)]);

      if (sample_size)
        break;
    }

    if (channels_count > 2)
      throw xtl::format_operation_exception ("", "File has unsupported number of channels %u", channels_count);

    if (bits_per_sample != 16)
      throw xtl::format_operation_exception ("", "File has unsupported bits_per_sample %u", bits_per_sample);

    samples_count = (size_t)FLAC__stream_decoder_get_total_samples (decoder);

    if (!samples_count)
      throw xtl::format_operation_exception ("::FLAC__stream_decoder_get_total_samples", "Can't get samples count");

    sound_sample_info.frequency       = frequency;
    sound_sample_info.channels        = channels_count;
    sound_sample_info.bits_per_sample = bits_per_sample;
    sound_sample_info.samples_count   = samples_count;
  }
  catch (xtl::exception& e)
  {
    if (decoder)
      FLAC__stream_decoder_delete (decoder);

    e.touch ("media::sound::FlacInputStream::FlacInputStream (%s)", file_name);
    throw;
  }
}

FlacInputStream::~FlacInputStream ()
{
  FLAC__stream_decoder_delete (decoder);
}

size_t FlacInputStream::Read (size_t first_sample, size_t samples_count, void* data)
{
  if (!samples_count)
    return 0;

  output_buffer      = (char*)data;
  output_buffer_size = samples_count;

  if (first_sample >= excessive_data_first_sample && first_sample < (excessive_data_first_sample + excessive_data_samples_count)) //no seek needed
  {
    size_t buffer_samples_offset = first_sample - excessive_data_first_sample,
           buffer_bytes_offset   = buffer_samples_offset * sample_size * channels_count,
           copy_samples          = stl::min (excessive_data_samples_count - buffer_samples_offset, samples_count),
           copy_bytes            = copy_samples * sample_size * channels_count;

    memcpy (data, excessive_data.data () + buffer_bytes_offset, copy_bytes);

    if (copy_samples == samples_count)
      return samples_count;

    output_buffer      += copy_bytes;
    output_buffer_size -= copy_samples;
  }
  else if (current_sample_index != first_sample)
  {
    if (!FLAC__stream_decoder_seek_absolute (decoder, first_sample))
    {
      FLAC__StreamDecoderState state = FLAC__stream_decoder_get_state (decoder);

      FLAC__stream_decoder_flush (decoder);

      throw xtl::format_operation_exception ("::FLAC__stream_decoder_seek_absolute", FLAC__StreamDecoderStateString [state]);
    }

    current_sample_index = first_sample;
  }

  for (;output_buffer_size;)
  {
    if (!FLAC__stream_decoder_process_single (decoder)) //decode first frame to get stream info
      throw xtl::format_operation_exception ("::FLAC__stream_decoder_process_single", FLAC__StreamDecoderStateString [FLAC__stream_decoder_get_state (decoder)]);

    if (FLAC__stream_decoder_get_state (decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
      break;
  }

  return samples_count - output_buffer_size;
}

ISoundInputStream* default_flac_loader (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  return new FlacInputStream (file_name, sound_sample_info);
}

/*
   Flac decoding component
*/

class FlacLoaderComponent
{
  public:
    //Component loading
    FlacLoaderComponent ()
    {
      SoundSampleManager::RegisterLoader ("flac", &default_flac_loader);
    }
};

extern "C"
{

ComponentRegistrator<FlacLoaderComponent> FlacLoader ("media.sound.loaders.flac");

}

}

}
