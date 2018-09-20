#ifndef MEDIADECODE_H
#define MEDIADECODE_H

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/time.h>
	#include<libswresample/swresample.h>
	#include <libswscale/swscale.h>
}

#define AVCODEC_MAX_AUDIO_FRAME_SIZE (19200)

struct VideoFrame
{
	unsigned char* pVideoFrameBuf;
	unsigned int   nVideoFrameSize;
	unsigned int   nVideoFrameWidth;
	unsigned int   nVideoFrameHeight;
	int64_t        mVideoFramePts;
	double         fltOneFrameDuration;
	VideoFrame()
	{
		pVideoFrameBuf = NULL;
		nVideoFrameSize = 0;
		nVideoFrameWidth = 0;
		nVideoFrameHeight = 0;
		mVideoFramePts = 0;
		fltOneFrameDuration = 0.0;
	}
};

struct AudioFrame
{
	unsigned char* pPCMBuffer;
	unsigned int   nPCMBufSize;
	int64_t   mVideoFramePts;
	int       nSamplerate;
	int       nChannel;
	AudioFrame()
	{
		pPCMBuffer = NULL;
		nPCMBufSize = 0;
		mVideoFramePts = 0;
		nSamplerate = 0;
		nChannel = 0;
	}
};

class CMediaDecoder
{
public:
	CMediaDecoder();
	~CMediaDecoder();
public:
	bool DecodeH264Video(const unsigned char* pBuf,unsigned int nSize,int64_t pts,VideoFrame& vf);
	bool DecodeAACAudio(const unsigned char* pBuf,unsigned int nSize,int64_t pts,AudioFrame& af);
	inline bool getVideoWHReverseFlag() { return m_nInputWidth < m_nInputHeight;}
private:
	bool InitH264Decode();
	bool InitAACDecode(int nSampleRate,int channels);
	bool UninitH264Decode();
	bool UninitAACDecode();
private:
	AVCodec *       m_pVideoCodec;
	AVCodecContext *m_pVideoCodecContext;
	AVFrame *       m_pOutVideoFrame;
	int				m_nInputWidth;
	int				m_nInputHeight;
	unsigned char*  m_yuvbuf;
	unsigned int    m_yuvbufsize;
private:
	AVCodec *       m_pAudioCodec;
	AVCodecContext *m_pAudioCodecContext;
	AVFrame *       m_pOutAudioFrame;
	AVPacket*		m_pAudioPacket;

	unsigned char*  m_pAudioBuf;
	unsigned int    m_nAudioBufSize;
	
	unsigned int    m_WritePCMDataPos;

	SwrContext *     m_au_convert_ctx;
	int              m_nSampleRate;
	int              m_nChannels;

	bool			 m_decodeAudio;
private:
	bool            m_bVideoDecodeInitOK;
	bool            m_bAudioDecodeInitOK;
};

#endif