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
