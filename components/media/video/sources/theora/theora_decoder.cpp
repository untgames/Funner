#include <ogg/ogg.h>
#include <theora/theoradec.h>

#include <xtl/common_exceptions.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>

#include <media/video.h>

using namespace media;

namespace
{

const char* LOG_NAME = "media.TheoraDecoder";

const long OGG_BUFFER_SIZE       = 1024 * 256;
const long LAST_PACKET_SEEK_STEP = -128;        //шаг для поиска страницы по файлу в обратном направлении

unsigned char clamp_to_unsigned_char (int x)
{
    return x < 0x00 ? 0x00 : (x > 0xff ? 0xff : (unsigned char)x);
}

void YCbCr2RGBA (size_t yuv_pixel, IVideoDecoder::Pixel& result)
{
    unsigned char *yuv = (unsigned char*)&yuv_pixel;

    int OY = 298 * (yuv [2] - 16);
    int U  = yuv [1] - 128;
    int V  = yuv [0] - 128;

    result.red   = clamp_to_unsigned_char ((OY + 409 * V + 128) >> 8);           // R
    result.green = clamp_to_unsigned_char ((OY - 100 * U - 208 * V + 128) >> 8); // G
    result.blue  = clamp_to_unsigned_char ((OY + 516 * U + 128) >> 8);           // B
    result.alpha = 0xff;                                                         // A
}

void process_theora_YCbCr (void *ret_frame, const th_ycbcr_buffer ycbcr_buffer, int width, int height)
{
  size_t *write_ptr = (size_t*)ret_frame;

  for (int y = 0; y < height; y++)
  {
    unsigned char *y_data = ycbcr_buffer[0].data + ycbcr_buffer[0].stride * y;
    int           uv_step = y / 2;
    unsigned char *u_data = ycbcr_buffer[1].data + ycbcr_buffer[1].stride * uv_step;
    unsigned char *v_data = ycbcr_buffer[2].data + ycbcr_buffer[2].stride * uv_step;

    for (int x = 0; x < width; x++)
    {
      int uv_offset = x / 2;
      *write_ptr++ = (y_data[x] << 16) | (u_data[uv_offset] << 8) | v_data[uv_offset];
    }
  }
}

///Декодера theora видео
class TheoraDecoderImpl : public IVideoDecoder
{
  public:
///Конструктор/деструктор
    TheoraDecoderImpl (const char* file_name, VideoQuality quality)
      : log (LOG_NAME), video_file (file_name), th_data_setup_info (0), th_decoding_context (0),
        first_video_packet_index (0), total_frames_count (0), buffer_frame_number (-1),
        decoded_frame_number (-1)
    {
      static const char* METHOD_NAME = "media::TheoraDecoderImpl::TheoraDecoderImpl";

      static int ogg_stream_serial_number = 0;

      ogg_sync_init (&ogg_data_sync_state);

      if (ogg_stream_init (&video_stream, ogg_stream_serial_number++))
      {
        ogg_sync_clear (&ogg_data_sync_state);
        throw xtl::format_operation_exception (METHOD_NAME, "Can't init ogg stream");
      }

      th_info_init    (&th_data_info);
      th_comment_init (&th_data_comment);

      try
      {
        for (;;)
        {
          ReadOggPage ();

          if (ogg_page_bos (&ogg_data_page) <= 0)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't find video stream in file");

          ogg_stream_reset_serialno (&video_stream, ogg_page_serialno (&ogg_data_page));

          if (ogg_stream_pagein (&video_stream, &ogg_data_page))
            throw xtl::format_operation_exception (METHOD_NAME, "Can't submit data page to bitstream");

          for (;;)
          {
            GetOggPacket ();
            first_video_packet_index++;

            int decode_header_result = th_decode_headerin (&th_data_info, &th_data_comment, &th_data_setup_info, &ogg_data_packet);

            if (decode_header_result > 0)
              continue;

            switch (decode_header_result)
            {
              case 0:
                InitVideoDecoding (quality);
                return;
              case TH_EFAULT:
                throw xtl::format_operation_exception (METHOD_NAME, "Can't decode header - one of _info, _tc, or _setup was NULL");
              case TH_EBADHEADER:
                throw xtl::format_operation_exception (METHOD_NAME, "Can't decode header - _op was NULL, the packet was not the next header packet in the expected sequence, or the format of the header data was invalid");
              case TH_EVERSION:
                throw xtl::format_operation_exception (METHOD_NAME, "Can't decode header - The packet data was a Theora info header, but for a bitstream version not decodable with this version of libtheoradec");
              case TH_ENOTFORMAT:
                log.Printf ("Warning: found not theora stream, stream will be ignored");
                break;
              default:
                throw xtl::format_operation_exception (METHOD_NAME, "Can't decode header - unsupported return value received");
            }

            if (decode_header_result)
              break;
          }
        }
      }
      catch (...)
      {
        th_decode_free   (th_decoding_context);
        th_comment_clear (&th_data_comment);
        th_info_clear    (&th_data_info);
        ogg_stream_clear (&video_stream);
        ogg_sync_clear   (&ogg_data_sync_state);

        throw;
      }
    }

