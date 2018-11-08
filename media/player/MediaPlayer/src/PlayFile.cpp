#include "PlayFile.h"
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

#define DELMEM(p) { if(p){ delete p; p = NULL;}}

#define AUDIO_DIFF_AVG_NB   20

#define AV_NOSYNC_THRESHOLD 10.0

#define SAMPLE_CORRECTION_PERCENT_MAX 10


int			CPlayFile::m_openRefCount = 0;
CMutexLock  CPlayFile::m_refLock;
static av_always_inline av_const int isnan(float x)
{
    uint32_t v = av_float2int(x);
    if ((v & 0x7f800000) != 0x7f800000)
        return 0;
    return v & 0x007fffff;
}

static double get_clock(Clock *c)
{
    if (*c->queue_serial != c->serial)
        return NAN;
    if (c->paused) {
        return c->pts;
    } else {
        double time = av_gettime_relative() / 1000000.0;
        return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
    }
}

static void set_clock_at(Clock *c, double pts, int serial, double time)
{
    c->pts = pts;
    c->last_updated = time;
    c->pts_drift = c->pts - time;
    c->serial = serial;
}


static void set_clock(Clock *c, double pts, int serial)
{
    double time = av_gettime_relative() / 1000000.0;
    set_clock_at(c, pts, serial, time);
}

static void set_clock_speed(Clock *c, double speed)
{
    set_clock(c, get_clock(c), c->serial);
    c->speed = speed;
}

static void init_clock(Clock *c, int *queue_serial)
{
    c->speed = 1.0;
    c->paused = 0;
    c->queue_serial = queue_serial;
    set_clock(c, NAN, -1);
}

void CALLBACK _demux_read_media_loop(void *pUserData)
{
	if(pUserData)
	{
		CPlayFile * pUser = (CPlayFile *)pUserData;
		pUser->read_in_file_data_to_proc();
	}
}


CPlayFile::CPlayFile()
{
	m_str_in_file_url[0] = '\0';
	m_in_fmt_ctx = NULL; //输入格式上下文

	m_in_video_st = NULL;
	m_in_audio_st = NULL;

	m_in_video_codec_ctx = NULL;
	m_in_audio_codec_ctx = NULL ;

	m_in_video_codec = NULL;
	m_in_audio_codec = NULL;

	m_in_video_index = -1;
    m_in_audio_index = -1;

	m_pic_size = 0;
	m_aud_size = 0;

	m_au_convert_ctx = NULL;

	memset(&m_audio_hw_params,0,sizeof(AudioParams));
	memset(&m_audio_src,0,sizeof(AudioParams));
	m_push_and_play_pause = false;
	m_pic_bufffer = NULL;
	m_aud_bufffer = NULL;

	m_audio_buf = NULL;
	m_audio_buf_len = 0;
	m_audio_buf_size = 0;
	m_audio_pos = 0;
	m_video_cur_pts = 0;
	m_audio_cur_pts = 0;
	m_video_clock = 0;
	m_audio_clock = 0;
	m_video_duration = 0.0;
	m_audio_stream_timebase = 0.0;
	m_video_stream_timebase = 0.0;
	m_bIsHaveVideo = false;
	m_bIsHaveAudio = false;
	m_pool_free_avpacket_list.clear();

	m_audio_diff_avg_count = 0;
	m_audio_diff_cum = 0;
	m_audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
	m_audio_diff_threshold = 0;
	m_audio_hw_buf_size = 0;

	int nvserial = 0;
	int naserial = 0;
    init_clock(&m_vidclk, &nvserial);
    init_clock(&m_audclk, &naserial);

    m_audio_clock_serial = -1;
    m_audio_last_serial = -1;

	m_bIsPaly = false;

	m_refLock.Lock();
	m_openRefCount++;
	m_refLock.Unlock();
}

CPlayFile::~CPlayFile()
{
	m_refLock.Lock();
	m_openRefCount--;
	m_refLock.Unlock();
	CloseLocalMediaFile();
}

bool  CPlayFile::OpenLocalMediaFile(const char* szLocalFile)
{
	if(szLocalFile == NULL)
		return false;

	strcpy(m_str_in_file_url,szLocalFile);

	if(!open_in_file_and_get_stream_info())
		return false;
	
	if(m_in_audio_st)
	{
		m_bIsHaveAudio = true;
		m_audio_stream_timebase = (double)m_in_audio_st->time_base.den /m_in_audio_st->time_base.num;
	}
	if(m_in_video_st)
	{
		m_bIsHaveVideo  = true;
		m_video_duration = get_video_one_frame_duration();
		m_video_stream_timebase = (double)m_in_video_st->time_base.den /m_in_video_st->time_base.num;
	}
	m_read_file_and_proc_thread.Begin(_demux_read_media_loop,this);

	m_bIsPaly = true;
	return open_decoder();
}

