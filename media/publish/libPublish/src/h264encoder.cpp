#include <stdafx.h>
#include "h264encoder.h"

#define BASE_TIME 1000000

CH264Encoder::CH264Encoder()
{
	m_iDesVideoWidth  = 0;
	m_iDesVideoHeight = 0;
	
	m_szNalBuffer   = NULL;
	m_p264Nal       = NULL;
	m_p264Param     = NULL;
	m_p264Pic       = NULL;
	m_p264Handle    = NULL;

	//m_ff_avcodec = FFMPEGAVCodec::GetInterface();
	m_sws_context = NULL;
	m_sztModulePath[0] = '\0';;
	GetModuleFileNameA(NULL, m_sztModulePath, MAX_PATH);
	PathRemoveFileSpecA(m_sztModulePath);

}


CH264Encoder::CH264Encoder(int nVideoW,int nVideoH,int nBit,int nFps)
{
	
	m_iDesVideoWidth  = 0;
	m_iDesVideoHeight = 0;
	
	m_szNalBuffer   = NULL;
	m_p264Nal       = NULL;
	m_p264Param     = NULL;
	m_p264Pic       = NULL;
	m_p264Handle    = NULL;

	m_iDesVideoWidth = nVideoW;
	m_iDesVideoHeight = nVideoH;
	m_nBit = nBit;
	m_nFps = nFps;

	//m_ff_avcodec = FFMPEGAVCodec::GetInterface();
	m_sws_context = NULL;

}

CH264Encoder::~CH264Encoder()
{
	h264_EncodeUninit();
}

void CH264Encoder::h264_SetParam(int nVideoW,int nVideoH,int nBit,int nFps)
{
	m_iDesVideoWidth = nVideoW;
	m_iDesVideoHeight = nVideoH;
	m_nBit = nBit;
	m_nFps = nFps;

}

void CH264Encoder::h264_ReConfig(int nVideoW,int nVideoH,int nBit,int nFps)
{
	if(m_p264Param == NULL || m_p264Handle == NULL)
		return ;
	bool bIsChang = false;
	if(m_iDesVideoWidth != nVideoW || m_iDesVideoHeight != nVideoW)
	{
		m_iDesVideoWidth = nVideoW;
		m_iDesVideoHeight = nVideoH;
		m_p264Param->i_width   = m_iDesVideoWidth;   //set frame width   
		m_p264Param->i_height  = m_iDesVideoHeight;  //set frame height  
		bIsChang = true;
	}
	if(m_nBit != nBit)
	{
		m_nBit = nBit;
		m_p264Param->rc.i_bitrate  = m_nBit;  
		m_p264Param->rc.i_vbv_max_bitrate = m_nBit * 1.2;
		bIsChang = true;
	}

	if(m_nFps != nFps)
	{
		m_nFps = nFps;
		int nFpsNum = m_nFps;
		int nFpsDen = 1;
		if(m_nFps < 100)
		{
			nFpsNum = BASE_TIME * m_nFps;
			nFpsDen = nFpsDen * BASE_TIME;
		}
		m_p264Param->i_fps_num     = nFpsNum;  
		m_p264Param->i_fps_den     = nFpsDen;
		m_p264Param->i_keyint_max = m_nFps;  
		bIsChang = true;
	}
	if(bIsChang)
	{
		x264_encoder_reconfig(m_p264Handle,m_p264Param);
	}
}

