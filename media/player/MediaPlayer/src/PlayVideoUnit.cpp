#include "stdafx.h"
#include "MediaPlayer.h"
#include "PlayVideoUnit.h"
#include "MMSystem.h"
#include "flvHeader.h"
#include "MP4Encoder.h"

#define AV_NOSYNC_THRESHOLD 10000.0
int sample_rate[13] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350};

void CALLBACK  _ShowVideo(void* dwUser)
{

      CPlayVideoUnit * pThis=(CPlayVideoUnit*)dwUser;//由this指针获得实例的指针
      pThis->OnShowVideo();//调用要回调的成员方法

}

CMP4Encoder g_encMP4;
CPlayVideoUnit::CPlayVideoUnit(fEvent_Callback fcallback,void* dwUser)
{
	m_isVideoPlaying = 0;
	m_mapShowHand.clear();
	m_netProcess = NULL;

	m_nFlvParseBufSize = 512 * 1024;
	m_pFlvParseBuf = NULL;
	m_FlvParsePos = 0;

	m_nFlvBufSize = 128*1024;
	m_pFlvBuf = (unsigned char*)malloc(m_nFlvBufSize);

	memset(m_H264PPSAndSPS,0,128);
	m_nH264PPSAndSPSSize = 0;

	m_nPlayVideoTime = 0;
	m_nPlayAudioTime = 0;
	m_bIsHaveAudio = false;

	m_DrawBGBegin = 0;
	m_bIsPlayLocal = false;
	m_szPlayVideoURL[0] = '\0';
	m_fcallback = fcallback;
	m_dwUser = dwUser;
	m_bIsMainVideo = false;
	m_playFile = NULL;
	m_bIsShow =  false;
	m_pShowCallback = NULL;
	m_nLastReadMediaTime = 0;

    //g_encMP4.create("D:\\DLX_0.mp4");
}

CPlayVideoUnit::~CPlayVideoUnit()
{
	if(m_netProcess)
	{
		DestoryNetProcess(m_netProcess);
		m_netProcess = NULL;
	}

	if(m_playFile)
	{
		delete m_playFile;
		m_playFile = NULL;
	}

	if(m_pFlvBuf)
	{
		free(m_pFlvBuf);
		m_pFlvBuf = NULL;
	}
	if(m_pFlvParseBuf)
	{
		free(m_pFlvParseBuf);
		m_pFlvParseBuf = NULL;
	}

	ClearAllShowVideo();

    g_encMP4.close();
}


CShowVideo*  CPlayVideoUnit::FindByHand(HWND hShowWnd)
{
	CShowVideo*  pVS = NULL;
	m_mapShowLock.Lock();
	map<long,CShowVideo*>::iterator iter;
	iter = m_mapShowHand.find((long)hShowWnd);
	if(iter != m_mapShowHand.end())
		pVS = iter->second;
	m_mapShowLock.Unlock();
	return pVS;
}

 void* CPlayVideoUnit::GetShowHwnd()
 {
	 void* playhand = NULL;
	 m_mapShowLock.Lock();
	 if(m_mapShowHand.size() > 0)
	 {
		 map<long,CShowVideo*>::iterator iter = m_mapShowHand.begin();;
		 long Ken = iter->first;
		 playhand = (void*)Ken;
	 }
	 m_mapShowLock.Unlock();
	return playhand; 
 }

void  CPlayVideoUnit::UpdateShowVideo(PBITMAP hBGbitmap,HWND hShowWnd,int nType)
{
	CShowVideo* pSV = NULL;
	if(nType == DRAWTYPEBX)
	{
		pSV = new CShowVideoByDX(hBGbitmap,hShowWnd);
	}
	else if(nType == DRAWTYPEGDI)
	{
		pSV = new CShowBGByGDI(hBGbitmap,hShowWnd);
	}
	else
	{
		pSV = NULL;
	}

	ClearAllShowVideo();
	m_mapShowLock.Lock();
	m_mapShowHand[(long)hShowWnd] = pSV;
	m_mapShowLock.Unlock();
}

