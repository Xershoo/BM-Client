#pragma once

#include <memory.h>
#include <vector>
#include <string>

namespace biz
{
	#define MAXEDUINFO			(10)	//ѧ����ѧ����Ϣ���ʮ��

	#define MINTEXTLEN			(64)	
	#define DEFTEXTLEN			(128)
	#define BIGTEXTLEN			(512)
	#define MAXTEXTLEN			(1024)

	enum EServerConnectState
	{			
		eServerState_ConFailed, //����������ʧ��
		eServerState_ConDis,    //�ͻ������ӶϿ��������Ƿ�����������Ҳ��������������
		eServerState_ConClose,  //�ͻ��������ر�
		eServerState_connect,	//���������������
		eServerState_AutoReconnect		//��������Զ�����
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
		eMobileVerifyCode_Register = 1, //ע����֤��
		eMobileVerifyCode_Bind,         //���ֻ�
		eMobileVerifyCode_Password = 11, //�һ�����
	};

	enum EMobileVerifyCodeError
	{
		eVerifyCodeError_Ok = 0,
		eVerifyCodeError_InvalidMobile = -1,  //�����ֻ���
		eVerifyCodeError_Frequence = -2,      //���η��ͼ������
		eVerifyCodeError_Toomuch_times=-3,	  //һ�շ��ʹ�������
		eVerifyCodeError_Server_error = -4,   //���ŷ����̴���
		eVerifyCodeError_mobile_exist = -5,   //�ֻ����Ѱ�
		eVerifyCodeError_inner_server_error = -6, //�ڲ�����������

		eVerifyMobile_Ok = 0,
		eVerifyMobile_Invalid_param = -1,   //�Ƿ�����
		eVerifyMobile_wrong_serialid = -2,  //serialid������
		eVerifyMobile_exceed_time = -3,     //��֤�����
		eVerifyMobile_wrong_verifycode = -4,//��֤�����
		eVerifyMobile_server_error = -100,  //����������
	};

	enum ERegisterUserResult
	{
		eRegisterUser_Ok = 0,
		eRegisterUser_invalid_param = -1,    //�Ƿ�����
		eRegisterUser_wrong_serialid = -2,   //serialid������
		eRegisterUser_not_varify = -3,       //û��ͨ����֤
		eRegisterUser_mobile_exist = -4,     //�ֻ�����ע��
		eRegisterUser_server_error = -100,   //�ڲ�����������
	};

	enum PainType
	{
		PT_NONE,
		PT_PEN,
		PT_ERASER,
		PT_TXT,
		PT_UNDO = 103,
		PT_CLEAN,
		PT_LASERPOINTER, //�����
	};

	enum UserGender
	{
		Gender_unKnow = 0,
		Gender_Male = 1,
		Gender_Female,
	};

	enum EUserAuthority
	{
		UserAu_Visitor = 1,		//����
		UserAu_Observer = 10,	//�Ա�
		UserAu_Monitor = 20,	//����Ա��У���ȹ�����
		UserAu_Student = 30,	//ѧ��
		UserAu_Assistant = 31,	//����
		UserAu_Teacher = 40,	//��ʦ
	};

	enum eRegUserType
	{
		eRegUserType_UserName = 1, //�û���ע���û�
		eRegUserType_Mobile = 2,   //�ֻ�ע���û�
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
		Eclassroomstate_Free = 0,		//û���Ͽ�
		Eclassroomstate_Ready =  1,		//׼���Ͽ�
		Eclassroomstate_Going = 2,		//�ƻ��Ͽ�
		EClassroomstate_Doing = 4,		//��ʦ����
	};

	enum EClassMsgType
	{
		EClassMsgSign = 1,				// �ƻ�ʱ��ǰ10����
		EClassMsgBegin = 2,				// �ƻ�ʱ�䵽
		EClassMsgSignOut = 3,			// �ƻ�ʱ�䵽ǰ5����
		EClassMsgEnd = 4,				// �ƻ�ʱ�䵽
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
		eUserspeekstate_Ask_speak = 1, //����
		eUserspeekstate_cancel_Speak,  //ȡ������
		eUserspeekstate_allow_speak,   //��ʦ������
		eUserspeekstate_clean_speak,   //��ʦ�������

		eClassAction_Kicout,			//��ʦ��ѧ���߳�����
		eClassAction_add_student_video = 6, //��ʦ��ѧ����ӵ���Ƶ��
		eClassAction_del_student_video,    //��ʦ��ѧ������Ƶ���ƿ�
		eClassAction_Open_voice,			//������
		eClassAction_Close_voice,			//�ر�����
		eClassAction_Open_video,			//����Ƶ
		eClassAction_Close_video,			//�ر���Ƶ
		eClassAction_mute = 12,				//����
		eClassAction_unmute,				//�رս���
		eClassAction_teacher_enter_group,   //��ʦ�������۷���
		eClassAction_teacher_leave_group = 15, //��ʦ�뿪���۷���
		eClassAction_call_teacher,			//��������ѧ��������ʦ
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
		wchar_t			szSchoolName[DEFTEXTLEN];			//ѧУ
		wchar_t			szCollege[DEFTEXTLEN];				//ѧԺ
		wchar_t			szGrade[DEFTEXTLEN];				//�༶
		wchar_t			szMajor[DEFTEXTLEN];				//רҵ
		wchar_t			szStudentNo[DEFTEXTLEN];			//ѧ��
		int				nVerifyStatus;						//��֤��Ϣ

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
		int					_nBanType; //���״̬�� �д�ȷ�� ����
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
		char				_feedback;			//�Ƿ����۹����ڿ� 0��δ���� 1��������
        char                _mainshow;          //1Ϊ�μ���װ壻2Ϊ��Ƶ
		char				_nClassState;
		char				_nClassMode;		
		char				_nTeacherVideo;			//��ʦ������Ƶ
		char				_nClassType;			//�γ����ͣ��������
		char				_nAllowAside;			//�Ƿ���������
		int					_nMaxAsideNum;			//����Ա���
		int					_nAllowTry;				//�Ƿ���������	
		int					_nMaxTryNum;			//���������
		time_t				_nBeginTime;			//��ʼʱ��
        int                 _nBeforeTime;            //���Ͽλ��ж೤ʱ��
		int					_nProcessTime;			//�ѽ���ʱ��,�ӵ���Ͽο�ʼ
		int					_nClassTimeLen;			//����ʱ��೤,��λ��
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
		wchar_t				_szShowName[MAXTEXTLEN]; //�μ�����
		int					_nShowPage;
	};

	//�İ��¼�����ͼд�ֵȲ���
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
		int					_nPaintId; //��¼�ǵڼ���������ÿһ��������һ��ID����ʾ
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
		EBoardOpt_ModifyName = 3, //�޸İװ�����
	};
	//�װ����ʾ���أ�����ɾ���Ȳ���
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

	//�μ�����
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
		eCreateTalkGroup_precreate, //��ʦ���ڴ���
		eCreateTalkGroup_cancelcreate,//ȡ������
	};

	struct SLCreateTalkGroup
	{
		__int64				_nRoomId;
		__int64				_nUserId;
		ECreateTalkGroup	_nOpteType;
		int					_nGroupCount; //��������
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