bool  CPlayFile::CloseLocalMediaFile()
{
	if(!m_read_file_and_proc_thread.GetStop())
	{
		m_read_file_and_proc_thread.End(2);
	}

	m_refLock.Lock();
	if(CPlayFile::m_openRefCount <= 0 || !m_push_and_play_pause)
	{
		if(SDL_GetAudioStatus() != SDL_AUDIO_STOPPED)
		{
			SDL_CloseAudio();
		}
		m_push_and_play_pause = true;
	}
	m_refLock.Unlock();

	if(m_in_fmt_ctx)
	{
		close_decoder();
		avformat_close_input(&m_in_fmt_ctx);
		m_in_fmt_ctx = NULL;
	}
	clear_all_cache_list();

	if(m_pic_bufffer)
	{
		delete m_pic_bufffer;
		m_pic_bufffer = NULL;
	}
	if(m_aud_bufffer)
	{
		delete m_aud_bufffer;
		m_aud_bufffer = NULL;
	}
	
	if(m_audio_buf)
	{
		av_free(m_audio_buf);
		m_audio_buf = NULL;
	}
	//close out put

	if(m_au_convert_ctx)
	{
		swr_free(&m_au_convert_ctx);
		m_au_convert_ctx = NULL;
	}
	return false;
}

bool CPlayFile::open_in_file_and_get_stream_info()
{
	//打开输入文件
	if (avformat_open_input(&m_in_fmt_ctx, m_str_in_file_url, 0, 0)< 0)
	{
		return false;
	}

	//查找文件的流信息
	if (avformat_find_stream_info(m_in_fmt_ctx, 0) < 0)
	{
		return false;
	}

	av_dump_format(m_in_fmt_ctx, 0, m_str_in_file_url, 0);

	//解复用流
	for (int i = 0; i < m_in_fmt_ctx->nb_streams; i++) 
	{
		if ( m_in_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
		{
			m_in_video_index = i;
			m_in_video_st = m_in_fmt_ctx->streams[m_in_video_index];
		}
		if ( m_in_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_in_audio_index = i;
			m_in_audio_st = m_in_fmt_ctx->streams[m_in_audio_index];
		}
	}

	if(m_in_audio_st == NULL && m_in_audio_st == NULL)
	{
		return false;
	}

	if(m_in_video_st)
	{
		m_in_video_codec_ctx = m_in_video_st->codec;
	}
	//获取音频的编码相关信息
	if(m_in_audio_st)
	{
		m_in_audio_codec_ctx = m_in_audio_st->codec;
	}
	return true;
}


AVPacket* CPlayFile::get_free_pkt()
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
		av_init_packet(avPack);
	}
	m_pool_free_avpacket_lock.Unlock();
	return avPack;
}

void  CPlayFile::put_free_pkt(AVPacket* pNode)
{
	if(pNode)
	{

		m_pool_free_avpacket_lock.Lock();
		if(m_pool_free_avpacket_list.size() > 10)
		{
			av_free_packet(pNode);
			DELMEM(pNode);
		}
		else
		{
			av_free_packet(pNode);
			av_init_packet(pNode);
			m_pool_free_avpacket_list.push_back(pNode);
		}
		m_pool_free_avpacket_lock.Unlock();
	}
}

void CPlayFile::clear_busy_cache_list()
{
	m_pool_busy_audio_avpacket_lock.Lock();
	list<AVPacket*> ::iterator apk_iter;
	for(apk_iter = m_pool_busy_audio_avpacket_list.begin();apk_iter != m_pool_busy_audio_avpacket_list.end();apk_iter++)
	{
		AVPacket* pkt = (*apk_iter);
		if(pkt)
		{
			av_free_packet(pkt);
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
			av_free_packet(pkt);
			DELMEM(pkt);
		}
	}
	m_pool_busy_video_avpacket_list.clear();
	m_pool_busy_video_avpacket_lock.Unlock();
}

void CPlayFile::clear_all_cache_list()
{
	clear_busy_cache_list();
	list<AVPacket*> ::iterator apk_iter;
	m_pool_free_avpacket_lock.Lock();
	for(apk_iter = m_pool_free_avpacket_list.begin();apk_iter != m_pool_free_avpacket_list.end();apk_iter++)
	{
		AVPacket* pkt = (*apk_iter);
		if(pkt)
		{
			av_free_packet(pkt);
			DELMEM(pkt);
		}
	}
	m_pool_free_avpacket_list.clear();
	m_pool_free_avpacket_lock.Unlock();
}

bool CPlayFile::bIsGetMediaPacket()
{
	int nVideoPacketSize = 0;
	int nAudioPacketSize = 0;

	if(!m_push_and_play_pause)
	{
		if(m_in_video_st)
		{
			m_pool_busy_video_avpacket_lock.Lock();
			nVideoPacketSize = m_pool_busy_video_avpacket_list.size();
			m_pool_busy_video_avpacket_lock.Unlock();
		}
		else
		{
			nVideoPacketSize = 20;
		}

		if(m_in_audio_st)
		{
			m_pool_busy_audio_avpacket_lock.Lock();
			nAudioPacketSize = m_pool_busy_audio_avpacket_list.size();
			m_pool_busy_audio_avpacket_lock.Unlock();
		}
		else
		{
			nAudioPacketSize = 20;
		}

		if(nVideoPacketSize > 50 || nAudioPacketSize > 50)
		{
			return false;
		}

		if(nVideoPacketSize < 20 || nAudioPacketSize < 20)
			return true;
	}
	return false;
}