    ~TheoraDecoderImpl ()
    {
      th_decode_free   (th_decoding_context);
      th_comment_clear (&th_data_comment);
      th_info_clear    (&th_data_info);
      ogg_stream_clear (&video_stream);
      ogg_sync_clear   (&ogg_data_sync_state);
    }

///Операции
    float GetFramesPerSecond ()
    {
      return th_data_info.fps_numerator / (float)th_data_info.fps_denominator;;
    }

    size_t GetWidth ()
    {
      return th_data_info.pic_width;
    }

    size_t GetHeight ()
    {
      return th_data_info.pic_height;
    }

    size_t GetFramesCount ()
    {
      return total_frames_count;
    }

    float GetPixelAspectRatio ()
    {
      if (!th_data_info.aspect_numerator && !th_data_info.aspect_denominator)
        return 1.f;

      return th_data_info.aspect_numerator / (float)th_data_info.aspect_denominator;
    }
    
    void Decode (size_t frame, Pixel* data_buffer)
    {
      static const char* METHOD_NAME = "media::TheoraDecoderImpl::Decode";

      if (!data_buffer)
        throw xtl::make_null_argument_exception (METHOD_NAME, "data_buffer");

      if (buffer_frame_number > (int)frame)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Backward seek in theora files not supported yet");

      for (; decoded_frame_number < (int)frame;)
      {
        try
        {
          if (decoded_frame_number >= 0)  //первый пакет прочитан во время инициализации
            GetOggPacket ();

          if (th_decode_packetin (th_decoding_context, &ogg_data_packet, 0))
            throw xtl::format_operation_exception (METHOD_NAME, "Can't decode video frame");
        }
        catch (xtl::exception& e) //подавление исключений декодирования связанных с некорректными данными
        {
          log.Printf ("Exception while decoding frame %d: '%s'\n", decoded_frame_number + 1, e.what ());
        }
        catch (...)
        {
          log.Printf ("Unknown exception while decoding frame %d\n", decoded_frame_number + 1);
        }

        decoded_frame_number = (int)th_granule_frame (th_decoding_context, ogg_data_packet.granulepos);

        try
        {
          if (decoded_frame_number <= (int)frame || buffer_frame_number < 0)
          {
            if (th_decode_ycbcr_out (th_decoding_context, th_decoded_buffer))
              throw xtl::format_operation_exception (METHOD_NAME, "Can't get decoded video frame");

            process_theora_YCbCr (ycbcr_buffer.data (), th_decoded_buffer, th_data_info.frame_width, th_data_info.frame_height);

            buffer_frame_number = decoded_frame_number;
          }
        }
        catch (xtl::exception& e) //подавление исключений декодирования связанных с некорректными данными
        {
          log.Printf ("Exception while getting decoded frame %d: '%s'\n", decoded_frame_number, e.what ());
        }
        catch (...)
        {
          log.Printf ("Unknown exception while getting decoded frame %d\n", decoded_frame_number);
        }
      }

      if (buffer_frame_number < 0)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get first video frame");

      if (buffer_frame_number != frame)
        log.Printf ("Can't decode frame %u, returning data of frame %d instead", frame, buffer_frame_number);

      size_t *ycbcr_data = ycbcr_buffer.data ();

      for (int y = th_data_info.frame_height - 1; y >= 0; y--)
      {
        Pixel *current_pixel = data_buffer + y * th_data_info.frame_width;

        for (size_t x = 0; x < th_data_info.frame_width; x++, current_pixel++, ycbcr_data++)
          YCbCr2RGBA (*ycbcr_data, *current_pixel);
      }
    }

