#pragma once

#include <memory.h>
#include <vector>
#include <string>

namespace biz
{
	#define MAXEDUINFO			(10)	//学生的学籍信息最多十个

	#define MINTEXTLEN			(64)	
	#define DEFTEXTLEN			(128)
	#define BIGTEXTLEN			(512)
	#define MAXTEXTLEN			(1024)

	enum EServerConnectState
	{			
		eServerState_ConFailed, //服务器连接失败
		eServerState_ConDis,    //客户端连接断开，可能是服务器主动，也可以能网络问题
		eServerState_ConClose,  //客户端主动关闭
		eServerState_connect,	//与服务器建立连接
		eServerState_AutoReconnect		//与服务器自动重连
	};

	enum EErrorType
	{
		eError_Class_Server_DisConn = 1,
		eError_Class_Not_Save,
		eError_Class_Not_Exist,
	};

	enum AUERRNO
	{
		AU_OK,
		AU_SERVERERROR = -100,
		AU_INVALID_USERNAME = -2,
		AU_INVALID_PASSWORD = -3,
		AU_ACCOUNT_FREEZE	= -6,
		AU_ACCOUNT_LOCKING	= -7,
		AU_UNKNOW,

		Had_Teacher
	};

	enum EnterClassRoomError
	{
		ENTER_CLASSROOM_SUCCESS = 0,
		ENTER_CLASSROOM_ERROR_NOT_INTIME,
		ENTER_CLASSROOM_ERROR_FINISHED,
		ENTER_CLASSROOM_ERROR_AUTHORITY,
		ENTER_CLASSROOM_ERROR_BLACKLIST,
		ENTER_CLASSROOM_ERROR_LOCK,
	};

	enum EMobileVerifyCodeType
	{
		eMobileVerifyCode_Register = 1, //注册验证码
		eMobileVerifyCode_Bind,         //绑定手机
		eMobileVerifyCode_Password = 11, //找回密码
	};

	enum EMobileVerifyCodeError
	{
		eVerifyCodeError_Ok = 0,
		eVerifyCodeError_InvalidMobile = -1,  //错误手机号
		eVerifyCodeError_Frequence = -2,      //两次发送间隔过短
		eVerifyCodeError_Toomuch_times=-3,	  //一日发送次数过多
		eVerifyCodeError_Server_error = -4,   //短信服务商错误
		eVerifyCodeError_mobile_exist = -5,   //手机号已绑定
		eVerifyCodeError_inner_server_error = -6, //内部服务器错误

		eVerifyMobile_Ok = 0,
		eVerifyMobile_Invalid_param = -1,   //非法参数
		eVerifyMobile_wrong_serialid = -2,  //serialid不存在
		eVerifyMobile_exceed_time = -3,     //验证码过期
		eVerifyMobile_wrong_verifycode = -4,//验证码错误
		eVerifyMobile_server_error = -100,  //服务器错误
	};

	enum ERegisterUserResult
	{
		eRegisterUser_Ok = 0,
		eRegisterUser_invalid_param = -1,    //非法参数
		eRegisterUser_wrong_serialid = -2,   //serialid不存在
		eRegisterUser_not_varify = -3,       //没有通过验证
		eRegisterUser_mobile_exist = -4,     //手机号已注册
		eRegisterUser_server_error = -100,   //内部服务器错误
	};

	enum PainType
	{
		PT_NONE,
		PT_PEN,
		PT_ERASER,
		PT_TXT,
		PT_UNDO = 103,
		PT_CLEAN,
		PT_LASERPOINTER, //激光笔
	};

	enum UserGender
	{
		Gender_unKnow = 0,
		Gender_Male = 1,
		Gender_Female,
	};

	enum EUserAuthority
	{
		UserAu_Visitor = 1,		//试听
		UserAu_Observer = 10,	//旁边
		UserAu_Monitor = 20,	//管理员，校长等管理者
		UserAu_Student = 30,	//学生
		UserAu_Assistant = 31,	//助教
		UserAu_Teacher = 40,	//老师
	};

	enum eRegUserType
	{
		eRegUserType_UserName = 1, //用户名注册用户
		eRegUserType_Mobile = 2,   //手机注册用户
	};

	enum EUserShowType
	{
		eShowType_Blank = 0,
		eShowType_Cursewave = 1,
		eShowType_Whiteboard = 2,		
		
		eShowType_NotBegin = 1024,
	};

    enum EMainShowType
    {
        eMainShow_Unknown = 0,
        eMainShow_CWWB,
        eMainShow_VIDEO,
    };

