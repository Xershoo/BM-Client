#include "MediaDecode.h"

CMediaDecoder::CMediaDecoder()
{
	m_pVideoCodec = NULL;
	m_pVideoCodecContext = NULL;
	m_pOutVideoFrame = NULL;
	m_yuvbufsize = 0;
	m_yuvbuf = NULL;
	m_nInputWidth = 0;
	m_nInputHeight = 0;
	m_bVideoDecodeInitOK = false;
	m_bAudioDecodeInitOK = false;

	m_pAudioCodec = NULL;
	m_pAudioCodecContext = NULL;
	m_pOutAudioFrame = NULL;
	m_pAudioPacket = NULL;
	m_nSampleRate = 0;
	m_nChannels = 0;
	m_pAudioBuf = NULL;
	m_nAudioBufSize = 0;
	
	m_WritePCMDataPos = 0;
	m_decodeAudio = false;

	m_au_convert_ctx = NULL;
	av_register_all();
}

CMediaDecoder::~CMediaDecoder()
{
	UninitH264Decode();
	UninitAACDecode();
	if(m_yuvbuf)
	{
		free(m_yuvbuf);
		m_yuvbuf = NULL;
	}
}

bool CMediaDecoder::InitH264Decode()
{
	if(m_bVideoDecodeInitOK)
		return true;

	if(m_pVideoCodec == NULL)
	{
		m_pVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if(m_pVideoCodec == NULL)
		{
			return false;
		}
	}

	if(m_pVideoCodecContext == NULL)
	{
		m_pVideoCodecContext = (AVCodecContext *)avcodec_alloc_context3(m_pVideoCodec);

		if(m_pVideoCodecContext == NULL)
		{
			return false;
		}

		m_pVideoCodecContext->flags |= CODEC_FLAG_EMU_EDGE;
		if(m_pVideoCodec->capabilities&CODEC_CAP_TRUNCATED)  
		{
			m_pVideoCodecContext->flags|= CODEC_FLAG_TRUNCATED; 
		}
	}

	if(avcodec_open2(m_pVideoCodecContext, m_pVideoCodec,NULL) < 0)
	{
		return false;
	}

	if(m_pOutVideoFrame == NULL)
	{
		m_pOutVideoFrame = av_frame_alloc();
	}

	m_bVideoDecodeInitOK = true;
	return true;
}

bool CMediaDecoder::UninitH264Decode()
{
	if(m_pVideoCodecContext)
	{
		avcodec_close(m_pVideoCodecContext);
		avcodec_free_context(&m_pVideoCodecContext);
		m_pVideoCodecContext = NULL;
		m_pVideoCodec = NULL;
	}

	if(m_pOutVideoFrame)
	{
		av_frame_free(&m_pOutVideoFrame);
		m_pOutVideoFrame = NULL;
	}
	
	m_bVideoDecodeInitOK = false;
	return true;
}

bool CMediaDecoder::InitAACDecode(int nSampleRate,int channels)
{
	if(m_bAudioDecodeInitOK)
	{
		return true;
	}

	if(m_pAudioCodec == NULL)
	{
		m_pAudioCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
		if(m_pAudioCodec == NULL)
		{
			return false;
		}
	}

	if(m_pAudioCodecContext == NULL)
	{
		m_pAudioCodecContext = (AVCodecContext *)avcodec_alloc_context3(m_pAudioCodec);

		if(m_pAudioCodecContext == NULL)
		{
			return false;
		}
		m_pAudioCodecContext->sample_rate = nSampleRate;
		m_pAudioCodecContext->channels = channels;
	}

	if(avcodec_open2(m_pAudioCodecContext, m_pAudioCodec,NULL) < 0)
	{
		return false;
	}
	
	if(m_pOutAudioFrame == NULL)
	{
		m_pOutAudioFrame = av_frame_alloc();
	}

	if(NULL == m_pAudioPacket)
	{
		m_pAudioPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
		memset(m_pAudioPacket,NULL,sizeof(AVPacket));
		av_init_packet(m_pAudioPacket);
	}
	
	m_nSampleRate = nSampleRate;
	m_nChannels = channels;
	m_bAudioDecodeInitOK = true;
	return true;
}



bool CMediaDecoder::UninitAACDecode()
{
	if(m_pAudioCodecContext)
	{
		avcodec_close(m_pAudioCodecContext);
		avcodec_free_context(&m_pAudioCodecContext);
		m_pAudioCodecContext = NULL;
		m_pAudioCodec = NULL;
	}
	if(m_pOutAudioFrame)
	{
		av_frame_free(&m_pOutAudioFrame);
		m_pOutAudioFrame = NULL;
	}

	if(m_pAudioPacket)
	{
		av_free(m_pAudioPacket);		
		m_pAudioPacket = NULL;
	}

	if(NULL != m_pAudioBuf)
	{
		free(m_pAudioBuf);
		m_pAudioBuf = 0;
	}

	m_nAudioBufSize = 0;

	m_bAudioDecodeInitOK = false;
	return true;
}