void  CPlayVideoUnit::AddShowVideo(PBITMAP hBGbitmap,HWND hShowWnd,int nType)
{
	CShowVideo* pSV = NULL;
	if(nType == DRAWTYPEBX)
	{
		pSV = new CShowVideoByDX(hBGbitmap,hShowWnd);
	}
	else if(nType == DRAWTYPEGDI)
	{
		pSV = new CShowBGByGDI(hBGbitmap,hShowWnd);
	}
	else
	{
		pSV = NULL;
	}

	m_mapShowLock.Lock();
	m_mapShowHand[(long)hShowWnd] = pSV;
	m_mapShowLock.Unlock();
}

void  CPlayVideoUnit::DelShowVideo(HWND hShowWnd)
{
	m_mapShowLock.Lock();
	map<long,CShowVideo*>::iterator iter;
	iter = m_mapShowHand.find((long)hShowWnd);
	if(iter != m_mapShowHand.end())
	{
		if(iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		m_mapShowHand.erase(iter);
	}
	m_mapShowLock.Unlock();
}

void  CPlayVideoUnit::ClearAllShowVideo()
{
	m_mapShowLock.Lock();
	map<long,CShowVideo*>::iterator iter;
	for(iter = m_mapShowHand.begin();iter != m_mapShowHand.end(); iter++)
	{
		CShowVideo* pSV = iter->second;
		if(pSV)
		{
			if(pSV->getShowType() == DRAWTYPEBX)
				delete (CShowVideoByDX*)pSV;
		}
	}
	m_mapShowHand.clear();
	m_mapShowLock.Unlock();
}

CNetProcess * CPlayVideoUnit::CreateNetProcess(const char* szURL)
{
	CNetProcess* pn = NULL;
	if(szURL == NULL)
		return pn;
	if(strncmp(szURL,"rtmp://",strlen("rtmp://")) == 0)
	{
		pn = new CRTMPProcess();
	}
	else if(strncmp(szURL,"udp://",strlen("udp://")) == 0)
	{
		pn = new CUDPProcess();
	}
	else if(strncmp(szURL,"http://",strlen("http://")) == 0)
	{
	}
	return pn;
}

void CPlayVideoUnit::DestoryNetProcess(CNetProcess * np)
{
	if(np)
	{
		delete np;
		np = NULL;
	}
}

 void  CPlayVideoUnit::DrawVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFile)
 {
	m_mapShowLock.Lock();
	map<long,CShowVideo*>::iterator iter;
	for(iter = m_mapShowHand.begin();iter != m_mapShowHand.end(); iter++)
	{
		CShowVideo* pSV = iter->second;
		if(pSV)
		{
			pSV->ShowVideo(pBuf,nBufSize,nVH,nVW,bIsPlayFile,m_pShowCallback);
		}
	}

	m_mapShowLock.Unlock();
 }

 void CPlayVideoUnit::DrawBGImage()
 {
	m_mapShowLock.Lock();
	map<long,CShowVideo*>::iterator iter;
	for(iter = m_mapShowHand.begin();iter != m_mapShowHand.end(); iter++)
	{
		CShowVideo* pSV = iter->second;
		if(pSV)
		{
			pSV->DrawBGImage();
		}
	}
	m_mapShowLock.Unlock();
 }

int  CPlayVideoUnit::GetShowHandCount()
{
	int nSize = 0;
	m_mapShowLock.Lock();
	nSize = m_mapShowHand.size();
	m_mapShowLock.Unlock();
	return nSize;
}

void  CPlayVideoUnit::setPlayAudioTime(int64_t nPlayAudioTime,bool bIsHaveAudio)
{
	m_nPlayAudioTime = nPlayAudioTime;
	m_bIsHaveAudio = bIsHaveAudio;
}


