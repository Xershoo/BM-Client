#include "stdafx.h"
#include "SourFile.h"
#include "TimeUtils.h"
#include "CalculationInterval.h"
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

#define DELMEM(p) { if(p){ delete p; p = NULL;}}
UINT_PTR nTimeEvnet = 1;

void CALLBACK _demux_read_media_loop(void *pUserData)
{
	if(pUserData)
	{
		CSourFile * pUser = (CSourFile *)pUserData;
		pUser->read_in_file_data_to_proc();
	}
}

static void CALLBACK _send_video_loop(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(dwUser)
	{
		CSourFile * pUser = (CSourFile *)dwUser;
		pUser->send_video_porc(uTimerID);
	}
}

static void CALLBACK _send_audio_loop(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(dwUser)
	{
		CSourFile * pUser = (CSourFile *)dwUser;
		pUser->send_audio_porc(uTimerID);
	}
}

void CALLBACK _send_loop(void *pUserData)
{
	if(pUserData)
	{
		CSourFile * pUser = (CSourFile *)pUserData;
		pUser->send_porc(1);
	}
}

void CALLBACK _decode_video_loop(void *pUserData)
{
	if(pUserData)
	{
		CSourFile * pUser = (CSourFile *)pUserData;
		pUser->decode_video_data();
	}
}

void CALLBACK _decode_audio_loop(void *pUserData)
{
	if(pUserData)
	{
		CSourFile * pUser = (CSourFile *)pUserData;
		pUser->decode_audio_data();
	}
}
static int output_thread(void *arg)
{
	if(arg)
	{
		CSourFile* pro = (CSourFile*)arg;
		pro->OutPouLoop();
	}
	return 0;
}

CSourFile::CSourFile(HWND hWnd,int nUseDrawIamgeToolType)
{
	m_str_in_file_url[0] = '\0';
	m_str_out_file_url[0] = '\0';
	memset(&m_show_rect,0,sizeof(HShowWindow));
	m_show_rect.hwnd = hWnd;
	m_show_rect.nDrawToolType = nUseDrawIamgeToolType;
	m_in_fmt_ctx = NULL; //输入格式上下文
	m_out_fmt_ctx = NULL; //输出格式上下文

	m_in_video_index = -1;
	m_in_audio_index = -1;
	m_in_video_st = NULL;
	m_in_audio_st = NULL;

	m_out_video_st = NULL;
	m_out_audio_st = NULL;

	m_in_video_codec_ctx = NULL;
	m_in_audio_codec_ctx = NULL;

	m_in_video_codec = NULL;
	m_in_audio_codec = NULL;

	m_is_video_transcoding = false; //判断视频是否需要转码
	m_is_audio_transcoding = false; //判断音频是否需要转码

	m_stream_count = 0;  //流得个数
	m_sync_time_base = false; //时间轴是否相

	m_pool_free_avpacket_list.clear();
	m_pool_busy_video_avpacket_list.clear();
	m_pool_busy_audio_avpacket_list.clear();
	m_pool_busy_video_frame_list.clear();
	m_pool_busy_audio_frame_list.clear();
	m_pool_busy_send_video_avpacket_list.clear();
	m_pool_busy_sned_audio_avpacket_list.clear();
	m_pool_send_avpacket_list.clear();
	m_pool_busy_video_avpacket_cond.conde_setwaittime(200);
	m_pool_busy_video_frame_cond.conde_setwaittime(200);
	m_pool_busy_audio_avpacket_cond.conde_setwaittime(200);

	m_pscreen = NULL;
	m_video_width = 0;
	m_video_height = 0;
	m_show_window_width = 0;
	m_show_window_height = 0;  
	m_poverlay = NULL;
	m_wanted_spec ;  
	m_audio_buf = NULL;
	m_audio_buf_len = 0;
	m_audio_buf_size = 0;
	m_audio_pos = 0;
	m_au_convert_ctx = NULL;
	memset(&m_audio_hw_params,0,sizeof(AudioParams));
	memset(&m_audio_src,0,sizeof(AudioParams));
	m_push_and_play_pause = false;
	m_video_cur_pts = 0;
	m_audio_cur_pts = 0;
	m_out_pkt_pts_last_time = 0;
	m_out_pkt_dts_last_time = 0;

	m_prev_pts = 0;
	m_prev_dts = 0;
	m_lpDD = NULL;    // DirectDraw 对象指针
	m_lpDDSPrimary = NULL;  // DirectDraw 主表面指针
	m_lpDDSOffScr = NULL;  // DirectDraw 离屏表面指针
	m_outputThread = NULL;
	m_bThreshold =  true;

	m_video_time_count = 0;
	m_audio_time_count = 0;
	m_send_video_pts = 0;
	m_send_audio_pts = 0;
	m_send_audio_prev_pts = 0;
	m_send_video_prev_pts = 0;

	m_send_busy_audio_prev_sys_time = 0;
	m_send_free_audio_sys_time = 0;
	m_audio_pkt_pts_time = 0;

	m_send_busy_video_prev_sys_time = 0;
	m_send_free_video_sys_time = 0;
	m_video_pkt_pts_time = 0;

	m_send_Timer = 0;
	//ffmpeg 注册
	def_ff_av_register_all();
	//初始化网络
	def_ff_avformat_network_init();
}

CSourFile::~CSourFile()
{
	close_file_and_uninit();
}


bool CSourFile::copy_video_stream_info(AVStream* ostream, AVStream* istream, AVFormatContext* ofmt_ctx)
{
	AVRational rr = def_ff_av_stream_get_r_frame_rate(istream);

	int ret = def_ff_avcodec_copy_context(ostream->codec, istream->codec);
	if (ret < 0)
	{
		printf( "Failed to copy context from input to output stream codec context\n");
		return false;
	}
	def_ff_av_stream_set_r_frame_rate(ostream,rr);
	ostream->avg_frame_rate = rr;
	ostream->nb_frames = istream->nb_frames;
//	ostream->codec->bit_rate = 1000000;
	ostream->codec->codec_tag = 0;
	if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		ostream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	return true ;
}

bool CSourFile::copy_audio_stream_info(AVStream* ostream, AVStream* istream, AVFormatContext* ofmt_ctx)
{
	int ret = def_ff_avcodec_copy_context(ostream->codec, istream->codec);
	if (ret < 0)
	{
		printf( "Failed to copy context from input to output stream codec context\n");
		return false;
	}
	ostream->codec->codec_tag = 0;
	if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		ostream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	return true ;
}

void CSourFile::improvement_sleep_time(int64_t& n_sleep_time,float& flt_correction_time)
{
	if((abs(flt_correction_time)/1000) > n_sleep_time)
	{
		n_sleep_time = 0;
		flt_correction_time = 0;
	}
	else
	{
		if(abs(flt_correction_time) > 1000)
		{
			n_sleep_time -= (flt_correction_time/1000);
			flt_correction_time = 0;
		}
	}
}