bool CMediaDecoder::DecodeH264Video(const unsigned char* pBuf,unsigned int nSize,int64_t pts,VideoFrame& vf)
{
	if(!m_bVideoDecodeInitOK && !InitH264Decode())
	{
		return false;
	}

	if(pBuf == NULL || nSize <= 4)
	{
		return false;
	}

	AVPacket avpkt;
	int nUsedLen = 0;
	int nGotPicture = 0;
	av_init_packet(&avpkt);

	int pktSize = nSize + (64 - nSize % 32)+ 64;
	unsigned char * pktBuf = (unsigned char*)malloc(pktSize);
	if(NULL == pktBuf)
	{
		return false;
	}

	memset(pktBuf,NULL,sizeof(unsigned char) * pktSize);
	memcpy(pktBuf,pBuf,nSize);

	avpkt.size = nSize;
	avpkt.data = (uint8_t*)pktBuf;//(uint8_t*)pBuf;
	avpkt.pts = pts;
	
	nUsedLen = avcodec_decode_video2(m_pVideoCodecContext, m_pOutVideoFrame, &nGotPicture, &avpkt);
		
	if (nGotPicture != 0)
	{
		m_nInputWidth = m_pVideoCodecContext->coded_width > m_pVideoCodecContext->width ?m_pVideoCodecContext->width :  m_pVideoCodecContext->coded_width;
		m_nInputHeight = m_pVideoCodecContext->coded_height > m_pVideoCodecContext->height ? m_pVideoCodecContext->height : m_pVideoCodecContext->coded_height;
		int nSize = m_nInputWidth * m_nInputHeight * 3 / 2;
		if(m_yuvbufsize < nSize)
		{
			m_yuvbufsize = nSize;
			if(m_yuvbuf)
			{
				free(m_yuvbuf);
				m_yuvbuf = NULL;
			}
			m_yuvbuf = (unsigned char*)malloc(m_yuvbufsize);
		}

		for(int i=0, nDataLen=0; i<3; i++)
		{
			int nShift = (i == 0) ? 0 : 1;
			unsigned char *pYUVData = m_pOutVideoFrame->data[i];
			unsigned int  nW = m_pOutVideoFrame->width >> nShift;
			unsigned int  nH = m_pOutVideoFrame->height >> nShift;
			for(int j = 0; j< nH;j++)
			{
				memcpy(m_yuvbuf+nDataLen,m_pOutVideoFrame->data[i] + j * m_pOutVideoFrame->linesize[i], nW); 
				nDataLen += nW;
			}
		}

		vf.mVideoFramePts = m_pOutVideoFrame->pts;
		vf.nVideoFrameHeight = m_nInputHeight;
		vf.nVideoFrameWidth = m_nInputWidth;
		vf.nVideoFrameSize = m_yuvbufsize;
		vf.pVideoFrameBuf = m_yuvbuf;

		free(pktBuf);

		return true;
	}
	else
	{
		int n = 0;
	}

	free(pktBuf);

	return false;	
}