	enum EClassRoomState
	{
		Eclassroomstate_Free = 0,		//没有上课
		Eclassroomstate_Ready =  1,		//准备上课
		Eclassroomstate_Going = 2,		//计划上课
		EClassroomstate_Doing = 4,		//老师讲课
	};

	enum EClassMsgType
	{
		EClassMsgSign = 1,				// 计划时间前10分钟
		EClassMsgBegin = 2,				// 计划时间到
		EClassMsgSignOut = 3,			// 计划时间到前5分钟
		EClassMsgEnd = 4,				// 计划时间到
	};

	enum EClassRoomMode
	{
		Eclassroommode_Speakdisable = 0,
		Eclassroommode_Speakable = 1,
		Eclassroommode_Textable = 2,
		Eclassroommode_Textdisable = 3,
		Eclassroommode_Asideable = 4,
		Eclassroommode_Asidedisable = 5,
		Eclassroommode_Imageable = 8,
		Eclassroommode_Imagedisable = 9,
		Eclassroommode_Lock = 16,
		Eclassroommode_UnLock = 17,
	};

	enum EUserSpeekState
	{
		eUserspeekstate_Ask_speak = 1, //举手
		eUserspeekstate_cancel_Speak,  //取消举手
		eUserspeekstate_allow_speak,   //老师允许发言
		eUserspeekstate_clean_speak,   //老师清除发言

		eClassAction_Kicout,			//老师把学生踢出房间
		eClassAction_add_student_video = 6, //老师把学生添加到视频区
		eClassAction_del_student_video,    //老师把学生从视频区移开
		eClassAction_Open_voice,			//打开声音
		eClassAction_Close_voice,			//关闭声音
		eClassAction_Open_video,			//打开视频
		eClassAction_Close_video,			//关闭视频
		eClassAction_mute = 12,				//禁言
		eClassAction_unmute,				//关闭禁言
		eClassAction_teacher_enter_group,   //老师进入讨论分组
		eClassAction_teacher_leave_group = 15, //老师离开讨论分组
		eClassAction_call_teacher,			//讨论组里学生呼叫老师
	};

	enum eUserState
	{
		eUserState_ask_speak = 1,
		eUserState_speak = 2,
		eUserState_user_voice = 4,
		eUserState_user_video = 8,
		eUserState_user_mute = 16,
	};

	enum eStudentVerifyStaus
	{
		eVerifyStatus_Not = 0,
		eVerifyStatus_code_send = 1,
		eVerifyStatus_verifyed = 100,
		eVerifyStatus_verify_failed = 99,
	};

	struct SLUserEduInfo
	{
		SLUserEduInfo() 
		{
			memset(this, 0, sizeof(SLUserEduInfo));
		}
		wchar_t			szSchoolName[DEFTEXTLEN];			//学校
		wchar_t			szCollege[DEFTEXTLEN];				//学院
		wchar_t			szGrade[DEFTEXTLEN];				//班级
		wchar_t			szMajor[DEFTEXTLEN];				//专业
		wchar_t			szStudentNo[DEFTEXTLEN];			//学号
		int				nVerifyStatus;						//认证信息

	};
	
	struct SLUserSttingInfo
	{
		SLUserSttingInfo()
		{
			//MOD_CONTROL = 0x0002, MOD_ALT = 0x0001, MOD_SHIFT = 0x0004
			//BK_UP = 0x26, VK_DOWN = 0x28
			nHotKeyID[0] = 0xA001;			//nIDShowHideMainWnd
			HotKeyMod[0] = 0x0002 | 0x0004; //modShowHideMainWnd 
			HotKeyVK[0] = 0x5A;//Z

			nHotKeyID[1] = 0xA002;			//nIDPackPopUpChatWnd
			HotKeyMod[1] = 0x0004 | 0x0002;
			HotKeyVK[1] = 0x54;//T

			nHotKeyID[2] = 0xA003;			//nIDPackPopUpTechWnd 
			HotKeyMod[2] = 0x0004 | 0x0002;
			HotKeyVK[2] = 0x56;//V

			nHotKeyID[3] = 0xA004;			//nIDPackPopUpCoursewareWnd 
			HotKeyMod[3] = 0x0004 | 0x0002;
			HotKeyVK[3] = 0x44;//D

			nHotKeyID[4] = 0xA005;			//nIDCutScreen 
			HotKeyMod[4] = 0x0004 | 0x0002;
			HotKeyVK[4] = 0x41;//A

			nHotKeyID[5] = 0xA006;			//nIDOpenCloseClassRoomSound 
			HotKeyMod[5] = 0x0004 | 0x0002;
			HotKeyVK[5] = 0x58;//X

			nHotKeyID[6] = 0xA007;			//nIDOpenCloseMic 
			HotKeyMod[6] = 0x0004 | 0x0002;
			HotKeyVK[6] = 0x43;//C

			nHotKeyID[7] = 0xA008;			//nIDHandsUpDown
			HotKeyMod[7] = 0x0002;
			HotKeyVK[7] = 0x48;//0x0D;//Enter

			nHotKeyID[8] = 0xA009;			//nIDDecreaseClassRoomSound
			HotKeyMod[8] = 0x0002;
			HotKeyVK[8] = 0x26;

			nHotKeyID[9] = 0xA00A;			//nIDIncreaseClassRoomSound
			HotKeyMod[9] = 0x0002;
			HotKeyVK[9] = 0x28;
		}

