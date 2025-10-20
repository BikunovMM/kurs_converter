#ifndef CONVERTER_H
#define CONVERTER_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

#include "ServerRequester.hpp"
//#include "MainWindow.hpp"

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>

    #include <libavutil/frame.h>
    #include <libavformat/avio.h>
    #include <libavutil/channel_layout.h>

    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libswresample/swresample.h>
    #include <libavutil/audio_fifo.h>
}

namespace converter{
    enum class MEDIA_TYPE {
        MEDIA_UNKNOWN,
        MEDIA_IMAGE,
        MEDIA_VIDEO,
        MEDIA_AUDIO
    };

    class file_data {
    public:
        const char *video_codec_str;
        const char *audio_codec_str;
        const char *pix_fmt_str;
        const char *iformat_str;
        std::map<const std::string, const std::string> metadata;// std::map<const char*, const char*> metadata;

        unsigned int nb_streams;

        unsigned int frame_width;
        unsigned int frame_height;

        std::size_t duration;

        std::size_t sample_rate;
        std::size_t bit_rate;
    };
/*
    struct fmt_par {
        AVCodecID       vcodec;
        AVCodecID       acodec;
        AVCodecID       icodec;
        AVPixelFormat   pixfmt;
        AVSampleFormat  smplfmt;
    };
*/
    struct fmtpar {
        const char*  vcodec;
        const char*  acodec;
        const char*  pixfmt;
        const char*  smplfmt;
    };

    struct format_data {
        unsigned int ext_index;
        char* const* extensions;
        enum MEDIA_TYPE media_type;
    };

    struct convertation_data {
        AVCodecID         video_codec;
        AVCodecID         audio_codec;
        AVPixelFormat     pixfmt;
        AVSampleFormat    smplfmt;
        int      max_width;
        int      sample_rate;
        int      bit_rate;
    };

    struct format_data get_supported_extensions(const char *filename);
    int convert(const char *inpath, const char *outpath,
                const struct convertation_data *cdata,
                std::function<void(char*, long long)> show_banner);
    struct file_data get_file_data(const char *filename);

    //struct fmtpar get_fmt_supp_par(const char *format, enum MEDIA_TYPE type);
/*
    struct img_par {
        std::vector<std::pair<AVCodecID, std::vector<AVPixelFormat>>> img_codecs;
    };
    struct aud_par {
        std::vector<std::pair<AVCodecID, std::vector<AVSampleFormat>>> aud_codecs;
    };
    struct vid_par {
        struct img_par vid_codecs;
        struct aud_par audio_codecs;
    };


    const std::map<const char*, struct img_par> img_formats = {
        {
            "png",
            {
                {AV_CODEC_ID_PNG, {AV_PIX_FMT_RGBA, AV_PIX_FMT_RGB24, AV_PIX_FMT_BGRA, AV_PIX_FMT_BGR24, AV_PIX_FMT_PAL8, AV_PIX_FMT_GRAY8}}
            }
        },
        {
            "jpg",
            {
                {AV_CODEC_ID_MJPEG, {AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVJ420P}}
            }
        },
        {
            "jpeg",
            {
                {AV_CODEC_ID_MJPEG, {AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVJ420P}}
            }
        },
        {
            "ico",
            {
                {AV_CODEC_ID_PNG, {AV_PIX_FMT_RGBA, AV_PIX_FMT_RGB24, AV_PIX_FMT_BGRA, AV_PIX_FMT_BGR24},
                    {AV_CODEC_ID_BMP, {AV_PIX_FMT_BGRA, AV_PIX_FMT_BGR24}}},

            }
        },
        {
            "webp",
            {
                {AV_CODEC_ID_WEBP, {AV_PIX_FMT_BGRA, AV_PIX_FMT_YUV420P}}
            }
        },
        {
            "bmp",
            {
                {AV_CODEC_ID_BMP, {AV_PIX_FMT_BGRA, AV_PIX_FMT_BGR24, AV_PIX_FMT_BGR8, AV_PIX_FMT_GRAY8, AV_PIX_FMT_PAL8, AV_PIX_FMT_MONOBLACK}}
            }
        }
    };

    const std::map<const char*, struct img_par> audio_formats = {
        {
         "mp3",
         {
            {AV_CODEC_ID_MP3, {AV_SAMPLE_FMT_S16}}
         }
        },
        {
         "opus",
         {
            {AV_CODEC_ID_OPUS, {AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S16}}
         }
        },
        {
         "ogg",
         {
            {AV_CODEC_ID_VORBIS, {AV_SAMPLE_FMT_FLTP}}
         }
        },
        {
         "wav",
         {
            {AV_CODEC_ID_PCM_S16LE, {AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_FLT}}
         }
        },
        {
         "aac",
         {
            {AV_CODEC_ID_AAC, {AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_S16}}
         }
        },
        {
         "flac",
         {
            {AV_CODEC_ID_FLAC, {AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S32}}
         }
        },
    };

    const std::map<const char*, struct vid_par> video_formats = {
        {
         "mp4",
         {
          {{AV_CODEC_ID_H264, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_HEVC, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_MPEG4, {AV_PIX_FMT_YUV420P}}},
          {audio_formats["aac"], audio_formats["mp3"]}
          //{{AV_CODEC_ID_AAC, {AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_S16}}, {AV_CODEC_ID_MP3, {AV_SAMPLE_FMT_S16}}}
         }
        },
        {
         "mov",
         {
           {{AV_CODEC_ID_H264, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_HEVC, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_MPEG4, {AV_PIX_FMT_YUV420P}}},
           {audio_formats["aac"], audio_formats["mp3"]}
          //{{AV_CODEC_ID_AAC, {AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_S16}}, {AV_CODEC_ID_MP3, {AV_SAMPLE_FMT_S16}}}
         }
        },
        {
         "mkv",
         {
            {{AV_CODEC_ID_H264, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_HEVC, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_MPEG4, {AV_PIX_FMT_YUV420P}}},
            {audio_formats["aac"], audio_formats["mp3"], audio_formats["opus"], audio_formats["ogg"], audio_formats["flac"]},
            //{{AV_CODEC_ID_AAC, {AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_S16}}, {AV_CODEC_ID_MP3, {AV_SAMPLE_FMT_S16}},
             //{AV_CODEC_ID_OPUS, {AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S16}}, {AV_CODEC_ID_VORBIS, {AV_SAMPLE_FMT_FLT}},
             //{AV_CODEC_ID_FLAC, {AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S32}}}
         }
        },
        {
         "webm",
         {
             {{AV_CODEC_ID_VP9, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_VP8, {AV_PIX_FMT_YUV420P}}, {AV_CODEC_ID_AV1, {AV_PIX_FMT_YUV420P}}},
             {audio_format["opus"], audio_format["ogg"]}
             //{{AV_CODEC_ID_OPUS, {AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S16}}, {AV_CODEC_ID_VORBIS, {AV_SAMPLE_FMT_FLT}}}
         }
        }
    };
  */


