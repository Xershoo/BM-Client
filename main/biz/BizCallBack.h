//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�BizCallBack.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.19
//	˵���������BIZ�Ļص������Լ���UI�Ľ������ඨ���ͷ�ļ�
//	�޸ļ�¼��
//**********************************************************************

#ifndef BIZ_CALLBACK_H
#define BIZ_CALLBACK_H

#include <QtCore/QObject>
#include <string>
#include "common/varname.h"
#include "biz/interface/callback.h"

using namespace std;

struct ServerErrorInfo
{
public:
	ServerErrorInfo(int nServerState,const char* pszErrorInfo)
	{
		_nServerState = nServerState;
		if(NULL == pszErrorInfo)
		{
			_pszErrorInfo = new char[1];
			_pszErrorInfo[0] = '\0';
		}
		else
		{
			int nLen = strlen(pszErrorInfo) + 1;
			_pszErrorInfo = new char[nLen];
			memset(_pszErrorInfo,NULL,sizeof(char) * nLen);
			
			strcpy(_pszErrorInfo,pszErrorInfo);
		}
	};

    ServerErrorInfo()
    {
        _nServerState = 0;
        _pszErrorInfo = new char[1];
        _pszErrorInfo[0] = '\0';
    };

    ServerErrorInfo(const ServerErrorInfo& a)
    {
        _nServerState = a._nServerState;

        if(NULL == a._pszErrorInfo)
        {
            _pszErrorInfo = new char[1];
            _pszErrorInfo[0] = '\0';
        }
        else
        {
            int nLen = strlen(a._pszErrorInfo) + 1;
            _pszErrorInfo = new char[nLen];
            memset(_pszErrorInfo,NULL,sizeof(char) * nLen);
            strcpy(_pszErrorInfo,a._pszErrorInfo);
        }
    };

	~ServerErrorInfo()
	{
		if(NULL != _pszErrorInfo)
		{
			delete[] _pszErrorInfo;
			_pszErrorInfo = NULL;
		}
	};

public:
	int		_nServerState;
	char *	_pszErrorInfo;
};

struct LoginInfo
{
public:
	LoginInfo(__int64 nUserId, char utype)
	{
		_nUserId = nUserId;
		_utype = utype;
	};

    LoginInfo()
    {
        _nUserId = 0;
        _utype = 0;
    };

    LoginInfo(const LoginInfo& a)
    {
        _nUserId = a._nUserId;
        _utype = a._utype;
    };

public:
	__int64		_nUserId;
	char		_utype;
};

struct ClassErrorInfo
{
public:
	ClassErrorInfo(__int64 nRoomId, int nErrorCode)
	{
		_nRoomId = nRoomId;
		_nErrorCode = nErrorCode;
	};

    ClassErrorInfo()
    {
        _nRoomId = 0;
        _nErrorCode = 0;
    };

    ClassErrorInfo(const ClassErrorInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nErrorCode = a._nErrorCode;
    };

public:
	__int64		_nRoomId;
	int			_nErrorCode;
};

struct UserLeaveInfo
{
public:
	UserLeaveInfo(__int64 nUserId, __int64 nRoomId, __int64 nOptId)
	{
		_nUserId = nUserId;
		_nRoomId = nRoomId;
		_nOptId = nOptId;
	};

    UserLeaveInfo()
    {
        _nUserId = 0;
        _nRoomId = 0;
        _nOptId = 0;
    };

    UserLeaveInfo(const UserLeaveInfo& a)
    {
        _nUserId = a._nUserId;
        _nRoomId = a._nRoomId;
        _nOptId =  a._nOptId;
    };

public:
	__int64 _nUserId;
	__int64 _nRoomId;
	__int64 _nOptId;
};

struct DiscussGroupOptInfo
{
public:
	DiscussGroupOptInfo(int nOptType, __int64 nRoomId, const wchar_t* pTopic)
	{
		_nOptType = nOptType;
		_nRoomId = nRoomId;

		if(NULL == pTopic)
		{
			_pTopic = new wchar_t[1];
			_pTopic[0] = L'\0';
		}
		else
		{
			int nLen = wcslen(pTopic) + 1;
			_pTopic = new wchar_t[nLen];
			memset(_pTopic,NULL,sizeof(wchar_t) * nLen);
			wcscpy(_pTopic,pTopic);
		}
	};

