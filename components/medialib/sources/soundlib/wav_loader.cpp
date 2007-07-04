#include "shared.h"

using namespace common;
using namespace medialib;

namespace
{

class WavCodec : public medialib::SoundCodec 
{
  public:
    WavCodec (const char* file_name, medialib::SoundSampleInfo& sound_sample_info);
    ~WavCodec () {file->Close ();}

    size_t Read (size_t first_sample, size_t samples_count, void* data);

    WavCodec* Clone ();
    
    common::StdFile* file;
    size_t           data_chunk_pos;
    size_t           sample_size;
    size_t           chunk_samples;
};

int SeekToWAVChunk (StdFile* file, const char* chunk_name)
{
  char read_buffer[8];
  int  ret_value;

  file->Seek (12, FILE_SEEK_SET);
  file->Read (read_buffer, 8);
  while (_strnicmp(chunk_name, read_buffer, 4) && !file->Eof())
  {
    file->Seek ((*((int*)(read_buffer + 4))), FILE_SEEK_CUR);
    file->Read (read_buffer, 8);  
  }

  if (file->Eof())
    ret_value = -1;
  else
    ret_value = *(int*)(read_buffer + 4);

  return ret_value;
}

}

WavCodec::WavCodec (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  file = new StdFile (file_name, FILE_MODE_READ_ONLY);

  if (!file)
    Raise <Exception> ("WavCodec::WavCodec", "Can't open file '%s'.", file_name);

  char read_buffer[16];
  int  chunk_size;    

  file->Seek (0, FILE_SEEK_SET);
  file->Read (read_buffer, 16);
  if (_strnicmp("RIFF", read_buffer, 4))
    Raise <Exception> ("WavCodec::WavCodec", "No 'RIFF' chunk in file.");
  if (_strnicmp("WAVE", read_buffer+8, 4))
    Raise <Exception> ("WavCodec::WavCodec", "No 'WAVE' ID in file.");

  if (SeekToWAVChunk (file, "fmt ") < 0)
    Raise <Exception> ("WavCodec::WavCodec", "No 'fmt ' chunk in file.");
  file->Read (read_buffer, 16);
  if (*(short*)(read_buffer) != 1)
    RaiseNotSupported ("WavCodec::WavCodec", "Compressed wav currently not supported.");
  sound_sample_info.channels        = *(short*)(read_buffer + 2);
  sound_sample_info.frequency       = *(int*)  (read_buffer + 4);
  sound_sample_info.bits_per_sample = *(short*)(read_buffer + 14);

  chunk_size = SeekToWAVChunk (file, "data");
  if (chunk_size < 0)
    Raise <Exception> ("WavCodec::WavCodec", "No 'data' chunk in file.");
  data_chunk_pos = file->Tell();
  sample_size    = sound_sample_info.bits_per_sample / 8 * sound_sample_info.channels;

  sound_sample_info.samples_count = chunk_samples = chunk_size / sample_size;
}

size_t WavCodec::Read (size_t first_sample, size_t samples_count, void* data)
{
  size_t ret_value;

  file->Seek ((filepos_t)data_chunk_pos + (filepos_t)(sample_size * first_sample), FILE_SEEK_SET);
  ret_value = file->Read (data, (size_t)(stl::min(chunk_samples - first_sample, samples_count) * sample_size));
  ret_value /= sample_size;

  return ret_value;
}

WavCodec* WavCodec::Clone ()
{
  WavCodec* ret_value = new WavCodec (*this);

  ret_value->file = new StdFile (*file);

  return ret_value;
}

namespace medialib
{

SoundCodec* SoundSample::DefaultWavLoader (const char* file_name, SoundSampleInfo& sound_sample_info)
{
  return new WavCodec (file_name, sound_sample_info);
}

}