		bool operator == (const SLUserSttingInfo &other)
		{
			for (int i = 0; i < 10; i++)
			{
				if (nHotKeyID[i] != other.nHotKeyID[i])
					return false;
				if (HotKeyMod[i] != other.HotKeyMod[i])
					return false;
				if (HotKeyVK[i] != other.HotKeyVK[i])
					return false;
			}
			return true;
		}

		bool operator != (const SLUserSttingInfo &other)
		{
			return !(*this == other);
		}

		int nHotKeyID[10];
		unsigned int HotKeyMod[10];
		unsigned int HotKeyVK[10];
	};

	struct SLUserInfo
	{
		SLUserInfo()
		{
			memset(this, 0, sizeof(SLUserInfo) - sizeof(SLUserSttingInfo));
		}

		SLUserInfo(__int64 uid)
		{
			memset(this, 0, sizeof(SLUserInfo) - sizeof(SLUserSttingInfo));
			nUserId = uid;
		}

		__int64				nUserId;
		wchar_t				szRealName[DEFTEXTLEN];
		wchar_t				szNickName[DEFTEXTLEN];
		wchar_t				szSignature[DEFTEXTLEN];
		EUserAuthority		nUserAuthority;		
		char				nGender;
		wchar_t				szPicUrl[MAXTEXTLEN];
		wchar_t				szPullUrl[MAXTEXTLEN];
		wchar_t				szPushUrl[MAXTEXTLEN];
		int					_nUserState;
		wchar_t				szMobile[20];
		wchar_t				szEmail[50];
		int					nDevice;
		__int64				_nClassRoomId;
		int					_nBanType; //封禁状态， 有待确认 ？？
		int					_nEduInfoSize;
		SLUserEduInfo		_sEduInfo[MAXEDUINFO];
		SLUserSttingInfo	_UnserSettingInfo;
	};

	enum EClassType
	{

	};

	struct SLCourseInfo
	{
		SLCourseInfo()
		{
			memset(this,0,sizeof(SLCourseInfo));
		}
				
		SLCourseInfo& operator=(SLCourseInfo& sli)
		{
			this->_nCourseId		= sli._nCourseId;
			this->_nCompulSoryType	= sli._nCompulSoryType;
			this->_nCourseCredit	= sli._nCourseCredit;
			this->_nTeacherId		= sli._nTeacherId;
			this->_nOpenType		= sli._nOpenType;
			this->_nOnlineType		= sli._nOnlineType;

			this->_nAsideType       = sli._nAsideType     ;
			this->_nMaxAsideCount   = sli._nMaxAsideCount ;
			this->_nGuestType       = sli._nGuestType     ;
			this->_nMaxAguestCount  = sli._nMaxAguestCount;

			wcscpy_s(this->_szCourseName,sli._szCourseName);
			wcscpy_s(this->_szCourseDesc,sli._szCourseDesc);
			wcscpy_s(this->_szUserHeadUrl,sli._szUserHeadUrl);

			wcscpy_s(this->_szCourseCoverUrl,sli._szCourseCoverUrl);
			wcscpy_s(this->_szSnaptShotUrl,sli._szSnaptShotUrl);
			wcscpy_s(this->_szHostUniversity,sli._szHostUniversity);

			wcscpy_s(this->_szHostCollege,sli._szHostCollege);
			wcscpy_s(this->_szHostMajor,sli._szHostMajor);
			wcscpy_s(this->_szSchoolYear,sli._szSchoolYear);
			wcscpy_s(this->_szSchoolTerm,sli._szSchoolTerm);
			wcscpy_s(this->_szOfflineClassroomAddress,sli._szOfflineClassroomAddress);

			for(int i=0;i<(int)sli._vecStudentsId.size();i++)
			{
				this->_vecStudentsId.push_back(sli._vecStudentsId[i]);	
			}
			
			return *this;
		};

