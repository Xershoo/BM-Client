//ChatHistoryMgr.h

#pragma once

#include "OperationDB.h"
#include "biz/interface/data.h"
#include "../../common/varname.h"

namespace biz
{
	struct SLTextMsgInfo;
}

class CChatHistoryMgr
{
public:

	CChatHistoryMgr()
	{
		strcpy_s(m_szTable, "ChatHistory");
	}
	
	~CChatHistoryMgr(){};

public:

	bool Init(LPSTR pszError);

	bool UnInit();

	bool Write(biz::SLTextMsgInfo SLDInfo, int nUserID, char *pszSendName, LPSTR pszError);

	bool Write(biz::SLTextMsgInfo SLDInfo, int nUserID, char *pszSendName, int nGender , int nAuthority, LPSTR pszError);

	//讨论
	bool Read(int nClassID, int nUserID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError);
	//分组讨论
	bool Read(int nClassID, int nUserID, int nGroupID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError);
	//私聊
	bool Read2(int nClassID, int nUserID, int nOtherID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError);

	bool Clear(int nClassID, LPSTR pszError);

protected:

	int Write(LPSTR pszSQL, LPSTR pszError);

	int Read(LPSTR pszSQL, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError);

	void GetDBFilePath(LPSTR pszFile);

private:

	COperationDB m_OperaDB;

	char		 m_szTable[20];
};