  private:
//Работа с theora
    void InitVideoDecoding (VideoQuality quality)
    {
      static const char* METHOD_NAME = "media::TheoraDecoderImpl::InitVideoDecoding";

      if ((th_data_info.frame_width != th_data_info.pic_width) || (th_data_info.frame_height != th_data_info.pic_height) ||
          th_data_info.pic_x || th_data_info.pic_y)  //??????????????
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported video format - invalid frame size. Must be multiply of 16");

      if (th_data_info.pixel_fmt != TH_PF_420)  //????????????
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported pixel format. Must be TH_PF_420");

/*????????????
      log.Printf ("Colorspace is ");
      switch (th_data_info.colorspace)
      {
        case TH_CS_UNSPECIFIED:   log.Printf ("unspecified");         break;
        case TH_CS_ITU_REC_470M:  log.Printf ("TH_CS_ITU_REC_470M");  break;
        case TH_CS_ITU_REC_470BG: log.Printf ("TH_CS_ITU_REC_470BG"); break;
        default:                  log.Printf ("unknown");             break;
      }*/

      th_decoding_context = th_decode_alloc (&th_data_info, th_data_setup_info);

      if (!th_decoding_context)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't create theora decoding context");

      th_setup_free (th_data_setup_info);

      int int_buffer = 0;

      switch (th_decode_ctl (th_decoding_context, TH_DECCTL_GET_PPLEVEL_MAX, &int_buffer, sizeof (int_buffer)))
      {
        case TH_EFAULT:
        case TH_EINVAL:
        case TH_EIMPL:
          int_buffer = 0;
          log.Printf ("Can't get maximum postprocessing level, turning postprocessing off");
          break;
        default:
        {
          switch (quality)
          {
            case VideoQuality_Low:
              int_buffer = 0;
              break;
            case VideoQuality_Middle:
              int_buffer /= 2;
              break;
            default:
              break;
          }
        }
      }

      switch (th_decode_ctl (th_decoding_context, TH_DECCTL_SET_PPLEVEL, &int_buffer, sizeof (int_buffer)))
      {
        case TH_EFAULT:
        case TH_EINVAL:
        case TH_EIMPL:
          log.Printf ("Can't set postprocessing level");
      }

      GetLastOggPacket ();

      total_frames_count = (size_t)th_granule_frame (th_decoding_context, ogg_data_packet.granulepos) + 1;

      GetOggPacket (first_video_packet_index);

      while (first_video_packet_index && th_packet_iskeyframe (&ogg_data_packet) != 1)
        GetOggPacket (--first_video_packet_index);

      ycbcr_buffer.resize (th_data_info.frame_width * th_data_info.frame_height);
    }

//Работа с контейнером
    void ReadDataToOgg ()
    {
      static const char* METHOD_NAME = "media::TheoraDecoderInternal::ReadDataToOgg";

      if (video_file.Eof ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't read data from file, file ended");

      char* buffer = ogg_sync_buffer (&ogg_data_sync_state, OGG_BUFFER_SIZE);

      size_t readed_bytes = video_file.Read (buffer, OGG_BUFFER_SIZE);

      if (ogg_sync_wrote (&ogg_data_sync_state, readed_bytes))
        throw xtl::format_operation_exception (METHOD_NAME, "ogg_sync_wrote error");
    }

    void ReadOggPage ()
    {
      static const char* METHOD_NAME = "media::TheoraDecoderInternal::ReadOggPage";

      switch (ogg_sync_pageout (&ogg_data_sync_state, &ogg_data_page))
      {
        case 1:
          return;
        case -1:
          throw xtl::format_operation_exception (METHOD_NAME, "ogg_sync_pageout returned -1. Do not know what to do with it");
        case 0:
          ReadDataToOgg ();
          ReadOggPage ();
          return;
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "ogg_sync_pageout returned unsupported value");
      }
    }

