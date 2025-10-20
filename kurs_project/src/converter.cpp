#include "converter.h"

namespace converter
{
    struct format_data get_supported_extensions(const char *filename)
    {
        struct format_data fdata;
        for (unsigned int i = 0; i < 6; ++i) {
            if (std::strstr(filename, supp_img_ext[i])) {
                fdata.media_type = MEDIA_TYPE::MEDIA_IMAGE;
                fdata.extensions = supp_img_ext;
                return fdata;
            }
            else if (std::strstr(filename, supp_aud_ext[i])) {
                fdata.media_type = MEDIA_TYPE::MEDIA_AUDIO;
                fdata.extensions = supp_aud_ext;
                return fdata;
            }
            else if (i < 4 && std::strstr(filename, supp_vid_ext[i])) {
                fdata.media_type = MEDIA_TYPE::MEDIA_VIDEO;
                fdata.extensions = supp_vid_ext;
                return fdata;
            }
        }

        fdata.media_type = MEDIA_TYPE::MEDIA_UNKNOWN;
        fdata.extensions = nullptr;

        return fdata;
    }

    struct file_data get_file_data(const char *filename)
    {
        AVFormatContext     *ifmt_ctx   = nullptr;
        struct file_data     idata;
        int                  res        = 0;

        res = avformat_open_input(&ifmt_ctx, filename, NULL, NULL);
        if (res < 0) {
            fprintf(stderr, "Failed to open_input. __LINE__: %d\n", __LINE__);
            return idata;
        }

        res = avformat_find_stream_info(ifmt_ctx, NULL);
        if (res < 0) {
            fprintf(stderr, "Failed to find_Stream_info. __LINE__: %d\n", __LINE__)   ;
            goto cleanup;
        }

        printf("input_file duration: %zu, nb_streams: %d, bit_rate: %zu\n", ifmt_ctx->duration, ifmt_ctx->nb_streams, ifmt_ctx->bit_rate);

        idata.duration = ifmt_ctx->duration;
        idata.nb_streams = ifmt_ctx->nb_streams;
        idata.bit_rate = ifmt_ctx->bit_rate;
        idata.iformat_str = ifmt_ctx->iformat->long_name;

        for (unsigned int i = 0; i < ifmt_ctx->nb_streams; ++i) {
            AVCodecParameters   *dec_par    = nullptr;
            const AVCodec       *dec;
            AVCodecContext      *dec_ctx    = nullptr;
            AVPacket            *pkt        = nullptr;
            AVFrame             *frame      = nullptr;
            AVDictionaryEntry   *tag = nullptr;

            if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                puts("* Video");
            }
            else if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                puts("* Audio");
            }

            dec_par = ifmt_ctx->streams[i]->codecpar;
            dec = avcodec_find_decoder(dec_par->codec_id);

            dec_ctx = avcodec_alloc_context3(dec);
            avcodec_parameters_to_context(dec_ctx, dec_par);
            avcodec_open2(dec_ctx, dec, nullptr);

            pkt = av_packet_alloc();
            frame = av_frame_alloc();

            av_read_frame(ifmt_ctx, pkt);

            avcodec_send_packet(dec_ctx, pkt);
            avcodec_receive_frame(dec_ctx, frame);

            while ((tag = av_dict_get(frame->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
                idata.metadata.insert(std::make_pair(tag->key, tag->value));// [tag->key] = tag->value;
            }

            av_packet_unref(pkt);
            av_frame_unref(frame);

        }

    cleanup:
        avformat_close_input(&ifmt_ctx);
        return idata;
    }