    DiscussGroupOptInfo()
    {
        _nOptType = 0;
        _nRoomId = 0;

        _pTopic = new wchar_t[1];
        _pTopic[0] = L'\0';
        
    };

    DiscussGroupOptInfo(const DiscussGroupOptInfo& a)
    {
        _nOptType = a._nOptType;
        _nRoomId = a._nRoomId;

        if(NULL == a._pTopic)
        {
            _pTopic = new wchar_t[1];
            _pTopic[0] = L'\0';
        }
        else
        {
            int nLen = wcslen(a._pTopic) + 1;
            _pTopic = new wchar_t[nLen];
            memset(_pTopic,NULL,sizeof(wchar_t) * nLen);
            wcscpy(_pTopic,a._pTopic);
        }
    };

	~DiscussGroupOptInfo()
	{
		if(NULL != _pTopic)
		{
			delete[] _pTopic;
			_pTopic = NULL;
		}
	};
public:
	int			_nOptType;
	__int64		_nRoomId;
	wchar_t*	_pTopic;
};

struct UserSpeakActionInfo
{
public:
	UserSpeakActionInfo(__int64 nRoomId, __int64 nUserId, int nAction)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
		_nAction = nAction;
	}

    UserSpeakActionInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
        _nAction = 0;
    }

    UserSpeakActionInfo(const UserSpeakActionInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
        _nAction = a._nAction;
    }

public:
	__int64		_nRoomId;
	__int64		_nUserId;
	int			_nAction;
};

struct TeacherEnterDiscussGroupInfo
{
public:
	TeacherEnterDiscussGroupInfo(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt)
	{
		_nRoomId = nRoomId;
		_nGroupId = nGroupId;
		_nTeacherId = nTeacherId;
		_nOpt = nOpt;
	};

    TeacherEnterDiscussGroupInfo()
    {
        _nRoomId = 0;
        _nGroupId = 0;
        _nTeacherId = 0;
        _nOpt = 0;
    };

    TeacherEnterDiscussGroupInfo(const TeacherEnterDiscussGroupInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nGroupId = a._nGroupId;
        _nTeacherId = a._nTeacherId;
        _nOpt = a._nOpt;
    };

public:
	int		_nRoomId;
	int		_nGroupId;
	__int64 _nTeacherId;
	int		_nOpt;
};

struct GroupCallTeacherInfo
{
public:
	GroupCallTeacherInfo(__int64 nRoomId, __int64 nUserId)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
	};

    GroupCallTeacherInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
    };

    GroupCallTeacherInfo(const GroupCallTeacherInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
};

struct ClassStateChangeInfo
{
public:
	ClassStateChangeInfo(__int64 nCourseId, __int64 nClassId, char nState, __int64 nUserId,int ret)
	{
		_nCourseId = nCourseId;
		_nClassId = nClassId;
		_nState = nState;
		_nUserId = nUserId;
		_ret = ret;
	};

    ClassStateChangeInfo()
    {
        _nCourseId = 0;
        _nClassId = 0;
        _nState = 0;
        _nUserId = 0;
        _ret = 0;
    };

    ClassStateChangeInfo(const ClassStateChangeInfo& a)
    {
        _nCourseId = a._nCourseId;
        _nClassId = a._nClassId;
        _nState = a._nState;
        _nUserId = a._nUserId;
        _ret = a._ret;
    };

public:
	__int64		_nCourseId;
	__int64		_nClassId;
	char		_nState;
	__int64		_nUserId;
	int			_ret;
};

struct TopVideoChangeInfo
{
public:
	TopVideoChangeInfo(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
		_nTeacherId = nTeacherId;
		_bAddToTop = bAddToTop;
	};

    TopVideoChangeInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
        _nTeacherId = 0;
        _bAddToTop = 0;
    };

    TopVideoChangeInfo(const TopVideoChangeInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
        _nTeacherId = a._nTeacherId;
        _bAddToTop = a._bAddToTop;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
	__int64		_nTeacherId;
	bool		_bAddToTop;
};