void CPlayFile::read_in_file_data_to_proc()
{
	while(!m_read_file_and_proc_thread.GetStop())
	{
		if(bIsGetMediaPacket())
		{
			m_play_lock.Lock();
			AVPacket *pkt = get_free_pkt();
			if(av_read_frame(m_in_fmt_ctx, pkt) >= 0)
			{
				AVStream *in_stream, *out_stream;
				in_stream  = m_in_fmt_ctx->streams[pkt->stream_index];
				out_stream = m_in_fmt_ctx->streams[pkt->stream_index];
			
				//转换PTS/DTS（Convert PTS/DTS）
				pkt->pts = av_rescale_q_rnd(pkt->pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				pkt->dts = av_rescale_q_rnd(pkt->dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base, out_stream->time_base);

				if(pkt->stream_index == m_in_video_index)
				{
					m_pool_busy_video_avpacket_lock.Lock();
					m_pool_busy_video_avpacket_list.push_back(pkt);
					m_pool_busy_video_avpacket_lock.Unlock();
				}
				else if(pkt->stream_index == m_in_audio_index)
				{
					m_pool_busy_audio_avpacket_lock.Lock();
					m_pool_busy_audio_avpacket_list.push_back(pkt);
					m_pool_busy_audio_avpacket_lock.Unlock();
				}
				m_play_lock.Unlock();
				continue;
			}
			else
			{
				if(m_in_audio_st)
				{
					m_audio_cur_pts = m_in_fmt_ctx->duration * m_in_audio_st->time_base.den / (AV_TIME_BASE * m_in_audio_st->time_base.num);
				}
				else if(m_in_video_st)
				{
					m_video_cur_pts = m_in_fmt_ctx->duration * m_in_video_st->time_base.den / (AV_TIME_BASE * m_in_video_st->time_base.num);
				}

				m_play_lock.Unlock();
				put_free_pkt(pkt);
			}
		}
		av_usleep(50 * 1000);
	}
}


bool CPlayFile::open_decoder()
{
	if(m_in_video_codec_ctx)
	{
		m_in_video_codec = avcodec_find_decoder(m_in_video_codec_ctx->codec_id);
		if (!m_in_video_codec) 
		{
			fprintf(stderr, "could not find video decoder!\n");
			return false;
		}
		if (avcodec_open2(m_in_video_codec_ctx, m_in_video_codec,NULL) < 0) 
		{
			fprintf(stderr, "could not open video codec!\n");
			return false;
		}

		m_pic_size =m_in_video_codec_ctx->coded_width * m_in_video_codec_ctx->coded_height * 3 / 2;
		if(m_pic_size == 0)
			return false;
	}

	if(m_in_audio_codec_ctx)
	{
		m_in_audio_codec = avcodec_find_decoder(m_in_audio_codec_ctx->codec_id);
		if (!m_in_audio_codec)
		{
			fprintf(stderr, "could not find audio decoder!\n");
			return false;
		}

		if (avcodec_open2(m_in_audio_codec_ctx, m_in_audio_codec,NULL) < 0) 
		{
			fprintf(stderr, "could not open audio codec!\n");
			return false;
		}

		m_aud_size = AVCODEC_MAX_AUDIO_FRAME_SIZE ;

		int	out_linesize = 0;
		av_samples_get_buffer_size(&out_linesize,m_in_audio_codec_ctx->channels,
				m_in_audio_codec_ctx->frame_size,m_in_audio_codec_ctx->sample_fmt, 1); 
	}
	return true;
}

bool CPlayFile::close_decoder()
{
	if(m_in_video_codec)
	{
		avcodec_close(m_in_video_codec_ctx);
		m_in_audio_codec_ctx = NULL;
	}
	if(m_in_audio_codec)
	{
		avcodec_close(m_in_audio_codec_ctx);
		m_in_audio_codec_ctx = NULL;
	}
	return true;
}

AVPacket* CPlayFile::pull_video_busy_pkt()
{
	AVPacket* avp = NULL;
	m_pool_busy_video_avpacket_lock.Lock();
	if(m_pool_busy_video_avpacket_list.size() > 0)
	{
		avp = m_pool_busy_video_avpacket_list.front();
		m_pool_busy_video_avpacket_list.pop_front();
	}
	m_pool_busy_video_avpacket_lock.Unlock();
	return avp;
}

AVPacket* CPlayFile::pull_audio_busy_pkt()
{
	AVPacket* avp = NULL;
	m_pool_busy_audio_avpacket_lock.Lock();
	if(m_pool_busy_audio_avpacket_list.size() > 0)
	{
		avp = m_pool_busy_audio_avpacket_list.front();
		m_pool_busy_audio_avpacket_list.pop_front();
		
	}
	m_pool_busy_audio_avpacket_lock.Unlock();
	return avp;
}

bool CPlayFile::decode_one_video_frame(VideoFrame&  vf,int& nSpan)
{
	if(m_push_and_play_pause)
		return false;
	bool bIsOK = false;
	int got_picture;
	AVFrame *pframe = av_frame_alloc();
	AVPacket* pkt = NULL;
	pkt = pull_video_busy_pkt();
	if(pkt )
	{
		if(pkt->stream_index == m_in_video_index && m_in_video_codec_ctx)
		{
			int nR = avcodec_decode_video2(m_in_video_codec_ctx, pframe, &got_picture, pkt);
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
					pframe->pts = av_frame_get_best_effort_timestamp(pframe);
				}

				double sp  = 0.0;
				AVRational base_time  = {m_in_fmt_ctx->streams[m_in_video_index]->avg_frame_rate.den,m_in_fmt_ctx->streams[m_in_video_index]->avg_frame_rate.num};
				if(base_time.num != 0)
				{
					sp =  av_q2d(base_time) * 1000.0;
					nSpan = sp ;
				}
				else
					nSpan = (pframe->pts - m_video_cur_pts) ;

				if(m_video_duration > -0.0000001 && m_video_duration < 0.0000001)
					vf.fltOneFrameDuration = nSpan ;
				else
				{
					vf.fltOneFrameDuration = m_video_duration * 1000;
				}

				m_video_cur_pts = pframe->pts;
				vf.nVideoFrameHeight = pframe->height;
				vf.nVideoFrameWidth= pframe->width;

				vf.mVideoFramePts = (m_video_cur_pts *1.0 / m_video_stream_timebase) * 1000; 

				if(m_pic_bufffer == NULL)
				{
					m_pic_bufffer = new unsigned char[m_pic_size];
				}
				for(int i=0, nDataLen=0; i<3; i++)
				{
					int nShift = (i == 0) ? 0 : 1;
					unsigned char *pYUVData = pframe->data[i];
					unsigned int  nW = pframe->width >> nShift;
					unsigned int  nH = pframe->height >> nShift;
					for(int j = 0; j< nH;j++)
					{
						memcpy(m_pic_bufffer+nDataLen,pframe->data[i] + j * pframe->linesize[i], nW); 
						nDataLen += nW;
					}
				}
				vf.pVideoFrameBuf = m_pic_bufffer;
				vf.nVideoFrameSize = m_pic_size;
				bIsOK = true;
			}
		}
		put_free_pkt(pkt);
	}
	av_frame_free(&pframe);
	return bIsOK;
}

