#include "stdafx.h"
#include"SourCamera.h"
#include "gdiPlus.h"


//
#if PW_OE_VIDEO_ENHANCE
//#ifdef __cplusplus
extern "C" {
//#endif
#include "YUV2RGB.h"
#include "AVE_VIDEO_FILTER.h"
//#ifdef __cplusplus
}
//#endif

#endif
using namespace Gdiplus;

#define DEFALUT_VIDEO_WIDTH		(640)
#define DEFAULT_VIDEO_HEIGHT	(480)

CSourCamera::CSourCamera(HWND hShowWindow,int nDesVideoWidth,int nDesVideoHeight,string sname):ISourData(SOURCECAMERA,sname)
{
	m_bIsEffective = false;
	m_iSrcVideoWidth  = DEFALUT_VIDEO_WIDTH;
	m_iSrcVideoHeight = DEFAULT_VIDEO_HEIGHT;

	m_nDestVideoWidth = nDesVideoWidth;
	m_nDestVideoHeight = nDesVideoHeight;

	m_nCameraBufferSize = 0;
	m_pCameraBuffer = NULL;
	m_tmpImageBuf = NULL;
	m_showImageBuf = NULL;
	m_pInCameraBufferYUV = NULL;
	m_pOutCameraBufferYUV = NULL;

	
	m_bIsStart = false;
	m_pBmpInfo = NULL;
	m_hShowWindow = hShowWindow;
	m_pBmpInfo = NULL;//BmpInfoÖ¸Õë
	m_bDrawLocRect = false;

	m_pAMCapturer4A3 = new AMCaputer();
	m_pAMCapturer16A9 = new AMCaputer();

	m_pAMCapturer4A3->Init(hShowWindow,nDesVideoWidth,nDesVideoHeight);

	m_pAMCapturer16A9 = NULL;

	m_pAMCapturer = m_pAMCapturer4A3;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*255);

	HANDLE hloc = NULL;
	RGBQUAD *rgbquad = NULL;
	hloc = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	rgbquad = (RGBQUAD *) LocalLock(hloc);
	//³õÊ¼»¯µ÷É«°å
	for(int i=0;i<256;i++)
	{
		rgbquad[i].rgbBlue=i;
		rgbquad[i].rgbGreen=i;
		rgbquad[i].rgbRed=i;
		rgbquad[i].rgbReserved=0;
	}

	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 24;
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	memcpy(m_pBmpInfo->bmiColors, rgbquad, sizeof(RGBQUAD) * 256);
	LocalUnlock(hloc);
	LocalFree(hloc);
	memset(&m_posRect,0,sizeof(RECT));
	m_nSourID = -1;

	for(int i = 0; i< 16 ;i++)
	{
		m_arrCamNode[i] = new CamNode();
	}

	for(int i = 0; i< 64 ;i++)
	{
		m_szUnLawFul[i] = new char[128];
		memset(m_szUnLawFul[i],0,128);
	}

	m_nUnLawfulNum = 0;
	m_nCamNum = 0;
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"e2eSoft VCam");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"6RoomsCamV9");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"9158Capture");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"YY°éÂÂ");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"Luminositi Softcam [VFW]");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"MagicCamera Capture(User Mode)");
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"17GuaGua Cam");//guagauÊÓÆµ
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"Virtual Cam"); //mvboxÐéÄâÉãÏñÍ·
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"Fantasy Capture"); //ÃÎ»ÃÐéÄâÊÓÆµ
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"WebcamMax Capture"); //´óÂóÊÓÆµÌØÐ§WebcamMax
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"IMIShowBox Camera"); //°¬Ã×Ðã±¦(ÐéÄâÉãÏñÍ·Èí¼þ
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"ASUS Virtual Camera"); //»ªË¶ÐéÄâÉãÏñÍ·ASUS Virtual Camera¡¾ÐéÄâÉãÏñÍ·¡¿
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"Èð·çÐéÄâÉãÏñÍ·"); //Èð·çÐéÄâÉãÏñÍ·
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"LoveLiao Magic Camera"); //haomtvÐéÄâÉãÏñÍ·
	strcpy(m_szUnLawFul[m_nUnLawfulNum++],"LingXiuMV"); //ÁìÐãMVÐéÄâÉãÏñÍ·

	GetDevCameraCount();

	m_nBGBuf = NULL;
	m_nBGSize = 0;
	m_nBGW = 0;
	m_nBGH = 0;

	// video analysis
