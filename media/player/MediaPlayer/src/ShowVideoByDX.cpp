#include "ShowVideoByDX.h"

CShowVideoByDX::CShowVideoByDX(PBITMAP hBGbitmap,HWND hShowWnd):CShowVideo(DRAWTYPEBX)
{
	m_lpDD = NULL;    // DirectDraw 对象指针
	m_lpDDSPrimary = NULL;  // DirectDraw 主表面指针
	m_lpDDSOffScr = NULL;  // DirectDraw 离屏表面指针
	m_video_width = 0;
	m_video_height = 0;
	m_show_window_width = 0;
	m_show_window_height = 0;
	memset(&m_show_rect,0,sizeof(HShowWindow));
	memset(&m_showBGRect,0,sizeof(m_showBGRect));
	m_show_rect.hwnd = hShowWnd;
	m_gdiShowBG = NULL;
	m_gdiShowBG = new CShowBGByGDI(hBGbitmap,hShowWnd);
	m_bIsStatus = true;

	m_sws_context = NULL;
	m_pRGBBuf = NULL;
	m_nRGBSize = 0;

	m_pRGBBackBuf = NULL;
	m_nRGBBackSize = 0;

	m_bFirst = TRUE; 
	m_bYUVSucess = TRUE;

	m_nErrCountNum = 0;
	m_bIsInitYUVEnv = InitShowEnv();
}

CShowVideoByDX::~CShowVideoByDX()
{
	UninitShowEnv();
	if(m_gdiShowBG)
	{
		delete m_gdiShowBG;
		m_gdiShowBG = NULL;
	}

	if(m_sws_context)
	{
		m_sws_context = NULL;
	}

	if(m_pRGBBuf)
	{
		delete m_pRGBBuf;
		m_pRGBBuf = NULL;
	}
	if( m_pRGBBackBuf )
	{
		delete m_pRGBBackBuf;
		m_pRGBBackBuf = NULL;
	}
	m_nRGBBackSize = 0;

	m_nRGBSize = 0;
}

bool CShowVideoByDX::getShowHwndStatus()
{
	if(m_show_rect.hwnd)
	{
		RECT  rc;
		GetClientRect(m_show_rect.hwnd,&rc);
		if(rc.bottom >= 0 && rc.left >= 0 && rc.right >= 0 &&  rc.top >= 0)
			return true;
	}
	return false;
}

void CShowVideoByDX::get_show_window_width_and_height()
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