bool CMediaDecoder::DecodeAACAudio(const unsigned char* pBuf,unsigned int nSize,int64_t pts,AudioFrame& af)
{
	if(NULL==pBuf||4>=nSize||m_decodeAudio)
	{
		return false;
	}

	if(m_nSampleRate != af.nSamplerate || m_nChannels != af.nChannel || NULL == m_pAudioCodecContext )
	{
		m_bAudioDecodeInitOK = false;
		UninitAACDecode();
	}

	if(!m_bAudioDecodeInitOK && !InitAACDecode(af.nSamplerate,af.nChannel))
	{	
		return false;
	}
	
	if (NULL==m_pAudioPacket)
	{
		return false;
	}

	m_decodeAudio = true;
	
	int nAllDecodeSize = 0;
	bool ret = true;
	unsigned char* pPCMBuf = af.pPCMBuffer;
	
	av_init_packet(m_pAudioPacket);
		
	m_nAudioBufSize = nSize + (64 - nSize % 32)+ 64;;
	m_pAudioBuf = (unsigned char*)malloc(m_nAudioBufSize);

	if(NULL ==m_pAudioBuf)
	{
		return false;
	}

	memset(m_pAudioBuf,NULL,sizeof(unsigned char) * m_nAudioBufSize);
	memcpy(m_pAudioBuf,pBuf,nSize);

	m_pAudioPacket->size = nSize;
	m_pAudioPacket->data = m_pAudioBuf;
	m_pAudioPacket->pts = pts;
	
	while(m_pAudioPacket->size > 0)
	{	
		bool bDecode = false;
		int nDecodeSize = 0;
		int nGotPcm = nSize;

		do 
		{
			av_frame_unref(m_pOutAudioFrame);
			try
			{	
				nDecodeSize = avcodec_decode_audio4(m_pAudioCodecContext, m_pOutAudioFrame, &nGotPcm, m_pAudioPacket);
			}
			catch (...)
			{
				break;
			}
			
			if(nDecodeSize < 0 )
			{
				m_pAudioPacket->size = 0;
				break;
			}
			
			if(nGotPcm == 0)
			{
				break;
			}

			int sampleBufSize = av_samples_get_buffer_size(
				NULL, 
				m_pAudioCodecContext->channels,
				m_pOutAudioFrame->nb_samples,
				m_pAudioCodecContext->sample_fmt, 1);	
			if(sampleBufSize <= 0)
			{
				break;
			}

			int wanted_nb_samples = m_pOutAudioFrame->nb_samples;
			int64_t dec_channel_layout = m_pOutAudioFrame->channel_layout;
			if(dec_channel_layout || av_frame_get_channels(m_pOutAudioFrame) != av_get_channel_layout_nb_channels(dec_channel_layout))
			{
				dec_channel_layout =  av_get_default_channel_layout(av_frame_get_channels(m_pOutAudioFrame));
			}
				
			if (m_pOutAudioFrame->format != AV_SAMPLE_FMT_S16 || 
				dec_channel_layout !=  m_pAudioCodecContext->channel_layout|| 
				m_pOutAudioFrame->sample_rate!= m_nSampleRate ||
				NULL == m_au_convert_ctx) 
			{
				swr_free(&m_au_convert_ctx);

				m_au_convert_ctx = NULL;
				m_au_convert_ctx = swr_alloc_set_opts(NULL,
					4,
					AV_SAMPLE_FMT_S16 ,
					m_nSampleRate,
					dec_channel_layout, 
					(AVSampleFormat)m_pOutAudioFrame->format, 
					m_pOutAudioFrame->sample_rate,
					0, 
					NULL);

				if (NULL==m_au_convert_ctx || swr_init(m_au_convert_ctx) < 0)
				{					 
					break;
				}				
			}
			
			if(m_pOutAudioFrame->sample_rate <=0)
			{
				memcpy(pPCMBuf+nAllDecodeSize,m_pOutAudioFrame->data[0],nDecodeSize);
				nAllDecodeSize += nDecodeSize;
				bDecode = true;
				break;
			}

			int nb_samples = wanted_nb_samples * m_nSampleRate / m_pOutAudioFrame->sample_rate + 256;
			int samples_size  = av_samples_get_buffer_size(NULL, m_nChannels, nb_samples, AV_SAMPLE_FMT_S16, 0);
			if (samples_size < 0) 
			{					  
				break;
			}

			if (wanted_nb_samples != m_pOutAudioFrame->nb_samples) 
			{
				int sample_delta =  (wanted_nb_samples - m_pOutAudioFrame->nb_samples) * m_nSampleRate / m_pOutAudioFrame->sample_rate;
				int comp_dist = wanted_nb_samples * m_nSampleRate / m_pOutAudioFrame->sample_rate;
				
				if (swr_set_compensation(
					m_au_convert_ctx,
					sample_delta,
					comp_dist) < 0) 
				{						  
					break;
				}
			}

			unsigned char* outBuf = pPCMBuf + nAllDecodeSize;
			const uint8_t **inBuf = (const uint8_t **)m_pOutAudioFrame->extended_data;
			uint8_t **swrOut = &outBuf;

			int cov_num = swr_convert(m_au_convert_ctx, swrOut, nb_samples, inBuf, m_pOutAudioFrame->nb_samples);
			if (cov_num < 0)
			{					  
				break;
			}
			
			if (cov_num == nb_samples) 
			{
				swr_init(m_au_convert_ctx);
			}
			
			unsigned int pcmBufSize = cov_num * m_nChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
			nAllDecodeSize += pcmBufSize;

			bDecode = true;
		} while (false);
		
		if(!bDecode)
		{
			ret = false;
			break;
		}
		
		m_pAudioPacket->size -= nDecodeSize;
		m_pAudioPacket->data += nDecodeSize;
		m_pAudioPacket->dts = m_pAudioPacket->pts = AV_NOPTS_VALUE;
	}
	
	af.mVideoFramePts = pts;
	af.nPCMBufSize = nAllDecodeSize;			
	av_free_packet(m_pAudioPacket);			
	free(m_pAudioBuf);
	m_pAudioBuf = NULL;
	m_decodeAudio = false;
		
	return ret;
}