#if PW_OE_VIDEO_ENHANCE
	m_fp = NULL;
	nEhancementMainID = R_FILTER_NONE;
	nEhancementSubID = R_FILTER_NONE;
	bTmpStates = (bool*)malloc(20*sizeof(bool));
	memset(bTmpStates, 0, 20);
	pInVFrame = NULL;
	pOutVFrame = NULL;

	AVE_SpecialEffect_Filter_Video_Init();
#endif
}

CSourCamera::~CSourCamera()
{
	m_bIsStart = false;
	m_pAMCapturer = NULL;

	if(m_pAMCapturer4A3)
	{
		m_pAMCapturer4A3->Release();
		delete m_pAMCapturer4A3;
		m_pAMCapturer4A3 = NULL;
	}

	if(m_pAMCapturer16A9)
	{
		m_pAMCapturer16A9->Release();
		delete m_pAMCapturer16A9;
		m_pAMCapturer16A9 = NULL;
	}

	if(m_pCameraBuffer)
	{
		delete m_pCameraBuffer ;
		m_pCameraBuffer = NULL;
		m_nCameraBufferSize = 0;
	}

	if(m_showImageBuf)
	{
		delete m_showImageBuf ;
		m_showImageBuf = NULL;
	}
	if(m_pBmpInfo)
	{
		free(m_pBmpInfo);
		m_pBmpInfo = NULL;
	}
	for(int i = 0; i< 16 ;i++)
	{
		if(m_arrCamNode[i])
		{
			delete m_arrCamNode[i];
			m_arrCamNode[i] = NULL;
		}
	}

	for(int i = 0; i< 64 ;i++)
	{
		if(m_szUnLawFul[i])
		{
			delete m_szUnLawFul[i];
			m_szUnLawFul[i] = NULL;
		}
	}
	// Video Enhancements
#if PW_OE_VIDEO_ENHANCE	
	if(m_pInCameraBufferYUV)
	{
		delete m_pInCameraBufferYUV;
		m_pInCameraBufferYUV = NULL;
		m_nCameraBufferSize = 0;
	}

	if(m_pOutCameraBufferYUV)
	{
		delete m_pOutCameraBufferYUV;
		m_pOutCameraBufferYUV = NULL;
		m_nCameraBufferSize = 0;
	}

	if(m_tmpImageBuf)
	{
		delete m_tmpImageBuf;
		m_tmpImageBuf = NULL;
	}
	
	// Video Enhancements

	//AVE_SpecialEffect_Filter_Video_Release();

	if(pInVFrame)
	{
		delete pInVFrame;
		pInVFrame = NULL;
	}

	if(pOutVFrame)
	{
		delete pOutVFrame;
		pOutVFrame = NULL;
	}
	  
	if(bTmpStates)
	{
		delete bTmpStates;
		bTmpStates = NULL;
	}

	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
	if(m_nBGBuf)
	{
		delete m_nBGBuf;
		m_nBGBuf = NULL;
	}
#endif
}

bool  CSourCamera:: bIsCameLawful(char* szName)
{
	if(szName == NULL)
		return false;

	for(int i = 0; i< m_nUnLawfulNum;i++)
	{
		if(m_szUnLawFul[i] && strcmp(m_szUnLawFul[i],szName) == 0)
		{
			return false;
		}
	}
	return true;
}
	//»ñÈ¡±¾µØÉãÏñÍ·ÊýÄ¿