    constexpr enum AVCodecID img_codecs[] = {
        AV_CODEC_ID_PNG,
        AV_CODEC_ID_MJPEG,
        AV_CODEC_ID_MJPEG,
        AV_CODEC_ID_PNG,
        AV_CODEC_ID_WEBP,
        AV_CODEC_ID_BMP
    };
    constexpr enum AVCodecID aud_codecs[] = {
        AV_CODEC_ID_MP3,
        AV_CODEC_ID_OPUS,
        AV_CODEC_ID_VORBIS,
        AV_CODEC_ID_PCM_S16LE,
        AV_CODEC_ID_AAC,
        AV_CODEC_ID_FLAC
    };
    constexpr enum AVCodecID vid_codecs[] = {
        AV_CODEC_ID_H264,
    };

    constexpr enum AVPixelFormat img_pix_fmts[] = {
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_YUV420P,
        AV_PIX_FMT_YUV420P,
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_BGR24
    };
    constexpr enum AVSampleFormat aud_smpl_fmts[] = {
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_FLT,
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_FLTP,
        AV_SAMPLE_FMT_S16
    };

    constexpr char* supp_vid_ext[] = {
         (char*)"mp4",
         (char*)"mov",
         (char*)"mkv",
         (char*)"webm",
         NULL
    };
    constexpr char* supp_img_ext[] = {
        (char*)"png",
        (char*)"jpg",
        (char*)"jpeg",
        (char*)"ico",
        (char*)"bmp",
        NULL
    };
    constexpr char* supp_aud_ext[] = {
        (char*)"mp3",
        (char*)"opus",
        (char*)"wav",
        (char*)"aac",
        (char*)"flac",
        NULL
    };

    constexpr int OUT_AUDIO_CH = 2;
    constexpr int OUT_BIT_RATE = 96000;

    constexpr char *fdiag_img_exts   = (char*)
        "Images (*.png *.jpg *.jpeg *.ico *.bmp)";
    constexpr char *fdiag_video_exts = (char*)
        "Videos (*.mp4 *.mov *.mkv *.webm)";
    constexpr char *fdiag_audio_exts = (char*)
        "Audio (*.mp3 *.opus *.wav *.aac *.flac)";

    constexpr char *fdiag_upl_exts = (char*)
        "Images (*.png *.jpg *.jpeg *.ico *.bmp);;Videos (*.mp4 *.mov *.mkv *.webm);;Audio (*.mp3 *.opus *.wav *.aac *.flac)";
}

#endif // CONVERTER_H