bool CH264Encoder::h264_EncodeInit(int nSrcW,int nSrcH)
{
	m_szNalBuffer = new unsigned char[DATA_SIZE]; 
	memset(m_szNalBuffer,0,DATA_SIZE);  
	 
	 
	m_p264Param   = new x264_param_t();  
	m_p264Pic     = new x264_picture_t();  
	memset(m_p264Pic,0,sizeof(x264_picture_t)); 

	x264_param_default(m_p264Param);  //set default param   
	//x264_param_default_preset(m_p264Param,"veryfast", "zerolatency"); 
	int err = x264_param_parse(m_p264Param,"preset","veryfast");
	m_p264Param->i_threads = 1;  
	m_p264Param->i_width   = m_iDesVideoWidth;   //set frame width   
	m_p264Param->i_height  = m_iDesVideoHeight;  //set frame height   
	/*baseline level 1.1*/  
	m_p264Param->b_cabac  = 0;   
	m_p264Param->i_bframe = 0;  
	m_p264Param->b_interlaced  = 0;  
	m_p264Param->rc.i_rc_method= X264_RC_ABR;//X264_RC_CQP ;
	//m_p264Param->i_level_idc   = 21;  
	m_p264Param->rc.i_bitrate  = m_nBit;  
	m_p264Param->rc.i_vbv_max_bitrate = m_nBit * 1.2;
	
	// optimized h.264 paras
    // Author: Ray Zhang 20150309
	// Infor: level-6
	m_p264Param->b_deblocking_filter = 1;
	m_p264Param->b_cabac = 1;
	m_p264Param->analyse.intra = X264_ANALYSE_I4x4;
	m_p264Param->analyse.inter = (X264_ANALYSE_PSUB8x8|X264_ANALYSE_I4x4);
	m_p264Param->analyse.i_me_method = ME_UMH;
	m_p264Param->analyse.i_me_range = 20;
	m_p264Param->analyse.i_subpel_refine = 2;
	m_p264Param->i_frame_reference = 1;
	m_p264Param->analyse.i_trellis = 0;
	//m_p264Param->
	m_p264Param->analyse.b_fast_pskip = 1;
	m_p264Param->analyse.b_dct_decimate = 0;
	
	
	int nFpsNum = m_nFps;
	int nFpsDen = 1;
	if(m_nFps < 100)
	{
		nFpsNum = BASE_TIME * m_nFps;
		nFpsDen = nFpsDen * BASE_TIME;
	}
	m_p264Param->i_fps_num     = nFpsNum;  
	m_p264Param->i_fps_den     = nFpsDen;

	m_p264Param->i_keyint_max = m_nFps;  

	if((m_p264Handle = x264_encoder_open(m_p264Param)) == NULL)  
	{  
		AfxMessageBox(L"x264_encoder_open failed£¡");
		return false;  
	} 
	x264_picture_alloc(m_p264Pic, X264_CSP_I420, m_p264Param->i_width, m_p264Param->i_height);  
	m_p264Pic->i_type = X264_TYPE_AUTO;  
	
	m_sws_context = def_ff_sws_getContext(nSrcW, nSrcH, AV_PIX_FMT_BGR24, m_iDesVideoWidth, m_iDesVideoHeight, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

	return true;
}

bool CH264Encoder::h264_EncodeUninit()
{
	if(m_sws_context)
	{
	//	def_ff_sws_freeContext(m_sws_context);
		m_sws_context = NULL;
	}

	if(m_p264Handle)
	{
		x264_encoder_close(m_p264Handle);
		m_p264Handle = NULL;
	}
	
	if(m_szNalBuffer)
	{
		delete [] m_szNalBuffer;
		m_szNalBuffer = NULL;
	}

	if(m_p264Param)
	{
		delete m_p264Param;
		m_p264Param = NULL;
	}

	if(m_p264Pic)
	{
		x264_picture_clean(m_p264Pic);
		delete m_p264Pic;
		m_p264Pic = NULL;
	}
	return true;
}

bool CH264Encoder::h264_copyHeaderParam(unsigned char* pBuf,int &nSize)
{
	if(pBuf == NULL || m_p264Param == NULL)
		return false;

	char* szTmp = (char*)pBuf;

	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string(szTmp, "@setDataFrame" );  
	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string(szTmp, "onMetaData" );  
	szTmp=put_byte(szTmp, AMF_OBJECT );   
	szTmp=put_amf_string( szTmp, "author" );  
	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string( szTmp, "GT" );  
	szTmp=put_amf_string( szTmp, "copyright" );  
	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string( szTmp, "PWRD" );  
	szTmp=put_amf_string( szTmp, "presetname" );  
	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string( szTmp, "Custom" ); 
	szTmp=put_amf_string( szTmp, "width" );  
	szTmp=put_amf_double( szTmp, m_p264Param->i_width );  
	szTmp=put_amf_string( szTmp, "height" );  
	szTmp=put_amf_double( szTmp, m_p264Param->i_height );  
	szTmp=put_amf_string( szTmp, "framerate" );  
	szTmp=put_amf_double( szTmp, (double)m_p264Param->i_fps_num / m_p264Param->i_fps_den );  
	szTmp=put_amf_string( szTmp, "videocodecid" );  
	szTmp=put_byte(szTmp, AMF_STRING );  
	szTmp=put_amf_string( szTmp, "avc1" );  
	szTmp=put_amf_string( szTmp, "videodatarate" );  
	szTmp=put_amf_double( szTmp, m_p264Param->rc.i_bitrate );   
	szTmp=put_amf_string( szTmp, "avclevel" );  
	szTmp=put_amf_double( szTmp, m_p264Param->i_level_idc );   
	szTmp=put_amf_string( szTmp, "avcprofile" );  
	szTmp=put_amf_double( szTmp, 0x42 );   
	szTmp=put_amf_string( szTmp, "videokeyframe_frequency" );  
	szTmp=put_amf_double( szTmp, 3 );
	nSize = szTmp - (char*)pBuf;
	return true;
}
bool CH264Encoder::h264_CopyPPSandSPS(unsigned char* pBuf,int &nSize)
{
	bs_t bs={0}; 

	if(pBuf == NULL)
		return false;
	char * szTemp = (char*)pBuf;
	short slen=0;  
	bs_init(&bs,m_szNalBuffer,16);  
	
	if(m_p264Handle->sps->vui.i_num_units_in_tick < 100 && m_p264Handle->sps->vui.i_time_scale < 100)
	{
		m_p264Handle->sps->vui.i_num_units_in_tick = m_p264Handle->sps->vui.i_num_units_in_tick * 1000000;
		m_p264Handle->sps->vui.i_time_scale = m_p264Handle->sps->vui.i_time_scale * 1000000;
	}
	x264_sps_write(&bs, m_p264Handle->sps);//¶ÁÈ¡±àÂëÆ÷µÄSPS   
	//int nsize =  sizeof(x264_sps_t);
	slen=bs.p-bs.p_start+1;//spslen£¨short£©   
	slen=htons(slen);  
	memcpy(szTemp,&slen,sizeof(short));  
	szTemp+=sizeof(short);  
	*szTemp=0x67;  
	szTemp+=1;  
	memcpy(szTemp,bs.p_start,bs.p-bs.p_start);  
//	*(szTemp+11) = 0x03;
	szTemp+=bs.p-bs.p_start;  
	*szTemp=0x01;  
	szTemp+=1;  
	bs_init(&bs,m_szNalBuffer,DATA_SIZE);
	x264_pps_write(&bs, m_p264Handle->pps); 
	slen=bs.p-bs.p_start+1;  
	slen=htons(slen);  
	memcpy(szTemp,&slen,sizeof(short));  
	szTemp+=sizeof(short);  
	*szTemp=0x68;  
	szTemp+=1;  
	memcpy(szTemp,bs.p_start,bs.p-bs.p_start);  
	szTemp+=bs.p-bs.p_start; 
	nSize = szTemp - (char*)pBuf;
	return true;
}

bool CH264Encoder::h264_Encode(unsigned char* pDestBuf,unsigned int& nDestBufSize,unsigned char* pSRGBBuf,unsigned char * pDesBuf ,int nSrcW,int nSrcH)
{
	
	if(pDestBuf == NULL || pSRGBBuf == NULL || pDesBuf == NULL)
		return false;

	unsigned  char * pNal = m_szNalBuffer;  
	if(m_sws_context)
	{
		//·­×ªbuffer
	   for(int k=0;k<nSrcH;k++)  
	   {
			memcpy(pSRGBBuf+(nSrcW*3)*(nSrcH-k-1),pDesBuf+(nSrcW*3)*k,nSrcW*3); 
		}

		int nRGBWidthStep = nSrcW * nSrcH;
		int nDstW = m_iDesVideoWidth;   
		int nDstH = m_iDesVideoHeight; 
		uint8_t *pSrcBuff[3] = {pSRGBBuf,pSRGBBuf + nRGBWidthStep,pSRGBBuf+ nRGBWidthStep * 2};
		uint8_t *pDstBuff[3] = {m_p264Pic->img.plane[0],m_p264Pic->img.plane[0]+ nDstW * nDstH,m_p264Pic->img.plane[0]+nDstW * nDstH * 5 / 4};

		int nSrcStride[3];
		int nDstStride[3];

		for (int i=0; i<3; i++)
		{
			nSrcStride[i] = nSrcW * 3 ;
		}

		nDstStride[0] = nDstW;
		nDstStride[1] = nDstW / 2;
		nDstStride[2] = nDstW / 2;
		try
		{
			def_ff_sws_scale(m_sws_context, pSrcBuff,nSrcStride, 0, nSrcH,pDstBuff, nDstStride);;
		}
		catch(...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if(x264_encoder_encode(m_p264Handle, &m_p264Nal, &m_i264Nal, m_p264Pic ,&m_outPic) < 0)  
	{   
		return false;
	}

	for( int i = 0; i < m_i264Nal; i++ )  
	{  
			int i_size = 0;
		int i_data = DATA_SIZE;  
		if( ( i_size = x264_nal_encode(pNal, &i_data, 1, &m_p264Nal[i]) ) > 0 )  
		{  
			if ((pNal[4]&0x60)==0)  
			{  
				continue;  
			}  
			if (pNal[4]==0x67)  
			{  
				i_size = 0;
				continue;  
			}  
			if (pNal[4]==0x68)  
			{  
				i_size = 0;
				continue;  
			}  
			memmove(pNal,pNal+4,i_size-4);  
			pNal+=i_size-4;  
		}  
	}

	unsigned int nSize=pNal-m_szNalBuffer;  

	if(nDestBufSize < nSize+9)
	{
		nDestBufSize = 0;
		return false;
	}

	if (m_i264Nal>1)  
	{  
		pDestBuf[0]=0x17;  
	}  
	else  
	{  
		pDestBuf[0]=0x27;  
	}  

	nDestBufSize =nSize + 9;  
	put_be32((char *)pDestBuf+5,nSize);  

	if(nSize > 0)
		memcpy(pDestBuf+9,m_szNalBuffer,nSize);
	return true;
}


bool CH264Encoder::h264_Encode(unsigned char* pDestBuf,unsigned int& nDestBufSize,unsigned char * pDesBuf ,int nSrcW,int nSrcH)
{
	
	if(pDestBuf == NULL || pDesBuf == NULL)
		return false;

	unsigned  char * pNal = m_szNalBuffer;  
	if(m_sws_context)
	{
		int nRGBWidthStep = nSrcW * nSrcH;
		int nDstW = m_iDesVideoWidth;   
		int nDstH = m_iDesVideoHeight; 
		uint8_t *pSrcBuff[3] = {pDesBuf,pDesBuf + nRGBWidthStep,pDesBuf+ nRGBWidthStep * 2};
		uint8_t *pDstBuff[3] = {m_p264Pic->img.plane[0],m_p264Pic->img.plane[0]+ nDstW * nDstH,m_p264Pic->img.plane[0]+nDstW * nDstH * 5 / 4};

		int nSrcStride[3];
		int nDstStride[3];

		for (int i=0; i<3; i++)
		{
			nSrcStride[i] = nSrcW * 3 ;
		}

		nDstStride[0] = nDstW;
		nDstStride[1] = nDstW / 2;
		nDstStride[2] = nDstW / 2;
		try
		{
			def_ff_sws_scale(m_sws_context, pSrcBuff,nSrcStride, 0, nSrcH,pDstBuff, nDstStride);;
		}
		catch(...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if(x264_encoder_encode(m_p264Handle, &m_p264Nal, &m_i264Nal, m_p264Pic ,&m_outPic) < 0)  
	{   
		return false;
	}

	for( int i = 0; i < m_i264Nal; i++ )  
	{  
			int i_size = 0;
		int i_data = DATA_SIZE;  
		if( ( i_size = x264_nal_encode(pNal, &i_data, 1, &m_p264Nal[i]) ) > 0 )  
		{  
			if ((pNal[4]&0x60)==0)  
			{  
				continue;  
			}  
			if (pNal[4]==0x67)  
			{  
				i_size = 0;
				continue;  
			}  
			if (pNal[4]==0x68)  
			{  
				i_size = 0;
				continue;  
			}  
			memmove(pNal,pNal+4,i_size-4);  
			pNal+=i_size-4;  
		}  
	}

	unsigned int nSize=pNal-m_szNalBuffer;  

	if(nDestBufSize < nSize+9)
	{
		nDestBufSize = 0;
		return false;
	}

	if (m_i264Nal>1)  
	{  
		pDestBuf[0]=0x17;  
	}  
	else  
	{  
		pDestBuf[0]=0x27;  
	}  

	nDestBufSize =nSize + 9;  
	put_be32((char *)pDestBuf+5,nSize);  

	if(nSize > 0)
		memcpy(pDestBuf+9,m_szNalBuffer,nSize);
	return true;
}