#pragma once
#include "ffmpeg_header.h"

#define  FFMPEG_AVCODEC_DLL_NAME     L"avcodec-55.dll"
#define  FFMPEG_SWSCALE_DLL_NAME     L"swscale-2.dll"
#define  FFMPEG_AVUTIL_DLL_NAME      L"avutil-52.dll"
#define  FFMPEG_AVFORMAT_DLL_NAME    L"avformat-55.dll"
#define  FFMPEG_SWRESAMPLE_DLL_NAME  L"swresample-0.dll"

// #define  FFMPEG_AVCODEC_DLL_NAME     L"avcodec-57.dll"
// #define  FFMPEG_SWSCALE_DLL_NAME     L"swscale-4.dll"
// #define  FFMPEG_AVUTIL_DLL_NAME      L"avutil-55.dll"
// #define  FFMPEG_AVFORMAT_DLL_NAME    L"avformat-57.dll"
// #define  FFMPEG_SWRESAMPLE_DLL_NAME  L"swresample-2.dll"


class CFFMPEGProxy
{
private:
	CFFMPEGProxy();
	~CFFMPEGProxy();
public:
	static inline CFFMPEGProxy* GetInterface() { return m_FFMPEGProxy; }
	static void CreateFFMPEGProxy() ;
	static void DestoryFFMPEGProxy() ;
private:
	HMODULE           m_avcodec_module;
	HMODULE			m_swscale_module;
	HMODULE			m_avutil_module;
	HMODULE			m_avforamt_module;
	HMODULE			m_swresample_module;
public:
	ff_av_register_all        m_pff_av_register_all;
	#define def_ff_av_register_all CFFMPEGProxy::GetInterface()->m_pff_av_register_all

	ff_avformat_network_init  m_pff_avformat_network_init;
	#define def_ff_avformat_network_init CFFMPEGProxy::GetInterface()->m_pff_avformat_network_init

	ff_av_stream_get_r_frame_rate m_pff_av_stream_get_r_frame_rate;
	#define def_ff_av_stream_get_r_frame_rate CFFMPEGProxy::GetInterface()->m_pff_av_stream_get_r_frame_rate

	ff_av_stream_set_r_frame_rate m_pff_av_stream_set_r_frame_rate;
	#define def_ff_av_stream_set_r_frame_rate CFFMPEGProxy::GetInterface()->m_pff_av_stream_set_r_frame_rate

	ff_avformat_find_stream_info  m_pff_avformat_find_stream_info;
	#define def_ff_avformat_find_stream_info CFFMPEGProxy::GetInterface()->m_pff_avformat_find_stream_info

	ff_avformat_alloc_output_context2 m_pff_avformat_alloc_output_context2;
	#define def_ff_avformat_alloc_output_context2 CFFMPEGProxy::GetInterface()->m_pff_avformat_alloc_output_context2

	ff_avformat_new_stream m_pff_avformat_new_stream;
	#define def_ff_avformat_new_stream CFFMPEGProxy::GetInterface()->m_pff_avformat_new_stream

	ff_avio_open2 m_pff_avio_open2;
	#define def_ff_avio_open2 CFFMPEGProxy::GetInterface()->m_pff_avio_open2

	ff_avio_flush m_pff_avio_flush;
	#define def_ff_avio_flush CFFMPEGProxy::GetInterface()->m_pff_avio_flush

	ff_avformat_write_header m_pff_avformat_write_header;
	#define def_ff_avformat_write_header CFFMPEGProxy::GetInterface()->m_pff_avformat_write_header

	ff_av_interleaved_write_frame m_pff_av_interleaved_write_frame;
	#define def_ff_av_interleaved_write_frame CFFMPEGProxy::GetInterface()->m_pff_av_interleaved_write_frame

	ff_av_write_frame m_pff_av_write_frame;
	#define def_ff_av_write_frame CFFMPEGProxy::GetInterface()->m_pff_av_write_frame

	ff_avformat_free_context m_pff_avformat_free_context;
	#define def_ff_avformat_free_context CFFMPEGProxy::GetInterface()->m_pff_avformat_free_context

	ff_av_read_pause m_pff_av_read_pause;
	#define def_ff_av_read_pause CFFMPEGProxy::GetInterface()->m_pff_av_read_pause
	ff_av_read_play m_pff_av_read_play;
	#define def_ff_av_read_play CFFMPEGProxy::GetInterface()->m_pff_av_read_play