struct UploadTokenInfo
{
public:
	UploadTokenInfo(int retCode,__int64 nUserId,int nValidateTime,const char* pszToken)
	{
		_retCode = retCode;
		_nUserId = nUserId;
		_nValidateTime = nValidateTime;

		if(NULL == pszToken)
		{
			_pszToken = new char[1];
			_pszToken[0] = '\0';
		}
		else
		{
			int nLen = strlen(pszToken) + 1;
			_pszToken = new char[nLen];
			memset(_pszToken,NULL,sizeof(char) * nLen);
			strcpy(_pszToken,pszToken);
		}
	};

    UploadTokenInfo()
    {
        _retCode = 0;
        _nUserId = 0;
        _nValidateTime = 0;

        _pszToken = new char[1];
        _pszToken[0] = '\0';        
    };

    UploadTokenInfo(const UploadTokenInfo& a)
    {
        _retCode = a._retCode;
        _nUserId = a._nUserId;
        _nValidateTime = a._nValidateTime;

        if(NULL == a._pszToken)
        {
            _pszToken = new char[1];
            _pszToken[0] = '\0';
        }
        else
        {
            int nLen = strlen(a._pszToken) + 1;
            _pszToken = new char[nLen];
            memset(_pszToken,NULL,sizeof(char) * nLen);
            strcpy(_pszToken,a._pszToken);
        }
    };

	~UploadTokenInfo()
	{
		if(NULL != _pszToken)
		{
			delete[] _pszToken;
			_pszToken = NULL;
		}
	};

public:
	int			_retCode;
	__int64		_nUserId;
	int			_nValidateTime;
	char*		_pszToken;
};

struct StudentVideoListInfo
{
public:
	StudentVideoListInfo(__int64 nRoomId,std::vector<__int64>& stuVideoList)
	{
		_nRoomId = nRoomId;
		_stuVideoList = stuVideoList;
	};

    StudentVideoListInfo()
    {
        _nRoomId = 0;        
    };

    StudentVideoListInfo(const StudentVideoListInfo& a)
    {
        _nRoomId = a._nRoomId;
        _stuVideoList = a._stuVideoList;
    };

	~StudentVideoListInfo()
	{
		_stuVideoList.clear();
	};

public:
	__int64					_nRoomId;
	std::vector<__int64>	_stuVideoList;
};

struct PrevCoursewareListInfo
{
public:
	PrevCoursewareListInfo(__int64 nCourseId,std::vector<biz::SLCoursewareInfo>& cwList)
	{
		_nCourseId = nCourseId;
		_cwList = cwList;
	};

    PrevCoursewareListInfo()
    {
        _nCourseId = 0;      
    };

    PrevCoursewareListInfo(const PrevCoursewareListInfo& a)
    {
        _nCourseId = a._nCourseId;
        _cwList = a._cwList;
    };

	~PrevCoursewareListInfo()
	{
		_cwList.clear();
	};

public:
	__int64			_nCourseId;
	std::vector<biz::SLCoursewareInfo> _cwList;
};

struct SystemMessageInfo
{
public:
	SystemMessageInfo(__int64 nUserId,const wchar_t* pwszMsg)
	{
		_nUserId = nUserId;

		if(NULL == pwszMsg)
		{
			_pwszMsg = new wchar_t[1];
			_pwszMsg[0] = L'\0';
		}
		else
		{
			int nLen = wcslen(pwszMsg) + 1;
			_pwszMsg = new wchar_t[nLen];
			memset(_pwszMsg,NULL,sizeof(wchar_t) * nLen);
			wcscpy(_pwszMsg,pwszMsg);
		}
	};

    SystemMessageInfo()
    {
        _nUserId = 0;

        _pwszMsg = new wchar_t[1];
        _pwszMsg[0] = L'\0';        
    };

    SystemMessageInfo(const SystemMessageInfo& a)
    {
        _nUserId = a._nUserId;

        if(NULL == a._pwszMsg)
        {
            _pwszMsg = new wchar_t[1];
            _pwszMsg[0] = L'\0';
        }
        else
        {
            int nLen = wcslen(a._pwszMsg) + 1;
            _pwszMsg = new wchar_t[nLen];
            memset(_pwszMsg,NULL,sizeof(wchar_t) * nLen);
            wcscpy(_pwszMsg,a._pwszMsg);
        }
    };

