#include<Windows.h>
#include "loadffmpeg.h"
#include "shlwapi.h"
#include <assert.h>

/*#define ff_init_fuction_get_proc_address(module,str_name) \
	{ \
		m_pff_#strname = (ff_#strname)GetProcAddress(module,str_name);\
		assert(m_pff_#strname ); \
	}*/

CFFMPEGProxy*  CFFMPEGProxy:: m_FFMPEGProxy = NULL;

CFFMPEGProxy::CFFMPEGProxy()
{
	m_pff_av_register_all = NULL;
	m_pff_av_mallocz = NULL;
	m_pff_avio_alloc_context = NULL;
	m_pff_av_probe_input_buffer = NULL;
	m_pff_avformat_alloc_context = NULL;
	m_pff_avformat_open_input = NULL;
	m_pff_av_find_stream_info = NULL;
	m_pff_av_dump_format = NULL;
	m_pff_av_read_frame = NULL;
	m_pff_av_free_packet = NULL;
	m_pff_av_init_packet = NULL;
	m_pff_av_free = NULL;
	m_pff_avcodec_flush_buffers = NULL;
	m_pff_avcodec_find_decoder = NULL;
	m_pff_avcodec_open2 = NULL;
	m_pff_av_samples_get_buffer_size = NULL;
	m_pff_avcodec_decode_audio4 = NULL;
	m_pff_avcodec_decode_video2 = NULL;
	m_pff_swr_init = NULL;
	m_pff_av_rescale_q = NULL;
	m_pff_av_frame_get_best_effort_timestamp = NULL;
	m_pff_av_get_bytes_per_sample = NULL;
	m_pff_swr_set_compensation = NULL;
	m_pff_swr_convert = NULL;
	m_pff_swr_free = NULL;
	m_pff_av_frame_get_channels = NULL;
	m_pff_swr_alloc_set_opts = NULL;
	m_pff_av_get_channel_layout_nb_channels = NULL;
	m_pff_av_get_default_channel_layout = NULL;
	m_pff_avcodec_alloc_frame = NULL;
	m_pff_av_fast_malloc = NULL;
	m_pff_av_log2 = 0;
	m_pff_avformat_close_input = NULL;
	m_pff_avformat_find_stream_info = NULL;
	m_pff_av_stream_set_r_frame_rate = NULL;
	m_pff_av_stream_get_r_frame_rate = NULL;
	m_pff_avformat_network_init = NULL;
	m_pff_avcodec_copy_context = NULL;
	m_pff_avformat_alloc_output_context2 = NULL;
	m_pff_avformat_new_stream = NULL;
	m_pff_avio_open2 = NULL;
	m_pff_av_rescale_q_rnd = NULL;
	m_pff_av_interleaved_write_frame = NULL;
	m_pff_av_write_frame = NULL;
	m_pff_avformat_free_context = NULL;
	m_pff_av_copy_packet = NULL;
	m_pff_avcodec_free_frame = NULL;
	m_pff_av_usleep = NULL;
	m_pff_av_read_pause = NULL;
	m_pff_av_read_play = NULL;
	m_pff_sws_freeContext = NULL;
	m_pff_sws_getContext = NULL;
	m_pff_sws_scale = NULL;
	m_pff_avio_flush = NULL;
	m_pff_av_opt_set_int = NULL;

	//char szFullName[1024] = { 0 };
	//char* moduledir = sc_module_GetLibDir();
	//assert(moduledir!=NULL);

	//sprintf(szFullName,"%s\\ffmpeg",moduledir);
	//SetCurrentDirectory(szFullName);


	TCHAR old_current_directory[MAX_PATH] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);

	GetCurrentDirectory(MAX_PATH, old_current_directory);
	SetCurrentDirectory(szPath);

	m_avcodec_module = LoadLibrary(FFMPEG_AVCODEC_DLL_NAME);
	assert(m_avcodec_module);
	if (m_avcodec_module)
	{
		m_pff_av_init_packet = (ff_av_init_packet)GetProcAddress(m_avcodec_module,"av_init_packet");
		assert(m_pff_av_init_packet);
		m_pff_av_free_packet = (ff_av_free_packet)GetProcAddress(m_avcodec_module,"av_free_packet");
		assert(m_pff_av_free_packet);
		m_pff_avcodec_flush_buffers = (ff_avcodec_flush_buffers)GetProcAddress(m_avcodec_module,"avcodec_flush_buffers");
		assert(m_pff_avcodec_flush_buffers);
		m_pff_avcodec_find_decoder = (ff_avcodec_find_decoder)GetProcAddress(m_avcodec_module,"avcodec_find_decoder");
		assert(m_pff_avcodec_find_decoder);
		m_pff_avcodec_open2 = (ff_avcodec_open2)GetProcAddress(m_avcodec_module,"avcodec_open2");
		assert(m_pff_avcodec_open2);
		m_pff_avcodec_decode_video2 = (ff_avcodec_decode_video2)GetProcAddress(m_avcodec_module,"avcodec_decode_video2");
		assert(m_pff_avcodec_decode_video2);
		m_pff_avcodec_decode_audio4 =  (ff_avcodec_decode_audio4)GetProcAddress(m_avcodec_module,"avcodec_decode_audio4");
		assert(m_pff_avcodec_decode_audio4);
		
		m_pff_avcodec_close = (ff_avcodec_close)GetProcAddress(m_avcodec_module,"avcodec_close");
		assert(m_pff_avcodec_close);
		m_pff_avcodec_copy_context = (ff_avcodec_copy_context)GetProcAddress(m_avcodec_module,"avcodec_copy_context");
		assert(m_pff_avcodec_copy_context);
		m_pff_av_copy_packet = (ff_av_copy_packet)GetProcAddress(m_avcodec_module,"av_copy_packet");
		assert(m_pff_av_copy_packet);
		
	}

	m_swscale_module =  LoadLibrary(FFMPEG_SWSCALE_DLL_NAME);
	assert(m_swscale_module);
	if(m_swscale_module)
	{
		m_pff_sws_getContext = (ff_sws_getContext)GetProcAddress(m_swscale_module,"sws_getContext");
		assert(m_pff_sws_getContext);
		m_pff_sws_freeContext = (ff_sws_freeContext)GetProcAddress(m_swscale_module,"sws_freeContext");
		assert(m_pff_sws_freeContext);
		m_pff_sws_scale = (ff_sws_scale)GetProcAddress(m_swscale_module,"sws_scale");
		assert(m_pff_sws_scale);
	}
	m_avutil_module = LoadLibrary(FFMPEG_AVUTIL_DLL_NAME);
	assert(m_avutil_module);
	if(m_avutil_module)
	{
		m_pff_av_mallocz = (ff_av_mallocz)GetProcAddress(m_avutil_module,"av_mallocz");
		assert(m_pff_av_mallocz);
		m_pff_av_free = (ff_av_free)GetProcAddress(m_avutil_module,"av_free");
		assert(m_pff_av_free);
		m_pff_av_samples_get_buffer_size = (ff_av_samples_get_buffer_size)GetProcAddress(m_avutil_module,"av_samples_get_buffer_size");
		assert(m_pff_av_samples_get_buffer_size);
		m_pff_av_rescale_q = (ff_av_rescale_q)GetProcAddress(m_avutil_module,"av_rescale_q");
		assert(m_pff_av_rescale_q);
		m_pff_av_frame_get_best_effort_timestamp = (ff_av_frame_get_best_effort_timestamp)GetProcAddress(m_avutil_module,"av_frame_get_best_effort_timestamp");
		assert(m_pff_av_frame_get_best_effort_timestamp);
		m_pff_av_get_bytes_per_sample = (ff_av_get_bytes_per_sample)GetProcAddress(m_avutil_module,"av_get_bytes_per_sample");
		assert(m_pff_av_get_bytes_per_sample);
		m_pff_av_frame_get_channels = (ff_av_frame_get_channels)GetProcAddress(m_avutil_module,"av_frame_get_channels");
		assert(m_pff_av_frame_get_channels);
		m_pff_av_get_channel_layout_nb_channels = (ff_av_get_channel_layout_nb_channels)GetProcAddress(m_avutil_module,"av_get_channel_layout_nb_channels");
		assert(m_pff_av_get_channel_layout_nb_channels);
		m_pff_av_get_default_channel_layout = (ff_av_get_default_channel_layout)GetProcAddress(m_avutil_module,"av_get_default_channel_layout");
		assert(m_pff_av_get_default_channel_layout);
		m_pff_av_log2 = (ff_av_log2)GetProcAddress(m_avutil_module,"av_log2");
		assert(m_pff_av_log2);
		m_pff_av_fast_malloc =  (ff_av_fast_malloc)GetProcAddress(m_avutil_module,"av_fast_malloc");
		assert(m_pff_av_fast_malloc);
		m_pff_av_rescale_q_rnd =  (ff_av_rescale_q_rnd)GetProcAddress(m_avutil_module,"av_rescale_q_rnd");
		assert(m_pff_av_rescale_q_rnd);
		m_pff_av_usleep = (ff_av_usleep)GetProcAddress(m_avutil_module,"av_usleep");
		assert(m_pff_av_usleep);

		m_pff_av_opt_set_int = (ff_av_opt_set_int)GetProcAddress(m_avutil_module,"av_opt_set_int");
		assert(m_pff_av_opt_set_int);

		m_pff_avcodec_alloc_frame = (ff_avcodec_alloc_frame)GetProcAddress(m_avutil_module,"av_frame_alloc");
		assert(m_pff_avcodec_alloc_frame);

		m_pff_avcodec_free_frame = (ff_avcodec_free_frame)GetProcAddress(m_avutil_module,"av_frame_free");
		assert(m_pff_avcodec_free_frame);
	}
	
	m_avforamt_module = LoadLibrary(FFMPEG_AVFORMAT_DLL_NAME);
	assert(m_avforamt_module);
	if(m_avforamt_module)
	{
		m_pff_av_register_all = (ff_av_register_all)GetProcAddress(m_avforamt_module,"av_register_all");
		assert(m_pff_av_register_all);

		m_pff_avformat_network_init = (ff_avformat_network_init)GetProcAddress(m_avforamt_module,"avformat_network_init");
		assert(m_pff_av_register_all);
		m_pff_av_stream_get_r_frame_rate = (ff_av_stream_get_r_frame_rate)GetProcAddress(m_avforamt_module,"av_stream_get_r_frame_rate");
		assert(m_pff_av_stream_get_r_frame_rate);

		m_pff_av_stream_set_r_frame_rate = (ff_av_stream_set_r_frame_rate)GetProcAddress(m_avforamt_module,"av_stream_set_r_frame_rate");
		assert(m_pff_av_stream_set_r_frame_rate);
		m_pff_avformat_find_stream_info = (ff_avformat_find_stream_info)GetProcAddress(m_avforamt_module,"avformat_find_stream_info");
		assert(m_pff_avformat_find_stream_info);

		m_pff_avformat_alloc_output_context2 = (ff_avformat_alloc_output_context2)GetProcAddress(m_avforamt_module,"avformat_alloc_output_context2");
		assert(m_pff_avformat_alloc_output_context2);
		m_pff_avformat_new_stream = (ff_avformat_new_stream)GetProcAddress(m_avforamt_module,"avformat_new_stream");
		assert(m_pff_avformat_new_stream);
		m_pff_avio_open2 = (ff_avio_open2)GetProcAddress(m_avforamt_module,"avio_open2");
		assert(m_pff_avio_open2);

		m_pff_avio_flush = (ff_avio_flush)GetProcAddress(m_avforamt_module,"avio_flush");
		assert(m_pff_avio_flush);

		m_pff_avformat_write_header = (ff_avformat_write_header)GetProcAddress(m_avforamt_module,"avformat_write_header");
		assert(m_pff_avformat_write_header);
		m_pff_av_interleaved_write_frame = (ff_av_interleaved_write_frame)GetProcAddress(m_avforamt_module,"av_interleaved_write_frame");
		assert(m_pff_av_interleaved_write_frame);

		m_pff_av_write_frame =  (ff_av_write_frame)GetProcAddress(m_avforamt_module,"av_write_frame");
		assert(m_pff_av_write_frame);

		m_pff_avformat_free_context = (ff_avformat_free_context)GetProcAddress(m_avforamt_module,"avformat_free_context");
		assert(m_pff_avformat_free_context);
		m_pff_av_read_pause = (ff_av_read_pause)GetProcAddress(m_avforamt_module,"av_read_pause");
		assert(m_pff_av_read_pause);
		m_pff_av_read_play = (ff_av_read_play )GetProcAddress(m_avforamt_module,"av_read_play");
		assert(m_pff_av_read_play);

		m_pff_avio_alloc_context = (ff_avio_alloc_context)GetProcAddress(m_avforamt_module,"avio_alloc_context");
		assert(m_pff_avio_alloc_context);
		m_pff_av_probe_input_buffer = (ff_av_probe_input_buffer)GetProcAddress(m_avforamt_module,"av_probe_input_buffer");
		assert(m_pff_av_probe_input_buffer);
		m_pff_avformat_alloc_context = (ff_avformat_alloc_context)GetProcAddress(m_avforamt_module,"avformat_alloc_context");
		assert(m_pff_avformat_alloc_context);
		m_pff_avformat_open_input = (ff_avformat_open_input)GetProcAddress(m_avforamt_module,"avformat_open_input");
		assert(m_pff_avformat_open_input);
		m_pff_av_find_stream_info = (ff_av_find_stream_info)GetProcAddress(m_avforamt_module,"av_find_stream_info");
		assert(m_pff_av_find_stream_info);
		m_pff_av_dump_format = (ff_av_dump_format)GetProcAddress(m_avforamt_module,"av_dump_format");
		assert(m_pff_av_dump_format);
		m_pff_av_read_frame = (ff_av_read_frame)GetProcAddress(m_avforamt_module,"av_read_frame");
		assert(m_pff_av_read_frame);
		m_pff_avformat_close_input = (ff_avformat_close_input)GetProcAddress(m_avforamt_module,"avformat_close_input");
		assert(m_pff_avformat_close_input);
		m_pff_avio_close = (ff_avio_close)GetProcAddress(m_avforamt_module,"avio_close");
		assert(m_pff_avio_close);
		m_pff_avformat_seek_file = (ff_avformat_seek_file)GetProcAddress(m_avforamt_module,"avformat_seek_file");
		assert(m_pff_avformat_seek_file);
	}

	m_swresample_module = LoadLibrary(FFMPEG_SWRESAMPLE_DLL_NAME);
	assert(m_swresample_module);
	if(m_swresample_module)
	{
		m_pff_swr_init = (ff_swr_init)GetProcAddress(m_swresample_module,"swr_init");
		assert(m_pff_swr_init);
		m_pff_swr_convert = (ff_swr_convert)GetProcAddress(m_swresample_module,"swr_convert");
		assert(m_pff_swr_convert);
		m_pff_swr_set_compensation = (ff_swr_set_compensation)GetProcAddress(m_swresample_module,"swr_set_compensation");
		assert(m_pff_swr_set_compensation);
		m_pff_swr_free = (ff_swr_free)GetProcAddress(m_swresample_module,"swr_free");
		assert(m_pff_swr_free);
		m_pff_swr_alloc_set_opts = (ff_swr_alloc_set_opts)GetProcAddress(m_swresample_module,"swr_alloc_set_opts");
		assert(m_pff_swr_alloc_set_opts);
	}

	SetCurrentDirectory(old_current_directory);
	//free(moduledir);
	//tdef_ff_av_register_all();
}

CFFMPEGProxy::~CFFMPEGProxy()
{
	if (m_avcodec_module)
	{
		FreeLibrary(m_avcodec_module);
		m_avcodec_module = NULL;
	}

	if (m_swscale_module)
	{
		FreeLibrary(m_swscale_module);
		m_swscale_module = NULL;
	}

	if (m_avutil_module)
	{
		FreeLibrary(m_avutil_module);
		m_avutil_module = NULL;
	}
	if (m_avforamt_module)
	{
		FreeLibrary(m_avforamt_module);
		m_avforamt_module = NULL;
	}
}


void CFFMPEGProxy::CreateFFMPEGProxy() 
{
	if(m_FFMPEGProxy == NULL)
		m_FFMPEGProxy = new CFFMPEGProxy();
}

void CFFMPEGProxy::DestoryFFMPEGProxy() 
{
	if(m_FFMPEGProxy)
	{
		delete m_FFMPEGProxy;
		m_FFMPEGProxy = NULL;
	}
}