void  CPlayVideoUnit::PlayFileAndGetWaitTime(int& nSleepTime,VideoFrame&  vf)
{
	if(m_playFile)
	{
		double delay  = 0.0;
		double diff  = 0.0;
		int nSpan = 0;
		m_playFile->decode_one_video_frame(vf,nSpan);
		m_nPlayVideoTime = vf.mVideoFramePts;
		delay = vf.fltOneFrameDuration;
		diff = (((m_nPlayVideoTime - m_nPlayAudioTime) * 1.0));

		if(fabs(diff) < AV_NOSYNC_THRESHOLD) //求浮点数x的绝对值
		{
			if(diff <= -delay) 
			{
				delay = 10;       //如果 视频显示过慢，离音频 过于远 则 显示时间为10ms
			} 
			else if(diff >= delay)
			{
				delay = 2 * delay;  //如果 视频显示过快 则停留 两帧的时间
			}
		}
		nSleepTime = delay * 1000;

		m_DrawBGBegin = timeGetTime();
	}
}

void  CPlayVideoUnit::PlayLiveAndGetWaitTime(int& nSleepTime,VideoFrame&  vf)
{
	int nMaxJitterTime = 0;
	int nBufCacheTime = 0;
	H264VideoFrame hvf;
	m_vidJitterBuf.pop(hvf,nMaxJitterTime,nBufCacheTime,m_bIsHaveAudio);
	if(hvf.pFrame)
	{

		m_nPlayVideoTime = hvf.pts;
		m_decoder.DecodeH264Video(hvf.pFrame,hvf.nFrameSize,hvf.pts,vf);

		if(m_bIsHaveAudio && m_nPlayVideoTime - m_nPlayAudioTime  > 100)
		{
			nSleepTime = (hvf.nSpan * 1.5) * 1000;
		}
		else if (m_bIsHaveAudio && m_nPlayAudioTime - m_nPlayVideoTime  > 100)
		{
			if(m_nPlayAudioTime - m_nPlayVideoTime  > 1000)
				nSleepTime = 0;
			else
				nSleepTime = (hvf.nSpan * 0.5) * 1000;
		}
		else
		{
			nSleepTime = (hvf.nSpan * 0.9)  * 1000;
		}

		m_DrawBGBegin = timeGetTime();
		m_vidJitterBuf.freeVideoFrameMem(hvf);
	}

	if(m_bIsMainVideo && m_fcallback)
	{
		m_fcallback(m_nPlayAudioTime,m_bIsHaveAudio,m_dwUser);
	}
}

void CPlayVideoUnit::OnShowVideo()
{
	VideoFrame  vf;
	while(!m_threadShowVideo.GetStop())
	{
		int nSleepTime = 10 * 1000;
		if(m_bIsPlayLocal)
		{
			PlayFileAndGetWaitTime(nSleepTime,vf);
		}
		else
		{
			PlayLiveAndGetWaitTime(nSleepTime,vf);
		}
		m_ShowLock.Lock();
		
        if(m_bIsShow)
		{
			unsigned long nShowBegin = timeGetTime();
			int nDiffShowBGTime  = nShowBegin - m_DrawBGBegin;
            /*
		    if(m_bIsPlayLocal || m_pShowCallback == NULL )
			{
				if(nDiffShowBGTime < 15000)
				{
					if((vf.pVideoFrameBuf && vf.nVideoFrameSize > 0))
					{
						m_DrawBGBegin = timeGetTime();
						bool bIsFlag = m_decoder.getVideoWHReverseFlag() || m_playFile;
						DrawVideo(vf.pVideoFrameBuf,vf.nVideoFrameSize,vf.nVideoFrameHeight,vf.nVideoFrameWidth,bIsFlag);
					}
				}
				else
				{
                    if(m_bIsPlayLocal || m_pShowCallback == NULL )
                    {
					    DrawBGImage();
                    }
                    else
                    {
                        m_pShowCallback->ShowVideo(GetShowHwnd(),NULL,0,0,0);
                    }
				}
			}		
            else
			{
				if(nDiffShowBGTime < 15000)
				{
					m_pShowCallback->ShowVideo(GetShowHwnd(),vf.pVideoFrameBuf,vf.nVideoFrameSize,vf.nVideoFrameWidth,vf.nVideoFrameHeight);
				}
				else
				{
					m_pShowCallback->ShowVideo(GetShowHwnd(),NULL,0,0,0);
				}
			}
            */

            if(nDiffShowBGTime < 15000)
            {
                if((vf.pVideoFrameBuf && vf.nVideoFrameSize > 0))
                {
                    m_DrawBGBegin = timeGetTime();
                    bool bIsFlag = m_decoder.getVideoWHReverseFlag() || m_playFile;
                    DrawVideo(vf.pVideoFrameBuf,vf.nVideoFrameSize,vf.nVideoFrameHeight,vf.nVideoFrameWidth,bIsFlag);
                }
            }
            else
            {
                if(m_bIsPlayLocal || m_pShowCallback == NULL )
                {
                    DrawBGImage();
                }
            }

			unsigned long nShowEnd = timeGetTime();
			nDiffShowBGTime = nShowEnd - nShowBegin;
			nSleepTime -= nDiffShowBGTime;
		}

		m_ShowLock.Unlock();

		av_usleep(nSleepTime);
	}
}