	~SystemMessageInfo()
	{
		if(NULL != _pwszMsg)
		{
			delete []_pwszMsg;
			_pwszMsg = NULL;
		}
	};

public:
	__int64		_nUserId;
	wchar_t*	_pwszMsg;
};

struct TeacherSwitchVideoInfo
{
public:
	TeacherSwitchVideoInfo(__int64 nRoomId,__int64 nUserId,int nVideo)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
		_nVideo = nVideo;
	};

    TeacherSwitchVideoInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
        _nVideo = 0;
    };

    TeacherSwitchVideoInfo(const TeacherSwitchVideoInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
        _nVideo = a._nVideo;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
	int			_nVideo;
};

struct SetClassModeInfo
{
public:
	SetClassModeInfo(__int64 nRoomId,__int64 nUserId,int nMode)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
		_nMode = nMode;
	};

    SetClassModeInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
        _nMode = 0;
    };

    SetClassModeInfo(const SetClassModeInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
        _nMode = a._nMode;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
	int			_nMode;
};

struct SignMsgInfo
{
public:
	SignMsgInfo(__int64 nRoomId,__int64 nUserId)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
	};

    SignMsgInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
    };

    SignMsgInfo(const SignMsgInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
};

struct SetAssistantInfo
{
public:
	SetAssistantInfo(__int64 nRoomId,__int64 nUserId,bool bAs,int ret)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;
		_bAs = bAs;
		_ret = ret;
	};

    SetAssistantInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;
        _bAs = 0;
        _ret = 0;
    };

    SetAssistantInfo(const SetAssistantInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;
        _bAs = a._bAs;
        _ret = a._ret;
    };

public:
	__int64		_nRoomId;
	__int64		_nUserId;
	bool		_bAs;
	int			_ret;
};

struct NetDevAccessInfo
{
public:
	NetDevAccessInfo(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName)
	{
		_nRoomId = nRoomId;
		_nUserId = nUserId;

        if(NULL == pwszDevName)
        {
            _pwszDevName = new wchar_t[1];
            _pwszDevName[0] = L'\0';
        }
        else
        {
            int nLen = wcslen(pwszDevName) + 1;
            _pwszDevName = new wchar_t[nLen];
            memset(_pwszDevName,NULL,sizeof(wchar_t) * nLen);
            wcscpy(_pwszDevName,pwszDevName);
        }
	};

    NetDevAccessInfo()
    {
        _nRoomId = 0;
        _nUserId = 0;

        _pwszDevName = new wchar_t[1];
        _pwszDevName[0] = L'\0';
    };

    NetDevAccessInfo(const NetDevAccessInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nUserId = a._nUserId;

        if(NULL == a._pwszDevName)
        {
            _pwszDevName = new wchar_t[1];
            _pwszDevName[0] = L'\0';
        }
        else
        {
            int nLen = wcslen(a._pwszDevName) + 1;
            _pwszDevName = new wchar_t[nLen];
            memset(_pwszDevName,NULL,sizeof(wchar_t) * nLen);
            wcscpy(_pwszDevName,a._pwszDevName);
        }
    };

    ~NetDevAccessInfo()
    {
        if(NULL != _pwszDevName)
        {
            delete[] _pwszDevName;
            _pwszDevName = NULL;
        }
    };

public:
	__int64 _nRoomId;
	__int64 _nUserId;
	wchar_t*  _pwszDevName;
};

struct ChooseMobileInfo
{
public:
	ChooseMobileInfo(__int64 nTeacherId,__int64 nUserId,int ret,byte nAct)
	{
		_nTeacherId = nTeacherId;
		_nUserId = nUserId;
		_ret = ret;
		_nAct = nAct;
	};

    ChooseMobileInfo()
    {
        _nTeacherId = 0;
        _nUserId = 0;
        _ret = 0;
        _nAct = 0;
    };

    ChooseMobileInfo(const ChooseMobileInfo& a)
    {
        _nTeacherId = a._nTeacherId;
        _nUserId = a._nUserId;
        _ret = a._ret;
        _nAct = a._nAct;
    };

public:
	__int64 _nTeacherId;
	__int64 _nUserId;
	int		_ret;
	byte	_nAct;
};