long   CSourCamera::GetDevCameraCount(bool bIsReload)
{
	if(m_pAMCapturer == NULL)
	{
		if(m_pAMCapturer4A3 == NULL)
		{
			m_pAMCapturer4A3 = new  AMCaputer();
			m_pAMCapturer4A3->Init(NULL,m_nDestVideoWidth,m_nDestVideoHeight);
		}
		m_pAMCapturer = m_pAMCapturer4A3;
	}

	if(m_pAMCapturer == NULL)
		return 0;

	m_nCamNum = 0;
	USES_CONVERSION;
	int nDevCam = m_pAMCapturer->GetVideoDeviceNum();
	for(int i = 0 ;i < nDevCam && m_nCamNum < 16; i++)
	{
		char szName[256] = {'\0'};
		TCHAR*szDevName = m_pAMCapturer->GetVideoDeviceName(i);
		if(szDevName)
		{
			strcpy((LPSTR)szName,OLE2A(szDevName)); 
			if( bIsCameLawful(szName) && m_arrCamNode[m_nCamNum])
			{
				m_arrCamNode[m_nCamNum]->nIndex = i;
				strcpy(m_arrCamNode[m_nCamNum++]->szCamName,szName);
			}
		}
	}
	return m_nCamNum;
}

//»ñÈ¡ÉãÏñÍ·Ãû³Æ
bool  CSourCamera::GetDevCameraName(long nIndex,char szName[256])
{
	if(m_pAMCapturer)
	{
		if(nIndex >= m_nCamNum)
			return false;

		if(m_arrCamNode[nIndex])
		{
			strcpy((LPSTR)szName,m_arrCamNode[nIndex]->szCamName);
			return true;
		}
	}
	return false;
}


bool CSourCamera::GetBuffer(unsigned char** pBuf,unsigned int& nBufSize)
{
	if(m_bIsEffective && m_pAMCapturer && m_bIsStart)
	{
		long nCameraBufferSize = 0;
		ISampleGrabber * pSG = m_pAMCapturer->GetSampleGrabber();

		if(pSG != NULL && m_pCameraBuffer && m_showImageBuf)
		{
			HRESULT hr = pSG->GetCurrentBuffer(&m_nCameraBufferSize, reinterpret_cast<long*>(m_pCameraBuffer));
			if(hr == S_OK)
			{
#if PW_OE_VIDEO_ENHANCE                   //.................................
				//SwitchMirrorImage(m_pCameraBuffer);

				if(nEhancementMainID > R_FILTER_NONE && nEhancementSubID > R_FILTER_NONE)
				{
					if(nEhancementMainID == R_FILTER_FILTERBEAUTY && nEhancementSubID < 10) // Ä¥Æ¤
					{
						SpecialEffectsProcessing(m_pCameraBuffer, m_pCameraBuffer, nEhancementMainID, nEhancementSubID, true, nParam);
					}
					else
					{
						if(m_pInCameraBufferYUV && m_pOutCameraBufferYUV)
						{
							rgb2yuv420(m_pCameraBuffer, m_iSrcVideoWidth, m_iSrcVideoHeight, m_pInCameraBufferYUV, true);
							// YUV enhancments
							SpecialEffectsProcessing(m_pInCameraBufferYUV, m_pOutCameraBufferYUV, nEhancementMainID, nEhancementSubID, false, NULL);
							// YUV to RGB
							yuv2rgb(m_pCameraBuffer,m_iSrcVideoWidth, m_iSrcVideoHeight,m_pOutCameraBufferYUV, true);
						}
					}
					
				}
#endif
				//for(int i = 0; i < m_iSrcVideoWidth ;i++)
				//{
				//	for(int j = 0;j < m_iSrcVideoHeight ;j++)
				//	{
				//		int nPos = j * m_iSrcVideoWidth * 3;
				//		int nDPos = nPos + i * 3;
				//		int nSPos = nPos + (m_iSrcVideoWidth -  i) * 3;

				//		m_pCameraBuffer[nDPos] = m_showImageBuf[nSPos];
				//		m_pCameraBuffer[nDPos+1] = m_showImageBuf[nSPos+1];
				//		m_pCameraBuffer[nDPos+2] = m_showImageBuf[nSPos+2];
				//	}
				//}
				*pBuf = m_pCameraBuffer;
				nBufSize = m_nCameraBufferSize;
				//memcpy(m_showImageBuf,m_pCameraBuffer,m_nCameraBufferSize);

				//ShowImage((unsigned char*)m_showImageBuf);
				return true;
			}
	
		}
	}
	return false;
}


