//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�courseware.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.2
//	˵����
//	�޸ļ�¼��
//  ��ע��
//**********************************************************************

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include "../varname.h"
#include "./../token/UploadTokenMgr.h"

using namespace std;

#define MAX_URL_LENGTH  (512)

#define HTTP_OUTTIME	(600)

#define HTTP_DOWN_TIMEOUT   (120)

#define HTTP_DOWNFILE_REPEAT    (10)

#define COURSEWARE_TASK_TIMER   (5000)

#define MAX_COURSEWARE_AND_WB   (8)

//����ʾ����״̬��ʾ���ͣ��μ��ϴ�������ʧ�ܡ�δ�ϿΡ����¿Σ�
enum tagMainShowType
{
    MAIN_SHOW_TYPE_NO = 1001,
    MAIN_SHOW_TYPE_TRANS_FAILED,
    MAIN_SHOW_TYPE_UPLOAD_FAILED,
    MAIN_SHOW_TYPE_DOWN_FALIED,
    MAIN_SHOW_TYPE_CLASS_OVER,
    MAIN_TYPE_TYPE_IN_CLASS,
};

enum
{
    STATE_TASK_DOWN = 0,
    STATE_TASK_UPLOAD,
    STATE_TASK_ALL
};

enum CURSEWAVE_TYPE
{
    COURSEWARE_PPT = 0,				//PPT�μ�
    COURSEWARE_PDF,					//PDF�μ�
    COURSEWARE_IMG,					//ͼƬ�μ�
    COURSEWARE_VIDEO,				//��Ƶ�μ�
    COURSEWARE_AUDIO,				//��Ƶ�μ�
    COURSEWARE_TEXT,				//txt�ļ�
    COURSEWARE_DOC,					//DOC�ļ�
    COURSEWARE_EXCLE,				//excle�ļ�
    COURSEWARE_UNKNOWN
};

enum CURSEWAVE_CTRL
{	
    CTRL_PREV_PAGE = 0,				//��һҳ�������PDF��PPT
    CTRL_NEXT_PAGE,					//��һҳ�������PDF��PPT
    CTRL_HOME_PAGE,					//��ҳ�������PPT
    CTRL_LAST_PAGE,					//βҳ�������PPT
    CTRL_ZOOMIN_PAGE,				//�Ŵ������PDF	
    CTRL_ZOOMOUT_PAGE,				//��С�������PDF
    CTRL_SEEK_PAGE,					//��ҳ���������		
    CTRL_MEDIA_PLAY,				//����ʧ��
    CTRL_MEDIA_PAUSE,				//��ͣ����
    CTRL_MEDIA_STOP					//ֹͣ����	
};

enum
{
    CW_FILE_WAIT_DOWN = 0,
    CW_FILE_DOWNING,
    CW_FILE_DOWN_END,
};


//�μ�״̬
enum
{
    COURSEWARE_STATE_FAILED  = 0,
    COURSEWARE_STATE_WAIT,
    COURSEWARE_STATE_TRANS,
    COURSEWARE_STATE_UPLOAD,
    COURSEWARE_STATE_DOWN,
    COURSEWARE_STATE_OK
};

//�ļ�ת��״̬
enum
{
    COURSEWARE_TRANS_NO = 0,
    COURSEWARE_TRANS_FAILED,
    COURSEWARE_TRANS_OK
};

//�μ�����
enum
{
    COURSEWARE_TYPE_TRANSPDF = 0,    //����ת���μ�(������Ƶ�μ�)
    COURSEWARE_TYPE_TRANSONSERVER,   //������ת���μ�(������Ƶ�μ�)
    COURSEWARE_TYPE_TRANSMEDIA,      //����Ƶת���μ�
    COURSEWARE_TYPE_SKYFILE,         //���Ͽ�μ�
    COURSEWARE_TYPE_OTHER            //����
};

enum COURSEWATE_ACTION
{
    ACTION_UPLOAD_BENGIN = 0,
    ACTION_UPLOAD_END,
    ACTION_UPLOAD_FAILED,
    ACTION_DOWN_BENGIN,
    ACTION_DOWN_END,
    ACTION_DOWN_FAILED,
    ACTION_PROGRESS,
    ACTION_OTHER
};

enum COURSEWARE_ERROR
{
    COURSEWARE_ERR_OK = 0,
    COURSEWARE_ERR_DOWNFAILED,      //����ʧ��
    COURSEWARE_ERR_UPLOADFAILED,    //�ϴ�ʧ��
    COURSEWARE_ERR_REPEAT,          //�ļ��Ѵ���
    COURSEWARE_ERR_PATH,            //�ļ������ڣ��ļ�·������ȷ��
    COURSEWARE_ERR_URL,             //url����
    COURSEWARE_ERR_TOKEN,           //token����
    COURSEWARE_ERR_TRANSMEDIO,      //ת��ʧ��
    COURSEWARE_ERR_TYPE,            //��֧�ָÿμ���ʽ
};

enum EventInfoType;