	ff_avio_alloc_context	  m_pff_avio_alloc_context;
	#define def_ff_avio_alloc_context CFFMPEGProxy::GetInterface()->m_pff_avio_alloc_context
	ff_avio_close             m_pff_avio_close;
	#define def_ff_avio_close  CFFMPEGProxy::GetInterface()->m_pff_avio_close
	ff_av_probe_input_buffer  m_pff_av_probe_input_buffer;
	#define def_ff_av_probe_input_buffer CFFMPEGProxy:: GetInterface()->m_pff_av_probe_input_buffer
	ff_avformat_alloc_context m_pff_avformat_alloc_context;
	#define def_ff_avformat_alloc_context CFFMPEGProxy:: GetInterface()->m_pff_avformat_alloc_context
	ff_avformat_open_input    m_pff_avformat_open_input;
	#define def_ff_avformat_open_input CFFMPEGProxy:: GetInterface()->m_pff_avformat_open_input
	ff_av_find_stream_info    m_pff_av_find_stream_info;
	#define def_ff_av_find_stream_info CFFMPEGProxy:: GetInterface()->m_pff_av_find_stream_info
	ff_av_dump_format         m_pff_av_dump_format;
	#define def_ff_av_dump_format CFFMPEGProxy:: GetInterface()->m_pff_av_dump_format
	ff_av_read_frame          m_pff_av_read_frame;
	#define def_ff_av_read_frame  CFFMPEGProxy:: GetInterface()->m_pff_av_read_frame 
	ff_avformat_close_input   m_pff_avformat_close_input;
	#define def_ff_avformat_close_input CFFMPEGProxy:: GetInterface()->m_pff_avformat_close_input 
	ff_avformat_seek_file     m_pff_avformat_seek_file;
	#define def_ff_avformat_seek_file CFFMPEGProxy:: GetInterface()->m_pff_avformat_seek_file 
	
public:
	ff_avcodec_flush_buffers  m_pff_avcodec_flush_buffers;
	#define def_ff_avcodec_flush_buffers CFFMPEGProxy:: GetInterface()->m_pff_avcodec_flush_buffers
	ff_av_free_packet         m_pff_av_free_packet;
	#define def_ff_av_free_packet   CFFMPEGProxy:: GetInterface()->m_pff_av_free_packet
	ff_av_init_packet         m_pff_av_init_packet;
	#define def_ff_av_init_packet    CFFMPEGProxy:: GetInterface()->m_pff_av_init_packet 
	ff_avcodec_find_decoder   m_pff_avcodec_find_decoder;
	#define def_ff_avcodec_find_decoder    CFFMPEGProxy:: GetInterface()->m_pff_avcodec_find_decoder 
	ff_avcodec_open2          m_pff_avcodec_open2;
	#define def_ff_avcodec_open2     CFFMPEGProxy:: GetInterface()->m_pff_avcodec_open2 
	ff_avcodec_decode_video2  m_pff_avcodec_decode_video2;
	#define def_ff_avcodec_decode_video2      CFFMPEGProxy:: GetInterface()->m_pff_avcodec_decode_video2 
	ff_avcodec_decode_audio4  m_pff_avcodec_decode_audio4;
	#define def_ff_avcodec_decode_audio4      CFFMPEGProxy:: GetInterface()->m_pff_avcodec_decode_audio4 
	ff_avcodec_alloc_frame       m_pff_avcodec_alloc_frame;
	#define def_ff_avcodec_alloc_frame       CFFMPEGProxy:: GetInterface()->m_pff_avcodec_alloc_frame
	ff_avcodec_close            m_pff_avcodec_close;
	#define def_ff_avcodec_close  CFFMPEGProxy:: GetInterface()->m_pff_avcodec_close;

	ff_avcodec_copy_context m_pff_avcodec_copy_context;
	#define def_ff_avcodec_copy_context  CFFMPEGProxy:: GetInterface()->m_pff_avcodec_copy_context