bool CSourCamera::SetSourType(int nSourType)
{
	int nTemp = nSourType & SOURCECAMERA;
	if( nTemp == SOURCECAMERA)
		m_bIsEffective = true;
	else
		m_bIsEffective = false;
	return true;
}

int  CSourCamera::getSourID() 
{
	return m_nSourID;
}

bool CSourCamera::SourOpen(void* param) 
{
	BOOL bIsOK = TRUE;
	if(m_bIsEffective)
	{
		StreamParam sParam;
		if(param)
			 sParam = (*(StreamParam*)param);

		if(sParam.nVideoW / 4 == sParam.nVideoH / 3)
		{
			if(m_pAMCapturer != m_pAMCapturer4A3)
			{
				m_bIsStart = false;
				m_pAMCapturer16A9->StopCapture();
				m_pAMCapturer16A9->Release();
				delete m_pAMCapturer16A9;
				m_pAMCapturer16A9 = NULL;
			}
			if(m_pAMCapturer4A3 == NULL)
			{
				m_pAMCapturer4A3 = new  AMCaputer();
				m_pAMCapturer4A3->Init(NULL,m_nDestVideoWidth,m_nDestVideoHeight);
			}
			m_pAMCapturer = m_pAMCapturer4A3;
		}
		else
		{
			if(m_pAMCapturer != m_pAMCapturer16A9)
			{
				m_bIsStart = false;
				m_pAMCapturer4A3->StopCapture();
				m_pAMCapturer4A3->Release();
				delete m_pAMCapturer4A3;
				m_pAMCapturer4A3 = NULL;
			}

			if(m_pAMCapturer16A9 == NULL)
			{
				m_pAMCapturer16A9 = new  AMCaputer();
				m_pAMCapturer16A9->Init(NULL,1280,720);
			}
			m_pAMCapturer = m_pAMCapturer16A9;
		}

		if(m_pAMCapturer)
		{
			m_bDrawLocRect = sParam.bDrawLocRect;
			m_nSourID = sParam.VU[0].nSelectCameraID;

			m_hShowWindow = sParam.hShowHwnd;

			if(sParam.VU[0].nSelectCameraID >= 0 && !m_bIsStart)
			{
				if(sParam.VU[0].nSelectCameraID > m_nCamNum)
				{
					sParam.VU[0].nSelectCameraID = 0;
				}
				else
				{
					if(m_arrCamNode[sParam.VU[0].nSelectCameraID])
						sParam.VU[0].nSelectCameraID = m_arrCamNode[sParam.VU[0].nSelectCameraID]->nIndex;
					else
						sParam.VU[0].nSelectCameraID = 0;
				}
				if( sParam.VU[0].nSelectCameraID < 0 )
					return false;

				m_pAMCapturer->SetDevices(sParam.VU[0].nSelectCameraID);
				m_pAMCapturer->GetVideoSize(m_iSrcVideoWidth,m_iSrcVideoHeight);

				if(m_pCameraBuffer)
				{
					delete m_pCameraBuffer;
					m_pCameraBuffer = NULL;
				}
				m_nCameraBufferSize = m_iSrcVideoWidth*m_iSrcVideoHeight*3;

				m_pCameraBuffer = new unsigned char[m_nCameraBufferSize];
				if(m_showImageBuf)
				{
					delete m_showImageBuf;
					m_showImageBuf = NULL;
				}
				m_showImageBuf = new unsigned char[m_nCameraBufferSize];

				// video enhance
	#if PW_OE_VIDEO_ENHANCE	
				if(m_tmpImageBuf)
				{
					delete m_tmpImageBuf;
					m_tmpImageBuf = NULL;
				}
				m_tmpImageBuf = new unsigned char[m_nCameraBufferSize];

				if(m_pInCameraBufferYUV == NULL || m_nCameraBufferSizeYUV < (m_nCameraBufferSize>>1))
				{
					if(m_pInCameraBufferYUV)
					{
						delete m_pInCameraBufferYUV;
						m_pInCameraBufferYUV = NULL;
					}
					m_nCameraBufferSizeYUV = m_nCameraBufferSize>>1;
					m_pInCameraBufferYUV = new unsigned char[m_nCameraBufferSizeYUV];
				}
				if(m_pOutCameraBufferYUV == NULL || m_nCameraBufferSizeYUV < (m_nCameraBufferSize>>1))
				{
					if(m_pOutCameraBufferYUV)
					{
						delete m_pOutCameraBufferYUV;
						m_pOutCameraBufferYUV = NULL;
					}
					m_nCameraBufferSizeYUV = m_nCameraBufferSize>>1;
					m_pOutCameraBufferYUV = new unsigned char[m_nCameraBufferSizeYUV];
				}
	#endif
				bIsOK = m_pAMCapturer->StartCapture(FALSE);

				if(bIsOK)
				{
					m_posRect.left = ((m_iSrcVideoWidth)/6);
					m_posRect.right = ((m_iSrcVideoWidth)/6*5);
					m_posRect.top = m_iSrcVideoHeight - m_iSrcVideoHeight / 8;
					m_posRect.bottom = m_iSrcVideoHeight - m_iSrcVideoHeight / 8 * 6;
					m_bIsStart = true;
				}
				else
				{
					if(m_pAMCapturer == m_pAMCapturer4A3)
					{
						m_pAMCapturer4A3->Release();
						delete m_pAMCapturer4A3;
						m_pAMCapturer4A3 = NULL;
					}
					if(m_pAMCapturer == m_pAMCapturer16A9)
					{
						m_pAMCapturer16A9->Release();
						delete m_pAMCapturer16A9;
						m_pAMCapturer16A9 = NULL;
					}
					m_pAMCapturer = NULL;
				}
			}
		}
		
	}
	return bIsOK;
}