typedef struct tagCursewareData
{
    tagCursewareData()
    {
        memset(this, 0, sizeof(tagCursewareData));
    }
    tagCursewareData operator =( const tagCursewareData &temp)
    {
        this->m_nCoursewareID = temp.m_nCoursewareID;
        this->m_bISOpen = temp.m_bISOpen;
        this->m_nState = temp.m_nState;
        this->m_nDownState = temp.m_nDownState;
        this->m_nFileType = temp.m_nFileType;
        this->m_nDealPerc = temp.m_nDealPerc;
        this->m_nDealPerc = temp.m_nDealPerc;
        this->m_nBreakPDown = temp.m_nBreakPDown;
        this->m_nTrans = temp.m_nTrans;
        this->m_nHttpID = temp.m_nHttpID;
        this->m_bISOpen = temp.m_bISOpen;        
        this->m_nPage = temp.m_nPage;
        this->m_nFileSize = temp.m_nFileSize;
        this->m_nDownTime = temp.m_nDownTime;
        this->m_nCoursewareType = temp.m_nCoursewareType;
        wcscpy(this->m_szFilePath, temp.m_szFilePath);
        wcscpy(this->m_szSourcePath, temp.m_szSourcePath);
        wcscpy(this->m_szName, temp.m_szName);
        wcscpy(this->m_szNetFileName, temp.m_szNetFileName);
        wcscpy(this->m_szUploadUrl, temp.m_szUploadUrl);
        wcscpy(this->m_szDownUrl, temp.m_szDownUrl);
        return *this;
    }

    int         m_nCoursewareID;    
    int         m_nState;       //�ļ�״̬��ʧ�ܡ��ȴ���ת�����ϴ������ء���ɣ�
    int         m_nDownState;   //�μ�����״̬
    int         m_nDownTime;    //�μ����ش���
    int         m_nFileType;    //�ļ�����
    int         m_nDealPerc;    //�μ�׼������
    int         m_nBreakPDown;  //�ϵ�����(�μ������صİٷֱ�)
    int         m_nTrans;       //�μ�ת��״̬(0 ����Ҫת���� 1 ת��ʧ�ܣ� 2 ת���ɹ�)
    int         m_nHttpID;
    bool        m_bISOpen;    
    unsigned int m_nPage;    
    int         m_nCoursewareType;     //�μ�����(�磺����ת���μ� CURSEWARE_TYPE_TRANSPDF)
    long long   m_nFileSize;
    wchar_t     m_szFilePath[MAX_PATH];
    wchar_t     m_szSourcePath[MAX_PATH];
    wchar_t     m_szName[MAX_PATH];
    wchar_t     m_szNetFileName[MAX_PATH];
    wchar_t     m_szUploadUrl[MAX_URL_LENGTH];
    wchar_t     m_szDownUrl[MAX_URL_LENGTH];
}COURSEWAREDATA, *PCOURSEWAREDATA;

Q_DECLARE_METATYPE(COURSEWAREDATA)

typedef struct tagWaitTaskData 
{
    tagWaitTaskData()
    {
        memset(this, 0, sizeof(tagWaitTaskData));
    }

    int     m_nCoursewareID;
    wchar_t m_szName[MAX_PATH];
}WAITTASKDATA, *PWAITTASKDATA;


typedef vector<PCOURSEWAREDATA> CoursewareVector;
typedef deque<PWAITTASKDATA> WaitTaskDataDeque;


extern int	GetCoursewareFileType(LPCWSTR pszFile);

extern void GetCoursewareUploadFileName(LPWSTR pszLocalFile,LPWSTR pszRemoteFile,int nLength);

extern void GetCoursewareDownloadFileName(LPWSTR pszRemoteFile,LPWSTR pszLocalFile,int nLength);

extern void GetCoursewareTransFile(LPWSTR pszMD5,LPWSTR pszCopyFile,int nLength);

extern void GetCoursewareTransFile(LPWSTR pszMD5, LPWSTR pszCopyFile,LPWSTR pszType, int nLength);

extern void GetCoursewareTransFileMedia(LPWSTR pszMD5, LPWSTR pszCopyFile, LPWSTR pszType, int nLength);

extern long long GetCoursewareFileSize(LPCWSTR pszFile);

extern bool IsExistFile(LPCWSTR pszFilePath);

extern bool GetUploadToken(char *pszToken, __int64 nUserId = 0);

extern bool CopyFileToMD5Path(LPCWSTR pszMD5Path, LPCWSTR pszSourcePath);

extern bool GetHttpDownUrl(LPWSTR pszUrl, int nLength);

extern bool GetHttpUploadUrl(LPWSTR pszUrl, int nLength);

extern bool DeleteCoursewareFile(LPCWSTR pszFilePath);

extern bool DeleteFloderContent(LPCWSTR pszFloder, bool isDelSelf = false);

extern bool RenameFile(LPCWSTR pszFileName, LPCWSTR pszNewFileName);

extern bool GetErrorInfo(LPWSTR, int);