	ff_av_copy_packet m_pff_av_copy_packet;
	#define def_ff_av_copy_packet  CFFMPEGProxy:: GetInterface()->m_pff_av_copy_packet
	ff_avcodec_free_frame m_pff_avcodec_free_frame;
	#define def_ff_avcodec_free_frame  CFFMPEGProxy:: GetInterface()->m_pff_avcodec_free_frame

public:
	ff_av_mallocz             m_pff_av_mallocz;
	#define def_ff_av_mallocz     CFFMPEGProxy:: GetInterface()->m_pff_av_mallocz
	ff_av_free                m_pff_av_free;
	#define def_ff_av_free      CFFMPEGProxy:: GetInterface()->m_pff_av_free
	ff_av_samples_get_buffer_size  m_pff_av_samples_get_buffer_size;
	#define def_ff_av_samples_get_buffer_size      CFFMPEGProxy::GetInterface()->m_pff_av_samples_get_buffer_size
	ff_av_rescale_q             m_pff_av_rescale_q;
	#define def_ff_av_rescale_q       CFFMPEGProxy:: GetInterface()->m_pff_av_rescale_q 
	ff_av_frame_get_best_effort_timestamp m_pff_av_frame_get_best_effort_timestamp;
	#define def_ff_av_frame_get_best_effort_timestamp      CFFMPEGProxy:: GetInterface()->m_pff_av_frame_get_best_effort_timestamp
	ff_av_get_bytes_per_sample m_pff_av_get_bytes_per_sample;
	#define def_ff_av_get_bytes_per_sample      CFFMPEGProxy:: GetInterface()->m_pff_av_get_bytes_per_sample
	ff_av_frame_get_channels   m_pff_av_frame_get_channels;
	#define def_ff_av_frame_get_channels      CFFMPEGProxy:: GetInterface()->m_pff_av_frame_get_channels
	ff_av_get_channel_layout_nb_channels  m_pff_av_get_channel_layout_nb_channels;
	#define def_ff_av_get_channel_layout_nb_channels      CFFMPEGProxy:: GetInterface()->m_pff_av_get_channel_layout_nb_channels
	ff_av_get_default_channel_layout    m_pff_av_get_default_channel_layout;
	#define def_ff_av_get_default_channel_layout      CFFMPEGProxy:: GetInterface()->m_pff_av_get_default_channel_layout
	ff_av_fast_malloc    m_pff_av_fast_malloc;
	#define def_ff_av_fast_malloc      CFFMPEGProxy:: GetInterface()->m_pff_av_fast_malloc
	ff_av_log2          m_pff_av_log2;
	#define def_ff_av_log2      CFFMPEGProxy:: GetInterface()->m_pff_av_log2
	ff_av_rescale_q_rnd  m_pff_av_rescale_q_rnd;
	#define def_ff_av_rescale_q_rnd  CFFMPEGProxy:: GetInterface()->m_pff_av_rescale_q_rnd

	ff_av_usleep m_pff_av_usleep;
	#define def_ff_av_usleep  CFFMPEGProxy:: GetInterface()->m_pff_av_usleep

	ff_av_opt_set_int m_pff_av_opt_set_int;
	#define def_ff_av_opt_set_int  CFFMPEGProxy:: GetInterface()->m_pff_av_opt_set_int
public:
	ff_swr_init               m_pff_swr_init;
	#define def_ff_swr_init      CFFMPEGProxy:: GetInterface()->m_pff_swr_init
	ff_swr_convert            m_pff_swr_convert;
	#define def_ff_swr_convert       CFFMPEGProxy:: GetInterface()->m_pff_swr_convert 
	ff_swr_set_compensation   m_pff_swr_set_compensation;
	#define def_ff_swr_set_compensation       CFFMPEGProxy:: GetInterface()->m_pff_swr_set_compensation
	ff_swr_free               m_pff_swr_free;
	#define def_ff_swr_free        CFFMPEGProxy:: GetInterface()->m_pff_swr_free 
	ff_swr_alloc_set_opts     m_pff_swr_alloc_set_opts;
	#define def_ff_swr_alloc_set_opts        CFFMPEGProxy:: GetInterface()->m_pff_swr_alloc_set_opts 
public:
	ff_sws_getContext m_pff_sws_getContext;
	#define def_ff_sws_getContext  CFFMPEGProxy:: GetInterface()->m_pff_sws_getContext 
	ff_sws_freeContext m_pff_sws_freeContext;
	#define def_ff_sws_freeContext  CFFMPEGProxy:: GetInterface()->m_pff_sws_freeContext
	ff_sws_scale m_pff_sws_scale;
	#define def_ff_sws_scale  CFFMPEGProxy:: GetInterface()->m_pff_sws_scale
private:
	static CFFMPEGProxy*   m_FFMPEGProxy;
};

#define FF_FFUNC_PROXY(cb_1 ,...) \
		assert(cb_1) ; \
		cb_1(  __VA_ARGS__);