bool CPlayVideoUnit::PlayMedia(const char* szURL,void* hPlayHand,void* hBGbitmap,ThreadProc thread_proc,void* pUserData,bool bIsMainVideo,IMediaPlayerEvent* pCallback)
{

	if(FindByHand((HWND)hPlayHand) == NULL)
	{
		AddShowVideo((PBITMAP)hBGbitmap,(HWND)hPlayHand,DRAWTYPEBX);
	}

	if(InterlockedExchange(&m_isVideoPlaying,m_isVideoPlaying))
	{
		return false;
	}

	m_pShowCallback = pCallback;


	if(m_netProcess == NULL)
	{
		m_netProcess = CreateNetProcess(szURL);
	}

	m_nPlayAudioTime = 0;
	m_bIsHaveAudio = false;

	if(m_netProcess)
	{
		moudle_commond_t cmd = OPEN;
		m_netProcess->Activate(szURL,0);
		if(!m_netProcess->SetCommond(cmd,NULL))
			return false;			

		strcpy(m_szPlayVideoURL,szURL);

		if(m_pFlvParseBuf == NULL)
		{
			m_pFlvParseBuf = (unsigned char*)malloc(m_nFlvParseBufSize);
		}
		m_bIsMainVideo = bIsMainVideo;
		m_threadReadMedia.Begin(thread_proc,pUserData);
		m_threadShowVideo.Begin(_ShowVideo,this);
		
	}
	m_bIsShow = true;
	InterlockedExchange(&m_isVideoPlaying,1);
	return true;
}


bool CPlayVideoUnit::ChangeMedia(void* hwnd,void* hBGbitmap,bool bIsMainVideo)
{
	if(InterlockedExchange(&m_isVideoPlaying,m_isVideoPlaying))
	{
		m_bIsMainVideo = bIsMainVideo;
		if(m_netProcess == NULL)
		{
			m_nLastReadMediaTime = 0;
			m_netProcess = CreateNetProcess(m_szPlayVideoURL);
			if(m_netProcess)
			{
				moudle_commond_t cmd = OPEN;
				m_netProcess->Activate(m_szPlayVideoURL,0);
				if(!m_netProcess->SetCommond(cmd,NULL))
					return false;			
			}
		}


		if(!m_bIsMainVideo)
		{
			m_nPlayAudioTime = 0;
			m_bIsHaveAudio = false;
		}

		if(FindByHand((HWND)hwnd) == NULL)
		{
			UpdateShowVideo((PBITMAP)hBGbitmap,(HWND)hwnd,DRAWTYPEBX);
		}
	}


	return true;
}

bool CPlayVideoUnit::StopMeida(void* hPlayHand)
{
	bool bIsStop = false;
	m_bIsShow = false;
	m_pShowCallback = NULL;
	
    if(InterlockedExchange(&m_isVideoPlaying,m_isVideoPlaying))
	{
		if(m_mapShowHand.size() == 1)
		{
			DelShowVideo((HWND)hPlayHand);
			if(m_netProcess)
			{
				moudle_commond_t cmd = STOP;
				m_netProcess->SetCommond(cmd,NULL);
			}
			m_threadReadMedia.End();
			m_threadShowVideo.End();
			bIsStop = true;
		}
		else
		{
			DelShowVideo((HWND)hPlayHand);
		}
	}

	if(bIsStop)
	{
		m_vidJitterBuf.clear();
		m_FlvParsePos = 0;
	}
	return bIsStop;
}