		__int64			_nCourseId;					
		wchar_t			_szCourseName[DEFTEXTLEN];
		wchar_t			_szCourseDesc[BIGTEXTLEN];
		wchar_t			_szUserHeadUrl[DEFTEXTLEN];
		wchar_t			_szCourseCoverUrl[MAXTEXTLEN];
		wchar_t			_szSnaptShotUrl[MAXTEXTLEN];
		int				_nCompulSoryType;
		int				_nCourseCredit;
		__int64			_nTeacherId;
		wchar_t			_szHostUniversity[DEFTEXTLEN]; 
		wchar_t			_szHostCollege[DEFTEXTLEN];
		wchar_t			_szHostMajor[DEFTEXTLEN];
		wchar_t			_szSchoolYear[DEFTEXTLEN];
		wchar_t			_szSchoolTerm[DEFTEXTLEN];
		int				_nOnlineType;
		int				_nOpenType;
		wchar_t			_szOfflineClassroomAddress[BIGTEXTLEN];
		int				_nAsideType;
		int				_nMaxAsideCount;
		int				_nGuestType;
		int				_nMaxAguestCount;		
        int             _nMaxStudent;

		std::vector<__int64>		_vecStudentsId;
		std::vector<__int64>		_vecSuperAdminId;
	};

	struct SLClassRoomBaseInfo
	{
		SLClassRoomBaseInfo()
		{
			memset(this, 0, sizeof(SLClassRoomBaseInfo));
		}
				
		__int64				_nRoomId;
		__int64				_nCourseId;
		wchar_t				_szRoomName[DEFTEXTLEN];
		wchar_t				_szUserHeadUrl[BIGTEXTLEN];
		__int64				_nTeacherId;
		char				_feedback;			//是否评价过本节课 0：未评价 1：已评价
        char                _mainshow;          //1为课件或白板；2为视频
		char				_nClassState;
		char				_nClassMode;		
		char				_nTeacherVideo;			//老师的主视频
		char				_nClassType;			//课程类型，是网络课
		char				_nAllowAside;			//是否允许旁听
		int					_nMaxAsideNum;			//最后旁边数
		int					_nAllowTry;				//是否允许试听	
		int					_nMaxTryNum;			//最大试听数
		time_t				_nBeginTime;			//开始时间
        int                 _nBeforeTime;            //离上课还有多长时间
		int					_nProcessTime;			//已进行时间,从点击上课开始
		int					_nClassTimeLen;			//课堂时间多长,单位分
		int					_nEndTime;
		wchar_t				_szDevAccessCode[MINTEXTLEN];
		wchar_t				_szPicUrl[BIGTEXTLEN];
		wchar_t				_szDescription[MAXTEXTLEN];
	};

	struct SLClassRoomShowInfo
	{
		SLClassRoomShowInfo()
		{
			memset(this, 0, sizeof(SLClassRoomShowInfo));
		}

        SLClassRoomShowInfo(const SLClassRoomShowInfo& a)
        {
            _nRoomId = a._nRoomId;
            _nShowType = a._nShowType;
            _nShowPage = a._nShowPage;
            wcscpy_s(_szShowName,a._szShowName);
        }

		__int64				_nRoomId;
		char				_nShowType;
		wchar_t				_szShowName[MAXTEXTLEN]; //课件名称
		int					_nShowPage;
	};

	//拍板事件，画图写字等操作
	struct SLWhiteBoardEvent
	{
		SLWhiteBoardEvent()
		{
			memset(this, 0, sizeof(SLWhiteBoardEvent));
		}

        SLWhiteBoardEvent(const SLWhiteBoardEvent& a)
        {
            _nUserId = a._nUserId;
            _nOwerUserId = a._nOwerUserId;
            _nRoomId = a._nRoomId;
            _nPageId = a._nPageId;
            _nPaintId = a._nPaintId; 
            _nPainType = a._nPainType;
            _strParam = a._strParam;
        }

		__int64				_nUserId;
		__int64				_nOwerUserId;
		__int64				_nRoomId;
		int					_nPageId;
		int					_nPaintId; //记录是第几个操作，每一个操作用一个ID来表示
		PainType			_nPainType;
		std::string			_strParam;
	};

	enum EWhiteBoard_Opt
	{
		EBoardOpt_Show = 1,
		EBoardOpt_Hide,
		EBoardOpt_Change,
		EBoardOpt_Create = 1,
		EBoardOpt_Delete = 2,
		EBoardOpt_ModifyName = 3, //修改白板名字
	};
	//白板的显示隐藏，创建删除等操作
	struct SLWhiteBoardOpt
	{
		SLWhiteBoardOpt()
		{
			memset(this, 0, sizeof(SLWhiteBoardOpt));
		}

