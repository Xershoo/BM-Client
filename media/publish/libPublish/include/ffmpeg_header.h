#ifndef FFMPEG_HERADER_H
#define FFMPEG_HERADER_H

extern "C"
{
	#include "libavutil/time.h"
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	#include "libswresample/swresample.h"
}

typedef void (*ff_av_register_all)();

typedef int (*ff_avformat_network_init)(void);

typedef void* (*ff_av_mallocz)(size_t size);

typedef AVIOContext* (*ff_avio_alloc_context)(
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int64_t (*seek)(void *opaque, int64_t offset, int whence));

typedef int (*ff_avio_close)(AVIOContext *s);

typedef int (*ff_av_probe_input_buffer)(AVIOContext *pb, AVInputFormat **fmt,
                          const char *filename, void *logctx,
                          unsigned int offset, unsigned int max_probe_size);

typedef AVFormatContext* (*ff_avformat_alloc_context)(void);

typedef int (*ff_avformat_open_input)(AVFormatContext **ps, const char *filename, AVInputFormat *fmt, AVDictionary **options);

typedef int (*ff_av_find_stream_info)(AVFormatContext *ic);

typedef void (*ff_av_dump_format)(AVFormatContext *ic,
                    int index,
                    const char *url,
                    int is_output);

typedef int (*ff_av_read_frame)(AVFormatContext *s, AVPacket *pkt);

typedef void (*ff_av_free_packet)(AVPacket *pkt);

typedef void (*ff_av_init_packet)(AVPacket *pkt);

typedef void (*ff_av_free)(void *ptr);

typedef void (*ff_avcodec_flush_buffers)(AVCodecContext *avctx);

typedef AVCodec* (*ff_avcodec_find_decoder)(enum AVCodecID id);

typedef int (*ff_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

typedef int (*ff_av_samples_get_buffer_size)(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);

typedef  int (*ff_avcodec_decode_video2)(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const AVPacket *avpkt);

typedef  int (*ff_avcodec_decode_audio4)(AVCodecContext *avctx, AVFrame *frame,
                          int *got_frame_ptr, const AVPacket *avpkt);

typedef int  (*ff_swr_init)(struct SwrContext *s);

typedef int64_t (*ff_av_rescale_q)(int64_t a, AVRational bq, AVRational cq);

typedef int64_t (*ff_av_frame_get_best_effort_timestamp)(const AVFrame *frame);

typedef int (*ff_av_get_bytes_per_sample)(enum AVSampleFormat sample_fmt);

typedef int (*ff_swr_convert)(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);

typedef int (*ff_swr_set_compensation)(struct SwrContext *s, int sample_delta, int compensation_distance);

typedef void (*ff_swr_free)(struct SwrContext **s);

typedef struct SwrContext *(*ff_swr_alloc_set_opts)(struct SwrContext *s,
                                      int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
                                      int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
                                      int log_offset, void *log_ctx);

typedef  int     (*ff_av_frame_get_channels)  (const AVFrame *frame);

typedef int (*ff_av_get_channel_layout_nb_channels)(uint64_t channel_layout);

typedef int64_t (*ff_av_get_default_channel_layout)(int nb_channels);

typedef AVFrame * (*ff_avcodec_alloc_frame)(void);

typedef void (*ff_av_fast_malloc)(void *ptr, unsigned int *size, size_t min_size);

typedef av_const int (*ff_av_log2)(unsigned v);

typedef void (*ff_avformat_close_input)(AVFormatContext **s);

typedef int (*ff_avcodec_close)(AVCodecContext *avctx);\

typedef int (*ff_avformat_seek_file)(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);

typedef AVRational (*ff_av_stream_get_r_frame_rate)(const AVStream *s);

typedef void      (*ff_av_stream_set_r_frame_rate)(AVStream *s, AVRational r);

typedef int (*ff_avcodec_copy_context)(AVCodecContext *dest, const AVCodecContext *src);

typedef int (*ff_avformat_find_stream_info)(AVFormatContext *ic, AVDictionary **options);

typedef int (*ff_avformat_alloc_output_context2)(AVFormatContext **ctx, AVOutputFormat *oformat,
                                   const char *format_name, const char *filename);

typedef AVStream *(*ff_avformat_new_stream)(AVFormatContext *s, const AVCodec *c);

typedef int (*ff_avio_open2)(AVIOContext **s, const char *url, int flags,
               const AVIOInterruptCB *int_cb, AVDictionary **options);

typedef int (*ff_avformat_write_header)(AVFormatContext *s, AVDictionary **options);

typedef int64_t (*ff_av_rescale_q_rnd)(int64_t a, AVRational bq, AVRational cq,
                         enum AVRounding) av_const;

typedef int (*ff_av_interleaved_write_frame)(AVFormatContext *s, AVPacket *pkt);

typedef int (*ff_av_write_frame)(AVFormatContext *s, AVPacket *pkt);

typedef void (*ff_avformat_free_context)(AVFormatContext *s);

typedef int (*ff_av_copy_packet)(AVPacket *dst, const AVPacket *src);

typedef void (*ff_avcodec_free_frame)(AVFrame **frame);

typedef int (*ff_av_usleep)(unsigned usec);

typedef int (*ff_av_read_pause)(AVFormatContext *s);

typedef int (*ff_av_read_play)(AVFormatContext *s);

typedef struct SwsContext *(*ff_sws_getContext)(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
                                  int flags, SwsFilter *srcFilter,
                                  SwsFilter *dstFilter, const double *param);

typedef void (*ff_sws_freeContext)(struct SwsContext *swsContext);

typedef int (*ff_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);

typedef void (*ff_avio_flush)(AVIOContext *s);

typedef int (*ff_av_opt_set_int )(void *obj, const char *name, int64_t     val, int search_flags);
#endif