bool CSourCamera::SourClose(bool bIsColseDev)
{
	if(m_pAMCapturer && m_bIsEffective&&m_bIsStart)
	{
		m_pAMCapturer->StopCapture();
		m_bIsStart = false;
		m_bIsEffective = false;
	}

	if(m_pAMCapturer && bIsColseDev)
	{
		if (m_pAMCapturer4A3)
		{
			m_pAMCapturer4A3->Release();
			delete m_pAMCapturer4A3;
			m_pAMCapturer4A3 = NULL;
		}
		if(m_pAMCapturer16A9)
		{
			m_pAMCapturer16A9->Release();
			delete m_pAMCapturer16A9;
			m_pAMCapturer16A9 = NULL;
		};
		m_pAMCapturer = NULL;
	}
	return true;
}

void CSourCamera::GetVideoWAndH(int& nW,int& nH)
{
	if(m_bIsEffective)
	{
		nW = m_iSrcVideoWidth;
		nH = m_iSrcVideoHeight;
	}
	else
	{
		nW = 0;
		nH = 0;
	}
}

long CSourCamera::GetVideoFPS()
{
	return m_pAMCapturer ? m_pAMCapturer->GetVideoFPS():0;
}

bool CSourCamera::AVE_SetID(int nMainValue, int nSubValue, int nParams[2])
{
	nEhancementMainID = nMainValue;
	nEhancementSubID = nSubValue;
	
    if(nParams != NULL)
	{
		nParam[0] = nParams[0];
		nParam[1] = nParams[1];
	}
	
	return 0;
}

