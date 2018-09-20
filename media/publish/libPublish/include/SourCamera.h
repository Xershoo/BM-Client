#pragma once
#include "amcap.h"
#include "SourData.h"
#if PW_OE_VIDEO_ENHANCE
#include "ave_global.h"
#endif
struct CamNode
{
	int nIndex;
	char szCamName[256];
	CamNode()
	{
		szCamName[0] = '\0';
		nIndex = -1;
	}
};

class CSourCamera:ISourData
{
public:
	CSourCamera(HWND hShowWindow,int nDesVideoWidth,int nDesVideoHeight,string sname);
	virtual ~CSourCamera();
public:
	virtual bool GetBuffer(unsigned char** pBuf,unsigned int& nBufSize);

	virtual void GetVideoWAndH(int& nW,int& nH);

	virtual long GetVideoFPS();

	virtual bool SetSourType(int nSourType);

	virtual bool SourOpen(void* param) ;

	virtual bool SourClose(bool bIsColseDev);

	virtual bool AVE_SetID(int nMainValue, int nSubValue, int nParams[2]);

	virtual HWND GetShowHand() {return m_hShowWindow;}

	virtual int  getSourID() ;

public:
	long   GetDevCameraCount(bool bIsReload = false);
	bool   GetDevCameraName(long nIndex,char szName[256]);
	bool   bIsCameLawful(char* szName);
	void ShowImage(unsigned char* pBuf);
	virtual void ShowImage(unsigned char* pBuf,unsigned int nBufSize,int nVW,int nVH);
	void ShowBG();

	virtual void GetShowHwndRect(ScRect& rc);
private:

	AMCaputer*        m_pAMCapturer;
	AMCaputer*        m_pAMCapturer4A3;
	AMCaputer*        m_pAMCapturer16A9;

	long		      m_nCameraBufferSize;
	unsigned char *	  m_pCameraBuffer ;
	unsigned char *   m_showImageBuf;
	int       m_iSrcVideoWidth;
	int       m_iSrcVideoHeight;
	int       m_nDestVideoWidth;
	int       m_nDestVideoHeight
		;
	bool      m_bIsEffective;
	bool         m_bIsStart;
	BITMAPINFO* m_pBmpInfo;//BmpInfo÷∏’Î
	HWND  m_hShowWindow;
	RECT  m_posRect;
	bool  m_bDrawLocRect;
	int   m_nSourID;

	int   m_nUnLawfulNum;
	int   m_nCamNum;
	CamNode* m_arrCamNode[16];
	char*  m_szUnLawFul[64];

	unsigned char* m_nBGBuf;
	unsigned int   m_nBGSize;
	unsigned int   m_nBGW;
	unsigned int   m_nBGH;

private:
	long		      m_nCameraBufferSizeYUV;
#if PW_OE_VIDEO_ENHANCE
	AVE_Video_Frame   *pInVFrame;
	AVE_Video_Frame   *pOutVFrame;
#endif
	unsigned char     * m_pInCameraBufferYUV;
	unsigned char     * m_pOutCameraBufferYUV;
	
	bool             *bTmpStates;
	int              SpecialEffectsProcessing(unsigned char *pBufYUV, unsigned char *pOutBufYUV, int nAVEMainID, int nAVESubID, bool bRGB,int nParams[2]);
	bool             bHandsupDetect;
	bool             bWindowsDetect;
	bool             bVoiceDetect;

	FILE             * m_fp;
	int              nEhancementMainID;
	int              nEhancementSubID;
	int              nParam[2];
	//int              nGetFrame;
	//unsigned char    * m_markImgYUV;
	unsigned char    * m_tmpImageBuf;
	void SwitchMirrorImage(unsigned char* pInBuf);
};