    int encode_video(AVFrame         *frame,
                     AVFormatContext *ofmt_ctx,
                     AVFormatContext *ifmt_ctx,
                     AVCodecContext  *enc_ctx,
                     AVPacket        *out_pkt,
                     int              video_index)
    {
        int       res        = 0;
        AVStream *dec_stream = ifmt_ctx->streams[video_index];
        AVStream *enc_stream = ofmt_ctx->streams[video_index];

        res = avcodec_send_frame(enc_ctx, frame);
        if (res < 0 && res != AVERROR_EOF) {
            fprintf(stderr, "Failed to send_frame."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        res = avcodec_receive_packet(enc_ctx, out_pkt);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
            goto cleanup;
        }
        else if (res < 0) {
            fprintf(stderr, "Failed to receive_frame. On line: %d.\n",
                    __LINE__);
            goto cleanup;
        }        

        out_pkt->stream_index = video_index;
        out_pkt->duration     = enc_stream->time_base.den
                            / enc_stream->time_base.num
                            / dec_stream->avg_frame_rate.num
                            * dec_stream->avg_frame_rate.den;

        av_packet_rescale_ts(out_pkt, dec_stream->time_base,
                             enc_stream->time_base);

        res = av_interleaved_write_frame(ofmt_ctx, out_pkt);
        if (res < 0) {
            fprintf(stderr, "Failed to write_frame."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

    cleanup:
        av_packet_unref(out_pkt);
        return res;
    }

    int apts = 0;

    int encode_audio_frame(AVFrame         *frame,
                           AVFormatContext *ofmt_ctx,
                           AVFormatContext *ifmt_ctx,
                           AVCodecContext  *enc_ctx,
                           AVPacket        *out_pkt,
                           int             *data_presended,
                           int              audio_index)
    {
        int res = 0;

        if (frame) {
            frame->pts = apts;
            apts += frame->nb_samples;
        }

        *data_presended = 0;
        res = avcodec_send_frame(enc_ctx, frame);
        if (res < 0 && res != AVERROR_EOF) {
            fprintf(stderr, "Failed to send_frame."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        res = avcodec_receive_packet(enc_ctx, out_pkt);
        if (res == AVERROR(EAGAIN)) {
            res = 0;
            goto cleanup;
        }
        else if (res == AVERROR_EOF) {
            res = 0;
            goto cleanup;
        }
        else if (res < 0) {
            fprintf(stderr, "Failed to receive_packet."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }
        else {
            *data_presended = 1;
        }

        if (!frame) {
            std::cout << "adding pts to packets by hands" << std::endl;
            int64_t sample_duration = av_rescale_q(out_pkt->duration, av_make_q(1, enc_ctx->sample_rate), ofmt_ctx->streams[audio_index]->time_base);
            out_pkt->pts = apts;
            out_pkt->dts = apts;
            apts += sample_duration;
        }
        else {
            out_pkt->dts = out_pkt->pts;
        }

        res = av_interleaved_write_frame(ofmt_ctx, out_pkt);
        if (*data_presended && res < 0) {
            fprintf(stderr, "Failed to write_frame."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

    cleanup:
        av_packet_unref(out_pkt);
        return res;
    }



    int convert(const char *inpath, const char *outpath,
                const struct convertation_data *data,
                std::function<void(char*, long long)> show_banner)
    {
        AVFormatContext     *ifmt_ctx       = NULL;
        AVFormatContext     *ofmt_ctx       = NULL;
        AVCodecContext      *dec_ctx[3]     = {NULL, NULL, NULL};// 0 - video, 1 - audio, 2 - NULL
        AVCodecContext      *enc_ctx[3]     = {NULL, NULL, NULL};// 0 - video, 1 - audio, 2 - NULL
        AVPacket            *in_pkt         = NULL;
        AVPacket            *out_pkt        = NULL;
        AVFrame             *in_frame       = NULL;
        struct SwsContext   *sws_ctx        = NULL;
        struct SwrContext   *swr_ctx        = NULL;
        AVAudioFifo         *fifo           = NULL;
        AVFrame             *out_frame      = NULL;
        int                  res            = 0;
        int                  video_index    = 0;
        int                  audio_index    = 1;
        char                 cpy_video      = 0;
        char                 cpy_audio      = 0;
        char                 reading        = 1;

        /*
         *  IFMT_CTX
         */
        res = avformat_open_input(&ifmt_ctx, inpath, NULL, NULL);
        if (res < 0) {
            fprintf(stderr, "Failed to open_input. On line: %d.\n",
                    __LINE__);
            return res;
        }

        res = avformat_find_stream_info(ifmt_ctx, NULL);
        if (res < 0) {
            fprintf(stderr, "Failed to find_stream_info. On line: %d.\n",
                    __LINE__);
            goto cleanup;
        }

        /*
         *  OFMT_CTX
         */
        res = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, outpath);
        if (res < 0) {
            fprintf(stderr, "Failed to alloc_output_context2."
                    " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        std::cout << "inpath: " << inpath << ", outpath: " << outpath << "data->audio_codec: " << data->audio_codec << std::endl;

        /*
         *  SETTING UP DEC AND ENC CODEC_CONTEXTs
         */
        for (unsigned int i = 0; i < ifmt_ctx->nb_streams; ++i) {
            AVCodecParameters *dec_par = NULL;

            dec_par = ifmt_ctx->streams[i]->codecpar;

            if (ifmt_ctx->streams[i]->time_base.den) {
                std::cout << "i: " << i << ", den: " << ifmt_ctx->streams[i]->time_base.den << std::endl;
            }
            else {
                std::cerr << "nononono" << std::endl;
            }

            if (dec_par->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_index = i;

                if (!cpy_video) {
                    const AVCodec   *dec        = NULL;
                    const AVCodec   *enc        = NULL;
                    AVStream        *enc_stream = NULL;

                    /*
                     *  DEC_CTX
                     */
                    dec = avcodec_find_decoder(dec_par->codec_id);
                    if (!dec) {
                        fprintf(stderr, "Failed to find_decoder."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    dec_ctx[0] = avcodec_alloc_context3(dec);
                    if (!dec_ctx[0]) {
                        fprintf(stderr, "Failed to alloc_context3."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    res = avcodec_parameters_to_context(dec_ctx[0], dec_par);
                    if (res < 0) {
                        fprintf(stderr, "Failed to copy "
                                "parameters_to_context. On line: %d.\n",
                                 __LINE__);
                        goto cleanup;
                    }

                    res = avcodec_open2(dec_ctx[0], dec, NULL);
                    if (res < 0) {
                        fprintf(stderr, "Failed to open2 dec_ctx[0]."
                                " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    dec_ctx[0]->pkt_timebase = ifmt_ctx->streams[i]->time_base;

                    /*
                     *  ENC_CTX
                     */
                    enc_stream = avformat_new_stream(ofmt_ctx, NULL);
                    if (!enc_stream) {
                        fprintf(stderr, "Failed to new_stream."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    enc = avcodec_find_encoder(data->video_codec);
                    if (!enc) {
                        fprintf(stderr, "Failed to find_encoder."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    enc_ctx[0] = avcodec_alloc_context3(enc);
                    if (!enc_ctx[0]) {
                        fprintf(stderr, "Failed to alloc_context3."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    if (data->max_width
                        && (dec_ctx[0]->width > data->max_width || dec_ctx[0]->height > data->max_width))
                    {
                        if (dec_ctx[0]->width > dec_ctx[0]->height) {
                            const float scale = (float)dec_ctx[0]->width / (float)dec_ctx[0]->height;

                            enc_ctx[0]->width = data->max_width;
                            enc_ctx[0]->height = data->max_width / scale;
                        }
                        else {
                            const float scale = dec_ctx[0]->height / dec_ctx[0]->width;

                            enc_ctx[0]->width = data->max_width / scale;
                            enc_ctx[0]->height = data->max_width;
                        }
                    }
                    else {
                        enc_ctx[0]->width = dec_ctx[0]->width;
                        enc_ctx[0]->height = dec_ctx[0]->height;
                    }

                    if (data->bit_rate) {
                        enc_ctx[0]->bit_rate = data->bit_rate;
                    }
                    else {
                        enc_ctx[0]->bit_rate = 0;
                        enc_ctx[0]->rc_min_rate = 0;
                        enc_ctx[0]->rc_max_rate = 0;
                        enc_ctx[0]->rc_buffer_size = 0;

                        enc_ctx[0]->flags |= AV_CODEC_FLAG_QSCALE;
                        enc_ctx[0]->global_quality = FF_QP2LAMBDA * 75; // Качество 75%
                    }
                    /*
                    if (data->video_codec == AV_CODEC_ID_MJPEG) {
                        enc_ctx[0]->bit_rate = 0;
                        enc_ctx[0]->rc_min_rate = 0;
                        enc_ctx[0]->rc_max_rate = 0;
                        enc_ctx[0]->rc_buffer_size = 0;

                        enc_ctx[0]->flags |= AV_CODEC_FLAG_QSCALE;
                        enc_ctx[0]->global_quality = FF_QP2LAMBDA * 75; // Качество 75%
                    }
                    else {
                        enc_ctx[0]->bit_rate        = 2 * 1000 * 1000;
                        enc_ctx[0]->rc_buffer_size  = 4 * 1000 * 1000;
                        enc_ctx[0]->rc_max_rate     = 2 * 1000 * 1000;
                        enc_ctx[0]->rc_min_rate     = 2.5f * 1000 * 1000;
                    }
                    */
                    enc_ctx[0]->sample_aspect_ratio =
                        dec_ctx[0]->sample_aspect_ratio;
                    enc_ctx[0]->pix_fmt = data->pixfmt;

                    enc_ctx[0]->time_base = av_inv_q(
                        av_guess_frame_rate(ifmt_ctx,
                                            ifmt_ctx->streams[i], NULL));

                    res = avcodec_open2(enc_ctx[0], enc, NULL);
                    if (res < 0) {
                        fprintf(stderr, "Failed to open2 enc_ctx."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    avcodec_parameters_from_context(enc_stream->codecpar,
                                                    enc_ctx[0]);

                    /*
                     *  SWS_CTX
                     */
                    sws_ctx = sws_getContext(
                        dec_ctx[0]->width, dec_ctx[0]->height,
                        dec_ctx[0]->pix_fmt, enc_ctx[0]->width,
                        enc_ctx[0]->height, enc_ctx[0]->pix_fmt,
                        SWS_BICUBIC, NULL, NULL, NULL
                    );
                    if (!sws_ctx) {
                        fprintf(stderr, "Failed to sws_getContext."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }
                }
                else { /* COPY_VIDEO_STREAM */
                    AVStream *video_stream = NULL;

                    video_stream = avformat_new_stream(ofmt_ctx, NULL);
                    if (!video_stream) {
                        fprintf(stderr, "Failed to alloc new_stream."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    avcodec_parameters_copy(video_stream->codecpar,
                                            ifmt_ctx->streams[i]->codecpar);
                }
            }
            else if (dec_par->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_index = i;

                if (!cpy_audio) {
                    const AVCodec   *dec        = NULL;
                    const AVCodec   *enc        = NULL;
                    AVStream        *enc_stream = NULL;
                    AVDictionary    *opts       = NULL;

                    /*
                     *  DEC_CTX
                     */
                    dec = avcodec_find_decoder(dec_par->codec_id);
                    if (!dec) {
                        fprintf(stderr, "Failed to find_decoder."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    dec_ctx[1] = avcodec_alloc_context3(dec);
                    if (!dec_ctx[1]) {
                        fprintf(stderr, "Failed to alloc_context3."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    res = avcodec_parameters_to_context(dec_ctx[1], dec_par);
                    if (res < 0) {
                        fprintf(stderr, "Failed to copy "
                                        "parameters_to_context. On line: %d.\n",
                                __LINE__);
                        goto cleanup;
                    }

                    res = avcodec_open2(dec_ctx[1], dec, NULL);
                    if (res < 0) {
                        fprintf(stderr, "Failed to open2 dec_ctx[1]."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    dec_ctx[1]->pkt_timebase = ifmt_ctx->streams[i]->time_base;

                    /*
                     *  ENC_CTX
                     */
                    enc_stream = avformat_new_stream(ofmt_ctx, NULL);
                    if (!enc_stream) {
                        fprintf(stderr, "Failed to new_stream."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    enc = avcodec_find_encoder(data->audio_codec);
                    if (!enc) {
                        fprintf(stderr, "Failed to find_encoder."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    enc_ctx[1] = avcodec_alloc_context3(enc);
                    if (!enc_ctx[1]) {
                        fprintf(stderr, "Failed to alloc_context3."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    //                    

                    av_channel_layout_default(&enc_ctx[1]->ch_layout,
                                              OUT_AUDIO_CH);
                    enc_ctx[1]->sample_rate = dec_ctx[1]->sample_rate;//data->sample_rate;//dec_ctx[1]->sample_rate;//data->sample_rate;//
                    enc_ctx[1]->sample_fmt  = data->smplfmt;
                    enc_ctx[1]->bit_rate    = data->bit_rate ? data->bit_rate : 0;  //enc_ctx[1]->bit_rate    = OUT_BIT_RATE;

                    //enc_ctx[1]->time_base = ifmt_ctx->streams[i]->time_base;
                    //enc_stream->time_base = ifmt_ctx->streams[i]->time_base;
                    //enc_stream->duration  = ifmt_ctx->streams[i]->duration;
                    //enc_stream->time_base = ifmt_ctx->streams[i]->time_base;
                    enc_stream->time_base.den = dec_ctx[1]->sample_rate;//data->sample_rate;
                    enc_stream->time_base.num = 1;

                    av_dict_set(&opts, "strict", "experimental", 0);

                    res = avcodec_open2(enc_ctx[1], enc, &opts);
                    if (res < 0) {
                        fprintf(stderr, "Failed to open2 enc_ctx[1]."
                                        " On line: %d.\n", __LINE__);
                        av_dict_free(&opts);
                        goto cleanup;
                    }

                    av_dict_free(&opts);

                    res = avcodec_parameters_from_context(
                        enc_stream->codecpar, enc_ctx[1]);
                    if (res < 0) {
                        fprintf(stderr, "Failed to copy "
                                        "parameters_from_context."
                                        " On line: %d.\n", __LINE__);                        
                        goto cleanup;
                    }

                    /*
                     *  SWR_CTX
                     */
                    res = swr_alloc_set_opts2(&swr_ctx,
                                              &enc_ctx[1]->ch_layout,
                                               enc_ctx[1]->sample_fmt,
                                               enc_ctx[1]->sample_rate,
                                              &dec_ctx[1]->ch_layout,
                                               dec_ctx[1]->sample_fmt,
                                               dec_ctx[1]->sample_rate,
                                               0, NULL);
                    if (res < 0) {
                        fprintf(stderr, "Failed to alloc_set_opts2."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    res = swr_init(swr_ctx);
                    if (res < 0) {
                        fprintf(stderr, "Failed to init swr."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    /*
                     *  FIFO
                     */
                    fifo = av_audio_fifo_alloc(enc_ctx[1]->sample_fmt,
                                        enc_ctx[1]->ch_layout.nb_channels, 1);
                    if (!fifo) {
                        fprintf(stderr, "Failed to fifo_alloc."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }
                }
                else { /* COPY_AUDIO_STREAM */
                    AVStream *audio_stream = NULL;

                    audio_stream = avformat_new_stream(ofmt_ctx, NULL);
                    if (!audio_stream) {
                        fprintf(stderr, "Failed to alloc new_stream."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    avcodec_parameters_copy(audio_stream->codecpar,
                                            ifmt_ctx->streams[i]->codecpar);
                }
            }
            else {
                puts("skippind unknown stream");
            }
        }

        /*
         *  OFMT_CTX STUFF
         */
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            ofmt_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            res = avio_open(&ofmt_ctx->pb, outpath, AVIO_FLAG_WRITE);
            if (res < 0) {
                fprintf(stderr, "Failed to open output file."
                                " On line: %d.\n", __LINE__);
                goto cleanup;
            }
        }

        res = avformat_write_header(ofmt_ctx, NULL);
        if (res < 0) {
            fprintf(stderr, "Failed to write_header to output file."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        /*
         *  ALLOCATING IN_PKT AND FRAMEs (IN, OUT)
         */
        in_pkt = av_packet_alloc();
        if (!in_pkt) {
            fprintf(stderr, "Failed to packet_alloc."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        out_pkt = av_packet_alloc();
        if (!out_pkt) {
            fprintf(stderr, "Failed to packet_alloc."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        in_frame = av_frame_alloc();
        if (!in_frame) {
            fprintf(stderr, "Failed to frame_alloc."
                            " On line: %d.\n", __LINE__);
            goto cleanup;
        }

        if (!cpy_video || !cpy_audio) {
            out_frame = av_frame_alloc();
            if (!out_frame) {
                fprintf(stderr, "Failed to frame_alloc."
                                " On line: %d.\n", __LINE__);
                goto cleanup;
            }
        }

        /*
         *  READING INPUT FILEs PACKETS AND TRANSCODE OR REMUX THEM
         */

        while (1) {
            AVCodecParameters *codecpar = NULL;
            int                finished = 0;

            if (reading) {
                res = av_read_frame(ifmt_ctx, in_pkt);
                if (res < 0) {                    
                    if (res == AVERROR_EOF) {
                        finished = 1;
                        break;
                    }
                    else {
                        fprintf(stderr, "Failed to read_frame."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }
                }
            }

            codecpar = ifmt_ctx->streams[in_pkt->stream_index]->codecpar;

            if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                //reading = 1;

                if (!cpy_video) {
                    res = avcodec_send_packet(dec_ctx[0], in_pkt);
                    if (res < 0) {
                        fprintf(stderr, "Failed to send_packet."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }

                    while (res >= 0) {
                        res = avcodec_receive_frame(dec_ctx[0], in_frame);
                        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
                            break;
                        }
                        else if (res < 0) {
                            fprintf(stderr, "Failed to receive_frame."
                                            " On line: %d.\n", __LINE__);
                            goto cleanup;
                        }

                        /*
                         *  OUT_FRAME
                         */
                        out_frame->format = enc_ctx[0]->pix_fmt;
                        out_frame->width  = enc_ctx[0]->width;
                        out_frame->height = enc_ctx[0]->height;

                        res = av_image_alloc(out_frame->data,
                                             out_frame->linesize,
                                             enc_ctx[0]->width,
                                             enc_ctx[0]->height,
                                             enc_ctx[0]->pix_fmt,
                                             32);
                        if (res < 0) {
                            fprintf(stderr, "failed to av_image_alloc."
                                            "On line: %d.\n", __LINE__);
                            av_frame_free(&out_frame);
                            goto cleanup;
                        }

                        res = sws_scale(sws_ctx,
                                        (const uint8_t * const *)in_frame,
                                        in_frame->linesize, 0,
                                        in_frame->height, out_frame->data,
                                        out_frame->linesize);
                        if (res < 0) {
                            fprintf(stderr, "failed to sws_scale."
                                            "On line: %d.\n", __LINE__);
                            sws_freeContext(sws_ctx);
                            av_freep(&out_frame->data[0]);
                            av_frame_free(&out_frame);
                            goto cleanup;
                        }

                        res = encode_video(out_frame, ofmt_ctx, ifmt_ctx,
                                           enc_ctx[0], out_pkt, video_index);
                        if (res < 0) {
                            fprintf(stderr, "Failed to encode_video."
                                            "On line: %d.\n", __LINE__);
                            goto cleanup;
                        }

                        av_freep(&out_frame->data[0]);
                        av_frame_unref(in_frame);
                        av_frame_unref(out_frame);
                    }
                    av_packet_unref(in_pkt);
                }
                else { /* REMUX VIDEO */
                    av_packet_rescale_ts(in_pkt,
                                         ifmt_ctx->streams[video_index]->time_base,
                                         ofmt_ctx->streams[video_index]->time_base);

                    res = av_interleaved_write_frame(ofmt_ctx, in_pkt);
                    if (res < 0) {
                        fprintf(stderr, "Failed to interleaved_write_frame."
                                        " On line: %d.\n", __LINE__);
                        goto cleanup;
                    }
                    av_packet_unref(in_pkt);
                }
            }
            else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                if (!cpy_audio) {
                    if (av_audio_fifo_size(fifo) < enc_ctx[1]->frame_size) {
                        uint8_t **converted_in_samples = NULL;

                        reading = 1;

                        // read_decode_convert_and_store
                        res = avcodec_send_packet(dec_ctx[1], in_pkt);
                        if (res < 0) {
                            fprintf(stderr, "Failed to send_packet."
                                            " On line: %d.\n", __LINE__);
                            goto cleanup;
                        }

                        while (res >= 0) {
                            res = avcodec_receive_frame(dec_ctx[1], in_frame);

                            if (res == AVERROR(EAGAIN)) {
                                break;
                            }
                            else if (res == AVERROR_EOF) {
                                finished = 1;
                                break;
                            }
                            else if (res < 0) {
                                fprintf(stderr, "Failed to receive_frame."
                                                " On line: %d.\n", __LINE__);
                                goto cleanup;
                            }
                            //}

                            if (finished) {
                                res = 0;
                                break;
                            }

                            // init_converted_samples
                            converted_in_samples = (uint8_t**)calloc(
                                enc_ctx[1]->ch_layout.nb_channels,
                                sizeof(*converted_in_samples));
                            if (!converted_in_samples) {
                                fprintf(stderr, "Failed to calloc "
                                                "converted_in_samples."
                                                " On line: %d.\n", __LINE__);
                                goto cleanup;
                            }

                            res = av_samples_alloc(converted_in_samples, NULL,
                                                   enc_ctx[1]->ch_layout.nb_channels,
                                                   in_frame->nb_samples,//enc_ctx[1]->frame_size,
                                                   enc_ctx[1]->sample_fmt, 0);
                            if (res < 0) {
                                fprintf(stderr, "Failed to semples_alloc."
                                                " On line: %d.\n", __LINE__);
                                av_freep(&converted_in_samples[0]);
                                free(*converted_in_samples);
                                goto cleanup;
                            }

                            // convert_samples
                            res = swr_convert(swr_ctx, converted_in_samples,
                                              in_frame->nb_samples,
                                              (const uint8_t**)in_frame->extended_data,
                                              in_frame->nb_samples);
                            if (res < 0) {
                                std::cout << "err635: " <<  __LINE__ << std::endl;
                                fprintf(stderr, "Failed to swr_convert."
                                                " On line: %d.\n", __LINE__);
                                av_freep(&converted_in_samples[0]);
                                free(*converted_in_samples);
                                std::cout << "goto cleanup: " << __LINE__ << std::endl;
                                goto cleanup;
                            }

                            // add_samples_to_fifo
                            res = av_audio_fifo_realloc(fifo,
                                                        av_audio_fifo_size(fifo)
                                                            + in_frame->nb_samples);
                            if (res < 0) {
                                fprintf(stderr, "Failed to fifo_realloc."
                                                " On line: %d.\n", __LINE__);
                                av_freep(&converted_in_samples[0]);
                                free(*converted_in_samples);
                                goto cleanup;
                            }

                            res = av_audio_fifo_write(fifo,
                                                      (void**)converted_in_samples,
                                                      in_frame->nb_samples);
                            if (res < in_frame->nb_samples) {
                                fprintf(stderr, "Failed to fifo_write."
                                                " On line: %d.\n", __LINE__);
                                av_freep(&converted_in_samples[0]);
                                free(*converted_in_samples);
                                goto cleanup;
                            }

                            av_freep(&converted_in_samples[0]);
                            free(*converted_in_samples);
                        }
                        av_packet_unref(in_pkt);
                    }
                    else { /* FIFO == enc_ctx[1]->frame_size. Now write fifo's sample in ofmt_ctx */
                        reading = 0;
                    }

                    while (av_audio_fifo_size(fifo) >= enc_ctx[1]->frame_size
                           || (finished && av_audio_fifo_size(fifo) > 0)) {
                        //  init_output_frame
                        const int frame_size = std::min(av_audio_fifo_size(fifo),
                                                        enc_ctx[1]->frame_size);

                        out_frame->nb_samples  = frame_size;
                        av_channel_layout_copy(&out_frame->ch_layout,
                                               &enc_ctx[1]->ch_layout);
                        out_frame->format      = enc_ctx[1]->sample_fmt;
                        out_frame->sample_rate = enc_ctx[1]->sample_rate;

                        res = av_frame_get_buffer(out_frame, 0);
                        if (res < 0) {
                            fprintf(stderr, "Failed to frame_get_buffer."
                                            " On line: %d.\n", __LINE__);
                            goto cleanup;
                        }

                        //
                        res = av_audio_fifo_read(fifo, (void**)out_frame->data,
                                                 frame_size);
                        if (res < frame_size) {
                            fprintf(stderr, "Failed to fifo_read."
                                            " On line: %d.\n", __LINE__);
                            goto cleanup;
                        }

                        int data_written;
                        //  encode_audio_frame
                        res = encode_audio_frame(out_frame, ofmt_ctx, ifmt_ctx,
                                                 enc_ctx[1], out_pkt,
                                                 &data_written, audio_index);
                        if (res) {
                            fprintf(stderr, "Failed to encode_audio_frame."
                                            " On line: %d.\n", __LINE__);
                            goto cleanup;
                        }
                    }                    
                }
                else { /* REMUX AUDIO */
                    av_packet_rescale_ts(in_pkt,
                                         ifmt_ctx->streams[audio_index]->time_base,
                                         ofmt_ctx->streams[audio_index]->time_base);
                    res = av_interleaved_write_frame(ofmt_ctx, in_pkt);
                    if (res < 0) {
                        fprintf(stderr,
                                "Failed to interleaved_write_frame."
                                " On line: %d.\n",
                                __LINE__);
                        goto cleanup;
                    }
                    av_packet_unref(in_pkt);
                }
            }
        }
std::cout << "886" << std::endl;
        //  flush video
        if (data->video_codec != AV_CODEC_ID_NONE && !cpy_video) {
            do {
                res = encode_video(NULL, ofmt_ctx, ifmt_ctx,
                                   enc_ctx[0], out_pkt, video_index);
            } while (res == 0);
        }

        // flush audio
        if (data->audio_codec != AV_CODEC_ID_NONE && !cpy_audio) {
            puts("flushing");
            int data_written;
            int flushed_frames = 0;

            do {
                ++flushed_frames;
                res = encode_audio_frame(NULL, ofmt_ctx, ifmt_ctx,
                                         enc_ctx[1], out_pkt,
                                         &data_written, audio_index);
                if (res) {
                    fprintf(stderr,
                            "Failed to encode_audio_frame."
                            " On line: %d.\n", __LINE__);
                    goto cleanup;
                }

            } while (data_written);
            std::cout << "flushed_frames: " << flushed_frames << std::endl;
        }
std::cout << "912" << std::endl;
        res = av_write_trailer(ofmt_ctx);
        if (res < 0) {
            fprintf(stderr,
                    "Failed to write_trailer."
                    " On line: %d.\n", __LINE__);
            goto cleanup;
        }

    cleanup:
        fflush(stderr);
        std::cout << "923" << std::endl;
        if (fifo) {
            av_audio_fifo_free(fifo);
        }
        if (swr_ctx) {
            swr_free(&swr_ctx);
        }

        if (in_pkt) {
            av_packet_free(&in_pkt);
        }
        if (out_pkt) {
            av_packet_free(&out_pkt);
        }


        if (in_frame) {
            av_frame_free(&in_frame);
        }
        if (out_frame) {
            av_frame_free(&out_frame);
        }


        if (sws_ctx) {
            sws_freeContext(sws_ctx);
        }


        if (enc_ctx[0]) {
            avcodec_free_context(&enc_ctx[0]);
        }
        if (enc_ctx[1]) {
            std::cout << "941. free enc_ctx[1]" << std::endl;
            avcodec_free_context(&enc_ctx[1]);
        }


        if (dec_ctx[0]) {
            avcodec_free_context(&dec_ctx[0]);
        }
        if (dec_ctx[1]) {
            avcodec_free_context(&dec_ctx[1]);
        }


        if (ofmt_ctx->pb) {
            avio_closep(&ofmt_ctx->pb);
        }
        if (ofmt_ctx) {
            avformat_free_context(ofmt_ctx);
        }


        if (ifmt_ctx) {
            avformat_close_input(&ifmt_ctx);
        }


        Server::Requester::add_convertation_to_history(inpath, outpath, show_banner);

        return res;
    }
}