int CPlayVideoUnit::ReadMediaData()
{
	if(m_netProcess)
	{
		unsigned int nCurReadMediaTime = 0;
		if(m_nLastReadMediaTime == 0)
		{
			m_nLastReadMediaTime =  ::GetTickCount();
			nCurReadMediaTime = m_nLastReadMediaTime;
		}
		else
		{
			nCurReadMediaTime = ::GetTickCount();
		}
		int nReadSize = m_netProcess->ReadMedia(m_pFlvBuf,m_nFlvBufSize);
		if(nReadSize > 0)
		{
			m_nLastReadMediaTime = nCurReadMediaTime;
		}
		else
		{
			if(nCurReadMediaTime - m_nLastReadMediaTime > 60 * 1000)
			{
				moudle_commond_t cmd = STOP;
				m_netProcess->SetCommond(cmd,NULL);
				DestoryNetProcess(m_netProcess);
				m_netProcess = NULL;
			}
		}
		return nReadSize;
	}
	return 0;
}

void CPlayVideoUnit::netStopCommond()
{
	if(m_netProcess)
	{
		moudle_commond_t cmd = STOP;
		m_netProcess->SetCommond(cmd,NULL);
	}
}

int CPlayVideoUnit::flvParse(unsigned char**pFlvPBuf,unsigned int& nFlvParseBufSize,unsigned int& nFlvParseBufPos,unsigned char * pBuf,unsigned int nBufSize,ParseFlvParam &pNode)
{
	if(pBuf == NULL  || nBufSize <= 0 )
		return 2;
	unsigned int nBufBusySize = 0;

	unsigned char*pFlvParseBuf = (*pFlvPBuf);

	if(nBufSize > (nFlvParseBufSize  - nFlvParseBufPos))
	{
		nFlvParseBufSize = nFlvParseBufPos + nBufSize * 2;
		unsigned char *pTemp = (unsigned char*)malloc(nFlvParseBufSize);
		memcpy(pTemp,pFlvParseBuf,nFlvParseBufPos);
		free(pFlvParseBuf);
		pFlvParseBuf = pTemp;
	}
	memcpy(pFlvParseBuf + nFlvParseBufPos,pBuf,nBufSize);

	nBufBusySize = nFlvParseBufPos + nBufSize;
	if(nFlvParseBufPos == 0 && pFlvParseBuf[0] == 'F' && pFlvParseBuf[1] == 'L' && pFlvParseBuf[2] == 'V' && nBufBusySize > 13)
	{
		nFlvParseBufPos += 13;
		nBufBusySize -= 13;
	}

	flv_tag_header_t  *ph = NULL;
	int tag_header_size = sizeof(flv_tag_header_t);
	while(nBufBusySize >  tag_header_size)
	{
		uint64_t npts = 0;
		int nstmp = 0;
		unsigned int data_size = 0;
		int tag_all_size = 0;
		int tag_size = 0;

		ph = (flv_tag_header_t *)(pFlvParseBuf+ nFlvParseBufPos);
		swicth_int(ph->tag_data_length,sizeof(ph->tag_data_length),&data_size);

		tag_all_size = tag_header_size + data_size + sizeof(int);
		FLVFILE_COPYSTAMP_INT(nstmp,ph->timestamp);

		if(tag_all_size > (nFlvParseBufSize-nFlvParseBufPos)) //数据出错
		{
			nFlvParseBufPos = 0;
			return 1;
		}
		unsigned char * pprev = pFlvParseBuf + (nFlvParseBufPos + tag_all_size - sizeof(int));
		swicth_int(pprev,sizeof(tag_size),&tag_size);


		if(tag_size != tag_all_size -4)
		{
			return 0;
		}
		if(nBufBusySize < tag_all_size)
			break;

		nFlvParseBufPos += tag_header_size;
		nBufBusySize -= tag_header_size;

		switch(ph->tag_header_type )
		{
			case 0x08:
			{
                MP4FileData  dataAudio = { 0 };

				AACAudioFrame aaf;
				unsigned char* pstart = pFlvParseBuf + nFlvParseBufPos;
				pNode.bIsAudio = true;
				if(pstart[0] == 0xAF && pstart[1] == 0x00)
				{
					unsigned char nsamplerate = 0;
					unsigned char nchannel = 0;
					nsamplerate = ((( pstart[2] & 0x07) << 1) | (( pstart[3] & 0x80 ) >> 7));
					nchannel = ((pstart[3] & 0x78) >> 3);

					pNode.samplerate = sample_rate[nsamplerate];
					pNode.nchannel = 1;//nchannel;
					aaf.nFrameSize = data_size - 2;
					aaf.pFrame = pstart+2;
					aaf.pts = 0;
					pNode.bIsAAC = true;

					if(pNode.audJitterBuf)
						pNode.audJitterBuf->Push((char*)aaf.pFrame,aaf.nFrameSize,aaf.pts);

                    g_encMP4.setAudio(pNode.samplerate,pNode.nchannel);
                    
				}
				else if(pstart[0] == 0xAF && pstart[1] == 0x01)
				{
					aaf.pts = nstmp;
					aaf.nFrameSize  = data_size - 2;
					if(pNode.audJitterBuf)
						pNode.audJitterBuf->Push((char*)pstart+2,aaf.nFrameSize,aaf.pts);

                    dataAudio.buf = (char*)(pstart+2);
                    dataAudio.size = data_size - 2;
                    dataAudio.duration = nstmp;
                    dataAudio.type = 1;

                    g_encMP4.writeData(dataAudio);
				}

                break;
			}
			
			case 0x09:
			{
				//video
				unsigned char* pstart = pFlvParseBuf + nFlvParseBufPos;
				if(pNode.pPVU)
				{
                    pNode.pPVU->flvParseVideo(pstart,data_size,nstmp);
                }

                break;
			}
			
			case 0x12:
			{
				int n = 0;
                // 分析参数
                unsigned char* pstart = pFlvParseBuf + nFlvParseBufPos;
                int moveSize = 0;

                uint16_t videoWidth = 640;
                uint16_t videoHeight = 480;
                uint16_t videoFPS = 12;

                while(true)
                {
                    if(data_size - moveSize <= 3)
                    {
                        break;
                    }

                    char szName[32] = { 0 };
                    char szValue[16] = { 0 };
                    bool isValue = false;
                    do 
                    {                        
                        double fValue = 0.0;
                        memcpy(szName,pstart,3); //fps
                        if(stricmp(szName,"fps") == 0)
                        {
                            unsigned char* cValue=(unsigned char*)&fValue;
                            pstart +=4;
                            moveSize +=4;
                            
                            memcpy(szValue,pstart,8);

                            cValue[0] = szValue[7];
                            cValue[1] = szValue[6];
                            cValue[2] = szValue[5];
                            cValue[3] = szValue[4];
                            cValue[4] = szValue[3];
                            cValue[5] = szValue[2];
                            cValue[6] = szValue[1];
                            cValue[7] = szValue[0];

                            videoFPS = fValue;

                            pstart +=8;
                            moveSize +=8;

                            isValue = true;
                            break;
                        }

                        memcpy(szName,pstart,5); //width
                        if(stricmp(szName,"width") == 0)
                        {
                            unsigned char* cValue=(unsigned char*)&fValue;
                            pstart +=6;
                            moveSize +=6;

                            memcpy(szValue,pstart,8);

                            cValue[0] = szValue[7];
                            cValue[1] = szValue[6];
                            cValue[2] = szValue[5];
                            cValue[3] = szValue[4];
                            cValue[4] = szValue[3];
                            cValue[5] = szValue[2];
                            cValue[6] = szValue[1];
                            cValue[7] = szValue[0];

                            videoWidth = fValue;

                            pstart +=8;
                            moveSize +=8;

                            isValue = true;
                            break;
                        }

                        memcpy(szName,pstart,6); //height
                        if(stricmp(szName,"height") == 0)
                        {
                            unsigned char* cValue=(unsigned char*)&fValue;
                            pstart +=7;
                            moveSize +=7;

                            memcpy(szValue,pstart,8);

                            cValue[0] = szValue[7];
                            cValue[1] = szValue[6];
                            cValue[2] = szValue[5];
                            cValue[3] = szValue[4];
                            cValue[4] = szValue[3];
                            cValue[5] = szValue[2];
                            cValue[6] = szValue[1];
                            cValue[7] = szValue[0];
                            
                            videoHeight = fValue;

                            pstart +=8;
                            moveSize +=8;

                            isValue = true;
                            break;
                        }

                    } while (false);

                    if(!isValue)
                    {
                        pstart++;
                        moveSize++;
                    }
                }

                g_encMP4.setVideo(videoWidth,videoHeight,videoFPS);
                break;
			}
			
			default:
			break;
		}
		nFlvParseBufPos += (data_size + sizeof(int));
		nBufBusySize -= (data_size + sizeof(int));      
    }
	memcpy(pFlvParseBuf,pFlvParseBuf + nFlvParseBufPos,nBufBusySize);
	nFlvParseBufPos = nBufBusySize;
	return 0;
}