#if PW_OE_VIDEO_ENHANCE
int CSourCamera::SpecialEffectsProcessing(unsigned char *pBuf, unsigned char *pOutBuf, int nAVEMainID, int nAVESubID, bool bRGB, int nParams[2])
{
	// FOR YUV
	// by Ray Zhang
	if(!bRGB)
	{
		int nYSize = m_iSrcVideoWidth * m_iSrcVideoHeight;
		int nSize = nYSize *3 / 2;

		if(nAVEMainID == R_FILTER_NONE)
		{
			memcpy(pOutBuf, pBuf, nSize);

			return 0;
		}

		// First time processing
		if(pInVFrame == NULL)
		{
			pInVFrame = (AVE_Video_Frame*)malloc(sizeof(AVE_Video_Frame));
	
			pInVFrame->data[0] = pBuf;
			pInVFrame->data[1] = pBuf + nYSize;
			pInVFrame->data[2] = pInVFrame->data[1] + (nYSize>>2);

			pInVFrame->stride[0] = m_iSrcVideoWidth;
			pInVFrame->stride[1] = m_iSrcVideoWidth>>1;
			pInVFrame->stride[2] = m_iSrcVideoWidth>>1;

			pInVFrame->nWidth = m_iSrcVideoWidth;
			pInVFrame->nHeight = m_iSrcVideoHeight;
			pInVFrame->nRotate = 0;
		}

		if(pOutVFrame == NULL)
		{
			pOutVFrame = (AVE_Video_Frame*)malloc(sizeof(AVE_Video_Frame));
	
			pOutVFrame->data[0] = pOutBuf;
			pOutVFrame->data[1] = pOutBuf + nYSize;
			pOutVFrame->data[2] = pOutVFrame->data[1] + (nYSize>>2);

			pOutVFrame->stride[0] = m_iSrcVideoWidth;
			pOutVFrame->stride[1] = m_iSrcVideoWidth>>1;
			pOutVFrame->stride[2] = m_iSrcVideoWidth>>1;

			pOutVFrame->nWidth = m_iSrcVideoWidth;
			pOutVFrame->nHeight = m_iSrcVideoHeight;
			pOutVFrame->nRotate = 0;
		}

		AVE_SpecialEffect_Filter_Video_Frame(pInVFrame, nAVEMainID, nAVESubID, NULL, NULL, 0, pOutVFrame);
	//	bHandsupDetect = AVE_FILTER_HANDSUPDET_YUV(pInVFrame, 0, bTmpStates);
	
#if AVE_SAVING
	if(m_fp == NULL)
	{
		char szBuf[256];
		sprintf(szBuf,"D:\\Demo\\cam_w%d_h%d.yuv",m_iSrcVideoWidth,m_iSrcVideoHeight);
		m_fp = fopen(szBuf,"wb");
	}

	fwrite(pInVFrame->data[0],nSize,1,m_fp); 
#endif
	}
	else
	{
		// 
		AVE_SpecialEffect_Filter_Video_FrameRGB(pBuf, m_iSrcVideoWidth, m_iSrcVideoHeight, nAVEMainID, nAVESubID, NULL, NULL, 0, pOutBuf, nParams);

	}

	return 0;
}
#endif

void CSourCamera::SwitchMirrorImage(unsigned char* pInBuf)
{
	int k;
    for(int j = 0; j<m_iSrcVideoHeight; j++)
	{
		for(int i = 0; i<m_iSrcVideoWidth; i++)
		{
			memcpy(m_tmpImageBuf + (j*m_iSrcVideoWidth + m_iSrcVideoWidth - i - 1)*3, pInBuf + (j*m_iSrcVideoWidth + i)*3, 3*sizeof(char));
		}
	}
	memcpy(pInBuf, m_tmpImageBuf, m_iSrcVideoWidth*m_iSrcVideoHeight*3);
}