bool CSourFile::open_in_file_and_get_stream_info()
{
	//打开输入文件
	if (def_ff_avformat_open_input(&m_in_fmt_ctx, m_str_in_file_url, 0, 0)< 0)
	{
		printf( "Could not open input file.");
		return false;
	}

	//查找文件的流信息
	if (def_ff_avformat_find_stream_info(m_in_fmt_ctx, 0) < 0)
	{
		printf( "Failed to retrieve input stream information");
		return false;
	}

	def_ff_av_dump_format(m_in_fmt_ctx, 0, m_str_in_file_url, 0);

	//解复用流
	for (int i = 0; i < m_in_fmt_ctx->nb_streams; i++) 
	{
		if ( m_in_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
		{
			m_in_video_index = i;
			m_in_video_st = m_in_fmt_ctx->streams[m_in_video_index];
			m_stream_count++;
		}
		if ( m_in_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_in_audio_index = i;
			m_in_audio_st = m_in_fmt_ctx->streams[m_in_audio_index];
			m_stream_count++;
		}
	}
	
	if(m_stream_count == 1)
		m_sync_time_base = true;

	if(m_in_audio_st == NULL && m_in_audio_st == NULL)
	{
		printf("not find video and audio message\n");
		return false;
	}

	return true;
}


bool CSourFile::check_in_file_stream_code_type()
{
	//获取视频的编码相关信息
	if(m_in_video_st)
	{
		m_in_video_codec_ctx = m_in_video_st->codec;
		if(m_in_video_codec_ctx)
		{
			if(m_in_video_codec_ctx->codec_id == AV_CODEC_ID_H264 
			|| m_in_video_codec_ctx->codec_id == AV_CODEC_ID_FLV1
			|| m_in_video_codec_ctx->codec_id == AV_CODEC_ID_VP6
			|| m_in_video_codec_ctx->codec_id == AV_CODEC_ID_VP6A
			)
			{
				m_is_video_transcoding = false;
			}
			else
			{
				m_is_video_transcoding = true;
			}

		}
	}
	//获取音频的编码相关信息
	if(m_in_audio_st)
	{
		m_in_audio_codec_ctx = m_in_audio_st->codec;
		if(m_in_audio_codec_ctx)
		{
			if(m_in_audio_codec_ctx->codec_id == AV_CODEC_ID_MP3
			||m_in_audio_codec_ctx->codec_id == AV_CODEC_ID_AAC
			||m_in_audio_codec_ctx->codec_id == AV_CODEC_ID_SPEEX
			||m_in_audio_codec_ctx->codec_id == AV_CODEC_ID_FIRST_AUDIO
			)
			{
				m_is_audio_transcoding = false;
			}
			else
			{
				m_is_audio_transcoding = true;
			}
		}
	}
	
	if(m_is_audio_transcoding || m_is_video_transcoding)
		return false;
	return true;
}

bool CSourFile::open_out_file_and_add_stream_info()
{
	if(def_ff_avformat_alloc_output_context2(&m_out_fmt_ctx, NULL, "flv", m_str_out_file_url) < 0 || m_out_fmt_ctx == NULL)
	{
        return false;
    }

	if(m_in_video_st)
	{
		m_out_video_st = def_ff_avformat_new_stream(m_out_fmt_ctx, m_in_video_st->codec->codec);
		if(m_in_video_st && m_in_video_st)
		{
			copy_video_stream_info(m_out_video_st, m_in_video_st, m_out_fmt_ctx);
		}
	}

	if(m_in_audio_st)
	{
		m_out_audio_st = def_ff_avformat_new_stream(m_out_fmt_ctx, m_in_audio_st->codec->codec);
		if(m_out_audio_st && m_in_audio_st)
		{
			copy_video_stream_info(m_out_audio_st, m_in_audio_st, m_out_fmt_ctx);
		}
	}

	def_ff_av_dump_format(m_out_fmt_ctx, 0, m_str_out_file_url, 1);
	 // open file.
    if(def_ff_avio_open2(&m_out_fmt_ctx->pb, m_str_out_file_url, AVIO_FLAG_WRITE, &m_out_fmt_ctx->interrupt_callback, NULL) < 0)
	//if(avio_open(&out_fmt_ctx->pb, out_file_name, AVIO_FLAG_READ_WRITE) < 0)
	{
        return false;
    }

	//些文件头信息
	if(def_ff_avformat_write_header(m_out_fmt_ctx, NULL)< 0)
	{
        return false;
    }
	return true;
}

void CSourFile::read_in_file_data_to_proc()
{


	int64_t v_prev_next_space_time = 0;
	int64_t a_prev_next_space_time = 0;

	// read packet
	while(!m_read_file_and_proc_thread.GetStop())
	{
		if(m_bThreshold && !m_push_and_play_pause )
		{
			bool  bIsFlag = false;
			if(m_in_video_st)
			{
				m_pool_busy_send_video_avpacket_lock.Lock();
				bIsFlag = bIsFlag || (m_pool_busy_send_video_avpacket_list.size() < 10);
				m_pool_busy_send_video_avpacket_lock.Unlock();
			}
			if(m_in_audio_st)
			{
				m_pool_busy_send_audio_avpacket_lock.Lock();
				bIsFlag = bIsFlag || (m_pool_busy_sned_audio_avpacket_list.size() < 15);
				m_pool_busy_send_audio_avpacket_lock.Unlock();
			}

			m_play_lock.Lock();
			if(bIsFlag)
			{
				AVPacket *pkt = get_free_pkt();
				if(def_ff_av_read_frame(m_in_fmt_ctx, pkt) >= 0)
				{
					AVStream *in_stream, *out_stream;
					in_stream  = m_in_fmt_ctx->streams[pkt->stream_index];
					out_stream = m_out_fmt_ctx->streams[pkt->stream_index];
			
					//转换PTS/DTS（Convert PTS/DTS）
					pkt->pts = def_ff_av_rescale_q_rnd(pkt->pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
					pkt->dts = def_ff_av_rescale_q_rnd(pkt->dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
					pkt->duration = def_ff_av_rescale_q(pkt->duration, in_stream->time_base, out_stream->time_base);
					pkt->pos = -1;
					if(pkt->dts < 0)
						pkt->dts = 0;
					if(pkt->pts < 0)
						pkt->pts = 0;

					push_busy_pkt(pkt);
					m_prev_pts = pkt->pts;
					m_prev_dts = pkt->dts;

					pkt->pts += m_out_pkt_pts_last_time;
					pkt->dts += m_out_pkt_pts_last_time;
					if(pkt->stream_index == m_in_video_index) 
					{
						m_pool_busy_send_video_avpacket_lock.Lock();
						m_pool_busy_send_video_avpacket_list.push_back(pkt);
						m_pool_busy_send_video_avpacket_lock.Unlock();
					}
					if(pkt->stream_index == m_in_audio_index)
					{
						static int64_t last_audio_pts_save = 0;
						char szbuf[256] = {'/0'};
						sprintf(szbuf,"audio  pts %lld  diff  %lld\n",pkt->pts,pkt->pts - last_audio_pts_save);
						last_audio_pts_save = pkt->pts;
						OutputDebugString(CString(szbuf));
						m_pool_busy_send_audio_avpacket_lock.Lock();
						m_pool_busy_sned_audio_avpacket_list.push_back(pkt);
						m_pool_busy_send_audio_avpacket_lock.Unlock();
					}
					m_play_lock.Unlock();
					continue;
				}
				else
					put_free_pkt(pkt);
			}
			m_play_lock.Unlock();
		}
		def_ff_av_usleep(10*1000);
	}
}

void CSourFile::send_video_porc(UINT_PTR nEvnet)
{
	AVPacket* vpkt = NULL;
	m_pool_busy_send_video_avpacket_lock.Lock();
	if(m_pool_busy_send_video_avpacket_list.size() > 0 && !m_push_and_play_pause)
	{
		vpkt = m_pool_busy_send_video_avpacket_list.front();
		if(vpkt)
		{

			int64_t pkt_gap_sleep_time = 0;
			int64_t pkt_play_time = 0;
			int64_t pty_sys_time = 0;
			if(m_video_time_count == 0)
				m_send_video_pts = timeGetTime();

			if(m_send_free_video_sys_time > 0)
			{
				m_send_video_pts += m_send_free_video_sys_time;
				m_send_free_video_sys_time = 0;
			}

			if(vpkt->dts - m_send_video_prev_pts <200)
			{
				pkt_play_time = m_video_pkt_pts_time + (vpkt->dts - m_send_video_prev_pts);
			}
			else
				pkt_play_time = m_video_pkt_pts_time + 40;
			
			pty_sys_time = timeGetTime();
			pkt_gap_sleep_time = pty_sys_time - m_send_video_pts ;

			if(((pkt_gap_sleep_time <= pkt_play_time) && (pkt_play_time - pkt_gap_sleep_time <= 20)) 
			|| pkt_gap_sleep_time > pkt_play_time)
			{
				//char szbuf[256] = {'/0'};
				//sprintf(szbuf,"video  pts %lld  --- sys time %lld  \n",pkt_play_time,pkt_gap_sleep_time);
				//OutputDebugString(CString(szbuf));

				m_pool_busy_send_video_avpacket_list.pop_front();
				if(vpkt->dts - m_send_video_prev_pts <200)
					m_video_pkt_pts_time += (vpkt->dts - m_send_video_prev_pts);

				m_send_video_prev_pts = vpkt->dts;
				m_send_busy_video_prev_sys_time = pty_sys_time ;

			}
			else
			{
				vpkt = NULL;
			}
		}
		
	}
	else
	{
		if(m_video_time_count > 0)
			m_send_free_video_sys_time = (timeGetTime() - m_send_busy_video_prev_sys_time);
		//OutputDebugString(L"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	}
	m_pool_busy_send_video_avpacket_lock.Unlock();
	if(vpkt)
	{
		//write frame to file or net
		m_pool_send_avpacket_lock.Lock();
		m_pool_send_avpacket_list.push_back(vpkt);
		m_video_time_count++;
		//OutputDebugString(L"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
		m_pool_send_avpacket_lock.Unlock();
		//free pkt
	}
}

void CSourFile::send_audio_porc(UINT_PTR nEvnet)
{

	AVPacket* apkt = NULL;
	m_pool_busy_send_audio_avpacket_lock.Lock();
	if(m_pool_busy_sned_audio_avpacket_list.size() > 0 && !m_push_and_play_pause)
	{
		apkt = m_pool_busy_sned_audio_avpacket_list.front();
		if(apkt)
		{
			int64_t pkt_gap_sleep_time = 0;
			int64_t pkt_play_time = 0;
			int64_t pty_sys_time = 0;
			if(m_audio_time_count == 0)
				m_send_audio_pts = timeGetTime();
			if(m_send_free_audio_sys_time > 0)
			{
				m_send_audio_pts += m_send_free_audio_sys_time;
				m_send_free_audio_sys_time = 0;
			}

			
			if(apkt->dts - m_send_audio_prev_pts <100)
			{
				pkt_play_time = m_audio_pkt_pts_time + (apkt->dts - m_send_audio_prev_pts);
			}
			else
				pkt_play_time = m_audio_pkt_pts_time + 40;

			pty_sys_time = timeGetTime();
			pkt_gap_sleep_time =  pty_sys_time - m_send_audio_pts;

			if(((pkt_gap_sleep_time <= pkt_play_time) && (pkt_play_time - pkt_gap_sleep_time <= 20)) 
				|| pkt_gap_sleep_time > pkt_play_time)
			{
				
				char szbuf[256] = {'/0'};
				sprintf(szbuf,"audio  pts %lld  --- dts %lld\n",pkt_play_time,pkt_gap_sleep_time);
				//OutputDebugString(CString(szbuf));

				m_pool_busy_sned_audio_avpacket_list.pop_front();
				
				m_send_busy_audio_prev_sys_time = pty_sys_time;
				if(apkt->dts - m_send_audio_prev_pts < 100)
					m_audio_pkt_pts_time += (apkt->dts - m_send_audio_prev_pts);
				m_send_audio_prev_pts = apkt->dts;
				
			}
			else
			{
				apkt = NULL;
			}
		}
		
	}
	else
	{
		if(m_audio_time_count > 0)
			m_send_free_audio_sys_time = (timeGetTime() - m_send_busy_audio_prev_sys_time);
		//OutputDebugString(L"############################################\n");
	}
	m_pool_busy_send_audio_avpacket_lock.Unlock();

	if(apkt)
	{
		//write frame to file or net
		m_pool_send_avpacket_lock.Lock();
		m_pool_send_avpacket_list.push_back(apkt);
		m_audio_time_count++;
		m_pool_send_avpacket_lock.Unlock();
		//free pkt
		
	}
}

void CSourFile::send_porc(UINT_PTR uTimerID)
{
	while(!m_push_and_play_pause)
	{
		AVPacket* apkt = NULL;
		m_pool_send_avpacket_lock.Lock();
		int nSize  = m_pool_send_avpacket_list.size();
		if(nSize <= 0)
		{
			m_pool_send_avpacket_lock.Unlock();
			break;
		}
		apkt = m_pool_send_avpacket_list.front();
		m_pool_send_avpacket_list.pop_front();
		m_pool_send_avpacket_lock.Unlock();
		if(apkt)
		{
			DWORD beg = timeGetTime();
			int nR = def_ff_av_interleaved_write_frame(m_out_fmt_ctx, apkt);
			//def_ff_av_write_frame(m_out_fmt_ctx, apkt);
			def_ff_avio_flush(m_out_fmt_ctx->pb);
			DWORD end = timeGetTime();
			char szbuf[256] = {'/0'};
			sprintf(szbuf,"send one media frame data to net host rime %d\n",end-beg);
		    OutputDebugString(CString(szbuf));
			def_ff_av_free_packet(apkt);
		}
	}
}

bool CSourFile::open_file_and_init(const char* szInUrl,const char* szOutUrl)
{
	if(szInUrl == NULL)
		return false;

	strcpy(m_str_in_file_url,szInUrl);

	if(szOutUrl)
	{
		strcpy(m_str_out_file_url,szOutUrl);
	}

	if(!open_in_file_and_get_stream_info())
		return false;

	if(!check_in_file_stream_code_type())
		return false;

	if(szOutUrl && !open_out_file_and_add_stream_info())
		return false;

	m_read_file_and_proc_thread.Begin(_demux_read_media_loop,this);
	if(m_in_video_st)
	{
		m_send_video_Timer = timeSetEvent(10,1,&_send_video_loop,(DWORD)this,1);
		m_send_video_pts = timeGetTime();
	}
	if(m_in_audio_st)
	{
		m_send_audio_Timer = timeSetEvent(10,1,&_send_audio_loop,(DWORD)this,1);
		m_send_audio_pts = timeGetTime();
	}

	//m_send_Timer = timeSetEvent(5,1,&_send_loop,(DWORD)this,1);

	if(!open_sdl_output())
		return false;

	if(!open_decoder())
		return false;
	return true;
}

bool CSourFile::close_file_and_uninit()
{

	//close file
	if(m_in_fmt_ctx)
	{
		//end read in file thread 
		m_read_file_and_proc_thread.End();

		if(m_send_Timer)
		{
			timeKillEvent(m_send_Timer);
			m_send_Timer = 0;
		}
		//m_send_video_thread.End();
		//m_send_audio_thread.End();
		if(m_send_video_Timer)
		{
			timeKillEvent(m_send_video_Timer);
			m_send_video_Timer = 0;
			m_video_time_count = 0;
		}
		if(m_send_audio_Timer)
		{
			timeKillEvent(m_send_audio_Timer);
			m_send_audio_Timer = 0;
			m_audio_time_count = 0;
		}
		//close decoder
		close_decoder();

		//close show
		close_sdl_output();

	    def_ff_avformat_close_input(&m_in_fmt_ctx);
		m_in_fmt_ctx = NULL;
	}

	//close out put
	if(m_out_fmt_ctx)
	{
		if (m_out_fmt_ctx && !(m_out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
			def_ff_avio_close(m_out_fmt_ctx->pb);

		def_ff_avformat_free_context(m_out_fmt_ctx);
		m_out_fmt_ctx = NULL;
	}
	


	clear_all_cache_list();
	return true;
}

void CSourFile::clear_busy_cache_list(bool bIsCache)
{
	if(bIsCache)
	{
		m_pool_send_avpacket_lock.Lock();
		list<AVPacket*>::iterator itersend;
		for(itersend = m_pool_send_avpacket_list.begin();itersend != m_pool_send_avpacket_list.end();itersend++)
		{
			AVPacket* pkt = (*itersend);
			put_free_pkt(pkt);
		}
		m_pool_send_avpacket_list.clear();
		m_pool_send_avpacket_lock.Unlock();

		m_pool_busy_send_video_avpacket_lock.Lock();
		
		for(itersend = m_pool_busy_send_video_avpacket_list.begin();itersend != m_pool_busy_send_video_avpacket_list.end();itersend++)
		{
			AVPacket* pkt = (*itersend);
			put_free_pkt(pkt);
		}
		m_pool_busy_send_video_avpacket_list.clear();

		m_pool_busy_send_video_avpacket_lock.Unlock();

		m_pool_busy_send_audio_avpacket_lock.Lock();
		for(itersend = m_pool_busy_sned_audio_avpacket_list.begin();itersend != m_pool_busy_sned_audio_avpacket_list.end();itersend++)
		{
			AVPacket* pkt = (*itersend);
			put_free_pkt(pkt);
		}
		m_pool_busy_sned_audio_avpacket_list.clear();
		m_pool_busy_send_audio_avpacket_lock.Unlock();
	}
	m_pool_busy_audio_frame_lock.Lock();

	list<MeidaFrame> ::iterator iter_media_fream;
	for(iter_media_fream = m_pool_busy_audio_frame_list.begin();iter_media_fream != m_pool_busy_audio_frame_list.end();iter_media_fream++)
	{
		MeidaFrame maf = (*iter_media_fream);
		if(maf.buf)
		{
			delete maf.buf;
			maf.buf = NULL;
		}
	}
	m_pool_busy_audio_frame_list.clear();
	m_pool_busy_audio_frame_lock.Unlock();

	m_pool_busy_video_frame_lock.Lock();
	for(iter_media_fream = m_pool_busy_video_frame_list.begin();iter_media_fream != m_pool_busy_video_frame_list.end();iter_media_fream++)
	{
		MeidaFrame maf = (*iter_media_fream);
		if(maf.buf)
		{
			delete maf.buf;
			maf.buf = NULL;
		}
	}
	m_pool_busy_video_frame_list.clear();
	m_pool_busy_video_frame_lock.Unlock();

	m_pool_busy_audio_avpacket_lock.Lock();
	list<AVPacket*> ::iterator apk_iter;
	for(apk_iter = m_pool_busy_audio_avpacket_list.begin();apk_iter != m_pool_busy_audio_avpacket_list.end();apk_iter++)
	{
		AVPacket* pkt = (*apk_iter);
		if(pkt)
		{
			def_ff_av_free_packet(pkt);
			DELMEM(pkt);
		}
	}
	m_pool_busy_audio_avpacket_list.clear();
	m_pool_busy_audio_avpacket_lock.Unlock();

	m_pool_busy_video_avpacket_lock.Lock();
	for(apk_iter = m_pool_busy_video_avpacket_list.begin();apk_iter != m_pool_busy_video_avpacket_list.end();apk_iter++)
	{
		AVPacket* pkt = (*apk_iter);
		if(pkt)
		{
			def_ff_av_free_packet(pkt);
			DELMEM(pkt);
		}
	}
	m_pool_busy_video_avpacket_list.clear();
	m_pool_busy_video_avpacket_lock.Unlock();
}

void CSourFile::clear_all_cache_list()
{
	clear_busy_cache_list();
	list<AVPacket*> ::iterator apk_iter;
	m_pool_free_avpacket_lock.Lock();
	for(apk_iter = m_pool_free_avpacket_list.begin();apk_iter != m_pool_free_avpacket_list.end();apk_iter++)
	{
		AVPacket* pkt = (*apk_iter);
		if(pkt)
		{
			def_ff_av_free_packet(pkt);
			DELMEM(pkt);
		}
	}
	m_pool_free_avpacket_list.clear();
	m_pool_free_avpacket_lock.Unlock();

}

void CSourFile::push_busy_pkt(AVPacket* pkt)
{
	AVPacket* avp = get_free_pkt();
	if(avp)
	{
		def_ff_av_copy_packet(avp,pkt);
		if(pkt->stream_index == m_in_video_index)
		{
			m_pool_busy_video_avpacket_lock.Lock();
			if(m_pool_busy_video_avpacket_list.size() <= 0)
				m_pool_busy_video_avpacket_cond.cond_signal();
			m_pool_busy_video_avpacket_list.push_back(avp);
			m_pool_busy_video_avpacket_lock.Unlock();
		}
		else if(pkt->stream_index == m_in_audio_index)
		{
			m_pool_busy_audio_avpacket_lock.Lock();
			if(m_pool_busy_audio_avpacket_list.size() <= 0)
				m_pool_busy_audio_avpacket_cond.cond_signal();
			m_pool_busy_audio_avpacket_list.push_back(avp);
			m_pool_busy_audio_avpacket_lock.Unlock();
			//put_free_pkt(avp);
		}
	}
}

AVPacket* CSourFile::pull_video_busy_pkt()
{
	AVPacket* avp = NULL;
	m_pool_busy_video_avpacket_lock.Lock();
	if(m_pool_busy_video_avpacket_list.size() > 0)
	{
		avp = m_pool_busy_video_avpacket_list.front();
		m_pool_busy_video_avpacket_list.pop_front();
		m_pool_busy_video_avpacket_lock.Unlock();
	}
	else
	{
		m_pool_busy_video_avpacket_lock.Unlock();
		m_pool_busy_video_avpacket_cond.cond_wait();
	}
	return avp;
}

AVPacket* CSourFile::pull_audio_busy_pkt()
{
	AVPacket* avp = NULL;
	m_pool_busy_audio_avpacket_lock.Lock();
	if(m_pool_busy_audio_avpacket_list.size() > 0)
	{
		avp = m_pool_busy_audio_avpacket_list.front();
		m_pool_busy_audio_avpacket_list.pop_front();
		m_pool_busy_audio_avpacket_lock.Unlock();
	}
	else
	{
		m_pool_busy_audio_avpacket_lock.Unlock();
		m_pool_busy_audio_avpacket_cond.cond_wait();
	}
	return avp;
}

AVPacket* CSourFile::get_free_pkt()
{
	m_pool_free_avpacket_lock.Lock();
	AVPacket* avPack = NULL;
	if(m_pool_free_avpacket_list.size() > 0)
	{
		avPack = m_pool_free_avpacket_list.front();
		m_pool_free_avpacket_list.pop_front();
	}
	else
	{
		avPack = new AVPacket();
		def_ff_av_init_packet(avPack);
	}
	m_pool_free_avpacket_lock.Unlock();
	return avPack;
}

void  CSourFile::put_free_pkt(AVPacket* pNode)
{
	if(pNode)
	{

		m_pool_free_avpacket_lock.Lock();
		if(m_pool_free_avpacket_list.size() > 10)
		{
			def_ff_av_free_packet(pNode);
			DELMEM(pNode);
		}
		else
		{
			def_ff_av_free_packet(pNode);
			def_ff_av_init_packet(pNode);
			m_pool_free_avpacket_list.push_back(pNode);
		}
		m_pool_free_avpacket_lock.Unlock();
	}
}

bool CSourFile::open_decoder()
{
	if(m_in_video_codec_ctx)
	{
		m_in_video_codec = def_ff_avcodec_find_decoder(m_in_video_codec_ctx->codec_id);
		if (!m_in_video_codec) 
		{
			fprintf(stderr, "could not find video decoder!\n");
			return false;
		}
		if (def_ff_avcodec_open2(m_in_video_codec_ctx, m_in_video_codec,NULL) < 0) 
		{
			fprintf(stderr, "could not open video codec!\n");
			return false;
		}
		m_pic_size =m_in_video_codec_ctx->coded_width * m_in_video_codec_ctx->coded_height * 3;
		if(m_pic_size == 0)
			return false;

		m_decode_video_trhead.Begin(_decode_video_loop,this);
	}

	if(m_in_audio_codec_ctx)
	{
		m_in_audio_codec = def_ff_avcodec_find_decoder(m_in_audio_codec_ctx->codec_id);
		if (!m_in_audio_codec)
		{
			fprintf(stderr, "could not find audio decoder!\n");
			return false;
		}

		if (def_ff_avcodec_open2(m_in_audio_codec_ctx, m_in_audio_codec,NULL) < 0) 
		{
			fprintf(stderr, "could not open audio codec!\n");
			return false;
		}
		m_aud_size = AVCODEC_MAX_AUDIO_FRAME_SIZE ;

		int	out_linesize = 0;
		def_ff_av_samples_get_buffer_size(&out_linesize,m_in_audio_codec_ctx->channels,
				m_in_audio_codec_ctx->frame_size,m_in_audio_codec_ctx->sample_fmt, 1); 

		m_decode_audio_trhead.Begin(_decode_audio_loop,this);
	}
	return true;
}

bool CSourFile::close_decoder()
{
	if(m_in_video_codec)
	{
		m_decode_video_trhead.End();
		def_ff_avcodec_close(m_in_video_codec);
		m_in_video_codec = NULL;
	}
	if(m_in_audio_codec)
	{
		m_decode_audio_trhead.End();
		def_ff_avcodec_close(m_in_audio_codec);
		m_in_audio_codec = NULL;
	
	}
	return true;
}

bool CSourFile::copy_video_frame(AVFrame * frame,MeidaFrame&  mvf)
{
	if(frame)
	{
		mvf.height = frame->height;
		mvf.width = frame->width;
		mvf.pts = frame->pts;
		if(mvf.nbufsize == 0 && mvf.buf == NULL)
		{
			mvf.nbufsize = m_pic_size / 2;
			mvf.buf = new unsigned char[mvf.nbufsize];
			if(mvf.buf == NULL)
			{
				return false;
			}
		}

		for(int i=0, nDataLen=0; i<3; i++)
		{
			int nShift = (i == 0) ? 0 : 1;
			unsigned char *pYUVData = frame->data[i];
			unsigned int  nW = frame->width >> nShift;
			unsigned int  nH = frame->height >> nShift;
			for(int j = 0; j< nH;j++)
			{
				memcpy(mvf.buf+nDataLen,frame->data[i] + j * frame->linesize[i], nW); 
				nDataLen += nW;
			}
		}
		return true;
	}
	return false;
}


void CSourFile::push_busy_video_frame(AVFrame * frame)
{
	if(frame)
	{
		MeidaFrame  mvf;
		mvf.height = frame->height;
		mvf.width = frame->width;
		copy_video_frame(frame,mvf);
		m_pool_busy_video_frame_lock.Lock();
		if(m_pool_busy_video_frame_list.size() <= 0)
		{
			m_pool_busy_video_frame_cond.cond_signal();
		}
		m_pool_busy_video_frame_list.push_back(mvf);
		m_pool_busy_video_frame_lock.Unlock();
	}
}

MeidaFrame CSourFile::pull_busy_video_frame()
{
	MeidaFrame  mvf ;
	mvf.nType = VIDEOFRAME;
	m_pool_busy_video_frame_lock.Lock();

	int nSize = m_pool_busy_video_frame_list.size();
	char szbuf[256] = {'/0'};
	sprintf(szbuf,"### ### ### m_pool_busy_video_frame_list.size  %d\n", nSize);
	//OutputDebugString(CString(szbuf));

	if(nSize > 20)
		m_bThreshold = false;
	else
		m_bThreshold = true;

	if(nSize > 0)
	{
		mvf = m_pool_busy_video_frame_list.front();
		m_pool_busy_video_frame_list.pop_front();
		m_pool_busy_video_frame_lock.Unlock();
	}
	else
	{
		m_pool_busy_video_frame_lock.Unlock();
		m_pool_busy_video_frame_cond.cond_wait();
	}
	return mvf;
}

void  CSourFile::decode_video_data()
{
	int got_picture;
	AVFrame *pframe = def_ff_avcodec_alloc_frame();
	while(!m_decode_video_trhead.GetStop())
	{
		AVPacket* pkt = NULL;
		pkt = pull_video_busy_pkt();
		if(pkt && pkt->stream_index == m_in_video_index && m_in_video_codec_ctx)
		{
			//av_interleaved_write_frame(m_out_fmt_ctx, pkt);
			if(pframe)
			{
				//char szbuf[256] = {'/0'};
			//	sprintf(szbuf,"pts %lld  --- dts %lld  \n",pkt->pts,pkt->dts);
				//OutputDebugString(CString(szbuf));
				int nR = def_ff_avcodec_decode_video2(m_in_video_codec_ctx, pframe, &got_picture, pkt);
				if (got_picture)
				{
				
					if (pkt->pts != AV_NOPTS_VALUE)
					{
						pframe->pts = pframe->pkt_pts;
					}
					else if(pkt->dts != AV_NOPTS_VALUE)
					{
						pframe->pts = pframe->pkt_dts;
					}
					else
					{
						pframe->pts = def_ff_av_frame_get_best_effort_timestamp(pframe);
					}

					//pframe->pts = pkt->dts;
					push_busy_video_frame(pframe);
				}
			}
			put_free_pkt(pkt);
		}
		
	}
	def_ff_avcodec_free_frame(&pframe);
	pframe = NULL;
}


bool CSourFile::copy_audio_frame(AVFrame * pframe,MeidaFrame&  maf)
{ 

	unsigned int n_buf_size = 0;
	if(pframe)
	{
		if(maf.buf == NULL)
		{
			maf.nbufsize = m_aud_size;
			maf.buf = new unsigned char[m_aud_size];
		}
		unsigned char* audio_buf = maf.buf;

		int data_size = def_ff_av_samples_get_buffer_size(NULL, def_ff_av_frame_get_channels(pframe),
                                            pframe->nb_samples,
                                            (AVSampleFormat)pframe->format, 1);
		int64_t dec_channel_layout =
									(pframe->channel_layout && def_ff_av_frame_get_channels(pframe) == def_ff_av_get_channel_layout_nb_channels(pframe->channel_layout)) ?
									pframe->channel_layout : def_ff_av_get_default_channel_layout(def_ff_av_frame_get_channels(pframe));

		int wanted_nb_samples = pframe->nb_samples;

		//int wanted_nb_samples = synchronize_audio(is, pframe->nb_samples);

		if (pframe->format        != m_audio_src.fmt            ||
			dec_channel_layout       != m_audio_src.channel_layout ||
			pframe->sample_rate   != m_audio_src.freq           ||
			!m_au_convert_ctx) 
		{
			def_ff_swr_free(&m_au_convert_ctx);
			m_au_convert_ctx = NULL;
			m_au_convert_ctx = def_ff_swr_alloc_set_opts(NULL,m_audio_hw_params.channel_layout, m_audio_hw_params.fmt, m_audio_hw_params.freq,
												dec_channel_layout, (AVSampleFormat)pframe->format, pframe->sample_rate,
												0, NULL);
			if (!m_au_convert_ctx || def_ff_swr_init(m_au_convert_ctx) < 0)
			{
				return false;
			}

			m_audio_src.channel_layout = dec_channel_layout;
			m_audio_src.channels       = def_ff_av_frame_get_channels(pframe);
			m_audio_src.freq = pframe->sample_rate;
			m_audio_src.fmt = (AVSampleFormat)pframe->format;
		}

		if(m_au_convert_ctx)
		{
			unsigned char* out_buffer = audio_buf;
			const uint8_t **in = (const uint8_t **)pframe->extended_data;
			uint8_t **out = &out_buffer;
			int out_count = (int64_t)wanted_nb_samples * m_audio_hw_params.freq / pframe->sample_rate + 256;
			int out_size  = def_ff_av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, out_count, m_audio_hw_params.fmt, 0);
			int len2;
			if (out_size < 0) 
			{
				return false;
			}
			if (wanted_nb_samples != pframe->nb_samples) 
			{
				if (def_ff_swr_set_compensation(m_au_convert_ctx, (wanted_nb_samples - pframe->nb_samples) * m_audio_hw_params.freq / pframe->sample_rate,
											wanted_nb_samples * m_audio_hw_params.freq / pframe->sample_rate) < 0) 
				{
					return false;
				}
			}

			len2 = def_ff_swr_convert(m_au_convert_ctx, out, out_count, in, pframe->nb_samples);
			if (len2 < 0)
			{
				return false;
			}
			if (len2 == out_count) 
			{
				def_ff_swr_init(m_au_convert_ctx);
			}
			unsigned int nbufsize = len2 * m_audio_hw_params.channels * def_ff_av_get_bytes_per_sample(m_audio_hw_params.fmt);
			n_buf_size += nbufsize;
		}
		else
		{
			memcpy(audio_buf,pframe->data[0],data_size);
			n_buf_size += data_size;
		}
		maf.nbufsize = n_buf_size;
		maf.height = pframe->height;
		maf.width = pframe->width;
		maf.pts = pframe->pts;
		return true;
	}
	return false;
}

void CSourFile::push_busy_audio_frame(AVFrame * frame)
{
	if(frame)
	{
		MeidaFrame  maf;
		copy_audio_frame(frame,maf);
		m_pool_busy_audio_frame_lock.Lock();
		m_pool_busy_audio_frame_list.push_back(maf);
		m_pool_busy_audio_frame_lock.Unlock();
	}
}

MeidaFrame CSourFile::pull_busy_audio_frame()
{
	MeidaFrame  maf;
	m_pool_busy_audio_frame_lock.Lock();
	if(m_pool_busy_audio_frame_list.size() > 0)
	{
		//do
			maf = m_pool_busy_audio_frame_list.front();
			m_pool_busy_audio_frame_list.pop_front();
		//}while(m_pool_busy_audio_frame_list.size() > 10);
	
	}
	m_pool_busy_audio_frame_lock.Unlock();
	return maf;
}

void CSourFile::decode_audio_data()
{
	int got_picture;
	AVFrame *pframe = def_ff_avcodec_alloc_frame();

	while(!m_decode_audio_trhead.GetStop())
	{
		AVPacket* pkt = NULL;
		pkt = pull_audio_busy_pkt();
		if(pkt && pkt->stream_index == m_in_audio_index && m_in_audio_codec_ctx)
		{
			int len = 0;
			while(pkt->size > 0)
			{
				if ((len = def_ff_avcodec_decode_audio4(m_in_audio_codec_ctx, pframe, &got_picture, pkt)) >= 0) 
				{
					if(got_picture)
					{
						/*AVRational tb = {1, pframe->sample_rate};
						if (pkt->pts != AV_NOPTS_VALUE)
							pframe->pts = av_rescale_q(pkt->pts, m_in_audio_codec_ctx->time_base, tb);
						else if(pkt->dts != AV_NOPTS_VALUE)
							pframe->pts = av_rescale_q(pkt->dts, m_in_video_st->time_base, tb);*/
						pframe->pts = pkt->dts;
						push_busy_audio_frame(pframe);
						//char szbuf[256] = {'/0'};
						//sprintf(szbuf,"audio pts %lld  --- dts %lld  \n",pkt->pts,pkt->dts);
						//OutputDebugString(CString(szbuf));
					}
					pkt->size -= len;
				}
				else
				{
					break;
				}		
			}
			put_free_pkt(pkt);
		}
	}
	def_ff_avcodec_free_frame(&pframe);
	pframe = NULL;
}

void CSourFile::get_show_window_width_and_height()
{
	if(m_show_rect.hwnd)
	{ 
		RECT  rc;
		GetClientRect(m_show_rect.hwnd,&rc);
		m_show_rect.bottom = rc.bottom;
		m_show_rect.left = rc.left;
		m_show_rect.right = rc.right;
		m_show_rect.top = rc.top;
		m_show_window_width = m_show_rect.right - m_show_rect.left;
		m_show_window_height = m_show_rect.bottom - m_show_rect.top;
	}
	else
	{
		m_show_window_width = m_video_width;
		m_show_window_height = m_video_height;
	}
}


bool CSourFile::open_sdl_output()
{
	if (m_in_video_codec_ctx) 
	{
		m_video_width = m_in_video_codec_ctx->width;
		m_video_height = m_in_video_codec_ctx->height;
		get_show_window_width_and_height();
		if(m_show_rect.nDrawToolType == DRAWIAGER_SDL)
		{
			if(!DrawImageBySDLInit())
				return false;
		}
		else
		{
			if(!DrawImageByDXInit())
				return false;
		}
		
    }
	if(m_in_audio_codec_ctx)
	{
		if(audio_open() < 0)
			return false;

		//SDL_Delay(10);
		SDL_PauseAudio(0); 
	}
	m_outputThread = SDL_CreateThread(output_thread, this);
	return true;
}

bool CSourFile::close_sdl_output()
{
	if(m_outputThread)
	{
		SDL_KillThread(m_outputThread);
		m_outputThread = NULL;
	}
	SDL_CloseAudio();
	
	if(m_show_rect.nDrawToolType == DRAWIAGER_SDL)
		DrawImageBySDLUnInit();
	else
		DrawImageByDXUnInit();

	if(m_audio_buf)
	{
		def_ff_av_free(m_audio_buf);
		m_audio_buf = NULL;
	}
	return true;
}

bool CSourFile::switch_paly(bool bflag)
{
	if(SDL_GetAudioStatus() == SDL_AUDIO_STOPPED)
	{
		if(m_in_audio_codec_ctx && bflag)
		{
			if(audio_open() < 0)
				return false;

			SDL_Delay(10);
			SDL_PauseAudio(0); 
		}
	}
	else
	{
			SDL_CloseAudio();
	}
	return true;
}

int CSourFile::audio_open()
{
	static SDL_AudioSpec  spec;
    const char *env;
    static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
    static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	int wanted_sample_rate = m_in_audio_codec_ctx->sample_rate;
	int wanted_nb_channels = m_in_audio_codec_ctx->channels;
	int64_t wanted_channel_layout = m_in_audio_codec_ctx->channel_layout ;

    env = SDL_getenv("SDL_AUDIO_CHANNELS");
    if (env) 
	{
        wanted_nb_channels = atoi(env);
        wanted_nb_channels = def_ff_av_get_default_channel_layout(wanted_nb_channels);
    }
    if (!wanted_channel_layout || wanted_nb_channels != def_ff_av_get_channel_layout_nb_channels(wanted_channel_layout)) 
	{
        wanted_channel_layout = def_ff_av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    wanted_nb_channels = def_ff_av_get_channel_layout_nb_channels(wanted_channel_layout);
    m_wanted_spec.channels = wanted_nb_channels;
    m_wanted_spec.freq = wanted_sample_rate;
    if (m_wanted_spec.freq <= 0 || m_wanted_spec.channels <= 0) 
	{
        return -1;
    }
    while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= m_wanted_spec.freq)
        next_sample_rate_idx--;

    m_wanted_spec.format = AUDIO_S16SYS;
    m_wanted_spec.silence = 0;
    m_wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << def_ff_av_log2(m_wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    m_wanted_spec.callback = fill_audio;
    m_wanted_spec.userdata = this;

	while (SDL_OpenAudio(&m_wanted_spec, &spec) < 0)
	{
		m_wanted_spec.channels = next_nb_channels[FFMIN(7, m_wanted_spec.channels)];
		if (!m_wanted_spec.channels) 
		{
			m_wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			m_wanted_spec.channels = wanted_nb_channels;
			if (!m_wanted_spec.freq) 
			{
				return -1;
			}
		}
			wanted_channel_layout = def_ff_av_get_default_channel_layout(m_wanted_spec.channels);
	}

    if (spec.format != AUDIO_S16SYS) 
	{
        return -1;
    }
    if (spec.channels != m_wanted_spec.channels) 
	{
        wanted_channel_layout = def_ff_av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) 
		{
            return -1;
        }
    }

    m_audio_hw_params.fmt = AV_SAMPLE_FMT_S16;
    m_audio_hw_params.freq = spec.freq;
    m_audio_hw_params.channel_layout = wanted_channel_layout;
    m_audio_hw_params.channels =  spec.channels;
    m_audio_hw_params.frame_size = def_ff_av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, 1, m_audio_hw_params.fmt, 1);
    m_audio_hw_params.bytes_per_sec = def_ff_av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, m_audio_hw_params.freq, m_audio_hw_params.fmt, 1);

    if (m_audio_hw_params.bytes_per_sec <= 0 || m_audio_hw_params.frame_size <= 0) 
	{
        return -1;
    }

	m_audio_src = m_audio_hw_params;
    return spec.size;
}

void CSourFile::OutPouLoop()
{
	int64_t last_pts = 0;
	int n_sleep_pts = 40;
	while(1)
	{
		int64_t cur_pts = 0;
		MeidaFrame mvf;
		mvf = pull_busy_video_frame();
		cur_pts = mvf.pts;
		unsigned char* video_buf = NULL;
		video_buf = mvf.buf;

		if(video_buf != NULL)
		{
			if(!m_push_and_play_pause)
			{
				if(m_show_rect.nDrawToolType == DRAWIAGER_SDL)
					DrawImageBySDL(video_buf);
				else 
					DrawImageByDX(video_buf,mvf.height,mvf.width);
					
				m_video_cur_pts = cur_pts;

				if(last_pts > cur_pts)
					last_pts = cur_pts;

				n_sleep_pts = (cur_pts - last_pts) > 200 ? 40 : (cur_pts - last_pts);
				last_pts = cur_pts;
				if(m_sync_time_base)
				{
					if(m_audio_cur_pts -  m_video_cur_pts < 200)
					{
						SDL_Delay(n_sleep_pts);
					}
				}
				else
				{
					if(m_audio_cur_pts -  m_video_cur_pts < 500)
					{
						SDL_Delay(n_sleep_pts);
					}
				}
				char szbuf[256] = {'/0'};
				sprintf(szbuf,"*** *** *** video last_pts %lld cur_pts %lld  %d \n", last_pts,cur_pts,n_sleep_pts);
				//OutputDebugString(CString(szbuf));
			}
			else
			{
				char szbuf[256] = {'/0'};
				sprintf(szbuf,"###################################################### \n");
				//OutputDebugString(CString(szbuf));
			}
			delete mvf.buf;
		}
	}
}


void  CSourFile::fill_audio(void *udata,Uint8 *stream,int len)
{   
	CSourFile *sdlPro = (CSourFile *)udata;
	if(sdlPro)
	{
		sdlPro->audio_play(udata,stream,len);
	}
} 

void  CSourFile::audio_play(void *udata,Uint8 *stream,int len)
{
	int audio_len = 0;
	int len1 = 0;
	while(len > 0)
	{
		if(m_audio_pos >= m_audio_buf_len)
		{
			audio_len = get_audio_frame();
			if(audio_len < 0)
			{
				m_audio_buf_len = 1024;
				def_ff_av_fast_malloc(&m_audio_buf, &m_audio_buf_size, m_audio_buf_len);
				memset(m_audio_buf,0,m_audio_buf_len);
			}
			else
			{
				m_audio_buf_len = audio_len;
			}
		}

		len1 = m_audio_buf_len - m_audio_pos;
		if(len1 > len)
		{
			len1 = len;
		} 
		SDL_MixAudio(stream,(uint8_t *)m_audio_buf + m_audio_pos,len1,SDL_MIX_MAXVOLUME);
		//memcpy(stream,(uint8_t *)m_audio_buf + m_audio_pos,len1);
		m_audio_pos += len1;
		len -= len1;
		stream += len1;
	}
}

unsigned int CSourFile::get_audio_frame()
{
	 
	unsigned char* audio_buf = NULL;
	int audio_len = -1;
	unsigned int n_buf_size = 0;
	MeidaFrame maf;
	maf = pull_busy_audio_frame();
	n_buf_size = maf.nbufsize;
	audio_buf = maf.buf;

	if(n_buf_size > 0 && audio_buf)
	{
		audio_len = n_buf_size;  
		def_ff_av_fast_malloc(&m_audio_buf, &m_audio_buf_size, audio_len);
		if(m_audio_buf)
		{
			memcpy(m_audio_buf,audio_buf,audio_len);
		}
		else
		{
			audio_len = -1;
		}
		m_audio_cur_pts = maf.pts;
		//char szbuf[256] = {'/0'};
		//sprintf(szbuf,"### ### ### audiocur_pts %lld\n", maf.pts);
	    //OutputDebugString(CString(szbuf));
		delete maf.buf;
	}
	else
	{
		int n = 0;
	}
	m_audio_pos = 0;
	return  audio_len;
}

bool CSourFile::seek_in_file(unsigned int nSeekTime)
{
	m_play_lock.Lock();
	clear_busy_cache_list();
	if(m_in_fmt_ctx)
	{
		m_out_pkt_pts_last_time += m_prev_pts;
		m_out_pkt_dts_last_time += m_prev_dts;

		int64_t seet_time = ((int64_t)nSeekTime) * AV_TIME_BASE;
		if(def_ff_avformat_seek_file(m_in_fmt_ctx,-1,0,seet_time,m_in_fmt_ctx->duration,0)== 0)
		{
			m_play_lock.Unlock();
			return true;
		}
	}
	m_play_lock.Unlock();
	return false;
}

unsigned int CSourFile::get_in_file_duration()
{
	if(m_in_fmt_ctx)
	{
		return m_in_fmt_ctx->duration / AV_TIME_BASE;
	}
	return 0;
}

unsigned int CSourFile::get_in_file_current_play_time()
{
	if(m_in_audio_st)
		return m_audio_cur_pts/1000;
	else if(m_in_video_st)
		return m_video_cur_pts/1000;
	else
		return 0;
}

bool CSourFile::pause_in_file(bool bIsPause)
{
	m_play_lock.Lock();
	if(m_in_fmt_ctx)
	{
		m_push_and_play_pause = bIsPause;
		SDL_PauseAudio(bIsPause);
		if(bIsPause)
		{
			def_ff_av_read_pause(m_in_fmt_ctx);
			clear_busy_cache_list(false);
		}
		else
		{
			def_ff_av_read_play(m_in_fmt_ctx);
		}
	}
	m_play_lock.Unlock();
	return 0;
}

bool CSourFile::DrawImageBySDLInit()
{
	int      nInitflags;
	nInitflags = SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE;
	if (m_in_video_codec_ctx) 
	{
        nInitflags = SDL_INIT_VIDEO | nInitflags;
		if(m_show_rect.hwnd)
		{ 
			 char variable[256];
			 sprintf(variable, "SDL_WINDOWID=0x%lx", m_show_rect.hwnd);
			 int nResult = SDL_putenv(variable); 
			 int nRR = nResult; 
		}

		get_show_window_width_and_height();
		
    }

    if (m_in_audio_codec_ctx)
        nInitflags = SDL_INIT_AUDIO | nInitflags;
	
	if (SDL_Init(nInitflags))
	{
		return false;
    }

	int flag = SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF |SDL_RESIZABLE |SDL_NOFRAME;
	if(m_in_video_codec_ctx)
	{
		if(m_pscreen == NULL)
			m_pscreen = SDL_SetVideoMode(m_show_window_width, m_show_window_height,0, flag);  

		if(m_pscreen == NULL)  
		{  
			return false; 
		}  

		m_poverlay = SDL_CreateYUVOverlay(m_video_width,m_video_height,SDL_YV12_OVERLAY,m_pscreen);  
		if(m_poverlay == NULL)  
		{  
			return false; 
		}  
	}
	return true;
}

bool CSourFile::DrawImageBySDLUnInit()
{
	if(m_poverlay)
	{
		SDL_FreeYUVOverlay(m_poverlay);
		m_poverlay = NULL;
	}
	return true;
}

bool CSourFile::DrawImageBySDL(unsigned char* video_buf)
{
	if(m_poverlay)
	{
		SDL_FreeYUVOverlay(m_poverlay);
		m_poverlay = NULL;
	}
	m_poverlay = SDL_CreateYUVOverlay(m_video_width,m_video_height,SDL_YV12_OVERLAY,m_pscreen);

	if(m_poverlay && m_pscreen)
	{
		SDL_Rect rect;
		SDL_LockSurface(m_pscreen);
		SDL_LockYUVOverlay(m_poverlay);
		memcpy(m_poverlay->pixels[0], video_buf, m_video_width*m_video_height);
		memcpy(m_poverlay->pixels[1], video_buf + m_video_width*m_video_height/4 + m_video_width*m_video_height, m_video_width*m_video_height/4);
		memcpy(m_poverlay->pixels[2], video_buf + m_video_width*m_video_height, m_video_width*m_video_height/4);
					
		SDL_UnlockYUVOverlay(m_poverlay);
		SDL_UnlockSurface(m_pscreen);

		get_show_window_width_and_height();
		rect.w = m_show_window_width;
		rect.h = m_show_window_height;
		rect.x = rect.y = 0;
		SDL_DisplayYUVOverlay(m_poverlay, &rect);
	}
	return true;
}


bool CSourFile::DrawImageByDXInit()
{
	if (DirectDrawCreateEx(NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) 
	{
		return false;
	}

	// 创建DIRECTDRAW 显示YUV420数据 设置协作层
	if (m_lpDD->SetCooperativeLevel(m_show_rect.hwnd,DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES) != DD_OK)
	{
		return false;
	}

	// 创建主表面
	ZeroMemory(&m_ddsd, sizeof(m_ddsd));
	m_ddsd.dwSize = sizeof(m_ddsd);
	m_ddsd.dwFlags = DDSD_CAPS;
	m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (m_lpDD->CreateSurface(&m_ddsd, &m_lpDDSPrimary, NULL) != DD_OK)
	{
		return false;
	}

	LPDIRECTDRAWCLIPPER  pcClipper;   // Cliper
	if( m_lpDD->CreateClipper( 0, &pcClipper, NULL ) != DD_OK )
	{
		return false;
	}

	if( pcClipper->SetHWnd( 0, m_show_rect.hwnd) != DD_OK )
	{
		pcClipper->Release();
		return false;
	}

	if( m_lpDDSPrimary->SetClipper( pcClipper ) != DD_OK )
	{
		pcClipper->Release();
		return false;
	}
	pcClipper->Release();
	return true;
}

bool CSourFile::DrawImageByDXUnInit()
{
	if(m_lpDDSOffScr)
	{
		m_lpDDSOffScr->Release();
		m_lpDDSOffScr = NULL;
	}
	if(m_lpDDSPrimary)
	{
		m_lpDDSPrimary->Release();
		m_lpDDSPrimary = NULL;
	}
	if(m_lpDD)
	{
		m_lpDD->Release();
		m_lpDD = NULL;
	}
	return true;
}

bool CSourFile::DrawImageByDX(unsigned char* pBuf,int nVH,int nVW)
{
	// 创建YUV表面 
	if(m_lpDD != NULL)
	{ 
		if(m_lpDDSOffScr == NULL ||m_video_height !=  nVH ||m_video_width != nVW)
		{
			if(m_lpDDSOffScr)
			{
				m_lpDDSOffScr->Release();
				m_lpDDSOffScr= NULL;
			}

			m_video_width = nVW;
			m_video_height = nVH;
			ZeroMemory(&m_ddsd, sizeof(m_ddsd));
			m_ddsd.dwSize = sizeof(m_ddsd);
			m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			m_ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
			m_ddsd.dwWidth = m_video_width;
			m_ddsd.dwHeight = m_video_height;
			m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			m_ddsd.ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_YUV ;
			m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V','1','2');
			m_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
			if (m_lpDD->CreateSurface(&m_ddsd, &m_lpDDSOffScr, NULL) != DD_OK)
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	//将解码得到的YUV数据拷贝到YUV表面
	//设解码得到的YUV数据的指针分别是Y,U,V, 每行数据长度为BPS，具体拷贝代码如下，这里需要特别注意每拷一行都要对写指针加ddsd.lPitch（对于Y）或ddsd.lPitch/2（对于UV）：

	int ddRval;
	do 
	{
		ddRval = m_lpDDSOffScr->Lock(NULL,&m_ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
	} while(ddRval == DDERR_WASSTILLDRAWING);
	if(ddRval != DD_OK)
	{
		return false;
	}

	int BpS  = nVW;

	LPBYTE lpSurf = (LPBYTE)m_ddsd.lpSurface;
	int nCh0 = m_video_width*m_video_height;
	int nCh1 = m_video_width*m_video_height + (m_video_width * m_video_height)/4;

	LPBYTE PtrY = pBuf; 
	LPBYTE PtrU = pBuf+nCh0;
	LPBYTE PtrV = pBuf+nCh1;

	int iHeight = m_video_height;

	RECT rect;
	rect.left = 0;
	rect.right = m_video_width;
	rect.top = 0 ; 
	rect.bottom = m_video_height;

	// 填充离屏表面
	if(lpSurf)
	{
		PtrY += m_video_width * rect.top + rect.left;
		for (int i=0;i<iHeight;i++)
		{
			memcpy(lpSurf, PtrY, m_ddsd.dwWidth);
			PtrY += BpS;
			lpSurf += m_ddsd.lPitch;
		}

		PtrV += (m_video_width/2) * (rect.top/2) + rect.left/2;
		for (int i=0;i<iHeight/2;i++)
		{
			memcpy(lpSurf, PtrV, m_ddsd.dwWidth/2);
			PtrV += BpS / 2; 
			lpSurf += m_ddsd.lPitch /2;
		}

		PtrU += (m_video_width/2) * (rect.top/2) + rect.left/2;

		for (int i=0;i<iHeight/2;i++)
		{
			memcpy(lpSurf, PtrU, m_ddsd.dwWidth/2);
			PtrU += BpS / 2;
			lpSurf += m_ddsd.lPitch /2;
		}

	}
	m_lpDDSOffScr->Unlock(NULL);

	//YUV表面的显示
	//现在就可以直接将YUV表面Blt到主表面进行显示了
	HWND hwnd = m_show_rect.hwnd;
	RECT  rc;
	get_show_window_width_and_height();
	rc.left = m_show_rect.left;
	rc.right = m_show_rect.right;
	rc.top = m_show_rect.top;
	rc.bottom = m_show_rect.bottom;

	ClientToScreen(hwnd, (LPPOINT)&rc.left); 
	ClientToScreen(hwnd, (LPPOINT)&rc.right);
	ddRval = m_lpDDSPrimary->Blt(&rc, m_lpDDSOffScr,NULL , DDBLT_WAIT, NULL);
	return true;
}


void CSourFile::send_video_porc()
{
	int64_t v_prev_pts = 0;
	float flt_time_cost = 0;
	float flt_sleep_time_cost = 0;
	CCalculationInterval cl;

	while(!m_send_video_thread.GetStop())
	{
		AVPacket* vpkt = NULL;
		m_pool_busy_send_video_avpacket_lock.Lock();
		if(m_pool_busy_send_video_avpacket_list.size() > 0)
		{
			vpkt = m_pool_busy_send_video_avpacket_list.front();
			m_pool_busy_send_video_avpacket_list.pop_front();
		}
		m_pool_busy_send_video_avpacket_lock.Unlock();
		if(vpkt)
		{
			cl.Begin();
			int64_t pkt_gap_sleep_time = 0;
			pkt_gap_sleep_time = vpkt->dts - v_prev_pts < 100 ? vpkt->dts - v_prev_pts : 0; // two pkt time gap
			v_prev_pts = vpkt->dts;
			
			//write frame to file or net
			m_play_lock.Lock();
			int nR = def_ff_av_interleaved_write_frame(m_out_fmt_ctx, vpkt);
			m_play_lock.Unlock();
			//free pkt
			def_ff_av_free_packet(vpkt);

			float n_temp = cl.End()* 1000; //count this proc cost time 
			flt_time_cost += n_temp; 

			improvement_sleep_time(pkt_gap_sleep_time,flt_time_cost); //improvement time
			if(pkt_gap_sleep_time > 0)  
			{
				def_ff_av_usleep(pkt_gap_sleep_time* 1000);
				//count sleep really time and theory time diff
				flt_sleep_time_cost = (pkt_gap_sleep_time*1000);
				flt_time_cost -= flt_sleep_time_cost;
			}
		}
		else
		{
			def_ff_av_usleep(10*1000);
		}
	}
}

void CSourFile::send_audio_porc()
{
	int64_t a_prev_pts = 0;
	float flt_time_cost = 0;
	float flt_sleep_time_cost = 0;
	CCalculationInterval cl;

	ScSystemTime st_beg_time;
	ScSystemTime st_cur_time;
	int64_t st_dts_gap_time = 0;
	TIME_GetLocalTime(&st_beg_time);
	st_cur_time = st_beg_time;

	while(!m_send_audio_thread.GetStop())
	{
		cl.Begin();
		AVPacket* apkt = NULL;
		m_pool_busy_send_audio_avpacket_lock.Lock();
		if(m_pool_busy_sned_audio_avpacket_list.size() > 0)
		{
			apkt = m_pool_busy_sned_audio_avpacket_list.front();
			m_pool_busy_sned_audio_avpacket_list.pop_front();
		}
		m_pool_busy_send_audio_avpacket_lock.Unlock();

		if(apkt)
		{
			int64_t pkt_gap_sleep_time = 0;
			pkt_gap_sleep_time = apkt->dts - a_prev_pts < 100 ? apkt->dts - a_prev_pts : 0; // two pkt time gap
			a_prev_pts = apkt->dts;
			//write frame to file or net
			m_play_lock.Lock();
			def_ff_av_interleaved_write_frame(m_out_fmt_ctx, apkt);
			m_play_lock.Unlock();
			//free pkt
			def_ff_av_free_packet(apkt);

			if(pkt_gap_sleep_time > 0)  
			{
				TIME_GetLocalTime(&st_cur_time);
				int n_gap_time = TIME_GetSystemTimeDiff(st_cur_time,st_beg_time);
				if(n_gap_time > 200)
				{
					char szbuf[256] = {'/0'};
					sprintf(szbuf,"### ### ### pkt_gap_sleep_time %lld  flt_time_cost  %f\n", pkt_gap_sleep_time,st_dts_gap_time - n_gap_time);
					OutputDebugString(CString(szbuf));
					if(st_dts_gap_time - n_gap_time > 0)
						pkt_gap_sleep_time = pkt_gap_sleep_time +  (st_dts_gap_time - n_gap_time)*0.8;
					else
						pkt_gap_sleep_time = pkt_gap_sleep_time +  (st_dts_gap_time - n_gap_time);
					st_dts_gap_time = 0;
					st_beg_time = st_cur_time;
				}
				st_dts_gap_time += pkt_gap_sleep_time;
				def_ff_av_usleep(pkt_gap_sleep_time* 1000);
			}
		}
		else
			def_ff_av_usleep(10*1000);
	}
}