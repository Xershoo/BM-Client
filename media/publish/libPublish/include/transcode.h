#ifndef TRANSCODE_H
#define TRANSCODE_H
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/mathematics.h"  
#include "libavutil/time.h"  
#include "libavutil/fifo.h"
#include "stdint.h"
};
#include "StreamHeader.h"
#include "sc_Thread.h"
#include "sc_CSLock.h"

#include <list>
using namespace std;

enum AVSampleFormat_t 
{
	AV_SAMPLE_FMT_NONE_t = -1,
	AV_SAMPLE_FMT_U8_t,          ///< unsigned 8 bits
	AV_SAMPLE_FMT_S16_t,         ///< signed 16 bits
	AV_SAMPLE_FMT_S32_t,         ///< signed 32 bits
	AV_SAMPLE_FMT_FLT_t,         ///< float
	AV_SAMPLE_FMT_DBL_t,         ///< double

	AV_SAMPLE_FMT_U8P_t,         ///< unsigned 8 bits, planar
	AV_SAMPLE_FMT_S16P_t,        ///< signed 16 bits, planar
	AV_SAMPLE_FMT_S32P_t,        ///< signed 32 bits, planar
	AV_SAMPLE_FMT_FLTP_t,        ///< float, planar
	AV_SAMPLE_FMT_DBLP_t,        ///< double, planar

	AV_SAMPLE_FMT_NB_t           ///< Number of sample formats. DO NOT USE if linking dynamically
};

class CTranscode
{
public:
	CTranscode();
	~CTranscode();
public:
	//szSourFileName ת��Դ�ļ�
	//szDestFileName ת��Ŀ���ļ�
	//szErr ������ʾ
	bool Transcoding(const char* szSourFileName,const char* szDestFileName,char* szErr);
	bool TranscodingEx();
	bool stopTrancoding();
	bool SetEventCallback(scEventCallBack pEC,void* dwUser);
private:
	
	bool init_demux(char *szErr);
	bool init_mux(char * szErr);
	bool uinit_mux();
	bool uinit_demux();

	AVStream * add_out_stream(AVFormatContext* output_format_context,AVMediaType codec_type_t);
	AVStream * add_out_stream2(AVFormatContext* output_format_context,AVMediaType codec_type_t,AVCodec **codec); 

	bool init_decode(int stream_type);
	bool init_code(int stream_type);
	bool uinit_decode(int stream_type);
	bool uinit_code(int stream_type);

	int perform_decode(int stream_type,AVFrame * picture,AVPacket* pkt);
	void perform_yuv_conversion(AVFrame * pinframe,AVFrame * poutframe);
	int perform_code(int stream_type,AVFrame * picture);
	void write_frame(AVFormatContext *ocodec,int ID,AVPacket* pkt_t);
	SwrContext * init_pcm_resample(AVFrame *in_frame, AVFrame *out_frame);
	int preform_pcm_resample(SwrContext * pSwrCtx,AVFrame *in_frame, AVFrame *out_frame);
	void uinit_pcm_resample(AVFrame * poutframe,SwrContext * swr_ctx);
	int audio_support(AVCodec * pCodec,int *channel,int * playout,int *samplePerSec,AVSampleFormat_t *sample_fmt);
	int video_support(AVCodec * pCodec,AVPixelFormat * video_pixelfromat);
private:
	AVPacket* get_free_pkt();
	void      put_free_pkt(AVPacket*);
private:
	char m_szSourFileName[256];
	char m_szDestFileName[256];
	char m_szErrMsg[260];

	uint8_t ** m_dst_data;

	CSCThread  m_threadTrancode;
	CMutexLock  m_tranLock;
private:
	AVFormatContext* m_inAVFormatContext;
	int   m_nVideoStreamIdx;
	int   m_nAudioStreamIdx;
	AVFormatContext* m_outAVFormatContext;
	AVBitStreamFilterContext * m_vbsf_aac_adtstoasc;
	AVStream * m_outVideoST;
	AVStream * m_outAudioST;
	AVCodec * m_audio_codec;
	AVCodec * m_video_codec;

	struct SwsContext * m_img_convert_ctx_video ;
	int m_sws_flags; //��ֵ�㷨,˫����
private:
	//video param
	int m_nVideoWidth;
	int m_nVideoHeight;
	double m_dbFrameRate;  //֡��                                                  
	AVCodecID m_nVideoCodecID;
	AVPixelFormat m_VideoPixelfromat;
	int  m_nBitRrate;
	int  m_nGopSize;
	int  m_nMaxBframe;
	int  m_ThreadCount;

	//audio param
	int m_nChannelCount;      //����
	AVSampleFormat_t m_nBitsPerSample ;    //����
	int m_nFrequency ;        //������
	AVCodecID m_outAudioCodecID;
	int m_nAudioFrameSize;
	AVFifoBuffer * m_fifo; //���pcm����
	int64_t m_first_audio_pts; //��һ֡����Ƶpts
	int m_is_first_audio_pts; //�Ƿ��Ѿ���¼��һ֡��Ƶʱ���
	int m_audiomuxtimebasetrue;  //��Ƶ��mux��timebse�Ƿ���ȷ

	bool m_bIsVideoTranscode ;
	bool m_bAudioVideoTranscode;
	bool m_bIsTrancode; 
	bool m_bIsConversion; //�Ƿ���Ƶ��Ҫ��ʽת������
	unsigned int m_nAllFrame;

	list<AVPacket*>   m_pool_free_avpacket_list;
	CMutexLock	      m_pool_free_avpacket_lock;
	
private:
	scEventCallBack			 m_pEventCallBack;
	void *					m_EventpUser;

};

#endif