void CSourCamera::ShowImage(unsigned char* pBuf)
{
	if(m_hShowWindow && pBuf)
	{
		RECT  rc;
		GetClientRect(m_hShowWindow,&rc);
		int nPicWidth = m_iSrcVideoWidth;
		int nPicHeight = m_iSrcVideoHeight;
		m_pBmpInfo->bmiHeader.biHeight = -m_iSrcVideoHeight;	
		m_pBmpInfo->bmiHeader.biWidth =  m_iSrcVideoWidth;
		if(m_bDrawLocRect)
		{
			int ny = (m_posRect.top - m_posRect.bottom) / (rc.bottom - rc.top) + 1;
			int nx = (m_posRect.right - m_posRect.left) / (rc.right - rc.left) + 1;
			int nMax = ny > nx ? ny : nx;
			for(int j = m_posRect.top;j >= m_posRect.bottom;j--)
			{
				for(int i = m_posRect.left;i<= m_posRect.right;i++)
				{
					if( j < (m_posRect.bottom +nMax) || j > (m_posRect.top-nMax) )
					{
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)) = 255;
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+1) = 255;
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+2) = 255;
					}
					else
					{
						if(i < (m_posRect.left+ nMax) || i > (m_posRect.right-nMax))
						{
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)) = 255;
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+1) = 255;
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+2) = 255;
						}
					}

				}
			}
		}
		Rect  showRect;	
		showRect.X=rc.left;
		showRect.Y=rc.top;
		showRect.Width=rc.right-rc.left;
		showRect.Height=rc.bottom-rc.top;

		HDC myHdc=GetDC(m_hShowWindow);
		int ret = SetStretchBltMode(myHdc,STRETCH_HALFTONE); //STRETCH_DELETESCANS

		ret = StretchDIBits(myHdc,0,showRect.Height,showRect.Width,-showRect.Height,
			0,0,nPicWidth,nPicHeight,pBuf,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);

		int n = GetLastError();
		ReleaseDC(m_hShowWindow,myHdc);
	}
}

void CSourCamera::ShowBG()
{
	m_nBGW = 240;
	m_nBGH = 180;

	m_nBGSize = m_nBGW * m_nBGH * 3;
	if(m_nBGBuf == NULL)
	{
		m_nBGBuf = new unsigned char[m_nBGSize];
		memset(m_nBGBuf,0,m_nBGSize);
	}

	RECT  rc = { 0 };

	GetClientRect(m_hShowWindow,&rc);
		
	if(rc.bottom - rc.top == 0 || rc.right - rc.left == 0)
	{
		return;
	}

	int nPicWidth = m_nBGW;
	int nPicHeight = m_nBGH;

	m_pBmpInfo->bmiHeader.biHeight = -m_nBGH;	
	m_pBmpInfo->bmiHeader.biWidth =  m_nBGW;

	HDC myHdc=GetDC(m_hShowWindow);
	int ret = SetStretchBltMode(myHdc,STRETCH_HALFTONE); //STRETCH_DELETESCANS

	Rect  showRect;	
	showRect.X=rc.left;
	showRect.Y=rc.top;
	showRect.Width=rc.right-rc.left;
	showRect.Height=rc.bottom-rc.top;

	ret = StretchDIBits(myHdc,0,showRect.Height,showRect.Width,-showRect.Height,
	0,0,nPicWidth,nPicHeight,m_nBGBuf,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);

	ReleaseDC(m_hShowWindow,myHdc);
}

