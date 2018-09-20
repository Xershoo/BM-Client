#include <Windows.h>
#include "transcode.h"
#include "loadffmpeg.h"

#define DELMEM(p) { if(p){ delete p; p = NULL;}}

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")

#define AUDIO_ID            0  
#define VIDEO_ID            1

void CALLBACK _trancde_loop(void *pUserData)
{
	if(pUserData)
	{
		CTranscode * pUser = (CTranscode *)pUserData;
		pUser->TranscodingEx();
	}
}

CTranscode::CTranscode()
{
	m_szSourFileName[0] = '\0';
	m_szDestFileName[0] = '\0';
	m_szErrMsg[0] = '\0';
	m_inAVFormatContext = NULL;
	m_nVideoStreamIdx = -1;
	m_nAudioStreamIdx = -1;
	m_outAVFormatContext = NULL;
	m_outVideoST = NULL;
	m_outAudioST = NULL;
	m_audio_codec = NULL;
	m_video_codec = NULL;

	//video param
	m_nVideoWidth = 640;
	m_nVideoHeight = 480;
	m_dbFrameRate = 25;  //帧率                                                  
	m_nVideoCodecID = AV_CODEC_ID_H264;
	m_VideoPixelfromat = AV_PIX_FMT_YUV420P;
	m_nBitRrate = 1000000;
	m_nGopSize = 12;
	m_nMaxBframe = 2;
	m_ThreadCount = 2;

	//audio param
	m_nChannelCount = 2;      //声道
	m_nBitsPerSample = AV_SAMPLE_FMT_S16_t;    //样本
	m_nFrequency = 44100;     //采样率
	m_audiomuxtimebasetrue = 1;

	//aac
	m_nAudioFrameSize  = 1024;
	m_outAudioCodecID = AV_CODEC_ID_AAC;

	m_fifo = NULL;
	m_first_audio_pts = 0; 
	m_is_first_audio_pts = 0;


	m_vbsf_aac_adtstoasc = NULL;

	m_bIsVideoTranscode  = false;
	m_bAudioVideoTranscode  = false;

	m_img_convert_ctx_video = NULL;
	m_sws_flags = SWS_BICUBIC; //差值算法,双三次
	m_dst_data = NULL; 
	m_bIsTrancode = false;
	m_pEventCallBack = NULL;
	m_EventpUser =NULL;
	m_nAllFrame = 0;
	m_bIsConversion = false;

	av_register_all();
	avformat_network_init();
}

CTranscode::~CTranscode()
{
	m_threadTrancode.End();
	uinit_mux();
	uinit_demux();
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
	avformat_network_deinit();
}