int    CPlayVideoUnit::flvParseVideo(unsigned char*pstart,unsigned int data_size,int nstmp)
{
	if(pstart[0] == 0x17 && pstart[1] == 0x00)
	{
		pstart += 13;
		if(pstart[0] == 0x67)
		{						
			m_nH264PPSAndSPSSize = data_size - 13 + 4 + 4;
			m_H264PPSAndSPS[0] = 0x00;
			m_H264PPSAndSPS[1] = 0x00;
			m_H264PPSAndSPS[2] = 0x00;
			m_H264PPSAndSPS[3] = 0x01;
			int nPos = data_size - 13- 4;
			memcpy(m_H264PPSAndSPS+4,pstart,nPos);
			
            uint32_t spsSize = nPos - 3;
            uint8_t* sps = pstart;

			pstart += nPos ;
			m_H264PPSAndSPS[nPos+4] = 0x00;
			m_H264PPSAndSPS[nPos+5] = 0x00;
			m_H264PPSAndSPS[nPos+6] = 0x00;
			m_H264PPSAndSPS[nPos+7] = 0x01;
			memcpy(m_H264PPSAndSPS+4+nPos+4,pstart,4);

            uint32_t ppsSize = 4;
            uint8_t * pps = pstart;
            
            g_encMP4.setSpsAndPps(sps,spsSize,pps,ppsSize);
		}
	}
	else
	{
        MP4FileData     dataVideo = { 0 };
        char *          bufFrame = NULL;
        uint32_t        sizeBuf = 0;

		pstart += 9;

		unsigned char nNalType = pstart[0] & 0x1f;
        unsigned char prefix[4] = {0x00,0x00,0x00,0x01};

		if(nNalType == 5 || pstart[0] == 0x65)
		{
			m_H264PPSAndSPS[m_nH264PPSAndSPSSize] = 0x00;
			m_H264PPSAndSPS[m_nH264PPSAndSPSSize+1] = 0x00;
			m_H264PPSAndSPS[m_nH264PPSAndSPSSize+2] = 0x00;
			m_H264PPSAndSPS[m_nH264PPSAndSPSSize+3] = 0x01;
			m_vidJitterBuf.push(m_H264PPSAndSPS,m_nH264PPSAndSPSSize+4,pstart,data_size - 9,nstmp);
		}
		else
		{	
			m_vidJitterBuf.push(prefix,4,pstart,data_size - 9,nstmp);
		}
              
        sizeBuf = 4 + (data_size - 9);
        bufFrame = (char*)malloc(sizeBuf);
        memcpy(bufFrame,prefix,4);
        memcpy(bufFrame + 4,pstart,data_size-9);

        dataVideo.buf = (char*)bufFrame;
        dataVideo.size = sizeBuf;
        dataVideo.duration = nstmp;
        dataVideo.type = 0;
        dataVideo.frame = true;

        g_encMP4.writeData(dataVideo);

        free(bufFrame);
	}
	return 1;
}


