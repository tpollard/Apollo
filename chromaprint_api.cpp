#include "chromaprint_api.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 28, 0)
#define avcodec_free_frame av_freep
#endif

#define CLIENT_ID "8XaBELgH"

int decode_audio_file(ChromaprintContext *chromaprint_ctx, const char *fileName, int max_length, int *duration)
{
    int remaining, length, consumed, got_frame;

    int max_dst_nb_samples = 0, dst_linsize = 0;
    uint8_t *dst_data[1] = { NULL };
    uint8_t **data;
    AVPacket packet;

    std::cout << "Processing file " << fileName << std::endl;
    AVFormatContext *formatContext = NULL;

    if (avformat_open_input(&formatContext, fileName, NULL, NULL) != 0) {
        std::cerr << "Error opening the file: " << fileName << std::endl;
        return 1;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        avformat_close_input(&formatContext);
        std::cerr << "Error finding the stream info: " << fileName << std::endl;
        return 1;
    }

    AVCodec *codec = NULL;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (streamIndex < 0) {
        avformat_close_input(&formatContext);
        std::cout << "Could not find any audio stream in the file" << std::endl;
        return 1;
    }

    //std::cout << "Using stream number: " << streamIndex + 1 << " (out of " << formatContext->nb_streams << ")" << std::endl;
    AVStream * audioStream = formatContext->streams[streamIndex];
    //std::cout << "codec type: " << audioStream->codec->codec_type << "  id: " << audioStream->codec->codec_id << std::endl;
    //std::cout << "codec type: " << codec->type << "  id: " << codec->id << std::endl;

    AVCodecContext * codecContext = audioStream->codec;
    codecContext->codec = codec;

    if (avcodec_open2(codecContext, codec, NULL) != 0) {
        avformat_close_input(&formatContext);
        std::cout << "Couldn't open the context with the decoder" << std::endl;
        return 1;
    }

    //std::cout << "This stream has " << codecContext->channels << " channels and a sample rate of " << codecContext->sample_rate << "Hz" << std::endl;
    //std::cout << "The data is in the format " << av_get_sample_fmt_name(codecContext->sample_fmt) << std::endl;

    if (codecContext->channels <= 0) {
        avformat_close_input(&formatContext);
        std::cout << "No channels found in the audio stream" << std::endl;
        return 1;
    }

    AVAudioResampleContext *convert_ctx = NULL;
    if (codecContext->sample_fmt != AV_SAMPLE_FMT_S16) {
        int64_t channel_layout = codecContext->channel_layout;
        if (!channel_layout) {
            channel_layout = av_get_default_channel_layout(codecContext->channels);
        }

        convert_ctx = avresample_alloc_context();
        av_opt_set_int(convert_ctx, "out_channel_layout", channel_layout, 0);
        av_opt_set_int(convert_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        av_opt_set_int(convert_ctx, "out_sample_rate", codecContext->sample_rate, 0);
        av_opt_set_int(convert_ctx, "in_channel_layout", channel_layout, 0);
        av_opt_set_int(convert_ctx, "in_sample_fmt", codecContext->sample_fmt, 0);
        av_opt_set_int(convert_ctx, "in_sample_rate", codecContext->sample_rate, 0);
        if (!convert_ctx) {
            avformat_close_input(&formatContext);
            std::cout << "Couldn't allocate audio converter" << std::endl;
            return 1;
        }
        if (avresample_open(convert_ctx) < 0) {
            avresample_free(&convert_ctx);
            avformat_close_input(&formatContext);
            std::cout << "Couldn't initialize the audio converter" << std::endl;
            return 1;
        }
    }

    if (audioStream->duration != AV_NOPTS_VALUE) {
        *duration = audioStream->time_base.num * audioStream->duration / audioStream->time_base.den;
    } else if (formatContext->duration != AV_NOPTS_VALUE) {
        *duration = formatContext->duration / AV_TIME_BASE;
    } else {
        avresample_free(&convert_ctx);
        avformat_close_input(&formatContext);
        std::cout << "Couldn't detect the audio duration" << std::endl;
        return 1;
    }

    remaining = max_length * codecContext->channels * codecContext->sample_rate;
    chromaprint_start(chromaprint_ctx, codecContext->sample_rate, codecContext->channels);

    AVFrame * frame = avcodec_alloc_frame();

    while (1) {
        if (av_read_frame(formatContext, &packet) < 0) {
            break;
        }

        if (packet.stream_index == streamIndex) {
            avcodec_get_frame_defaults(frame);

            got_frame = 0;
            consumed = avcodec_decode_audio4(codecContext, frame, &got_frame, &packet);
            if (consumed < 0) {
                std::cerr << "WARNING: error decoding audio" << std::endl;
                continue;
            }

            if (got_frame) {
                data = frame->data;
                if (convert_ctx) {
                    if (frame->nb_samples > max_dst_nb_samples) {
                        av_freep(&dst_data[0]);
                        if (av_samples_alloc(dst_data, &dst_linsize, codecContext->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1) < 0) {
                            std::cerr << "ERROR: couldn't allocate audio converter buffer" << std::endl;
                            goto done;
                        }
                        max_dst_nb_samples = frame->nb_samples;
                    }

                    if (avresample_convert(convert_ctx, dst_data, 0, frame->nb_samples, (uint8_t **)frame->data, 0, frame->nb_samples) < 0) {
                        std::cerr << "ERROR: couldn't convert the audio" << std::endl;
                        goto done;
                    }
                    data = dst_data;
                }
                length = MIN(remaining, frame->nb_samples * codecContext->channels);
                if (!chromaprint_feed(chromaprint_ctx, data[0], length)) {
                    goto done;
                }

                if (max_length) {
                    remaining -= length;
                    if (remaining <= 0) {
                        goto finish;
                    }
                }
            }
        }
        av_free_packet(&packet);
    }

finish:
    if (!chromaprint_finish(chromaprint_ctx)) {
        std::cerr << "ERROR: fingerprint calculation failed" << std::endl;
        goto done;
    }

done:

    avcodec_free_frame(&frame);

    if (dst_data[0]) {
        av_freep(&dst_data[0]);
    }
    if (convert_ctx) {
        avresample_free(&convert_ctx);
    }

    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    return 0;
}

std::string getFingerprint(const char * fileName) {
    std::string lookup;
    char * fingerprint = NULL;
    int raw_fingerprint_size;
    int duration;
    int32_t *raw_fingerprint;

    ChromaprintContext *chromaprint_ctx;
    int algo = CHROMAPRINT_ALGORITHM_DEFAULT;
    //int algo = CHROMAPRINT_ALGORITHM_TEST1;
    //int algo = CHROMAPRINT_ALGORITHM_TEST2;
    //int algo = CHROMAPRINT_ALGORITHM_TEST3;
    //int algo = CHROMAPRINT_ALGORITHM_TEST4;

    int max_length = 120; // Length of audio to use for fingerprint. Default 120
    int raw = 0; // set to get raw fingerprint instead of ???FIXME???

    av_register_all();
    av_log_set_level(AV_LOG_ERROR);

    chromaprint_ctx = chromaprint_new(algo);

    // Set any options
    //chromaprint_set_option(chromaprint_ctx, name, atoi(value));

    if (decode_audio_file(chromaprint_ctx, fileName, max_length, &duration)) {
        std::cerr << "ERROR: unable to calculate fingerprint for file " << fileName << std::endl;
        chromaprint_free(chromaprint_ctx);
        return lookup;
    }

    //std::cout << "FILE=" << fileName << std::endl;
    //std::cout << "DURATION=" << duration << std::endl;
    if (raw) {
        if (!chromaprint_get_raw_fingerprint(chromaprint_ctx, (void **)&raw_fingerprint, &raw_fingerprint_size)) {
            std::cerr << "ERROR: unable to calculate raw fingerprint for file " << fileName << std::endl;
            chromaprint_free(chromaprint_ctx);
            return lookup;
        }
        //std::cout << "FINGERPRINT=";
        //for (int j = 0; j < raw_fingerprint_size; j++) {
        //    std::cout << raw_fingerprint[j];
        //    if (j + 1 < raw_fingerprint_size) {
        //        std::cout << ",";
        //    }
        //}
        //std::cout << std::endl;
        std::stringstream lookupSS;
        lookupSS << "http://api.acoustid.org/v2/lookup?"
            << "client=" << CLIENT_ID << "&"
            << "duration=" << duration << "&"
            << "fingerprint=" << fingerprint << "&"
            << "meta=recordings+releases+releasegroups"
        ;
        //recordings, recordingids, releases, releaseids, releasegroups, releasegroupids, tracks, compress, usermeta, sources

        lookup = lookupSS.str();

        chromaprint_dealloc(raw_fingerprint);
    } else {
        if (!chromaprint_get_fingerprint(chromaprint_ctx, &fingerprint)) {
            std::cerr << "ERROR: unable to calculate fingerprint for file " << fileName << std::endl;
            chromaprint_free(chromaprint_ctx);
            return lookup;
        }
        //std::cout << "FINGERPRINT=" << fingerprint << std::endl;

        std::stringstream lookupSS;
        lookupSS << "http://api.acoustid.org/v2/lookup?"
            << "client=" << CLIENT_ID << "&"
            << "duration=" << duration << "&"
            << "fingerprint=" << fingerprint << "&"
            << "meta=recordings+releases+releasegroups"
        ;
        //recordings, recordingids, releases, releaseids, releasegroups, releasegroupids, tracks, compress, usermeta, sources

        lookup = lookupSS.str();

        chromaprint_dealloc(fingerprint);
    }

    chromaprint_free(chromaprint_ctx);

    return lookup;
}