        SLWhiteBoardOpt(const SLWhiteBoardOpt& a)
        {
            _nUserId = a._nUserId;
            _nRoomId = a._nRoomId;
            _nPageId = a._nPageId;
            _nOpt = a._nOpt;
            wcscpy_s(_szwbName,a._szwbName);
        }

		__int64				_nUserId;
		__int64				_nRoomId;
		int					_nPageId;
		wchar_t				_szwbName[20];
		EWhiteBoard_Opt		_nOpt;
	};

	enum ECursewaveFile
	{
		CURSEWAVE_PPT_FILE = 1,
		CURSEWAVE_PDF_FILE ,
		CURSEWAVE_IMAGE_FILE ,
		CURSEWAVE_MEDIA_FILE ,
	};

	enum ECursewave_Opt
	{
		ECursewaveOpt_ADD = 1,
		ECursewaveOpt_DEL ,	
		ECursewaveOpt_CLOSE,
	};

	//课件操作
	struct SLCursewaveListOpt
	{
		SLCursewaveListOpt()
		{
			memset(this, 0, sizeof(SLCursewaveListOpt));
		}

        SLCursewaveListOpt(const SLCursewaveListOpt& a)
        {
            _nRoomId = a._nRoomId;
            _nUserId = a._nUserId;
            _nOpt = a._nOpt;
            _nType = a._nType;
            wcscpy_s(_szFileName,a._szFileName);
        }

		long			_nRoomId;
		long			_nUserId;
		unsigned char	_nOpt;
		wchar_t			_szFileName[BIGTEXTLEN];		
		unsigned char	_nType;
	};

	enum ETextMsgType
	{
		eTestMsgType_class = 1,
		eTestMsgType_group,
		eTestMsgType_user
	};
	struct SLTextMsgInfo
	{
		SLTextMsgInfo()
		{
			memset(this, 0, sizeof(SLTextMsgInfo));
		}

        SLTextMsgInfo(const SLTextMsgInfo& a)
        {
            _nClassRoomId = a._nClassRoomId;
            _nRecvUserId = a._nRecvUserId;
            _nSendUserId = a._nSendUserId;
            _nTimer = a._nTimer;
            _nGroupId = a._nGroupId;
            _nMsgType = a._nMsgType;
            _nMsgLen = a._nMsgLen;

            wcscpy_s(_szMsg,a._szMsg);
        }

		__int64		_nClassRoomId;
		__int64		_nRecvUserId;
		__int64		_nSendUserId;
		__int64		_nTimer;
		int			_nGroupId;		
		ETextMsgType	_nMsgType;
		wchar_t			_szMsg[MAXTEXTLEN];
		int				_nMsgLen;
	};

	enum ECreateTalkGroup
	{
		eCreateTalkGroup_start = 1,
		eCreateTalkGroup_end,
		eCreateTalkGroup_precreate, //老师正在创建
		eCreateTalkGroup_cancelcreate,//取消创建
	};

	struct SLCreateTalkGroup
	{
		__int64				_nRoomId;
		__int64				_nUserId;
		ECreateTalkGroup	_nOpteType;
		int					_nGroupCount; //讨论组数
	};

	struct SLCoursewareInfo
	{
	public:
		SLCoursewareInfo()
		{
			memset(this,NULL,sizeof(SLCoursewareInfo));
		}

		SLCoursewareInfo& operator=(SLCoursewareInfo& cwi)
		{
			wcscpy_s(this->_wszName,cwi._wszName);
			wcscpy_s(this->_wszFile,cwi._wszFile);

			return *this;
		}

		wchar_t		_wszName[BIGTEXTLEN];
		wchar_t		_wszFile[BIGTEXTLEN];
	};

	struct SLMessageInfo
	{
		SLMessageInfo()
		{
			memset(this,NULL,sizeof(SLMessageInfo));
		}

		SLMessageInfo& operator=(SLMessageInfo& msi)
		{
			wcscpy_s(this->_wszTitle,msi._wszTitle);
			wcscpy_s(this->_wszContent,msi._wszContent);
			wcscpy_s(this->_wszUrl,msi._wszUrl);

			return *this;
		}

		wchar_t		_wszTitle[DEFTEXTLEN];
		wchar_t		_wszContent[BIGTEXTLEN];
		wchar_t		_wszUrl[BIGTEXTLEN];
	};
}