AVStream * CTranscode::add_out_stream(AVFormatContext* output_format_context,AVMediaType codec_type_t)
{
	AVStream * output_stream = NULL;
	AVStream *in_stream = NULL;
	AVCodecContext* output_codec_context = NULL;

	switch (codec_type_t)
	{
	case AVMEDIA_TYPE_AUDIO:
		in_stream = m_inAVFormatContext->streams[m_nAudioStreamIdx];
		break;
	case AVMEDIA_TYPE_VIDEO:
		in_stream = m_inAVFormatContext->streams[m_nVideoStreamIdx];
		break;
	default:
		break;
	}

	output_stream = avformat_new_stream(output_format_context,in_stream->codec->codec);
	if (!output_stream)
	{
		return NULL;
	}

	output_stream->id = output_format_context->nb_streams - 1;
	output_codec_context = output_stream->codec;
    output_stream->time_base  = in_stream->time_base;

	int ret = 0;
	ret = avcodec_copy_context(output_stream->codec, in_stream->codec);
	if (ret < 0) 
	{
		printf("Failed to copy context from input to output stream codec context\n");
		return NULL;
	}

	//这个很重要，要么纯复用解复用，不做编解码写头会失败,
	//另或者需要编解码如果不这样，生成的文件没有预览图，还有添加下面的header失败，置0之后会重新生成extradata
	output_codec_context->codec_tag = 0; 

	//if(! strcmp( output_format_context-> oformat-> name,  "mp4" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "mov" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "3gp" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "flv"))
	if(AVFMT_GLOBALHEADER & output_format_context->oformat->flags)
	{
		output_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	return output_stream;
}

AVStream * CTranscode::add_out_stream2(AVFormatContext* output_format_context,AVMediaType codec_type_t,AVCodec **codec)
{
	AVCodecContext* output_codec_context = NULL;
	AVStream * in_stream = NULL;
	AVStream * output_stream = NULL;
	AVCodecID codecID;

	switch (codec_type_t)
	{
	case AVMEDIA_TYPE_AUDIO:
		codecID = m_outAudioCodecID;
        in_stream = m_inAVFormatContext->streams[m_nAudioStreamIdx];
		break;
	case AVMEDIA_TYPE_VIDEO:
		codecID = m_nVideoCodecID;
        in_stream = m_inAVFormatContext->streams[m_nVideoStreamIdx];
		break;
	default:
		break;
	}

	/* find the encoder */
	*codec = avcodec_find_encoder(codecID);
	if (!(*codec)) 
	{
		return NULL;
	}

	output_stream = avformat_new_stream(output_format_context,*codec);
	if (!output_stream)
	{
		return NULL;
	}

	output_stream->id = output_format_context->nb_streams - 1;
	output_codec_context = output_stream->codec;

	
	switch (codec_type_t)
	{
	case AVMEDIA_TYPE_AUDIO:
		output_codec_context->codec_id = m_outAudioCodecID;
		output_codec_context->codec_type = codec_type_t;
		output_stream->start_time = 0;
		output_codec_context->sample_rate = m_nFrequency;
		output_codec_context->channels = m_nChannelCount;
		if(m_nChannelCount > 2)
		{
			output_codec_context->channels = 2;
			output_codec_context->channel_layout = av_get_default_channel_layout(2);
		}
		else
		{
			output_codec_context->channels  = m_nChannelCount;
			if (m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channel_layout == 0)
			{
				output_codec_context->channel_layout = av_get_default_channel_layout(m_nChannelCount);
			}
			else
			{
				output_codec_context->channel_layout = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channel_layout;
			}
		}

		//这个码率有些编码器不支持特别大，例如wav的码率是1411200 比aac大了10倍多
		output_codec_context->bit_rate = 128000;//icodec->streams[audio_stream_idx]->codec->bit_rate;
		output_codec_context->frame_size = m_nAudioFrameSize;
		output_codec_context->sample_fmt = (AVSampleFormat)m_nBitsPerSample;
		//output_codec_context->block_align = 0;
		//查看音频支持的声道，采样率，样本
		audio_support(*codec,&output_codec_context->channels,
			(int *)&output_codec_context->channel_layout,
			&output_codec_context->sample_rate,
			(AVSampleFormat_t *)&output_codec_context->sample_fmt);
		//m_nChannelCount = output_codec_context->channels;
		m_nFrequency = output_codec_context->sample_rate;
		m_nBitsPerSample = (AVSampleFormat_t)output_codec_context->sample_fmt;
		break;
	case AVMEDIA_TYPE_VIDEO:
        AVRational r_frame_rate_t;
        r_frame_rate_t.num = 100;
        r_frame_rate_t.den = (int)(m_dbFrameRate * 100);
        output_codec_context->time_base = in_stream->codec->time_base;
        output_stream->time_base  = in_stream->time_base;
        output_stream->r_frame_rate.num = r_frame_rate_t.den;
        output_stream->r_frame_rate.den = r_frame_rate_t.num;
		output_codec_context->codec_id = m_nVideoCodecID;
		output_codec_context->codec_type = codec_type_t;
		output_stream->start_time = 0;
		output_codec_context->pix_fmt = m_VideoPixelfromat;
		output_codec_context->width = m_nVideoWidth;
		output_codec_context->height = m_nVideoHeight;
		output_codec_context->bit_rate = m_nBitRrate;
		output_codec_context->gop_size  = m_nGopSize;         /* emit one intra frame every twelve frames at most */;
		output_codec_context->max_b_frames = m_nMaxBframe;	//设置B帧最大数
		output_codec_context->thread_count = m_ThreadCount;  //线程数目
		output_codec_context->me_range = 16;
		output_codec_context->max_qdiff = 4;
		output_codec_context->qmin = 20; //调节清晰度和编码速度 //这个值调节编码后输出数据量越大输出数据量越小，越大编码速度越快，清晰度越差
		output_codec_context->qmax = 40; //调节清晰度和编码速度
		output_codec_context->qcompress = 0.6; 
		//查看视频支持的yuv格式
		video_support(*codec,&output_codec_context->pix_fmt);
		m_VideoPixelfromat = output_codec_context->pix_fmt;
		break;
	default:
		break;
	}
	//这个很重要，要么纯复用解复用，不做编解码写头会失败,
	//另或者需要编解码如果不这样，生成的文件没有预览图，还有添加下面的header失败，置0之后会重新生成extradata
	output_codec_context->codec_tag = 0; 

	//if(! strcmp( output_format_context-> oformat-> name,  "mp4" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "mov" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "3gp" ) ||
	//	!strcmp (output_format_context ->oformat ->name , "flv"))
	if(AVFMT_GLOBALHEADER & output_format_context->oformat->flags)
	{
		output_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	return output_stream;
}

bool CTranscode::init_decode(int stream_type)
{
	AVCodec *pcodec = NULL;
	AVCodecContext *cctext = NULL;

	if (stream_type == AUDIO_ID)
	{
		cctext = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec;
		pcodec = avcodec_find_decoder(cctext->codec_id);
	}
	else if (stream_type == VIDEO_ID)
	{
		cctext = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec;
		pcodec = avcodec_find_decoder(cctext->codec_id);
	}

	//打开解码器
	if(pcodec)
	{
		if (avcodec_open2(cctext, pcodec, NULL) >= 0)
		{
			return true;
		}
	}
	strcpy(m_szErrMsg,"解码器不支持!");
	return false;
}

bool CTranscode::init_code(int stream_type)
{
	AVCodecContext *cctext = NULL;

	if (stream_type == AUDIO_ID)
	{
		cctext = m_outAudioST->codec;
		//打开编码器
		if ( avcodec_open2(cctext, m_audio_codec, NULL) < 0)
		{
			strcpy(m_szErrMsg,"编码器打开失败!");
			return false;
		}
	}
	else if (stream_type == VIDEO_ID)
	{
		cctext = m_outVideoST->codec;
		//打开编码器
		int ret = avcodec_open2(cctext, m_video_codec, NULL);
		if ( ret< 0)
		{
			strcpy(m_szErrMsg,"编码器打开失败!");
			return false;
		}
	}
	return true;
}

bool CTranscode::uinit_decode(int stream_type)
{
	AVCodecContext *cctext = NULL;

	if (stream_type == AUDIO_ID)
	{
		if (NULL != m_outAudioST)
		{
			cctext = m_outAudioST->codec; 
		}
	}
	else if (stream_type == VIDEO_ID)
	{
		if(m_inAVFormatContext)
		{
			cctext = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec; 
		}
	}
	if(cctext)
	{
		avcodec_close(cctext);
	}
	return 1;
}

bool CTranscode::uinit_code(int stream_type)
{
	AVCodecContext *cctext = NULL;

	if (stream_type == AUDIO_ID)
	{
		if (NULL != m_outAudioST)
		{
			cctext = m_outAudioST->codec;
			avcodec_close(cctext);
		}
	}
	else if (stream_type == VIDEO_ID)
	{
		if(m_outVideoST)
		{
			cctext = m_outVideoST->codec; 
			avcodec_close(cctext);
		}
	}
	return 1;
}


bool CTranscode::uinit_mux()
{
	int i = 0;
	if(m_outAVFormatContext)
	{
		AVOutputFormat* ofmt = m_outAVFormatContext->oformat;

		if (av_write_trailer(m_outAVFormatContext) < 0)
		{
			printf("Call av_write_trailer function failed\n");
		}
		if (m_vbsf_aac_adtstoasc !=NULL)
		{
			av_bitstream_filter_close(m_vbsf_aac_adtstoasc); 
			m_vbsf_aac_adtstoasc = NULL;
		}
		av_dump_format(m_outAVFormatContext, -1, m_szDestFileName, 1); 
	}

	//如果是视频需要编解码
	if(m_bIsVideoTranscode && m_nVideoStreamIdx != -1)
	{
		uinit_decode(VIDEO_ID);
		uinit_code(VIDEO_ID);
		m_bIsVideoTranscode = false;
	}
	//如果是音频需要编解码
	if(m_bAudioVideoTranscode && m_nAudioStreamIdx != -1 )
	{
		uinit_decode(AUDIO_ID);
		uinit_code(AUDIO_ID);
		m_bAudioVideoTranscode = false;
	}

	if(m_outAVFormatContext)
	{
		if (!(m_outAVFormatContext->flags & AVFMT_NOFILE))
		{
			/* Close the output file. */
			avio_close(m_outAVFormatContext->pb);
		}
		avformat_free_context(m_outAVFormatContext);
		m_outAVFormatContext = NULL;
	}
	return true;
}

bool CTranscode::uinit_demux()
{
	if(m_inAVFormatContext)
	{
		/* free the stream */
		av_free(m_inAVFormatContext);
		m_inAVFormatContext = NULL;
		m_nAudioStreamIdx = -1;
		m_nVideoStreamIdx = -1;
		return true;
	}
	return false;
}

int CTranscode::perform_decode(int stream_type,AVFrame * picture,AVPacket* pkt)
{
	AVCodecContext *cctext = NULL;
	int frameFinished = 0 ; 

	if (stream_type == AUDIO_ID)
	{
		cctext = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec;
		avcodec_decode_audio4(cctext,picture,&frameFinished,pkt);
		if(frameFinished)
		{
			return 0;
		}
	}
	else if (stream_type == VIDEO_ID)
	{
		cctext = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec;
		avcodec_decode_video2(cctext,picture,&frameFinished,pkt);
		if(frameFinished)
		{
			return 0;
		}
	}
	return 1;
}

void CTranscode::perform_yuv_conversion(AVFrame * pinframe,AVFrame * poutframe)
{
	//设置转换context
	if (m_img_convert_ctx_video == NULL)   
	{
		m_img_convert_ctx_video = sws_getContext(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width, m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height, 
			m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->pix_fmt,
			m_nVideoWidth, m_nVideoHeight,
			m_VideoPixelfromat,
			m_sws_flags, NULL, NULL, NULL);
		
	}
	if(m_img_convert_ctx_video)
	{
		//开始转换
		sws_scale(m_img_convert_ctx_video, pinframe->data, pinframe->linesize,
			0,m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height, poutframe->data, poutframe->linesize);
		poutframe->pkt_pts = pinframe->pkt_pts;
		poutframe->pkt_dts = pinframe->pkt_dts;
		//有时pkt_pts和pkt_dts不同，并且pkt_pts是编码前的dts,这里要给avframe传入pkt_dts而不能用pkt_pts
		//poutframe->pts = poutframe->pkt_pts;
		poutframe->pts = pinframe->pkt_dts;
	}
}

void CTranscode::write_frame(AVFormatContext *ocodec,int ID,AVPacket* pkt_t)
{
	int64_t pts = 0, dts = 0;
	int nRet = -1;

	if(ID == VIDEO_ID)
	{
		AVPacket videopacket_t;
		av_init_packet(&videopacket_t);

		videopacket_t.pts = av_rescale_q_rnd(pkt_t->pts, m_inAVFormatContext->streams[m_nVideoStreamIdx]->time_base, m_outVideoST->time_base, AV_ROUND_NEAR_INF);
		videopacket_t.dts = av_rescale_q_rnd(pkt_t->dts, m_inAVFormatContext->streams[m_nVideoStreamIdx]->time_base, m_outVideoST->time_base, AV_ROUND_NEAR_INF);
		videopacket_t.duration = av_rescale_q(pkt_t->duration,m_inAVFormatContext->streams[m_nVideoStreamIdx]->time_base, m_outVideoST->time_base);
		videopacket_t.flags = pkt_t->flags;
		videopacket_t.stream_index = VIDEO_ID;
		videopacket_t.data = pkt_t->data;
		videopacket_t.size = pkt_t->size;
		av_interleaved_write_frame(ocodec, &videopacket_t);
	}
	else if(ID == AUDIO_ID)
	{
		AVPacket audiopacket_t;
		av_init_packet(&audiopacket_t);

		if(m_audiomuxtimebasetrue == 0)
		{
			audiopacket_t.pts = av_rescale_q_rnd(pkt_t->pts, m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base, m_outAudioST->time_base, AV_ROUND_NEAR_INF);
			audiopacket_t.dts = av_rescale_q_rnd(pkt_t->dts, m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base, m_outAudioST->time_base, AV_ROUND_NEAR_INF);
			audiopacket_t.duration = av_rescale_q(pkt_t->duration,m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base, m_outAudioST->time_base);
		}
		else
		{
			audiopacket_t.pts = av_rescale_q_rnd(pkt_t->pts, m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base, m_outAudioST->time_base, AV_ROUND_NEAR_INF);
			audiopacket_t.dts = av_rescale_q_rnd(pkt_t->dts, m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base, m_outAudioST->time_base, AV_ROUND_NEAR_INF);
			audiopacket_t.duration = av_rescale_q(pkt_t->duration,m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base, m_outAudioST->time_base);
		}
		audiopacket_t.flags = pkt_t->flags;
		audiopacket_t.stream_index = AUDIO_ID;
		audiopacket_t.data = pkt_t->data;
		audiopacket_t.size = pkt_t->size;

		//添加过滤器
		if(! strcmp( ocodec->oformat-> name,  "mp4" ) ||
			!strcmp (ocodec ->oformat ->name , "mov" ) ||
			!strcmp (ocodec ->oformat ->name , "3gp" ) ||
			!strcmp (ocodec ->oformat ->name , "flv" ))
		{
			if (m_outAudioST->codec->codec_id == AV_CODEC_ID_AAC)
			{
				if (m_vbsf_aac_adtstoasc != NULL)
				{
					AVPacket filteredPacket = audiopacket_t; 
					int a = av_bitstream_filter_filter(m_vbsf_aac_adtstoasc,                                           
						m_outAudioST->codec, NULL,&filteredPacket.data, &filteredPacket.size,
						audiopacket_t.data, audiopacket_t.size, audiopacket_t.flags & AV_PKT_FLAG_KEY); 
					if (a >  0)             
					{                
						av_free_packet(&audiopacket_t); 
						
						filteredPacket.destruct = av_destruct_packet;  
						audiopacket_t = filteredPacket;             
					}   
					else if (a == 0)
					{
						audiopacket_t = filteredPacket;   
					}
					else if (a < 0)            
					{                
						fprintf(stderr, "%s failed for stream %d, codec %s",
							m_vbsf_aac_adtstoasc->filter->name,audiopacket_t.stream_index,m_outAudioST->codec->codec ?  m_outAudioST->codec->codec->name : "copy");
						av_free_packet(&audiopacket_t);   

					}
				}
			}
		}
		av_interleaved_write_frame(ocodec, &audiopacket_t);
	}
}

int CTranscode::perform_code(int stream_type,AVFrame * picture)
{
	AVCodecContext *cctext = NULL;
	AVPacket pkt_t;
	av_init_packet(&pkt_t);
	pkt_t.data = NULL; // packet data will be allocated by the encoder
	pkt_t.size = 0;
	int frameFinished = 0 ;
	int nRet = 0;

	if (stream_type == AUDIO_ID)
	{
		cctext = m_outAudioST->codec;

		//如果进和出的的声道，样本，采样率不同,需要重采样
		if(m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_fmt != (AVSampleFormat)m_nBitsPerSample ||
			m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channels != m_nChannelCount ||
			m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate != m_nFrequency)
		{
			int64_t pts_t = picture->pts;
			int duration_t = 0;
			if(m_audiomuxtimebasetrue == 0)
			{
				duration_t = (double)cctext->frame_size * (m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base.den /m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base.num)/ 
					m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate;
			}
			else
			{
				duration_t = (double)cctext->frame_size * (m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base.den /m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base.num)/ 
					m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate;
			}

			int frame_bytes = cctext->frame_size * av_get_bytes_per_sample(cctext->sample_fmt)* cctext->channels;
			AVFrame * pFrameResample = av_frame_alloc();
			uint8_t * readbuff = new uint8_t[frame_bytes];

			if(av_sample_fmt_is_planar(cctext->sample_fmt))
			{
				frame_bytes /= cctext->channels;
			}

			while (av_fifo_size(m_fifo) >= frame_bytes) //取出写入的未读的包
			{
				pFrameResample->nb_samples = cctext->frame_size;
				av_fifo_generic_read(m_fifo, readbuff, frame_bytes, NULL);

				//这里一定要考虑音频分片的问题
				//如果是分片的avcodec_fill_audio_frame传入的buf是单声道的，但是buf_size 是两个声道加一起的数据量
				//如果不是分片的avcodec_fill_audio_frame传入的buf是双声道的，buf_size 是两个声道加一起的数据量
				if(av_sample_fmt_is_planar(cctext->sample_fmt))
				{
					avcodec_fill_audio_frame(pFrameResample,cctext->channels,cctext->sample_fmt,readbuff,frame_bytes * cctext->channels,1);
				}
				else
				{					
					avcodec_fill_audio_frame(pFrameResample,cctext->channels,cctext->sample_fmt,readbuff,frame_bytes,0);
				}

				if(m_is_first_audio_pts == 0)
				{
					m_first_audio_pts = pts_t;
					m_is_first_audio_pts = 1;
				}
				pFrameResample->pts = m_first_audio_pts;
				m_first_audio_pts += duration_t;


				pFrameResample->pts = av_rescale_q_rnd(pFrameResample->pts, m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->time_base, m_outAudioST->codec->time_base, AV_ROUND_NEAR_INF);
				nRet = avcodec_encode_audio2(cctext,&pkt_t,pFrameResample,&frameFinished);
				if (nRet>=0 && frameFinished)
				{
					write_frame(m_outAVFormatContext,AUDIO_ID,&pkt_t);
					av_free_packet(&pkt_t);
				}
			}
			if (readbuff)
			{
				delete []readbuff;
			}
			if (pFrameResample)
			{
				av_free(pFrameResample);
				pFrameResample = NULL;
			}
		}
		else
		{
			nRet = avcodec_encode_audio2(cctext,&pkt_t,picture,&frameFinished);
			if (nRet>=0 && frameFinished)
			{
				write_frame(m_outAVFormatContext,AUDIO_ID,&pkt_t);
				av_free_packet(&pkt_t);
			}
		}
	}
	else if (stream_type == VIDEO_ID)
	{
		cctext = m_outVideoST->codec;
		//if(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->ticks_per_frame != 1)
		//{
		//	AVRational time_base_video_t;
		//	time_base_video_t.num = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->time_base.num;
		//	time_base_video_t.den = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->time_base.den /m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->ticks_per_frame;
		//	picture->pts = av_rescale_q_rnd(picture->pts, time_base_video_t, m_outVideoST->codec->time_base, AV_ROUND_NEAR_INF);
		//}
		//else
		//{
		//	picture->pts = av_rescale_q_rnd(picture->pts, m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->time_base, m_outVideoST->codec->time_base, AV_ROUND_NEAR_INF);
		//}
        picture->pts = av_rescale_q_rnd(picture->pts, m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->time_base, m_outVideoST->codec->time_base, AV_ROUND_NEAR_INF);
		avcodec_encode_video2(cctext,&pkt_t,picture,&frameFinished);
		picture->pts++;
		if (frameFinished)
		{
			write_frame(m_outAVFormatContext,VIDEO_ID,&pkt_t);
			av_free_packet(&pkt_t);
		}
	}
	return 1;
}

SwrContext * CTranscode::init_pcm_resample(AVFrame *in_frame, AVFrame *out_frame)
{
	SwrContext * swr_ctx = NULL;
	swr_ctx = swr_alloc();
	if (!swr_ctx)
	{
		printf("swr_alloc error \n");
		return NULL;
	}
	AVCodecContext * audio_dec_ctx = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec;
	AVSampleFormat sample_fmt;
	sample_fmt = (AVSampleFormat)m_nBitsPerSample;
	if (audio_dec_ctx->channel_layout == 0)
	{
		audio_dec_ctx->channel_layout = av_get_default_channel_layout(m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channels);
	}
	/* set options */
	av_opt_set_int(swr_ctx, "in_channel_layout",    audio_dec_ctx->channel_layout, 0);
	av_opt_set_int(swr_ctx, "in_sample_rate",       audio_dec_ctx->sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_dec_ctx->sample_fmt, 0);

	if(m_nChannelCount > 2)
	{
		av_opt_set_int(swr_ctx, "out_channel_layout",    av_get_default_channel_layout(2), 0);	
	}
	else
	{
		av_opt_set_int(swr_ctx, "out_channel_layout",    audio_dec_ctx->channel_layout, 0);
	}
	av_opt_set_int(swr_ctx, "out_sample_rate",       audio_dec_ctx->sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", sample_fmt, 0);
	swr_init(swr_ctx);

	int64_t src_nb_samples = in_frame->nb_samples;
	out_frame->nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx,m_outAudioST->codec->sample_rate) + src_nb_samples,
		m_outAudioST->codec->sample_rate, m_outAudioST->codec->sample_rate, AV_ROUND_UP);

	int ret = av_samples_alloc(out_frame->data, &out_frame->linesize[0], 
		m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channels, out_frame->nb_samples,m_outAudioST->codec->sample_fmt,1);
	if (ret < 0)
	{
		return NULL;
	}

	//pcm分包初始化
	int buffersize = av_samples_get_buffer_size(NULL, m_outAudioST->codec->channels,
		2048, m_outAudioST->codec->sample_fmt, 1);
	m_fifo = av_fifo_alloc(buffersize);
	return swr_ctx;
}


int CTranscode::preform_pcm_resample(SwrContext * pSwrCtx,AVFrame *in_frame, AVFrame *out_frame)
{
	int ret = 0;
	if (pSwrCtx != NULL) 
	{
		ret = swr_convert(pSwrCtx, out_frame->data, out_frame->nb_samples, 
			(const uint8_t**)in_frame->data, in_frame->nb_samples);
		if (ret < 0)
		{
			return -1;
		}
		//修改分包内存
		int buffersize = av_samples_get_buffer_size(&out_frame->linesize[0], m_outAudioST->codec->channels,
			ret, m_outAudioST->codec->sample_fmt, 1);
		int sss = av_fifo_size(m_fifo);
		sss = av_fifo_realloc2(m_fifo, av_fifo_size(m_fifo) + out_frame->linesize[0]);
		sss = av_fifo_size(m_fifo);
		av_fifo_generic_write(m_fifo, out_frame->data[0], out_frame->linesize[0], NULL);

		out_frame->pkt_pts = in_frame->pkt_pts;
		out_frame->pkt_dts = in_frame->pkt_dts;
		//有时pkt_pts和pkt_dts不同，并且pkt_pts是编码前的dts,这里要给avframe传入pkt_dts而不能用pkt_pts
		//out_frame->pts = out_frame->pkt_pts;
		out_frame->pts = in_frame->pkt_dts;
	}
	return 0;
}

void CTranscode::uinit_pcm_resample(AVFrame * poutframe,SwrContext * swr_ctx)
{
	if (poutframe)
	{
		av_frame_free(&poutframe);
	}
	if (swr_ctx)
	{
		swr_free(&swr_ctx);
		swr_ctx = NULL;
	}
	//析构pcm分包结构
	if(m_fifo)
	{
		av_fifo_free(m_fifo);
		m_fifo = NULL;
	}
}

int CTranscode::audio_support(AVCodec * pCodec,int *channel,int * playout,int *samplePerSec,AVSampleFormat_t *sample_fmt)
{
	//支持的声道
	if(NULL != pCodec->channel_layouts)
	{
		uint64_t layout = av_get_default_channel_layout(*channel);
		if(0 == layout)
		{
			return 0;
		}

		int i = 0;	
		int j = 0;
		while(0 != pCodec->channel_layouts[j])
		{
			printf("pCodec->channel_layouts[j] : %d\n",pCodec->channel_layouts[j]);
			++j;
		}
		while(0 != pCodec->channel_layouts[i])
		{
			if(layout == pCodec->channel_layouts[i])
			{
				break;
			}
			++i;
		}
		//未找到
		if(0 == pCodec->channel_layouts[i])
		{
			*playout = pCodec->channel_layouts[i-1];
			*channel = av_get_channel_layout_nb_channels(*playout);
		}
	}

	//支持的采样率
	if(NULL != pCodec->supported_samplerates)
	{
		int i = 0;
		int j = 0;
		while(0 != pCodec->supported_samplerates[j])
		{
			printf("pCodec->supported_samplerates[j] : %d\n",pCodec->supported_samplerates[j]);
			++j;
		}
		while(0 != pCodec->supported_samplerates[i])
		{
			if(*samplePerSec == pCodec->supported_samplerates[i])
			{
				break;
			}
			++i;
		}
		//未找到
		if(0 == pCodec->supported_samplerates[i])
		{
			*samplePerSec = pCodec->supported_samplerates[i-1];
		}
	}

	//支持的样本
	if(NULL != pCodec->sample_fmts)
	{
		int i = 0;
		int j = 0;
		while(-1 != pCodec->sample_fmts[j])
		{
			printf("pCodec->sample_fmts[j] : %d\n",pCodec->sample_fmts[j]);
			++j;
		}
		while(-1 != pCodec->sample_fmts[i])
		{
			if(*sample_fmt == pCodec->sample_fmts[i])
			{
				break;
			}
			++i;
		}
		//未找到
		if(-1 == pCodec->sample_fmts[i])
		{
			*sample_fmt = (AVSampleFormat_t)pCodec->sample_fmts[i-1];
		}
	}
	return 1;
}

int CTranscode::video_support(AVCodec * pCodec,AVPixelFormat * video_pixelfromat)
{
	//支持的yuv格式
	if(NULL != pCodec->pix_fmts)
	{
		int i = 0;
		int j = 0;
		while(0 != pCodec->pix_fmts[j])
		{
			printf("pCodec->pix_fmts[j] : %d\n",pCodec->pix_fmts[j]);
			++j;
		}
		while(0 != pCodec->pix_fmts[i])
		{
			if(*video_pixelfromat == pCodec->pix_fmts[i])
			{
				break;
			}
			++i;
		}
		//未找到
		if(-1 == pCodec->pix_fmts[i])
		{
			*video_pixelfromat = pCodec->pix_fmts[i-1];
		}
	}
	return 1;
}

bool CTranscode::TranscodingEx()
{
	AVFrame *pinframe = NULL;
	AVFrame * pout_video_frame = NULL;
	AVFrame * pout_audio_frame = NULL;
	SwrContext * swr_ctx = NULL;

	int dst_nb_samples = -1;
	int resampled_data_size = 0;

	//分配一个AVFrame并设置默认值
	pinframe = av_frame_alloc(); 
	pout_video_frame = av_frame_alloc();
	pout_audio_frame = av_frame_alloc();

	pinframe->pts = 0;
	pout_video_frame->pts = 0;
	pout_audio_frame->pts = 0;

	if (pinframe == NULL)
	{
		printf("avcodec_alloc_frame pinframe error\n");
		return 0;
	}
	//video
	if (pout_video_frame == NULL)
	{
		printf("avcodec_alloc_frame pout_video_frame error\n");
		return 0;
	}

	int Out_size = avpicture_get_size(m_VideoPixelfromat, m_nVideoWidth,m_nVideoHeight);
	uint8_t * pOutput_buf =( uint8_t *)malloc(Out_size * 3 * sizeof(char)); //最大分配的空间，能满足yuv的各种格式
	avpicture_fill((AVPicture *)pout_video_frame, (unsigned char *)pOutput_buf, m_VideoPixelfromat,m_nVideoWidth, m_nVideoHeight); //内存关联

	//audio
	if (pout_audio_frame == NULL)
	{
		printf("avcodec_alloc_frame pout_audio_frame error\n");
		return 0;
	}

	av_frame_unref(pout_audio_frame); 	
	int nCount = 0;
	//开始解包
	AVPacket *pkt = NULL;
	//av_init_packet(&m_pkt);
	while (!m_threadTrancode.GetStop())
	{
		//av_init_packet(&m_pkt);

		AVPacket *pkt = get_free_pkt();

		if(pkt == NULL)
			break;

		if (m_inAVFormatContext&&av_read_frame(m_inAVFormatContext, pkt) < 0)
		{
			put_free_pkt(pkt);
			break;
		}

		char szBuf[256] = {'\0'};
		sprintf(szBuf,"now trancode ******************** %d\n",nCount);
		//OutputDebugStringA(szBuf);
		//视频
		if(pkt->stream_index == m_nVideoStreamIdx) 
		{
			//如果是视频需要编解码
			if(m_bIsVideoTranscode)
			{
				if (perform_decode(VIDEO_ID,pinframe,pkt) == 0)
				{
					if(m_bIsConversion)
					{
						perform_yuv_conversion(pinframe,pout_video_frame);
						perform_code(VIDEO_ID,pout_video_frame);
					}
					else
					{
						pinframe->pts = pinframe->pkt_pts;
						perform_code(VIDEO_ID,pinframe);
					}
				}
			}
			else
			{ 
				write_frame(m_outAVFormatContext,VIDEO_ID,pkt);
			}
		}
		
		//音频
		else if (pkt->stream_index == m_nAudioStreamIdx)
		{
			//如果是音频需要编解码
			if(m_bAudioVideoTranscode)
			{
				if (perform_decode(AUDIO_ID,pinframe,pkt) == 0)
				{
					//如果进和出的的声道，样本，采样率不同,需要重采样
					if(m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_fmt != (AVSampleFormat)m_nBitsPerSample ||
						m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channels != m_nChannelCount ||
						m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate != m_nFrequency)
					{
						if (swr_ctx == NULL)
						{
							swr_ctx = init_pcm_resample(pinframe,pout_audio_frame);
						}
						preform_pcm_resample(swr_ctx,pinframe,pout_audio_frame);
						perform_code(AUDIO_ID,pout_audio_frame);
					}
					else
					{
						pinframe->pts = pinframe->pkt_pts;
						perform_code(AUDIO_ID,pinframe);
					}
				}
			}
			else
			{
				write_frame(m_outAVFormatContext,AUDIO_ID,pkt);
			}
			
		}

		put_free_pkt(pkt);
		if(((nCount++) % 100 == 0)  && m_pEventCallBack)
		{
			TrancodeFileParam tfp;
			if( nCount < m_nAllFrame)
			{
				tfp.nCurTrancodePos = nCount;
				tfp.nTrancodeLen = m_nAllFrame;
			}
			m_pEventCallBack(TRANCODE_FILE,m_EventpUser,NULL,0,&tfp);
		}
	}
	if (pinframe)
	{
		av_frame_free(&pinframe);
	}
	if (pout_video_frame)
	{
		av_frame_free(&pout_video_frame);
	}
	uinit_pcm_resample(pout_audio_frame,swr_ctx);
	//av_free_packet(&m_pkt);
	m_bIsTrancode = false;

    if(true)
    {
        uinit_mux();
        uinit_demux();
        m_inAVFormatContext = NULL;
        m_outAVFormatContext = NULL;
        m_outVideoST = NULL;
        m_outAudioST = NULL;
    }

	//表示转码结束
	if(m_pEventCallBack)
	{
		TrancodeFileParam tfp;
		tfp.nCurTrancodePos = m_nAllFrame;
		tfp.nTrancodeLen = m_nAllFrame;
		m_pEventCallBack(TRANCODE_FILE,m_EventpUser,NULL,0,&tfp);
	}
	
    if(true)
    {   
        m_bIsTrancode = false;
        m_nAllFrame = 0;
    }

	return 1;
}

bool CTranscode::stopTrancoding()
{
	m_threadTrancode.End(0);
	uinit_mux();
	uinit_demux();
	m_inAVFormatContext = NULL;
	m_outAVFormatContext = NULL;
	m_outVideoST = NULL;
	m_outAudioST = NULL;
	m_bIsTrancode = false;
	m_nAllFrame = 0;
	return true;
}

bool CTranscode::Transcoding(const char* szSourFileName,const char* szDestFileName,char* szErr)
{
	if(m_bIsTrancode)
		return false;

	m_bIsTrancode = true;

	if(szSourFileName == NULL || szDestFileName == NULL)
		return false;
	//strcpy(m_szSourFileName,szSourFileName);
	//strcpy(m_szDestFileName,szDestFileName);
    memcpy(m_szSourFileName,szSourFileName,strlen(szSourFileName));
    m_szSourFileName[strlen(szSourFileName)] = 0;
    memcpy(m_szDestFileName,szDestFileName,strlen(szDestFileName));
    m_szDestFileName[strlen(szDestFileName)] = 0;

	if( !init_demux(szErr) )
	{
		uinit_demux();
		return false;
	}

	if(!init_mux(szErr))
	{
		return false;
	}
	m_threadTrancode.Begin(_trancde_loop,this);
	return true;
}

bool CTranscode::init_demux(char *szErr)
{

	char szErrMsg[260] = {'\0'};
	int nRet = avformat_open_input(&m_inAVFormatContext, (const char*)m_szSourFileName,NULL, NULL);
	if (nRet < 0)
	{
		av_strerror(nRet, szErrMsg, 256);
		printf(szErrMsg);
		sprintf(szErr,"%s 打开失败!",m_szSourFileName);
		return false;
	}
	if (avformat_find_stream_info(m_inAVFormatContext,NULL) < 0)
	{
		printf("Call av_find_stream_info function failed!\n");
		sprintf(szErr,"%s 打开失败!",m_szSourFileName);
		return false;
	}
	//输出视频信息
	av_dump_format(m_inAVFormatContext, -1, (const char*)m_szSourFileName, 0);

	//添加音频信息到输出context
	for (int i = 0; i < m_inAVFormatContext->nb_streams; i++)
	{
		if (m_inAVFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
		    double FrameRate = m_inAVFormatContext->streams[i]->r_frame_rate.num /(double)m_inAVFormatContext->streams[i]->r_frame_rate.den;
			m_dbFrameRate =(int)(FrameRate + 0.5); 
			m_nVideoStreamIdx = i;
			if(m_inAVFormatContext->streams[i]->nb_frames == 0)
			{
				m_nAllFrame += 100000;
			}
			else
			{
				m_nAllFrame += m_inAVFormatContext->streams[i]->nb_frames;
			}
		}
		else if (m_inAVFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_nAudioStreamIdx = i;
			if(m_inAVFormatContext->streams[i]->nb_frames == 0)
			{
				m_nAllFrame += 100000;
			}
			else
			{
				m_nAllFrame += m_inAVFormatContext->streams[i]->nb_frames;
			}
            if(m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base.den == 0 ||
                m_inAVFormatContext->streams[m_nAudioStreamIdx]->time_base.num == 0 ||
                m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate == 0)
            {
                m_audiomuxtimebasetrue = 0;
            }
		}
	}

	//如果是视频需要编解码
	if(m_nVideoStreamIdx >= 0)
	{
		if(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate <= 0)
		{
			m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate = m_nBitRrate;
		}
		int nduration  = m_inAVFormatContext->duration / AV_TIME_BASE;
		if(/*2000000 < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate ||*/
			1920 < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width ||
			1080 < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height ||
			3600 < nduration)
		{
			sprintf(szErr,"%s 文件太大，目前不支持转码!",m_szSourFileName);
			return false;
		}

		//如果是视频需要编解码
		if(400000 < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate ||
			m_nVideoCodecID != m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->codec_id)
		{
			double sp  = 0.0;

			int nTempWidth = 0;
			int nTempHeight = 0;
			AVRational base_time  = {m_inAVFormatContext->streams[m_nVideoStreamIdx]->avg_frame_rate.den,m_inAVFormatContext->streams[m_nVideoStreamIdx]->avg_frame_rate.num};
			if(base_time.num != 0)
			{
				sp =  av_q2d(base_time) * 1000.0;
				m_dbFrameRate = 1000 / sp ;  
				m_dbFrameRate =(int)(m_dbFrameRate + 0.5); 
			}

			unsigned int nTempBitRate = 0;
			if(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate > m_nBitRrate*2)
			{
				nTempBitRate = (m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate / 8 ) / 1000 * 1000;
			}
			else
			{
				nTempBitRate = (m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->bit_rate / 4 ) / 1000 * 1000;
			}
			
			m_nBitRrate = nTempBitRate  > m_nBitRrate  ? m_nBitRrate : nTempBitRate;
			m_nBitRrate = m_nBitRrate > 200000 ? m_nBitRrate : 200000;

			m_nGopSize = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->gop_size > m_dbFrameRate ? m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->gop_size : m_dbFrameRate;
			m_nMaxBframe = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->max_b_frames;
		
			if (m_nVideoWidth < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width ||
				m_nVideoHeight < m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height)
			{
				if(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width < m_nVideoWidth)
				{
					nTempWidth = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width;
				}
				else
				{
					nTempWidth = m_nVideoWidth;
				}
				if(m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height < m_nVideoHeight)
				{
					nTempHeight = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height;
				}
				else
				{
					nTempHeight = m_nVideoHeight;
				}
				if(nTempHeight != 0)
				{
					int nTranRate = (((nTempWidth * 1.0 ) / nTempHeight) * 1000);
					int nSurRate = (((m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width * 1.0 ) / m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height) * 1000);
					if(nTranRate != nSurRate)
					{
						nTempHeight = ((nTempWidth / ((nSurRate * 1.0) / 1000))) ;
						if(nTempHeight %2 != 0)
						{
							nTempHeight +=1;
						}
					}
				}
				m_nVideoWidth = nTempWidth;
				m_nVideoHeight = nTempHeight;
				m_bIsConversion = true; //视频需要格式转换缩放
			}
			else
			{
				m_nVideoWidth = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->width;
				m_nVideoHeight = m_inAVFormatContext->streams[m_nVideoStreamIdx]->codec->height;
			}
			m_bIsVideoTranscode = true; //视频需要编解码
		}
	}

	if(m_nAudioStreamIdx >= 0)
	{
		m_nChannelCount = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->channels;      //声道
		m_nBitsPerSample = (AVSampleFormat_t)m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_fmt;   //样本
		m_nFrequency = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_rate;      //采样率

		//如果是音频需要编解码
		if((m_outAudioCodecID != m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			/*&&(AV_CODEC_ID_MP3 != m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			&&(AV_CODEC_ID_MP2 != m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			&&(AV_CODEC_ID_MP1 != m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_WMAV1 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			/*||(AV_CODEC_ID_WMAV2 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			/*||(AV_CODEC_ID_AC3 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_FLAC == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_APE == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_RA_144 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_RA_288 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_DTS == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_VORBIS == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_DVAUDIO == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MACE3 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MACE6 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MP3ADU == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MP3ON4 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_ALAC == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_SPEEX == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MP1 == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_MP4ALS == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_AAC_LATM == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_PJS == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)
			||(AV_CODEC_ID_SSA == m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id)*/
			||(AV_SAMPLE_FMT_S16 != m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->sample_fmt))
		{
			if (strstr(m_inAVFormatContext->iformat->name,"mp3") != NULL)
			{
				m_outAudioCodecID = m_inAVFormatContext->streams[m_nAudioStreamIdx]->codec->codec_id;
			}
			else
			{
				m_nBitsPerSample = AV_SAMPLE_FMT_S16_t;
				m_bAudioVideoTranscode = true;//音频需要编解码
			}
		}
	}
	return true;
}

bool CTranscode::init_mux(char * szErr)
{
	int i = 0;
	int nRe = avformat_alloc_output_context2(&m_outAVFormatContext, NULL, NULL, m_szDestFileName);
	if (nRe < 0) 
	{
		sprintf(szErr,"创建文件 %s 失败!",m_szDestFileName);
		return false;

	}

	AVOutputFormat* ofmt = NULL;
	ofmt = m_outAVFormatContext->oformat;
	
	if (!(ofmt->flags & AVFMT_NOFILE))
	{
		if (avio_open(&m_outAVFormatContext->pb, m_szDestFileName, AVIO_FLAG_WRITE) < 0)
		{
			sprintf(szErr,"创建文件 %s 失败!",m_szDestFileName);
			return false;
		}
	}

	//添加音频信息到输出context
	if(m_nAudioStreamIdx != -1)
	{
		ofmt->audio_codec = m_outAudioCodecID;
		//如果是音频需要编解码
		if(m_bAudioVideoTranscode)
		{
			m_outAudioST = add_out_stream2(m_outAVFormatContext, AVMEDIA_TYPE_AUDIO,&m_audio_codec);
		}
		else
		{
			m_outAudioST = add_out_stream(m_outAVFormatContext, AVMEDIA_TYPE_AUDIO);
			
		}

		if(m_outAudioST == NULL)
		{
			return false;
		}
		if ((strstr(m_outAVFormatContext->oformat->name, "flv") != NULL) || 
			(strstr(m_outAVFormatContext->oformat->name, "mp4") != NULL) || 
			(strstr(m_outAVFormatContext->oformat->name, "mov") != NULL) ||
			(strstr(m_outAVFormatContext->oformat->name, "3gp") != NULL))    
		{
			if (m_outAudioST->codec->codec_id == AV_CODEC_ID_AAC) 
			{
				m_vbsf_aac_adtstoasc =  av_bitstream_filter_init("aac_adtstoasc");  
				if(m_vbsf_aac_adtstoasc == NULL)  
				{  
					return false;  
				} 
			}
		}

	}
	//添加视频信息到输出context
	if (m_nVideoStreamIdx != -1)
	{
		ofmt->video_codec = m_nVideoCodecID;
		//如果是视频需要编解码
		if(m_bIsVideoTranscode)
		{
			m_outVideoST = add_out_stream2(m_outAVFormatContext, AVMEDIA_TYPE_VIDEO,&m_video_codec);
		}
		else
		{
			m_outVideoST = add_out_stream(m_outAVFormatContext,AVMEDIA_TYPE_VIDEO);
		}
	}

	av_dump_format(m_outAVFormatContext, 0, m_szDestFileName, 1);

	//如果是视频需要编解码
	if(m_bIsVideoTranscode && m_nVideoStreamIdx != -1)
	{
		//解码初始化
		if(!init_decode(VIDEO_ID)||!init_code(VIDEO_ID))
		{
			strcpy(szErr,m_szErrMsg);
			return false;
		}
	}
	//如果是音频需要编解码
	if(m_bAudioVideoTranscode && m_nAudioStreamIdx != -1)
	{
		//解码初始化
		if(!init_decode(AUDIO_ID) || !init_code(AUDIO_ID))
		{
			strcpy(szErr,m_szErrMsg);
			return false;
		}
	}

	int ret = avformat_write_header(m_outAVFormatContext, NULL);
	if(ret != 0)
	{
		printf("Call avformat_write_header function failed.\n");
		return false;
	}
	return true;
}


bool CTranscode::SetEventCallback(scEventCallBack pEC,void* dwUser)
{
	m_pEventCallBack = pEC;
	m_EventpUser = dwUser;
	return true;
}

AVPacket* CTranscode::get_free_pkt()
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

void  CTranscode::put_free_pkt(AVPacket* pNode)
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