bool CPlayFile::decode_one_audio_frame(AudioFrame& af)
{
	if(m_push_and_play_pause)
		return false;

	bool bIsOK = false;
	AVFrame *pframe = av_frame_alloc();
	int got_picture;
	AVPacket* pkt = NULL;
	pkt = pull_audio_busy_pkt();
	if(pkt)
	{
		if(pkt->stream_index == m_in_audio_index && m_in_audio_codec_ctx)
		{
			int len = 0;
			while(pkt->size > 0)
			{
				if ((len = avcodec_decode_audio4(m_in_audio_codec_ctx, pframe, &got_picture, pkt)) >= 0) 
				{
					if(got_picture)
					{
						pframe->pts = pkt->dts;
						m_audio_cur_pts = pframe->pts;
						copy_audio_frame(pframe,af);
						bIsOK = true;
					}
					pkt->size -= len;
				}
				else
				{
					break;
				}		
			}
		}
		put_free_pkt(pkt);
	}
	av_frame_free(&pframe);
	return bIsOK;
}


double CPlayFile::get_clock(Clock *c)
{
    if (*c->queue_serial != c->serial)
        return NAN;

    if (c->paused) 
	{
        return c->pts;
    } 
	else 
	{
        double time = av_gettime_relative() / 1000000.0;
        return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
    }
}

double CPlayFile::get_master_clock(int ntype)
{
    double val;
    switch (ntype)
	{
        case AV_SYNC_VIDEO_MASTER:
            val = get_clock(&m_vidclk);
            break;
        case AV_SYNC_AUDIO_MASTER:
            val = get_clock(&m_audclk);
            break;
        default:
            val = get_clock(&m_audclk);
            break;
    }
    return val;
}

int CPlayFile::synchronize_audio(int nb_samples)
{
	int audio_diff_avg_count;
    int wanted_nb_samples = nb_samples;
    double diff, avg_diff;
    int min_nb_samples, max_nb_samples;
    diff = get_clock(&m_audclk) - get_master_clock(AV_SYNC_AUDIO_MASTER);

    if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) 
	{
        m_audio_diff_cum = diff + m_audio_diff_avg_coef * m_audio_diff_cum;
        if (m_audio_diff_avg_count < AUDIO_DIFF_AVG_NB)
		{
            /* not enough measures to have a correct estimate */
            m_audio_diff_avg_count++;
        } 
		else 
		{
            /* estimate the A-V difference */
            avg_diff = m_audio_diff_cum * (1.0 - m_audio_diff_avg_coef);

            if (fabs(avg_diff) >= m_audio_diff_threshold) 
			{
                wanted_nb_samples = nb_samples + (int)(diff * m_audio_src.freq);
                min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
                max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
                wanted_nb_samples = FFMIN(FFMAX(wanted_nb_samples, min_nb_samples), max_nb_samples);
            }
        }
    }
    return wanted_nb_samples;
}