    void GetOggPacket ()
    {
      static const char* METHOD_NAME = "media::TheoraDecoderInternal::GetOggPacket ()";

      switch (ogg_stream_packetout (&video_stream, &ogg_data_packet))
      {
        case 1:
          return;
        case -1:
          throw xtl::format_operation_exception (METHOD_NAME, "ogg_stream_packetout returned -1. Do not know what to do with it");
        case 0:
          for (;;)
          {
            ReadOggPage ();

            if (video_stream.serialno != ogg_page_serialno (&ogg_data_page))
              continue;

            if (ogg_stream_pagein (&video_stream, &ogg_data_page) == -1)
              throw xtl::format_operation_exception (METHOD_NAME, "Can't submit data page to bitstream");
            else
              break;
          }

          GetOggPacket ();

          return;
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "ogg_stream_packetout returned unsupported value");
      }
    }

    bool SeekToNextPage ()
    {
      while (!ogg_sync_pageseek (&ogg_data_sync_state, &ogg_data_page))
      {
        if (video_file.Eof ())
          return false;

        ReadDataToOgg ();
      }

      return true;
    }

    void GetOggPacket (size_t packet_index)
    {
      ogg_sync_reset   (&ogg_data_sync_state);
      ogg_stream_reset (&video_stream);

      video_file.Seek (0);

      ReadDataToOgg ();

      if (!SeekToNextPage ())
        throw xtl::format_operation_exception ("media::TheoraDecoderImpl::GetOggPacket (size_t)", "Can't seek to first page");

      ReadOggPage ();

      for (size_t i = 0; i <= packet_index; i++)
      {
        try
        {
          GetOggPacket ();
        }
        catch (...)
        {
          if (video_file.Eof ())
            throw;

          i--;
        }
      }
    }

    void GetLastOggPacket ()
    {
      long min_seek_value = -(int)video_file.Size ();

      for (long current_seek_value = LAST_PACKET_SEEK_STEP;; current_seek_value = stl::max (current_seek_value + LAST_PACKET_SEEK_STEP, min_seek_value))
      {
        ogg_sync_reset (&ogg_data_sync_state);
        ogg_stream_reset (&video_stream);

        video_file.Seek (current_seek_value, common::FileSeekMode_End);

        ReadDataToOgg ();

        if (!SeekToNextPage ())
          continue;

        try
        {
          ReadOggPage  ();

          do
          {
            GetOggPacket ();
          }
          while (ogg_data_packet.granulepos == -1);

          break;
        }
        catch (...)
        {
          if (current_seek_value == min_seek_value)
            throw xtl::format_operation_exception ("media::TheoraDecoderImpl::GetLastOggPacket", "Can't find last ogg packet");

          continue;
        }

        break;
      }
    }

  private:
    typedef xtl::uninitialized_storage <size_t> YCbCrBuffer;

  private:
    common::Log       log;
    common::InputFile video_file;
    YCbCrBuffer       ycbcr_buffer;
    ogg_sync_state    ogg_data_sync_state;
    ogg_stream_state  video_stream;
    ogg_page          ogg_data_page;
    ogg_packet        ogg_data_packet;
    th_info           th_data_info;
    th_comment        th_data_comment;
    th_setup_info     *th_data_setup_info;
    th_dec_ctx        *th_decoding_context;
    th_ycbcr_buffer   th_decoded_buffer;
    size_t            first_video_packet_index;
    size_t            total_frames_count;
    int               buffer_frame_number;
    int               decoded_frame_number;
};


IVideoDecoder* theora_loader (const char* file_name, VideoQuality quality)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::theora_loader", "file_name");

  return new TheoraDecoderImpl (file_name, quality);
}

/*
   Компонент декодирования theora видео
*/

class TheoraDecoderComponent
{
  public:
    //загрузка компонента
    TheoraDecoderComponent ()
    {
      VideoStreamManager::RegisterLoader ("ogv", &theora_loader);
      VideoStreamManager::RegisterLoader ("ogx", &theora_loader);
    }
};

extern "C"
{

common::ComponentRegistrator<TheoraDecoderComponent> TheoraDecoder ("media.video.decoders.theora");

}

}
