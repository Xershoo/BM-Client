#include "MediaConfig.h"

#define DEF_CAPSCREEN_VIDEO_WIDTH		(1024)
#define DEF_CAPSCREEN_VIDEO_HEIGHT_L	(768)
#define DEF_CAPSCREEN_VIDEO_HEIGHT_S	(576)

CMediaConfig::CMediaConfig()
{

}

CMediaConfig::~CMediaConfig()
{

}

void CMediaConfig::copyMediaParam(PublishParam pparam,StreamParam &sparam,bool capScreen /* = false */)
{    
	if(pparam.mr == LISTENERROLE)
	{
		pparam.al = AUDIONORMAL;
	}
	else
	{
		pparam.al = AUDIOMID;
	}

	pparam.al = AUDIOMID;

	if(capScreen) {
		getCapScreenConfig(pparam,sparam);
	}else {
		getMediaConfig(pparam.mb,pparam.ms,pparam.mr,pparam.ml,pparam.al,pparam.scale,pparam.bIsManyCamera,pparam.bIsHighAudio,sparam);
		sparam.nVUNum = pparam.nVUNum;
		memcpy(&sparam.VU,&pparam.VU,sizeof(pparam.VU));
	}

	sparam.bAudioKaraoke = pparam.bAudioKaraoke;
	sparam.bDrawLocRect = pparam.bDrawLocRect;
	sparam.bIsFullScreen = pparam.bIsFullScreen;
	sparam.bIsPublish = pparam.bIsPublish;
	sparam.hShowHwnd = pparam.hShowHwnd;
	sparam.nSelectMicID = pparam.nSelectMicID;
}

void CMediaConfig::getMediaConfig(MediabBusiness mb,MasterSlave ms,MediaRole mr,MediaLevel ml,AudioLevel al,SHOWSCALE ss,
								 bool bIsManyCamera,bool bIsHighAudio,StreamParam &param)
{
	if(mb == EDUCATION)
	{
		if(ms == MAINCAMERA)
		{
			if(mr == ANCHORROLE)
			{
				if(ml == SMOOTHLEVEL)
				{
					getAnchorMainSmoothConfig(param,bIsManyCamera,bIsHighAudio,ss,al);
				}
				else if(ml == STANDARRDLEVEL)
				{
					getAnchorMainStandConfig(param,bIsManyCamera,bIsHighAudio,ss,al);
				}
				else if(ml == HIGHLEVEL)
				{
					getAnchorMainHighConfig(param,bIsManyCamera,bIsHighAudio,ss,al);
				}
				else if(ml==SUPERHIGHLEVEL)
				{
					getAnchorMainMaxHighConfig(param,bIsManyCamera,bIsHighAudio,ss,al);
				}

			}
			else if(mr == LISTENERROLE)
			{
				if(ml == STANDARRDLEVEL )
					getListenerStandConfig(param,ss,al);
				else if( ml== SUPERHIGHLEVEL|| ml == HIGHLEVEL)
					getListenerHighConfig(param,ss,al);
				else
					getListenerConfig(param,ss,al);
			}
			
		}
		else if(ms == SUBCAMERA)
		{
			if(mr == ANCHORROLE)
			{
				if(ml == SMOOTHLEVEL)
				{
					getAnchorSubSmoothConfig(param,bIsHighAudio,ss,al);
				}
				else if(ml == STANDARRDLEVEL)
				{
					getAnchorSubStandConfig(param,bIsHighAudio,ss,al);
				}
				else if(ml == HIGHLEVEL)
				{
					getAnchorSubHighConfig(param,bIsHighAudio,ss,al);
				}
				else if(ml==SUPERHIGHLEVEL)
				{
					getAnchorSubMaxHighConfig(param,bIsHighAudio,ss,al);
				}
			}
			else if(mr == LISTENERROLE)
			{
				if(ml == STANDARRDLEVEL)
					getListenerStandConfig(param,ss,al);
				else if( ml== SUPERHIGHLEVEL|| ml == HIGHLEVEL)
					getListenerHighConfig(param,ss,al);
				else
					getListenerConfig(param,ss,al);
			}
		}
	}
}

void CMediaConfig::getAnchorMainSmoothConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
    param.nVideoFps = 15;
	if(bIsManyCamera)
	{		
		if(ss == SHOWSCALE4A3)
		{
			param.nVideoW = 320;
			param.nVideoH = 240;
		}
		else
		{
			param.nVideoW = 320;
			param.nVideoH = 180;
		}
		param.nVideoBitRate = 120;
	}
	else
	{	
		if(ss == SHOWSCALE4A3)
		{
			param.nVideoW = 480;
			param.nVideoH = 360;
		}
		else
		{
			param.nVideoW = 480;
			param.nVideoH = 270;
		}
		param.nVideoBitRate = 200;
	}

	param.nAudioChannels = 1;

	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 64000;
	}
}

void CMediaConfig::getAnchorMainStandConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
    param.nVideoFps = 25;
	if(bIsManyCamera)
	{	
		if(ss == SHOWSCALE4A3)
		{
			param.nVideoW = 480;
			param.nVideoH = 360;
		}
		else
		{
			param.nVideoW = 480;
			param.nVideoH = 270;
		}
		param.nVideoBitRate = 240;
	}
	else
	{	
		if(ss == SHOWSCALE4A3)
		{
			param.nVideoW = 640;
			param.nVideoH = 480;
		}
		else
		{
			param.nVideoW = 640;
			param.nVideoH = 360;
		}
		param.nVideoBitRate = 300;
	}

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 64000;
	}
}