bool  CPlayVideoUnit::PlayFile(const char* szURL,void* hPlayHand,void* hBGbitmap,play_audio_callback pCallback,void* dwUser,IMediaPlayerEvent* pVideoCallback /*= NULL*/)
{
	if(FindByHand((HWND)hPlayHand) == NULL)
	{
		AddShowVideo((PBITMAP)hBGbitmap,(HWND)hPlayHand,DRAWTYPEBX);
	}

	if(InterlockedExchange(&m_isVideoPlaying,m_isVideoPlaying))
	{
		return false;
	}
	m_bIsPlayLocal = true;
	strcpy(m_szPlayVideoURL,szURL);
	if(m_playFile == NULL)
		m_playFile = new CPlayFile();

    m_pShowCallback = pVideoCallback;

	if(m_playFile)
	{
		m_playFile->OpenLocalMediaFile(szURL);
		if(m_playFile->getExistVideoStream())
		{
			m_threadShowVideo.Begin(_ShowVideo,this);
		}

		if(m_playFile->getExistAudioStream())
		{
			m_playFile->audio_open(pCallback,dwUser);
		}
		InterlockedExchange(&m_isVideoPlaying,1);
		strcpy(m_szPlayVideoURL,szURL);
		m_bIsShow = true;
		return true;
	}
	return false;
}

