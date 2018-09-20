#include "ChatHistoryMgr.h"
#include "Env.h"
#include "../../common/Env.h"
#include <QDateTime>

#define MSG_MAX_LENGTH	(MAXTEXTLEN*2)

bool CChatHistoryMgr::Init(LPSTR pszError)
{
	char szFile[MAX_PATH] = {0};

	GetDBFilePath(szFile);

	return m_OperaDB.Init(szFile, pszError);
}

bool CChatHistoryMgr::UnInit()
{
	return m_OperaDB.UnInit();
}

bool CChatHistoryMgr::Write(biz::SLTextMsgInfo SLData, int nUserID, char *pszSendName,LPSTR pszError)
{
    QDateTime time_t = QDateTime::fromTime_t(SLData._nTimer);
    QString time = time_t.toString("yyyy-MM-dd HH:mm:ss");

	if (SLData._szMsg[2] == 0)
	{
		SLData._szMsg[2] = ' ';
	}

	QString strMsg = QString::fromWCharArray(SLData._szMsg, SLData._nMsgLen);
	int nLen = strMsg.toLocal8Bit().length();
	char szContent[MSG_MAX_LENGTH] = {0};
	strncpy(szContent, strMsg.toStdString().c_str(), nLen);
	
    int CID = SLData._nClassRoomId;
	int nSD = SLData._nSendUserId;
	int nRD = SLData._nRecvUserId;
	int GID = SLData._nGroupId;
	int nType = SLData._nMsgType;

	char szSQL1[MSG_MAX_LENGTH] = {0};
	sprintf(szSQL1, "insert into %s ([CID],[UserID],[USendID],[SendUserName],[URecvID],[GroupID],[Type],[Content],[Length],[tm]) values (%d, %d, %d, '%s', %d, %d, %d,", 
			m_szTable, CID, nUserID, nSD, pszSendName, nRD, GID, nType);
	char szSQL[MSG_MAX_LENGTH] = {0};
	sprintf(szSQL, "%s '%s', %d, '%s');", szSQL1, szContent, nLen, time.toStdString().c_str());
	int nRes = Write(szSQL, pszError);
	if (!nRes)
	{
		return true;
	}
	return false;
}

bool CChatHistoryMgr::Write(biz::SLTextMsgInfo SLData, int nUserID, char *pszSendName, int nGender , int nAuthority, LPSTR pszError)
{
    QDateTime time_t = QDateTime::fromTime_t(SLData._nTimer);
    QString time = time_t.toString("yyyy-MM-dd HH:mm:ss");

	if (SLData._szMsg[2] == 0)
	{
		SLData._szMsg[2] = ' ';
	}

	QString strMsg = QString::fromWCharArray(SLData._szMsg, SLData._nMsgLen);
	TCHAR nLen = strMsg.toLocal8Bit().length();
    
	char szContent[MSG_MAX_LENGTH] = {0};
    strncpy(szContent, strMsg.toStdString().c_str(), nLen);
	int CID = SLData._nClassRoomId;
	int nSD = SLData._nSendUserId;
	int nRD = SLData._nRecvUserId;
	int GID = SLData._nGroupId;
	int nType = SLData._nMsgType;

	char szSQL1[MSG_MAX_LENGTH] = {0};
	sprintf(szSQL1, "insert into %s ([CID],[UserID],[USendID],[SendUserName],[URecvID],[GroupID],[Type],[Content],[Length],[tm],[Gender],[Authority]) values (%d, %d, %d, '%s', %d, %d, %d,", 
		m_szTable, CID, nUserID, nSD, pszSendName, nRD, GID, nType);
	char szSQL[MSG_MAX_LENGTH] = {0};
	sprintf(szSQL, "%s '%s', %d, '%s', %d, %d);", szSQL1, szContent, nLen, time.toStdString().c_str(), nGender, nAuthority);
	int nRes = Write(szSQL, pszError);
	if (!nRes)
	{
		return true;
	}
	return false;
}

bool CChatHistoryMgr::Read(int nClassID, int nUserID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError)
{
	char szSQL[MAX_PATH] = {0};
	sprintf(szSQL, "select * from %s where CID=%d and UserID=%d and URecvID=0;", m_szTable, nClassID, nUserID);
	if (!Read(szSQL, CallBackFunc, pUserData, pszError))
	{
		return true;
	}
	return false;
}

bool CChatHistoryMgr::Read(int nClassID, int nUserID, int nGroupID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError)
{
	char szSQL[MAX_PATH] = {0};
	sprintf(szSQL, "select * from %s where CID=%d and UserID=%d and GroupID=%d;", m_szTable, nClassID, nUserID, nGroupID);
	if (!Read(szSQL, CallBackFunc, pUserData, pszError))
	{
		return true;
	}
	return false;
}

bool CChatHistoryMgr::Read2(int nClassID, int nUserID, int nOtherID, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError)
{
	char szSQL[MAX_PATH] = {0};
	sprintf(szSQL, "select * from %s where CID=%d and UserID=%d and (URecvID=%d and USendID=%d) or (URecvID=%d and USendID=%d) ;", m_szTable, nClassID, nUserID, nOtherID, nUserID, nUserID, nOtherID);
	if (!Read(szSQL, CallBackFunc, pUserData, pszError))
	{
		return true;
	}
	return false;
}

int CChatHistoryMgr::Write(LPSTR pszSQL, LPSTR pszError)
{
	return m_OperaDB.Write(pszSQL, pszError);
}

int CChatHistoryMgr::Read(LPSTR pszSQL, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError)
{
	return m_OperaDB.Read(pszSQL, CallBackFunc, pUserData, pszError);
}

bool CChatHistoryMgr::Clear(int nClassID, LPSTR pszError) 
{
	char szSQL[MAX_PATH] = {0};
	sprintf(szSQL, "delete from %s where CID=%d;", m_szTable, nClassID);
	return m_OperaDB.Write(szSQL, pszError);
}

void CChatHistoryMgr::GetDBFilePath(LPSTR pszFile)
{
	QString strPath(Env::strExePath);
    strPath = QString("%1..\\data\\ChatMG.db").arg(strPath);
}