bool CPlayFile::copy_audio_frame(AVFrame * pframe,AudioFrame & af)
{ 

	unsigned int n_buf_size = 0;
	if(pframe)
	{
		if(m_aud_bufffer == NULL)
		{
			m_aud_bufffer = new unsigned char[m_aud_size];
		}

		unsigned char* audio_buf = m_aud_bufffer;

		int data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(pframe),
                                            pframe->nb_samples,
                                           (AVSampleFormat)pframe->format, 1);

		int64_t dec_channel_layout = (pframe->channel_layout && av_frame_get_channels(pframe) == av_get_channel_layout_nb_channels(pframe->channel_layout)) ?
									pframe->channel_layout : av_get_default_channel_layout(av_frame_get_channels(pframe));

		int n_src_nb_samples = pframe->nb_samples;

		//int wanted_nb_samples = 1024;//pframe->nb_samples;

		int wanted_nb_samples = pframe->nb_samples; //synchronize_audio(pframe->nb_samples);

		if (pframe->format        != m_audio_src.fmt            ||
			dec_channel_layout       != m_audio_src.channel_layout ||
			pframe->sample_rate   != m_audio_src.freq           ||
			n_src_nb_samples != wanted_nb_samples  ||
			!m_au_convert_ctx) 
		{
			swr_free(&m_au_convert_ctx);
			m_au_convert_ctx = NULL;
			m_au_convert_ctx = swr_alloc_set_opts(NULL,m_audio_hw_params.channel_layout, m_audio_hw_params.fmt, m_audio_hw_params.freq,
												dec_channel_layout, (AVSampleFormat)pframe->format, pframe->sample_rate,
												0, NULL);
			if (!m_au_convert_ctx || swr_init(m_au_convert_ctx) < 0)
			{
				return false;
			}

			m_audio_src.channel_layout = dec_channel_layout;
			m_audio_src.channels       = av_frame_get_channels(pframe);
			m_audio_src.freq = pframe->sample_rate;
			m_audio_src.fmt = (AVSampleFormat)pframe->format;
		}

		if(m_au_convert_ctx)
		{
			unsigned char* out_buffer = audio_buf;
			const uint8_t **in = (const uint8_t **)pframe->extended_data;
			uint8_t **out = &out_buffer;
			int out_count = (int64_t)wanted_nb_samples * m_audio_hw_params.freq / pframe->sample_rate + 256;
			int out_size  = av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, out_count, m_audio_hw_params.fmt, 0);
			int len2;
			if (out_size < 0) 
			{
				return false;
			}

			if (wanted_nb_samples != pframe->nb_samples) 
			{
				if (swr_set_compensation(m_au_convert_ctx, (wanted_nb_samples - pframe->nb_samples) * m_audio_hw_params.freq / pframe->sample_rate,
											wanted_nb_samples * m_audio_hw_params.freq / pframe->sample_rate) < 0) 
				{
					return false;
				}
			}

			len2 = swr_convert(m_au_convert_ctx, out, out_count, in, pframe->nb_samples);

			if (len2 < 0)
			{
				return false;
			}
			if (len2 == out_count) 
			{
				swr_init(m_au_convert_ctx);
			}
			unsigned int nbufsize = len2 * m_audio_hw_params.channels * av_get_bytes_per_sample(m_audio_hw_params.fmt);
			n_buf_size += nbufsize;
		}
		else
		{
			memcpy(audio_buf,pframe->data[0],data_size);
			n_buf_size += data_size;
		}

		af.nPCMBufSize = n_buf_size;
		af.pPCMBuffer = m_aud_bufffer;
		af.mVideoFramePts = ( m_audio_cur_pts * 1.0 /m_audio_stream_timebase) * 1000;
		return true;
	}
	return false;
}

int CPlayFile::audio_open(play_audio_callback pCallback,void* dwUser)
{
	if(m_in_audio_codec_ctx == NULL)
		return 0;
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
        wanted_nb_channels = av_get_default_channel_layout(wanted_nb_channels);
    }
    if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) 
	{
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
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
    m_wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(m_wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    m_wanted_spec.callback = pCallback;
    m_wanted_spec.userdata = dwUser;

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
		wanted_channel_layout = av_get_default_channel_layout(m_wanted_spec.channels);
	}

    if (spec.format != AUDIO_S16SYS) 
	{
        return -1;
    }
    if (spec.channels != m_wanted_spec.channels) 
	{
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) 
		{
            return -1;
        }
    }

    m_audio_hw_params.fmt = AV_SAMPLE_FMT_S16;
    m_audio_hw_params.freq = spec.freq;
    m_audio_hw_params.channel_layout = wanted_channel_layout;
    m_audio_hw_params.channels =  spec.channels;
    m_audio_hw_params.frame_size = av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, 1, m_audio_hw_params.fmt, 1);
    m_audio_hw_params.bytes_per_sec = av_samples_get_buffer_size(NULL, m_audio_hw_params.channels, m_audio_hw_params.freq, m_audio_hw_params.fmt, 1);

    if (m_audio_hw_params.bytes_per_sec <= 0 || m_audio_hw_params.frame_size <= 0) 
	{
        return -1;
    }

	m_audio_src = m_audio_hw_params;

	if(spec.size >= 0)
		SDL_PauseAudio(0); 

	m_audio_hw_buf_size = spec.size;
	m_audio_diff_threshold = (double)(m_audio_hw_buf_size) /  m_audio_hw_params.bytes_per_sec;
    return spec.size;
}


unsigned int CPlayFile::get_audio_frame(AudioFrame af)
{
	 
	unsigned char* audio_buf = NULL;
	int audio_len = -1;
	unsigned int n_buf_size = 0;
	n_buf_size = af.nPCMBufSize;
	audio_buf = af.pPCMBuffer;
	if(n_buf_size > 0 && audio_buf)
	{
		audio_len = n_buf_size;  
		av_fast_malloc(&m_audio_buf, &m_audio_buf_size, audio_len);
		if(m_audio_buf)
		{
			memcpy(m_audio_buf,audio_buf,audio_len);
		}
		else
		{
			audio_len = -1;
		}
	}
	else
	{
		int n = 0;
	}
	m_audio_pos = 0;
	return  audio_len;
}


