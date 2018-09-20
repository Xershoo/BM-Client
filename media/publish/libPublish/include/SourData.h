#pragma once
#include <string>
#include "StreamHeader.h"
#include "MediaConfig.h"
using namespace std;
class ISourData
{
public:
	ISourData(int nType,string sname) {m_type = nType;m_name = sname; }
	virtual ~ISourData(){};
public:
	virtual bool GetBuffer(unsigned char** pBuf,unsigned int& nBufSize)=0 ;

	virtual bool SetSourType(int nSourType)=0;

	virtual bool SourOpen(void* param)=0;

	virtual bool SourClose(bool bIsColseDev)=0;

	virtual void SetShowHwnd(HWND hShow) {};

	virtual HWND GetShowHand() {return NULL;}

	virtual int GetSourType() {return m_type;}

	virtual void GetVideoWAndH(int& nW,int& nH) { nW = 0;nH = 0;}

	virtual string GetSourName()  {return m_name;}

	virtual void GetShowHwndRect(ScRect& rc) {};

	virtual int  getSourID() { return -1;}
	// Video Enhancements
	virtual bool AVE_SetID(int nMainValue, int nSubValue, int nParams[2]) = 0;
	virtual void ShowImage(unsigned char* pBuf,unsigned int nBufSize,int nVW,int nVH) {};
private:
	int    m_type;
	string m_name;
};