void CMediaConfig::getAnchorMainHighConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 25;

    /*
	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 640;
		param.nVideoH = 480;
	}
	else
	{
		param.nVideoW = 640;
		param.nVideoH = 360;
	}
    */
    
    if(ss == SHOWSCALE4A3)
    {
        param.nVideoW = 480;
        param.nVideoH = 360;
    }
    else
    {
        param.nVideoW = 480;
        param.nVideoH = 270;
    }

	param.nVideoBitRate = 500;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 64000;
	}
}

void CMediaConfig::getAnchorMainMaxHighConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
		// ”∆µ
	param.nVideoFps = 25;
	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 1280;
		param.nVideoH = 960;
		param.nVideoBitRate = 1000;
	}
	else
	{
		param.nVideoW = 1280;
		param.nVideoH = 720;
		param.nVideoBitRate = 1000;
	}

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 128000;
	}
	else
	{
		param.nAudioBitRate = 64000;
	}
}

void CMediaConfig::getAnchorSubSmoothConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 15;
	param.nVideoBitRate = 80;

	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 240;
		param.nVideoH = 180;
	}
	else
	{
		param.nVideoW = 320;
		param.nVideoH = 180;
	}


	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}

	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 32000;
	}
}

void CMediaConfig::getAnchorSubHighConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 20;

	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 320;
		param.nVideoH = 240;
	}
	else
	{
		param.nVideoW = 320;
		param.nVideoH = 180;
	}

	param.nVideoBitRate = 120;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}

	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 96000;
	}
	else
	{
		param.nAudioBitRate = 64000;
	}
}

void CMediaConfig::getAnchorSubStandConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 20;
	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 240;
		param.nVideoH = 180;
	}
	else
	{
		param.nVideoW = 320;
		param.nVideoH = 180;
	}
	param.nVideoBitRate = 80;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}

	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 48000;
	}
}

void CMediaConfig::getAnchorSubMaxHighConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 18;
	if(ss == SHOWSCALE4A3)
	{
		param.nVideoW = 640;
		param.nVideoH = 480;
	}
	else
	{
		param.nVideoW = 640;
		param.nVideoH = 360;
	}
	param.nVideoBitRate = 500;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	//“Ù∆µ
	if(bIsHighAudio)
	{
		param.nAudioBitRate = 64000;
	}
	else
	{
		param.nAudioBitRate = 48000;
	}
}

void CMediaConfig::getListenerConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 12;
	param.nVideoW = 128;
	param.nVideoH = 96;
	param.nVideoBitRate = 30;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}

	param.nAudioBitRate = 64000;
}

void CMediaConfig::getListenerStandConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 20;
	param.nVideoW = 320;
	param.nVideoH = 240;
	param.nVideoBitRate = 120;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	param.nAudioBitRate = 64000;
}

void CMediaConfig::getListenerHighConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al)
{
	// ”∆µ
	param.nVideoFps = 25;
	param.nVideoW = 640;
	param.nVideoH = 480;
	param.nVideoBitRate = 400;

	param.nAudioChannels = 1;
	if(al == AUDIOLOW)
	{
		param.nAudioSmpRateCap = 8000;
	}
	else if(al == AUDIONORMAL)
	{
		param.nAudioSmpRateCap = 16000;
	}
	else if(al == AUDIOMID)
	{
		param.nAudioSmpRateCap = 32000;
	}
	else
	{
		param.nAudioSmpRateCap = 44100;
	}
	param.nAudioBitRate = 64000;
}

void CMediaConfig::getCapScreenConfig(PublishParam pparam,StreamParam& param)
{
	param.nAudioSmpRateCap = 32000;
	param.nAudioChannels = 2;
	param.nAudioBitRate = 64000;
	param.nVideoFps = 5;
	param.nVideoBitRate = 2048;

	int nWndWidth = 0;
	int nWndHeight = 0;
	
	if( NULL == pparam.hShowHwnd) {
		 nWndWidth = GetSystemMetrics(SM_CXSCREEN);
		 nWndHeight =  GetSystemMetrics(SM_CYSCREEN); 
	} else {
		RECT rectWnd = { 0 };
		GetWindowRect(pparam.hShowHwnd,&rectWnd);

		nWndWidth = rectWnd.right - rectWnd.left;
		nWndHeight = rectWnd.bottom - rectWnd.top;
	}
	
	float rwh = (float)nWndHeight / (float)nWndWidth;
	if(rwh < 0.75){
		param.nVideoW = DEF_CAPSCREEN_VIDEO_WIDTH;
		param.nVideoH = DEF_CAPSCREEN_VIDEO_HEIGHT_S;

		param.nVideoW = nWndWidth;
		param.nVideoH = (int)((float)(nWndWidth*9)/(float)16);
	}else{
		param.nVideoW = DEF_CAPSCREEN_VIDEO_WIDTH;
		param.nVideoH = DEF_CAPSCREEN_VIDEO_HEIGHT_L;

		param.nVideoW = nWndWidth;
		param.nVideoH = (int)((float)(nWndWidth*3)/(float)4);
	}

	//param.nVideoW = nWndWidth * 3 / 4;
	//param.nVideoH = nWndHeight * 3 / 4;

	memcpy(&param.VU,&pparam.VU,sizeof(pparam.VU));
	param.nVUNum = 1;
	param.VU[0].nType = SOURCESCREEN;
}