void  CPlayFile::audio_play(void *udata,unsigned char *stream,int len,int64_t& pts)
{
	int audio_len = 0;
	int len1 = 0;
	while(len > 0)
	{
		AudioFrame af;
		if(m_audio_pos >= m_audio_buf_len)
		{
			decode_one_audio_frame(af);
			pts = af.mVideoFramePts;
			audio_len = get_audio_frame(af);
			if(audio_len < 0)
			{
				m_audio_buf_len = 1024;
				av_fast_malloc(&m_audio_buf, &m_audio_buf_size, m_audio_buf_len);
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

bool CPlayFile::pause_in_file(bool bIsPause)
{
	m_play_lock.Lock();
	bool br = false;
	do 
	{
		int nr = 0;
		if(NULL==m_in_fmt_ctx)
		{
			break;
		}

		if(m_push_and_play_pause == bIsPause)
		{
			br = true;
			break;
		}

		SDL_PauseAudio(bIsPause);
		if(bIsPause)
		{
			av_read_pause(m_in_fmt_ctx);
			clear_busy_cache_list();
		}
		else
		{
			av_read_play(m_in_fmt_ctx);
		}
		
		m_push_and_play_pause = bIsPause;		
	} while (false);
	
	m_play_lock.Unlock();
	return br;
}

bool CPlayFile::seek_in_file(unsigned int nSeekTime)
{
	m_play_lock.Lock();
	clear_busy_cache_list();
	if(m_in_fmt_ctx)
	{
		int64_t seet_time = ((int64_t)nSeekTime) * AV_TIME_BASE / 1000;
		bool br = avformat_seek_file(m_in_fmt_ctx,-1,0,seet_time,m_in_fmt_ctx->duration,0)>= 0 ? true : false;
		if(br)
		{
			m_play_lock.Unlock();
			return true;
		}
	}
	m_play_lock.Unlock();
	return false;
}

bool CPlayFile::seek_in_frame(unsigned int nSeekTime,bool video /* = true */)
{	
	bool br = false;
	m_play_lock.Lock();

	do 
	{
		if(m_push_and_play_pause)
		{
			av_read_play(m_in_fmt_ctx);
		}
		int64_t seet_time =  (int64_t)nSeekTime;
		clear_busy_cache_list();

		if(NULL==m_in_fmt_ctx)
		{
			break;
		}

		if(!video)
		{
			if(NULL==m_in_audio_st)
			{
				break;
			}

			seet_time = seet_time * m_in_audio_st->time_base.den /((double)m_in_audio_st->time_base.num * 1000);
			br = av_seek_frame(m_in_fmt_ctx,m_in_audio_index,seet_time,AVSEEK_FLAG_BACKWARD)>=0 ? true:false;
			break;
		}

		if(NULL==m_in_video_st)
		{
			break;
		}

		seet_time = seet_time * m_in_video_st->time_base.den /((double)m_in_video_st->time_base.num * 1000);
		br = av_seek_frame(m_in_fmt_ctx,m_in_video_index,seet_time,AVSEEK_FLAG_BACKWARD)>=0 ? true:false;

		if(m_push_and_play_pause)
		{
			av_read_pause(m_in_fmt_ctx);
		}
	} while (false);
	
	m_play_lock.Unlock();
	return br;
}


unsigned int CPlayFile::get_in_file_duration()
{
	if(m_in_fmt_ctx)
	{
		return m_in_fmt_ctx->duration * 1000 / AV_TIME_BASE;
	}
	return 0;
}

unsigned int CPlayFile::get_in_file_current_play_time()
{
	int audio_time = 0;
	int video_time = 0;
	
	if(m_in_audio_st)
	{
	    audio_time=m_audio_cur_pts * ((double)m_in_audio_st->time_base.num * 1000/m_in_audio_st->time_base.den);
	}
	
	if(m_in_video_st)
	{
        video_time=m_video_cur_pts* ((double)m_in_video_st->time_base.num * 1000/m_in_video_st->time_base.den);		
	}

	return audio_time == 0 ? video_time : audio_time;
}

unsigned int CPlayFile::get_in_file_current_stream_time(bool video /* = true */)
{
	int cur_time = 0;
	
	if(video)
	{
		if(m_in_video_st)
		{
			cur_time=m_video_cur_pts* ((double)m_in_video_st->time_base.num * 1000/m_in_video_st->time_base.den);		
		}
	}
	else
	{
		if(m_in_audio_st)
		{
			cur_time=m_audio_cur_pts * ((double)m_in_audio_st->time_base.num * 1000/m_in_audio_st->time_base.den);
		}
	}

	return cur_time;
}

double CPlayFile::get_video_one_frame_duration()
{
	double video_duration = 0.0;
	double dbFrameRate = 0.0; 
	if(m_in_video_st)
	{
		if(m_in_video_st->avg_frame_rate.num != 0)
		{
			dbFrameRate = av_q2d(m_in_video_st->avg_frame_rate); //平均帧速度
			if (strstr(m_in_fmt_ctx->iformat->name,"mpegts")!= NULL)
			{
				double time_base = 90 * 1000;
				video_duration = time_base / dbFrameRate /100000;
			}
			else
			{
				video_duration = 1 / dbFrameRate;
			}
		}
	}
	return video_duration;
}

bool CPlayFile::switch_paly(play_audio_callback pCallback,void* dwUser,bool bflag)
{
	m_refLock.Lock();

	SDL_audiostatus audioStatus = SDL_GetAudioStatus();
	if(!bflag)
	{
		if(audioStatus != SDL_AUDIO_STOPPED)
		{
			SDL_CloseAudio();
		}

		return true;
	}
	
	bool br = false;
	if(audioStatus != SDL_AUDIO_STOPPED)
	{
		SDL_CloseAudio();
	}

	if(m_in_audio_codec_ctx)
	{
		if(audio_open(pCallback,dwUser)>=0)
		{
			SDL_Delay(10);
			SDL_PauseAudio(0);
			br = true;
		}
	}	
	m_refLock.Unlock();
	return true;
}

bool CPlayFile::get_video_frame(unsigned int nFramePos,VideoFrame& vf,int& nSpan)
{
// 	if(!m_push_and_play_pause)
// 	{
// 		return false;
// 	}

	if(NULL == m_in_video_codec_ctx)
	{
		return false;
	}

	bool br = false;
	AVPacket *pkt = false;
	AVFrame *pframe = NULL;

	unsigned int frameTimestamp = nFramePos * m_in_video_st->time_base.den /((double)m_in_video_st->time_base.num * 1000);

	m_play_lock.Lock();
	pkt = get_free_pkt();

	av_read_play(m_in_fmt_ctx);

	while(true)
	{		
		if(NULL==pkt)
		{
			break;
		}

		if(av_read_frame(m_in_fmt_ctx, pkt)<0)
		{
			break;
		}

		if(pkt->stream_index != m_in_video_index)
		{
			continue;
		}

		AVStream *in_stream, *out_stream;
		in_stream  = m_in_fmt_ctx->streams[pkt->stream_index];
		out_stream = m_in_fmt_ctx->streams[pkt->stream_index];

		//转换PTS/DTS（Convert PTS/DTS）
		pkt->pts = av_rescale_q_rnd(pkt->pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt->dts = av_rescale_q_rnd(pkt->dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base, out_stream->time_base);

		int got_picture = 0;
		pframe = av_frame_alloc();
		if(NULL==pframe)
		{
			break;
		}

		avcodec_decode_video2(m_in_video_codec_ctx, pframe, &got_picture, pkt);

		if(!got_picture)
		{	
			av_frame_free(&pframe);
			pframe = NULL;
			continue;
		}

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
			pframe->pts = av_frame_get_best_effort_timestamp(pframe);
		}

		if(pframe->pts >= frameTimestamp)
		{
			if(pframe->pts == frameTimestamp)
			{
				br = true;
			}

			break;
		}

		av_frame_free(&pframe);
		pframe = NULL;
	};
	
	av_read_pause(m_in_fmt_ctx);

	double sp  = 0.0;
	AVRational base_time  = {m_in_fmt_ctx->streams[m_in_video_index]->avg_frame_rate.den,m_in_fmt_ctx->streams[m_in_video_index]->avg_frame_rate.num};
	if(base_time.num != 0)
	{
		sp =  av_q2d(base_time) * 1000.0;
		nSpan = sp ;
	}
	else
	{
		nSpan = (pframe->pts - m_video_cur_pts) ;
	}

	if(m_video_duration > -0.0000001 && m_video_duration < 0.0000001)
	{
		vf.fltOneFrameDuration = nSpan ;
	}
	else
	{
		vf.fltOneFrameDuration = m_video_duration * 1000;
	}

	m_video_cur_pts = pframe->pts;
	vf.nVideoFrameHeight = pframe->height;
	vf.nVideoFrameWidth= pframe->width;

	vf.mVideoFramePts = (m_video_cur_pts *1.0 / m_video_stream_timebase) * 1000; 

	if(m_pic_bufffer == NULL)
	{
		m_pic_bufffer = new unsigned char[m_pic_size];
	}
	for(int i=0, nDataLen=0; i<3; i++)
	{
		int nShift = (i == 0) ? 0 : 1;
		unsigned char *pYUVData = pframe->data[i];
		unsigned int  nW = pframe->width >> nShift;
		unsigned int  nH = pframe->height >> nShift;
		for(int j = 0; j< nH;j++)
		{
			memcpy(m_pic_bufffer+nDataLen,pframe->data[i] + j * pframe->linesize[i], nW); 
			nDataLen += nW;
		}
	}

	vf.pVideoFrameBuf = m_pic_bufffer;
	vf.nVideoFrameSize = m_pic_size;

	av_frame_free(&pframe);	
	put_free_pkt(pkt);
	m_play_lock.Unlock();
	
	return br;
}

bool CPlayFile::get_video_frame(unsigned int nFramePos,VideoFrame& vf)
{
	//打开输入文件
	AVFormatContext *	fmt_ctx = NULL;
	AVCodec *			video_cdc = NULL;
	AVCodecContext *	video_cdc_ctx = NULL;
	AVStream	*		video_st = NULL;
	AVStream	*		audio_st = NULL;
	AVPacket *			packet_read = NULL;
	AVFrame *			frame_read = NULL;

	bool	br = false;
	int		video_index = 0;
	int		audio_index = 0;
	int		space_time = 0;
	unsigned int frameTime = 0;

	do 
	{
		if (avformat_open_input(&fmt_ctx, m_str_in_file_url, 0, 0)< 0)
		{
			break;
		}

		//查找文件的流信息
		if (avformat_find_stream_info(fmt_ctx, 0) < 0)
		{
			return false;
		}

		av_dump_format(fmt_ctx, 0, m_str_in_file_url, 0);

		//解复用流
		for (int i = 0; i < fmt_ctx->nb_streams; i++) 
		{
			if ( fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
			{
				video_index = i;
				video_st = fmt_ctx->streams[video_index];
			}

			if ( fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				audio_index = i;
				audio_st = fmt_ctx->streams[audio_index];
			}
		}

		if(NULL == video_st)
		{
			break;
		}

		video_cdc_ctx = video_st->codec;
		if(NULL==video_cdc_ctx)
		{
			break;
		}

		video_cdc = avcodec_find_decoder(video_cdc_ctx->codec_id);
		if (NULL==video_cdc) 
		{
			break;
		}

		if (avcodec_open2(video_cdc_ctx, video_cdc,NULL) < 0) 
		{
			break;
		}

		frameTime = nFramePos * video_st->time_base.den /((double)video_st->time_base.num * 1000);
		packet_read = new AVPacket();
		if(NULL == packet_read)
		{
			break;
		}

		av_init_packet(packet_read);
		av_read_play(fmt_ctx);

		//find frame from 5 second before...xie wen bing 2018.11.8
		if(frameTime > 5000)
		{
			av_seek_frame(fmt_ctx,video_index,frameTime-5000,AVSEEK_FLAG_BACKWARD);
		}
		else
		{
			av_seek_frame(fmt_ctx,video_index,0,AVSEEK_FLAG_BACKWARD);
		}

		while(true)
		{
			int got_picture = 0;
			if(av_read_frame(fmt_ctx, packet_read)<0)
			{
				break;
			}

			if(packet_read->stream_index != video_index)
			{
				continue;
			}

			AVStream *in_stream = NULL;
			AVStream *out_stream = NULL;
			
			in_stream  = fmt_ctx->streams[packet_read->stream_index];
			out_stream = fmt_ctx->streams[packet_read->stream_index];

			//转换PTS/DTS（Convert PTS/DTS）
			packet_read->pts = av_rescale_q_rnd(packet_read->pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
			packet_read->dts = av_rescale_q_rnd(packet_read->dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
			packet_read->duration = av_rescale_q(packet_read->duration, in_stream->time_base, out_stream->time_base);

			frame_read = av_frame_alloc();
			if(NULL==frame_read)
			{
				break;
			}

			avcodec_decode_video2(video_cdc_ctx, frame_read, &got_picture, packet_read);

			if(!got_picture)
			{	
				av_frame_free(&frame_read);
				frame_read = NULL;
				continue;
			}

			if (packet_read->pts != AV_NOPTS_VALUE)
			{
				frame_read->pts = frame_read->pkt_pts;
			}
			else if(packet_read->dts != AV_NOPTS_VALUE)
			{
				frame_read->pts = frame_read->pkt_dts;
			}
			else
			{
				frame_read->pts = av_frame_get_best_effort_timestamp(frame_read);
			}

			if(frame_read->pts >= frameTime)
			{
				//add frame pos compare ......xiewenbing 2018.11.8 
				unsigned int read_pos = frame_read->pts * video_st->time_base.num * 1000 / video_st->time_base.den;
				if(frame_read->pts == frameTime || nFramePos == read_pos)
				{
					br = true;
				}

				break;
			}

			av_frame_free(&frame_read);
			frame_read = NULL;
		}

		if(!br)
		{
			break;
		}

		av_read_pause(fmt_ctx);

		double space  = 0.0;
		AVRational base_time  = {fmt_ctx->streams[video_index]->avg_frame_rate.den,fmt_ctx->streams[video_index]->avg_frame_rate.num};
		if(base_time.num != 0)
		{
			space =  av_q2d(base_time) * 1000.0;
			space_time = space ;
		}
		else
		{
			space_time = (frame_read->pts - m_video_cur_pts) ;
		}

		if(m_video_duration > -0.0000001 && m_video_duration < 0.0000001)
		{
			vf.fltOneFrameDuration = space_time ;
		}
		else
		{
			vf.fltOneFrameDuration = m_video_duration * 1000;
		}

		
		vf.nVideoFrameHeight = frame_read->height;
		vf.nVideoFrameWidth= frame_read->width;

		vf.mVideoFramePts = (m_video_cur_pts *1.0 / m_video_stream_timebase) * 1000; 

		if(m_pic_bufffer == NULL)
		{
			m_pic_bufffer = new unsigned char[m_pic_size];
		}
		for(int i=0, nDataLen=0; i<3; i++)
		{
			int nShift = (i == 0) ? 0 : 1;
			unsigned char *pYUVData = frame_read->data[i];
			unsigned int  nW = frame_read->width >> nShift;
			unsigned int  nH = frame_read->height >> nShift;
			for(int j = 0; j< nH;j++)
			{
				memcpy(m_pic_bufffer+nDataLen,frame_read->data[i] + j * frame_read->linesize[i], nW); 
				nDataLen += nW;
			}
		}

		vf.pVideoFrameBuf = m_pic_bufffer;
		vf.nVideoFrameSize = m_pic_size;

	} while (false);

	if(NULL!=frame_read)
	{
		av_frame_free(&frame_read);
		frame_read = NULL;
	}

	if(NULL !=packet_read)
	{
		av_free_packet(packet_read);
		DELMEM(packet_read);
	}

	if(NULL!=video_cdc)
	{
		avcodec_close(video_cdc_ctx);
		video_cdc = NULL;
	}
	
	if(NULL != fmt_ctx)
	{
		avformat_close_input(&fmt_ctx);
		fmt_ctx = NULL;
	}

	return br;
}