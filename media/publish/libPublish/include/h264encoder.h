#pragma once
#include "loadffmpeg.h"

#include "datautils.h"

extern "C"   
{   
#include "common/common.h"   
#include "common/cpu.h""   
#include "x264.h"   
#include "encoder/set.h" 
}  

#define DATA_SIZE     512 * 1024
//h264 coding class

#pragma comment(lib, "libx264.lib")


class CH264Encoder
{

public:
	//width,hight,bit framerate
	CH264Encoder();
	CH264Encoder(int nVideoW,int nVideoH,int nBit,int nFps);
	~CH264Encoder();
public:
	void h264_SetParam(int nVideoW,int nVideoH,int nBit,int nFps);
	void h264_ReConfig(int nVideoW,int nVideoH,int nBit,int nFps);
	bool h264_EncodeInit(int nSrcW,int nSrcH);
	bool h264_EncodeUninit();
	bool h264_Encode(unsigned char* pDestBuf,unsigned int& nDestBufSize,unsigned char* pSRGBBuf,unsigned char * pDesBuf,int nSrcW,int nSrcH);
	bool h264_Encode(unsigned char* pDestBuf,unsigned int& nDestBufSize,unsigned char * pDesBuf ,int nSrcW,int nSrcH);
	bool h264_CopyPPSandSPS(unsigned char* pBuf,int &nSize);
	bool h264_copyHeaderParam(unsigned char* pBuf,int &nSize);
	inline int  GetVideoWidth() {return m_iDesVideoWidth;}
	inline int  GetVideoHight() {return m_iDesVideoHeight;}
	inline int  GetVideoBitRate() {if(m_p264Param) {m_p264Param->rc.i_bitrate;} return 0;}
	inline int  GetLevelIdc() {if(m_p264Param) {m_p264Param->i_level_idc;} return 0;}
	inline double  GetVideoFrameRate() {if(m_p264Param) {(double)m_p264Param->i_fps_num / m_p264Param->i_fps_den;} return 0;}

private:

	int            m_iDesVideoWidth;
	int            m_iDesVideoHeight;
	int            m_nBit;
	int            m_nFps;
	unsigned char* m_szNalBuffer;    
	x264_nal_t*    m_p264Nal;  
	int            m_i264Nal;  
	x264_param_t*  m_p264Param;  
	x264_picture_t*m_p264Pic;  
	x264_t*        m_p264Handle; 
	x264_picture_t  m_outPic;

	//FFMPEGAVCodec* m_ff_avcodec;
	SwsContext* m_sws_context;
	char  m_sztModulePath[256];
	bool          m_nIsFirstEncode;

};