bool CPlayVideoUnit::PauseFile(bool bIsPause)
{
	if(m_playFile)
	{
		return m_playFile->pause_in_file(bIsPause);
	}
	return false;
}

bool CPlayVideoUnit::getPalyStutas()
{
	if(m_playFile)
	{
		return m_playFile->getPlayStutas();
	}
	return false;
}

bool CPlayVideoUnit::SeekFile(unsigned int nPalyPos)
{
	if(m_playFile)
	{
		return m_playFile->seek_in_file(nPalyPos);
	}
	return false;
}

bool CPlayVideoUnit::SwitchPaly(play_audio_callback pCallback,void* dwUser,bool bIsFlag)
{
	if(m_playFile)
	{
		return m_playFile->switch_paly(pCallback,dwUser,bIsFlag);
	}
	return false;
}

unsigned int CPlayVideoUnit::GetFileDuration()
{
	if(m_playFile)
	{
		return m_playFile->get_in_file_duration();
	}
	return 0;
}
unsigned int CPlayVideoUnit::getFileCurPlayTime()
{
	if(m_playFile)
	{
		return m_playFile->get_in_file_current_play_time();
	}
	return 0;
}

void CPlayVideoUnit::SetCallBack(IMediaPlayerEvent* pCallback)
{
   	m_mapShowLock.Lock();
    m_pShowCallback = pCallback;
    m_mapShowLock.Unlock();
}

void CPlayVideoUnit::stopFile()
{
	m_bIsShow = false;
    m_pShowCallback = NULL;
	if(InterlockedExchange(&m_isVideoPlaying,m_isVideoPlaying))
	{
		if(m_playFile)
		{
			m_threadShowVideo.End();
			m_playFile->CloseLocalMediaFile();
			delete m_playFile;
			m_playFile = NULL;
		}
	}
}

bool   CPlayVideoUnit::bIsHaveVideo()
{
	if(m_playFile)
	{
		return m_playFile->getExistVideoStream();
	}
	return false;
}

void  CPlayVideoUnit::SDLPlayAudio(void *udata,Uint8 *stream,int len)
{
	if(m_playFile && m_playFile->getExistAudioStream())
	{
		m_playFile->audio_play(udata,stream,len,m_nPlayAudioTime);

	}
}

void  CPlayVideoUnit::StopShowVideo()
{
    m_ShowLock.Lock();
    m_bIsShow = false;
    m_pShowCallback = NULL;
    m_ShowLock.Unlock();
}