bool CShowVideoByDX::InitShowEnv()
{
	if (DirectDrawCreateEx(NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) 
	{
		return false;
	}

	// 创建DIRECTDRAW 显示YUV420数据 设置协作层
	if (m_lpDD->SetCooperativeLevel(m_show_rect.hwnd,DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES ) != DD_OK)
	{
		return false;
	}
	return InitSurface();
}

bool CShowVideoByDX::UninitShowEnv()
{
	m_nErrCountNum = 0;
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

bool CShowVideoByDX::InitSurface()
{
	if(m_lpDD == NULL)
		return false;

	if(m_lpDDSPrimary)
	{
		m_lpDDSPrimary->Release();
		m_lpDDSPrimary = NULL;
	}

	// 创建主表面
	ZeroMemory(&m_ddsd, sizeof(m_ddsd));
	m_ddsd.dwSize = sizeof(m_ddsd);
	m_ddsd.dwFlags = DDSD_CAPS;
	m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE ;

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

bool CShowVideoByDX::ShowVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFlie,IMediaPlayerEvent* pShowCallback)
{
    if(pShowCallback)
    {
        CutBGRFromYUV420(pBuf,nVW,nVH);
        pShowCallback->ShowVideo(m_show_rect.hwnd,m_pRGBBuf,m_nRGBSize,nVW,nVH);
        return true;
    }

	if(!m_bIsInitYUVEnv || m_nErrCountNum > 3 || nVH > nVW)
	{
		if(m_gdiShowBG)
		{
			CutBGRFromYUV420(pBuf,nVW,nVH);
			m_gdiShowBG->DrawBGImage(m_pRGBBuf,m_nRGBSize,nVH,nVW,bIsPlayFlie);
			return true;
		}
		return false;
	}

	if(m_video_height !=  nVH ||m_video_width != nVW || !m_bIsStatus)
	{
		// 创建YUV表面 
		if( m_lpDD  && InitSurface())
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

			m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
			m_ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
			m_ddsd.dwWidth = m_video_width;
			m_ddsd.dwHeight = m_video_height;
			m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			m_ddsd.ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_YUV ;
			m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V','1','2');
			m_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;

			//HRESULT hr;
			if((m_lpDD->CreateSurface(&m_ddsd, &m_lpDDSOffScr, NULL) != DD_OK) || m_lpDDSOffScr == NULL)
			{
				//const char * szErr = DXGetErrorDescriptionA(hr);
				OutputDebugString("m_lpDD->CreateSurface error\n");
				m_bIsStatus = false;
				m_nErrCountNum++;
				return false;
			}
			m_bIsStatus = true;
		}
		else
		{
			OutputDebugString("InitShowEnv() error\n");
			m_bIsStatus = false;
			m_nErrCountNum++;
			return false ;
		}
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
		OutputDebugString("m_lpDDSOffScr->Lock error\n");
		m_bIsStatus = false;
		m_nErrCountNum++;
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

	if(bIsPlayFlie)
	{
		int nShowVideoW =  abs(m_show_rect.right - m_show_rect.left);
		int nShowVideoH =  abs(m_show_rect.bottom - m_show_rect.top);
		if(nShowVideoH <= 0 || nShowVideoW <= 0 || m_video_height <= 0)
			return false;

		float fltRate = (m_video_width * 1.0) / m_video_height;

		int nTopx = m_show_rect.left + nShowVideoW / 2;
		int nTopy = m_show_rect.top +  nShowVideoH / 2;	

		if(nShowVideoW !=  m_video_width && nShowVideoH != m_video_height)
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
			if(fltRate > 1)
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
	}
	else
	{
		rc.left = m_show_rect.left;
		rc.right = m_show_rect.right;
		rc.top = m_show_rect.top;
		rc.bottom = m_show_rect.bottom;
	}

	if( bIsPlayFlie )
	{
		if(m_pRGBBackBuf == NULL)
		{
			m_nRGBBackSize = 640 * 480 * 3;
			m_pRGBBackBuf = new unsigned char[m_nRGBBackSize];
			memset(m_pRGBBackBuf,0,m_nRGBBackSize);

		}
		bool bIsDraw = ((m_showBGRect.left != m_show_rect.left) || (m_showBGRect.right != m_show_rect.right) 
			|| (m_showBGRect.top != m_show_rect.top) ||( m_showBGRect.bottom != m_show_rect.bottom));

		if(m_gdiShowBG && bIsDraw )
		{
			m_gdiShowBG->DrawBGImage(m_pRGBBackBuf,m_nRGBBackSize,480,640,false);
			m_showBGRect.left = m_show_rect.left;
			m_showBGRect.right = m_show_rect.right;
			m_showBGRect.top = m_show_rect.top;
			m_showBGRect.bottom = m_show_rect.bottom;
		}
	}

	ClientToScreen(hwnd, (LPPOINT)&rc.left); 
	ClientToScreen(hwnd, (LPPOINT)&rc.right);
	if(ddRval = m_lpDDSPrimary->Blt(&rc, m_lpDDSOffScr,NULL , DDBLT_WAIT, NULL) != DD_OK)
	{
		m_nErrCountNum++;
		m_bIsStatus = false;
	}
	m_nErrCountNum = 0;
	return true;
}

bool CShowVideoByDX::DrawBGImage()
{
	if(m_gdiShowBG)
	{
		return m_gdiShowBG->DrawBGImage();
	}
	return false;
}

//YUV420转RGB
bool CShowVideoByDX::CutRGBFromYUV420(unsigned char *yuv420,int nWidth,int nHeight)
{
	if(yuv420 == NULL)
	{
		return false;
	}


	if(m_sws_context == NULL || (m_video_height !=  nHeight || m_video_width != nWidth ))
	{
		m_video_height = nHeight;
		m_video_width = nWidth;

		if(m_sws_context)
		{
			sws_freeContext(m_sws_context);
			m_sws_context = NULL;
		}
		m_sws_context = sws_getContext(nWidth, nHeight,AV_PIX_FMT_YUV420P , nWidth, nHeight,AV_PIX_FMT_RGB24,SWS_BILINEAR, NULL, NULL, NULL);
	}

	int nSize = nWidth * nHeight * 3;
	if(m_nRGBSize < nSize)
	{
		if(m_pRGBBuf)
		{
			delete m_pRGBBuf;
			m_pRGBBuf = NULL;
		}
		if(m_pRGBBuf == NULL)
		{
			m_pRGBBuf = new unsigned char[nSize];
		}
		m_nRGBSize = nSize;
	}


	if(m_sws_context)
	{
		int nRGBWidthStep = nWidth * nHeight;
		uint8_t *pSrcBuff[3]= {yuv420,yuv420+ nWidth * nHeight,yuv420+nWidth * nHeight * 5 / 4};
		uint8_t *pDstBuff[3] = {m_pRGBBuf,m_pRGBBuf + nRGBWidthStep,m_pRGBBuf+ nRGBWidthStep * 2};
		
		int nSrcStride[3];
		int nDstStride[3];

		for (int i=0; i<3; i++)
		{
			nDstStride[i] = nWidth * 3 ;
		}

		nSrcStride[0] = nWidth;
		nSrcStride[1] = nWidth / 2;
		nSrcStride[2] = nWidth / 2;
		sws_scale(m_sws_context, pSrcBuff,nSrcStride, 0, nHeight,pDstBuff, nDstStride);
	}
	return TRUE;
}

//YUV420转RGB
bool CShowVideoByDX::CutBGRFromYUV420(unsigned char *yuv420,int nWidth,int nHeight)
{
	if(yuv420 == NULL)
	{
		return false;
	}


	if(m_sws_context == NULL || (m_video_height !=  nHeight || m_video_width != nWidth ))
	{
		m_video_height = nHeight;
		m_video_width = nWidth;

		if(m_sws_context)
		{
			sws_freeContext(m_sws_context);
			m_sws_context = NULL;
		}
		m_sws_context = sws_getContext(nWidth, nHeight,AV_PIX_FMT_YUV420P , nWidth, nHeight,AV_PIX_FMT_BGR24,SWS_BILINEAR, NULL, NULL, NULL);
	}

	int nSize = nWidth * nHeight * 3;
	if(m_nRGBSize < nSize)
	{
		if(m_pRGBBuf)
		{
			delete m_pRGBBuf;
			m_pRGBBuf = NULL;
		}
		if(m_pRGBBuf == NULL)
		{
			m_pRGBBuf = new unsigned char[nSize];
		}
		m_nRGBSize = nSize;
	}


	if(m_sws_context)
	{
		int nRGBWidthStep = nWidth * nHeight;
		uint8_t *pSrcBuff[3]= {yuv420,yuv420+ nWidth * nHeight,yuv420+nWidth * nHeight * 5 / 4};
		uint8_t *pDstBuff[3] = {m_pRGBBuf,m_pRGBBuf + nRGBWidthStep,m_pRGBBuf+ nRGBWidthStep * 2};

		int nSrcStride[3];
		int nDstStride[3];

		for (int i=0; i<3; i++)
		{
			nDstStride[i] = nWidth * 3 ;
		}

		nSrcStride[0] = nWidth;
		nSrcStride[1] = nWidth / 2;
		nSrcStride[2] = nWidth / 2;
		sws_scale(m_sws_context, pSrcBuff,nSrcStride, 0, nHeight,pDstBuff, nDstStride);
	}
	return TRUE;
}

void CShowVideoByDX::InitConvertTable()
{
	long int crv,cbu,cgu,cgv;
	int i,ind;   

	crv = 104597; cbu = 132201;  
	cgu = 25675;  cgv = 53279;

	for (i = 0; i < 256; i++) 
	{
		crv_tab[i] = (i-128) * crv;
		cbu_tab[i] = (i-128) * cbu;
		cgu_tab[i] = (i-128) * cgu;
		cgv_tab[i] = (i-128) * cgv;
		tab_76309[i] = 76309*(i-16);
	}

	for (i=0; i<384; i++)
		clp[i] =0;
	ind=384;
	for (i=0;i<256; i++)
		clp[ind++]=i;
	ind=640;
	for (i=0;i<384;i++)
		clp[ind++]=255;
}

//YUV420转RGB
/*bool CShowVideoByDX::CutRGBFromYUV420(unsigned char *yuv420,int nWidth,int nHeight)
{
	if(yuv420 == NULL)
	{
		return false;
	}
	if(m_bFirst)
	{
		m_bFirst = false;
		InitConvertTable();
	}

	int nSize = nWidth * nHeight * 3;
	if(m_nRGBSize != nSize)
	{
		if(m_pRGBBuf)
		{
			delete m_pRGBBuf;
			m_pRGBBuf = NULL;
		}
		if(m_pRGBBuf == NULL)
		{
			m_pRGBBuf = new unsigned char[nSize];
		}
		m_nRGBSize = nSize;
	}
	int nRGBWidthStep = nWidth * nHeight;

	int y1,y2,u,v; 
	unsigned char *py1,*py2;
	int i,j, c1, c2, c3, c4;
	unsigned char *d1, *d2;

	uint8_t *pSrcBuff[3]= {yuv420,yuv420+ nWidth * nHeight,yuv420+nWidth * nHeight * 5 / 4};
	uint8_t *pDstBuff[3] = {m_pRGBBuf,m_pRGBBuf + nRGBWidthStep,m_pRGBBuf+ nRGBWidthStep * 2};

	unsigned char *srcY = (unsigned char *)(pSrcBuff[0]);//todo kany 外面取到的uv好像是反得，暂时把这里翻过来
	unsigned char *srcV = (unsigned char *)(pSrcBuff[1]);
	unsigned char *srcU = (unsigned char *)(pSrcBuff[2]);

	py1 = srcY;
	py2 = srcY + nWidth;
	//d1 = dstImg;						
	//d2 = dstImg+3*width;		
	d1 = (unsigned char *)(pDstBuff[0]);
	d2 = (unsigned char *)(pDstBuff[0] + 3*nWidth);

	for (j=0; j<nHeight; j+=2) 
	{
		for (i=0; i<nWidth; i+=2) 
		{
			u = *srcU++;
			v = *srcV++;

			c1 = crv_tab[v];
			c2 = cgu_tab[u];
			c3 = cgv_tab[v];
			c4 = cbu_tab[u];

			//up-left
			y1 = tab_76309[*py1++]; 
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];
			//down-left
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];

			//up-right
			y1 = tab_76309[*py1++]; 
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];
			//down-right
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];
		}

		d1+=3*nWidth;
		d2+=3*nWidth;

		py1+=(nWidth );
		py2+=(nWidth);

	}   
	return TRUE;
}*/
