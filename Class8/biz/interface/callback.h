#pragma once

#include "biz/interface/data.h"

namespace biz
{
	struct ILoginCallback
	{
		
	};


	struct IPaintCallback
	{
		
	};


	struct ICallback
	{
		virtual void OnConnectServerError(int nServerState, const char* pErrorInfo) = 0; //连接失败或者断线
		virtual void OnLogin(__int64 nUserId, char utype) = 0;		
		virtual void OnLogKickOut(__int64 nUserId) = 0;
		virtual void OnLoginError( int nErrorCode) = 0;

		virtual void OnEnterClassError(__int64 nRoomId,int nErrorCode) = 0;

		virtual void OnError(__int64 nRoomId, int nErrorCode) = 0; //错误信息，不包括登录错误

		virtual void OnUserEnter(SLUserInfo& Info) = 0;
		virtual void OnUserLever(__int64 nUserId, __int64 nRoomId, __int64 nOptId) = 0; //nOptId 指操作人的ID，可能是他把nUserId踢出房间的
		virtual void OnUserInfoReturn(SLUserInfo& sInfo) = 0;

		
		//白板上的一些事件，画笔，打字，撤销等
		virtual void OnWhiteBoardEvent(SLWhiteBoardEvent& sEventInfo) = 0;
		//创建新的白板，删除白板，用户显示隐藏白板，切换白板等操作
		virtual void OnWhiteBoardOpt(SLWhiteBoardOpt& sOptInfo) = 0;
		virtual void ShowTypeChange(SLClassRoomShowInfo& sInfo) = 0;
        virtual void MainShowChange(char nType, __int64 nRoomId,__int64 nTeacherId) = 0;
		virtual void ClassStateChange(__int64 nRoomId, __int64 nClassId, char nState, __int64 nUserId,int ret) = 0;
		virtual void OnRecvClassMessage(SLTextMsgInfo& sInfo) = 0;
		//分组讨论,是创建还是删除
		virtual void OnDiscussGroupOpt(int nOptType, __int64 nRoomId, const wchar_t* pTopic) = 0;
		virtual void OnUserSpeakAction(__int64 nRoomId, __int64 nUserId, int nAction) = 0;
		virtual void OnTeacherEnterDiscussGroup(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt) = 0;
		virtual void OnGroupCallTeacher(__int64 nRoomId, __int64 nUserId) = 0; //呼叫老师
		//课件列表操作
		virtual void OnCursewaveListEvent(SLCursewaveListOpt& sCWCtrl)=0;

		//bAddToTop : true表示是增加到顶部视频区，否则为删除
		virtual void OnTopVideoChange(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop) = 0;
				
		//课程信息回掉
		virtual void OnRecvCourseInfo(__int64 nCourseId) = 0;

		//用户信息刷新
		virtual void OnUpdateUserInfo(__int64 nRoomId) = 0;

		//取得token
		virtual void OnRecvUploadToken(int retCode,__int64 nUserId,int nValidateTime,const char* pwszToken) = 0;

		//视频区域用户列表
		virtual void OnRecvStudentVideoList(__int64 nRoomId,std::vector<__int64>& stuVideoList) = 0;

		//Web的课件列表信息
		virtual void OnRecvPrevCoursewareList(__int64 nCourseId,std::vector<SLCoursewareInfo>& cwList) = 0;

		//收到系统消息
		virtual void OnRecvSystemMessage(__int64 nUserId,const wchar_t* pwszMsg) = 0;

		//老师切换视频
		virtual void OnTeacherSwitchVideo(__int64 nRoomId,__int64 nUserId,int nVideo) = 0;

		//课堂全局控制
		virtual void OnSetClassMode(__int64 nRoomId,__int64 nUserId,char nMode) = 0;

		//用户提升为助教
		virtual void OnRecvSetAssistantMsg(__int64 nRoomId,__int64 nUserId,bool bAs,int ret) = 0;

		//课堂设备接入码刷新
		virtual void OnRecvUpdateDevCodeMsg(__int64 nRoomId) = 0;

		//设备接入课堂
		virtual void OnRecvNetDevAccess(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName) = 0;

		//手机摄像头停止或打开的应答
		virtual void OnRecvChooseMobileResponse(__int64 nTeacherId,__int64 nUserId, int ret, byte nAct) = 0;

		//手机摄像头掉线
		virtual void OnRecvMobileNotify(__int64 nTeacherId,__int64 nUserId) = 0;

		//查询消息列表返回
		virtual void OnRecvMsgList(__int64 nUserId,int nMsgType,std::vector<SLMessageInfo>& msgList) = 0;

		//查询自动登录TOKEN返回
		virtual void OnRecvAutoLoginToken(__int64 nUserId,int ret,LPCWSTR pwszToken) = 0;

		//课堂消息提醒
		virtual void OnRecvClassMsgReminder(__int64 nCourseId,__int64 nClassId,char nMsgType) = 0;
	};
}