struct MobileNotifyInfo
{
public:
	MobileNotifyInfo(__int64 nTeacherId,__int64 nUserId)
	{
		_nTeacherId = nTeacherId;
		_nUserId = nUserId;		
	};

    MobileNotifyInfo()
    {
        _nTeacherId = 0;
        _nUserId = 0;		
    };

    MobileNotifyInfo(const MobileNotifyInfo& a)
    {
        _nTeacherId = a._nTeacherId;
        _nUserId = a._nUserId;		
    };

public:
	__int64 _nTeacherId;
	__int64 _nUserId;
};

struct MessageListInfo
{
public:
	MessageListInfo(__int64 nUserId,int nMsgType,std::vector<biz::SLMessageInfo>& msgList)
	{
		_nUserId = nUserId;
		_nMsgType = nMsgType;
		_msgList = msgList;
	};

    MessageListInfo()
    {
        _nUserId = 0;
        _nMsgType = 0;        
    };

    MessageListInfo(const MessageListInfo& a)
    {
        _nUserId = a._nUserId;
        _nMsgType = a._nMsgType;
        _msgList = a._msgList;
    };

public:
	__int64								_nUserId;
	int									_nMsgType;
	std::vector<biz::SLMessageInfo>		_msgList;
};

struct AutoLoginTokenInfo
{
public:
	AutoLoginTokenInfo(__int64 nUserId,int retCode,const WCHAR* pwszToken)
	{
		_retCode = retCode;
		_nUserId = nUserId;
	
		if(NULL == pwszToken)
		{
			_pwszToken = new WCHAR[1];
			_pwszToken[0] = '\0';
		}
		else
		{
			int nLen = wcslen(pwszToken) + 1;
			_pwszToken = new WCHAR[nLen];
			memset(_pwszToken,NULL,sizeof(WCHAR) * nLen);
			wcscpy(_pwszToken,pwszToken);
		}
	};

    AutoLoginTokenInfo()
    {
        _retCode = 0;
        _nUserId = 0;

        _pwszToken = new WCHAR[1];
        _pwszToken[0] = '\0';
    };

    AutoLoginTokenInfo(const AutoLoginTokenInfo& a)
    {
        _retCode = a._retCode;
        _nUserId = a._nUserId;

        if(NULL == a._pwszToken)
        {
            _pwszToken = new WCHAR[1];
            _pwszToken[0] = '\0';
        }
        else
        {
            int nLen = wcslen(a._pwszToken) + 1;
            _pwszToken = new WCHAR[nLen];
            memset(_pwszToken,NULL,sizeof(WCHAR) * nLen);
            wcscpy(_pwszToken,a._pwszToken);
        }
    };

	~AutoLoginTokenInfo()
	{
		if(NULL != _pwszToken)
		{
			delete[] _pwszToken;
			_pwszToken = NULL;
		}
	};

public:
	int			_retCode;
	__int64		_nUserId;
	WCHAR*		_pwszToken;
};

struct ClassMsgReminderInfo
{
public:
	ClassMsgReminderInfo(__int64 nCourseId,__int64 nClassId,char nMsgType)
	{
		_nCourseId = nCourseId;
		_nClassId = nClassId;
		_nMsgType = nMsgType;
	}

    ClassMsgReminderInfo()
    {
        _nCourseId = 0;
        _nClassId = 0;
        _nMsgType = 0;
    }

    ClassMsgReminderInfo(const ClassMsgReminderInfo& a)
    {
        _nCourseId = a._nCourseId;
        _nClassId = a._nClassId;
        _nMsgType = a._nMsgType;
    }

public:
	__int64 _nCourseId;
	__int64 _nClassId ;
	char	_nMsgType ;
};


struct ClassMainShowInfo
{
public:
    ClassMainShowInfo(__int64 nRoomId,__int64 nTeacherId,char nShowType)
    {
        _nRoomId = nRoomId;
        _nTeacherId = nTeacherId ;
        _nShowType = nShowType ;
    }

    ClassMainShowInfo()
    {
        _nRoomId = 0;
        _nTeacherId = 0;
        _nShowType = 0;
    }

