//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：courseware.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：
//	修改记录：
//  备注：
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

//主显示区域状态提示类型（课件上传、下载失败、未上课、已下课）
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
    COURSEWARE_PPT = 0,				//PPT课件
    COURSEWARE_PDF,					//PDF课件
    COURSEWARE_IMG,					//图片课件
    COURSEWARE_VIDEO,				//视频课件
    COURSEWARE_AUDIO,				//音频课件
    COURSEWARE_TEXT,				//txt文件
    COURSEWARE_DOC,					//DOC文件
    COURSEWARE_EXCLE,				//excle文件
    COURSEWARE_UNKNOWN
};

enum CURSEWAVE_CTRL
{	
    CTRL_PREV_PAGE = 0,				//上一页，针对于PDF和PPT
    CTRL_NEXT_PAGE,					//下一页，针对于PDF和PPT
    CTRL_HOME_PAGE,					//首页，针对于PPT
    CTRL_LAST_PAGE,					//尾页，针对于PPT
    CTRL_ZOOMIN_PAGE,				//放大，针对于PDF	
    CTRL_ZOOMOUT_PAGE,				//缩小，针对于PDF
    CTRL_SEEK_PAGE,					//翻页或调整进度		
    CTRL_MEDIA_PLAY,				//播放失败
    CTRL_MEDIA_PAUSE,				//暂停播放
    CTRL_MEDIA_STOP					//停止播放	
};

enum
{
    CW_FILE_WAIT_DOWN = 0,
    CW_FILE_DOWNING,
    CW_FILE_DOWN_END,
};


//课件状态
enum
{
    COURSEWARE_STATE_FAILED  = 0,
    COURSEWARE_STATE_WAIT,
    COURSEWARE_STATE_TRANS,
    COURSEWARE_STATE_UPLOAD,
    COURSEWARE_STATE_DOWN,
    COURSEWARE_STATE_OK
};

//文件转换状态
enum
{
    COURSEWARE_TRANS_NO = 0,
    COURSEWARE_TRANS_FAILED,
    COURSEWARE_TRANS_OK
};

//课件类型
enum
{
    COURSEWARE_TYPE_TRANSPDF = 0,    //本地转换课件(非音视频课件)
    COURSEWARE_TYPE_TRANSONSERVER,   //服务器转换课件(非音视频课件)
    COURSEWARE_TYPE_TRANSMEDIA,      //音视频转换课件
    COURSEWARE_TYPE_SKYFILE,         //资料库课件
    COURSEWARE_TYPE_OTHER            //其他
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
    COURSEWARE_ERR_DOWNFAILED,      //下载失败
    COURSEWARE_ERR_UPLOADFAILED,    //上传失败
    COURSEWARE_ERR_REPEAT,          //文件已存在
    COURSEWARE_ERR_PATH,            //文件不存在（文件路径不正确）
    COURSEWARE_ERR_URL,             //url错误
    COURSEWARE_ERR_TOKEN,           //token过期
    COURSEWARE_ERR_TRANSMEDIO,      //转码失败
    COURSEWARE_ERR_TYPE,            //不支持该课件格式
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
    int         m_nState;       //文件状态（失败、等待、转换、上传、下载、完成）
    int         m_nDownState;   //课件下载状态
    int         m_nDownTime;    //课件下载次数
    int         m_nFileType;    //文件类型
    int         m_nDealPerc;    //课件准备进度
    int         m_nBreakPDown;  //断点下载(课件已下载的百分比)
    int         m_nTrans;       //课件转换状态(0 不需要转换， 1 转换失败， 2 转换成功)
    int         m_nHttpID;
    bool        m_bISOpen;    
    unsigned int m_nPage;    
    int         m_nCoursewareType;     //课件类型(如：本地转换课件 CURSEWARE_TYPE_TRANSPDF)
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