void CSourCamera::ShowImage(unsigned char* pBuf,unsigned int nBufSize,int nVW,int nVH)
{
	if(m_hShowWindow && pBuf)
	{
		RECT  rc = { 0 };
		GetClientRect(m_hShowWindow,&rc);
		
		if(rc.bottom - rc.top == 0 || rc.right - rc.left == 0)
		{
			return;
		}

		int nPicWidth = nVW;
		int nPicHeight = nVH;
		if(m_bDrawLocRect)
		{
			int ny = (m_posRect.top - m_posRect.bottom) / (rc.bottom - rc.top + 1);
			int nx = (m_posRect.right - m_posRect.left) / (rc.right - rc.left + 1);
			int nMax = ny > nx ? ny : nx;
			for(int j = m_posRect.top;j >= m_posRect.bottom;j--)
			{
				for(int i = m_posRect.left;i<= m_posRect.right;i++)
				{
					if( j < (m_posRect.bottom +nMax) || j > (m_posRect.top-nMax) )
					{
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)) = 255;
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+1) = 255;
						*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+2) = 255;
					}
					else
					{
						if(i < (m_posRect.left+ nMax) || i > (m_posRect.right-nMax))
						{
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)) = 255;
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+1) = 255;
							*(pBuf+(m_iSrcVideoWidth*j*3)+(i*3)+2) = 255;
						}
					}

				}
			}
		}

		HDC myHdc=GetDC(m_hShowWindow);
		int ret = SetStretchBltMode(myHdc,STRETCH_HALFTONE); //STRETCH_DELETESCANS

		int nShowWidth = rc.right-rc.left;
		int nShowHeight = rc.bottom-rc.top;

		if(nShowWidth > 0 && nShowHeight > 0)
		{
			if(nPicWidth/4 != nPicHeight/3)
			{
				RECT  m_show_rect = { 0 };
				m_show_rect.bottom = rc.bottom;
				m_show_rect.left = rc.left;
				m_show_rect.right = rc.right;
				m_show_rect.top = rc.top;
				int nShowVideoW =  abs(m_show_rect.right - m_show_rect.left);
				int nShowVideoH =  abs(m_show_rect.bottom - m_show_rect.top);
				int nWindowHeight = nShowVideoH;

				if(nShowVideoH <= 0 || nShowVideoW <= 0 || nVH <= 0)
					return ;

				float fltRate = (nVW * 1.0) / nVH;

				int nTopx = m_show_rect.left + nShowVideoW / 2;
				int nTopy = m_show_rect.top +  nShowVideoH / 2;	

				if(nShowVideoW !=  nVW && nShowVideoH != nVH)
				{
					float fltShow = ((nShowVideoW * 1.0) / nShowVideoH);
					if(fltShow < fltRate)
					{
						nShowVideoH = nShowVideoW / fltRate;
					}
					else if(fltShow > fltRate)
					{
						nShowVideoW = nShowVideoH * fltRate;
					}
					rc.left = nTopx - (nShowVideoW/2);
					rc.right = nTopx + (nShowVideoW/2);
					rc.top =  nTopy - (nShowVideoH/2);
					//rc.bottom = nTopy + (nShowVideoH/2);
					if(fltRate > 1.0)
					{
						rc.bottom = nTopy + (nShowVideoH/2);
					}
					else
					{
						rc.bottom = m_show_rect.bottom;
					}
				}
				else
				{
					rc.left = m_show_rect.left;
					rc.right = m_show_rect.right;
					rc.top = m_show_rect.top;
					rc.bottom = m_show_rect.bottom;
				}	

				int nShowWidth = rc.right-rc.left;
				int nShowHeight = rc.bottom-rc.top;

				m_pBmpInfo->bmiHeader.biWidth =  nPicWidth;
				m_pBmpInfo->bmiHeader.biHeight = -nPicHeight;

				ret = StretchDIBits(myHdc,rc.left,nShowHeight+(nWindowHeight-nShowHeight)/2,nShowWidth,-nShowHeight,
						0,0,nPicWidth,nPicHeight,pBuf, m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
			}
			else
			{
				Rect  showRect;	
				showRect.X=rc.left;
				showRect.Y=rc.top;
				showRect.Width=rc.right-rc.left;
				showRect.Height=rc.bottom-rc.top;
				m_pBmpInfo->bmiHeader.biWidth =  nVW;
				m_pBmpInfo->bmiHeader.biHeight = -nVH;
				ret = StretchDIBits(myHdc,0,showRect.Height,showRect.Width,-showRect.Height,
				0,0,nPicWidth,nPicHeight,pBuf,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
			}
		}
		ReleaseDC(m_hShowWindow,myHdc);
	}
}

void CSourCamera::GetShowHwndRect(ScRect& rc)
{
	if(m_hShowWindow)
	{
		RECT  show;
		GetClientRect(m_hShowWindow,&show);
		rc.nLeft = show.left;
		rc.nRight = show.right;
		rc.nBottom =show.bottom ;
		rc.nTop = show.top;
	}
}