    ClassMainShowInfo(const ClassMainShowInfo& a)
    {
        _nRoomId = a._nRoomId;
        _nTeacherId  = a._nTeacherId ;
        _nShowType  = a._nShowType ;
    }

public:
    __int64 _nRoomId;
    __int64 _nTeacherId ;
    char	_nShowType ;
};

/////////////////////////////////////////////////////////
class CBizCallBack : public QObject
    ,public biz::ICallback
{   
    Q_OBJECT
public:
	CBizCallBack();
	virtual ~CBizCallBack();

public:
	virtual void OnConnectServerError(int nServerState, const char* pErrorInfo);
	virtual void OnLogin(__int64 nUserId, char utype);	
	virtual void OnLogKickOut(__int64 nUserId);
	virtual void OnLoginError( int nErrorCode);
	virtual void OnEnterClassError(__int64 nRoomId,int nErrorCode);
	virtual void OnError(__int64 nRoomId, int nErrorCode);

	virtual void OnUserEnter(biz::SLUserInfo& Info);
	
	//nOptId �����ߵ�ID���û��뿪����������
	virtual void OnUserLever(__int64 nUserId, __int64 nRoomId, __int64 nOptId);
	virtual void OnUserInfoReturn(biz::SLUserInfo& sInfo);

	//�װ��ϵ�һЩ�¼������ʣ����֣�������
	virtual void OnWhiteBoardEvent(biz::SLWhiteBoardEvent& sEventInfo);
	//�����µİװ壬ɾ���װ壬�û���ʾ���ذװ壬�л��װ�Ȳ���
	virtual void OnWhiteBoardOpt(biz::SLWhiteBoardOpt& sOptInfo);

	virtual void OnDiscussGroupOpt(int nOptType, __int64 nRoomId, const wchar_t* pTopic);
	virtual void OnUserSpeakAction(__int64 nRoomId, __int64 nUserId, int nAction);
	virtual void OnTeacherEnterDiscussGroup(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt);
	virtual void OnGroupCallTeacher(__int64 nRoomId, __int64 nUserId);
	
	//�μ��б��¼�
	virtual void OnCursewaveListEvent(biz::SLCursewaveListOpt& sCWCtrl);

	virtual void ShowTypeChange(biz::SLClassRoomShowInfo& sInfo);
    virtual void MainShowChange(char nType, __int64 nRoomId,__int64 nTeacherId);
	virtual void ClassStateChange(__int64 nCourseId, __int64 nClassId, char nState, __int64 nUserId,int ret);
	virtual void OnRecvClassMessage(biz::SLTextMsgInfo& sInfo);

	virtual void OnTopVideoChange(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop);
	virtual void OnRecvCourseInfo(__int64 nCourseId);

	virtual void OnUpdateUserInfo(__int64 nUserId);

	virtual void OnRecvUploadToken(int retCode,__int64 nUserId,int nValidateTime,const char* pwszToken);
	virtual void OnRecvStudentVideoList(__int64 nRoomId,std::vector<__int64>& stuVideoList);
	virtual void OnRecvPrevCoursewareList(__int64 nCourseId,std::vector<biz::SLCoursewareInfo>& cwList);

	virtual void OnRecvSystemMessage(__int64 nUserId,const wchar_t* pwszMsg);
	virtual void OnTeacherSwitchVideo(__int64 nRoomId,__int64 nUserId,int nVideo);
	virtual void OnSetClassMode(__int64 nRoomId,__int64 nUserId,char nMode);
	virtual void OnRecvSignMsg(__int64 nRoomId,__int64 nUserId);
	virtual void OnRecvSetAssistantMsg(__int64 nRoomId,__int64 nUserId,bool bAs,int ret);

	//�����豸������ˢ��
	virtual void OnRecvUpdateDevCodeMsg(__int64 nRoomId);

	//�豸�������
	virtual void OnRecvNetDevAccess(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName);

	//�ֻ�����ͷֹͣ��򿪵�Ӧ��
	virtual void OnRecvChooseMobileResponse(__int64 nTeacherId,__int64 nUserId, int ret, byte nAct);

	//�ֻ�����ͷ����
	virtual void OnRecvMobileNotify(__int64 nTeacherId,__int64 nUserId);

	//��ѯ��Ϣ�б���
	virtual void OnRecvMsgList(__int64 nUserId,int nMsgType,std::vector<biz::SLMessageInfo>& msgList);

	//��ѯ�Զ���¼TOKEN����
	virtual void OnRecvAutoLoginToken(__int64 nUserId,int ret,LPCWSTR pwszToken);

	//������Ϣ����
	virtual void OnRecvClassMsgReminder(__int64 nCourseId,__int64 nClassId,char nMsgType);

signals:
    void ConnectServerError(ServerErrorInfo info);  //����Ͽ����ӣ���ʾ��¼��
    void Login(LoginInfo info);						//��¼�ɹ�����ʾ�γ̽���
    void LogKickOut(__int64 nUserId);				//���ߵ�
    void LoginError(int nErrorCode);				//��¼ʧ��
    void EnterClassError(ClassErrorInfo info);		//������÷���
    void Error(ClassErrorInfo info);				//�������ʧ�ܻ�����÷������Ͽ�����
    void UserEnter(__int64 nUserId);				//�û����룬�����Լ��Լ������û����룬�����û�������Ҫ��ӵ��û��б��Լ�ѧ����Ƶ�б��	
    void UserLeave(UserLeaveInfo info);				//�û��뿪��
    void UserInfoReturn(__int64 nUserId);			//����û���Ϣ���γ̽��洦��
    void WhiteBoardEvent(biz::SLWhiteBoardEvent info);		//�װ��¼�
    void WhiteBoardOpt(biz::SLWhiteBoardOpt info);			//�װ���ӻ���ɾ����������
    void DiscussGroupOpt(DiscussGroupOptInfo info);			//�������ۣ�������
    void UserSpeakAction(UserSpeakActionInfo info);			//ȡ������������������򿪹ر�����Ƶ�豸��Ϣ
    void TeacherEnterDiscussGroup(TeacherEnterDiscussGroupInfo info);		//��������
    void GroupCallTeacher(GroupCallTeacherInfo info);						//��������
    void CursewaveListEvent(biz::SLCursewaveListOpt info);					//�μ����������ɾ���ر�
    void ShowTypeChanged(biz::SLClassRoomShowInfo info);					//���ý���װ�ɼ�������ʾ���ݸı�
    void MainShowChanged(ClassMainShowInfo info);                                                 //���ý�������ʾ���ݸı�
    void ClassStateChanged(ClassStateChangeInfo info);						//����״̬�ı䣬�ϿΣ��¿�
    void RecvClassMessage(biz::SLTextMsgInfo info);							//�յ�������Ϣ
    void TopVideoChange(TopVideoChangeInfo info);							//ѧ����Ƶ�б�ı�
    void RecvCourseInfo(__int64 nCourseId);									//�ݲ�����
    void UpdateUserInfo(__int64 nUserId);									//�����û���Ϣ���û��б����Ƶ�б���
    void RecvUploadToken(UploadTokenInfo info);								//�յ��ϴ�token
    void RecvStudentVideoList(StudentVideoListInfo info);					//��Ƶ�б�
    void RecvPrevCoursewareList(PrevCoursewareListInfo info);				//web�μ��б�
    void RecvSystemMessage(SystemMessageInfo info);							//���µ���Ϣ����
    void TeacherSwitchVideo(TeacherSwitchVideoInfo info);					//�л�����Ƶ
    void SetClassMode(SetClassModeInfo info);								//�����������ԣ���������
    void RecvSignMsg(SignMsgInfo info);										//������
    void RecvSetAssistantMsg(SetAssistantInfo info);						//?��������

    void RecvUpdateDevCodeMsg(__int64 nRoomId);								//��������ͷ����֤��
    void RecvNetDevAccess(NetDevAccessInfo info);							//�յ���������ͷ�Ľ���
    void RecvChooseMobileResponse(ChooseMobileInfo info);					//ѡ����������ͷ	
    void RecvMobileNotify(MobileNotifyInfo info);							//��������ͷ�˳���
    void RecvMsgList(MessageListInfo info);									//��Ϣ�б�
    void RecvAutoLoginToken(AutoLoginTokenInfo info);						//�Զ���¼token
    void RecvClassMsgReminder(ClassMsgReminderInfo info);					//����ת̬����
};

#endif