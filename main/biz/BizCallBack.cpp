//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：BizCallBack.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.19
//	说明：网络库BIZ的回调管理以及与UI的交互的类定义的实现文件
//	修改记录：
//**********************************************************************

#include "BizCallBack.h"
#include "biz/interface/interface.h"

CBizCallBack::CBizCallBack()
{
    //注册自定义的信号与槽的参数
    qRegisterMetaType<ServerErrorInfo>("ServerErrorInfo");
    qRegisterMetaType<LoginInfo>("LoginInfo");
    qRegisterMetaType<UserLeaveInfo>("UserLeaveInfo");
    qRegisterMetaType<biz::SLWhiteBoardEvent>("SLWhiteBoardEvent");
    qRegisterMetaType<biz::SLWhiteBoardOpt>("SLWhiteBoardOpt");
    qRegisterMetaType<DiscussGroupOptInfo>("DiscussGroupOptInfo");
    qRegisterMetaType<UserSpeakActionInfo>("UserSpeakActionInfo");
    qRegisterMetaType<TeacherEnterDiscussGroupInfo>("TeacherEnterDiscussGroupInfo");
    qRegisterMetaType<GroupCallTeacherInfo>("GroupCallTeacherInfo");
    qRegisterMetaType<biz::SLCursewaveListOpt>("SLCursewaveListOpt");
    qRegisterMetaType<biz::SLClassRoomShowInfo>("SLClassRoomShowInfo");
    qRegisterMetaType<ClassStateChangeInfo>("ClassStateChangeInfo");
    qRegisterMetaType<biz::SLTextMsgInfo>("SLTextMsgInfo");
    qRegisterMetaType<TopVideoChangeInfo>("TopVideoChangeInfo");
    qRegisterMetaType<UploadTokenInfo>("UploadTokenInfo");
    qRegisterMetaType<StudentVideoListInfo>("StudentVideoListInfo");
    qRegisterMetaType<PrevCoursewareListInfo>("PrevCoursewareListInfo");
    qRegisterMetaType<SystemMessageInfo>("SystemMessageInfo");
    qRegisterMetaType<TeacherSwitchVideoInfo>("TeacherSwitchVideoInfo");
    qRegisterMetaType<SetClassModeInfo>("SetClassModeInfo");
    qRegisterMetaType<SignMsgInfo>("SignMsgInfo");
    qRegisterMetaType<SetAssistantInfo>("SetAssistantInfo");
    qRegisterMetaType<NetDevAccessInfo>("NetDevAccessInfo");
    qRegisterMetaType<ChooseMobileInfo>("ChooseMobileInfo");
    qRegisterMetaType<MobileNotifyInfo>("MobileNotifyInfo");
    qRegisterMetaType<MessageListInfo>("MessageListInfo");
    qRegisterMetaType<AutoLoginTokenInfo>("AutoLoginTokenInfo");
    qRegisterMetaType<ClassMsgReminderInfo>("ClassMsgReminderInfo");    
    qRegisterMetaType<ClassMsgReminderInfo>("ClassMainShowInfo");    

}

CBizCallBack::~CBizCallBack()
{	
}

void CBizCallBack::OnConnectServerError(int nServerState,const char* pErrorInfo)
{
    ServerErrorInfo info(nServerState,pErrorInfo);
    emit ConnectServerError(info);
}

void CBizCallBack::OnLogin(__int64 nUserId, char utype)
{
    LoginInfo info(nUserId,utype);	
    emit Login(info);
/*
#if CLIENT_RECODER_VERSION
	CEduSeeion::GetInst()->InitDevState();
#endif
*/
}

void CBizCallBack::OnLogKickOut(__int64 nUserId)
{   
	emit LogKickOut(nUserId);
}

void CBizCallBack::OnEnterClassError(__int64 nRoomId,int nErrorCode)
{   
	ClassErrorInfo info(nRoomId,nErrorCode);	
    emit EnterClassError(info);
}

void CBizCallBack::OnUserEnter(biz::SLUserInfo& Info)
{   
	emit UserEnter(Info.nUserId);
}

void CBizCallBack::OnUserInfoReturn(biz::SLUserInfo& sInfo)
{   
    emit UserInfoReturn(sInfo.nUserId);
}

void CBizCallBack::OnLoginError( int nErrorCode)
{    
	emit LoginError(nErrorCode);
}

void CBizCallBack::OnUserLever(__int64 nUserId, __int64 nRoomId, __int64 nOptId)
{    
	UserLeaveInfo info(nUserId,nRoomId,nOptId);
	emit UserLeave(info);
}

//修改成刷新个人，而不是全部人员
void CBizCallBack::OnUpdateUserInfo(__int64 nUserId)
{    
    emit UpdateUserInfo(nUserId);
}

void CBizCallBack::OnWhiteBoardEvent(biz::SLWhiteBoardEvent& sEventInfo)
{    
    emit WhiteBoardEvent(sEventInfo);
}

void CBizCallBack::OnWhiteBoardOpt(biz::SLWhiteBoardOpt& sOptInfo)
{    
	emit WhiteBoardOpt(sOptInfo);
}

void CBizCallBack::OnCursewaveListEvent(biz::SLCursewaveListOpt& sCWLOpt)
{    
	emit CursewaveListEvent(sCWLOpt);
}

void CBizCallBack::ShowTypeChange(biz::SLClassRoomShowInfo& sInfo)
{    
	emit ShowTypeChanged(sInfo);
}

void CBizCallBack::ClassStateChange(__int64 nCourseId, __int64 nClassId, char nState, __int64 nUserId,int ret)
{    
	ClassStateChangeInfo info (nCourseId, nClassId, nState, nUserId,ret);
	emit ClassStateChanged(info);
}

void CBizCallBack::MainShowChange(char nType, __int64 nRoomId,__int64 nTeacherId)
{
    ClassMainShowInfo info(nRoomId,nTeacherId,nType);
    emit MainShowChanged(info);
}

void CBizCallBack::OnRecvClassMessage(biz::SLTextMsgInfo& sInfo)
{    
	emit RecvClassMessage(sInfo);
}

void CBizCallBack::OnDiscussGroupOpt(int nOptType, __int64 nRoomId, const wchar_t* pTopic)
{    
	DiscussGroupOptInfo info(nOptType,nRoomId,pTopic);
	emit DiscussGroupOpt(info);
}

void CBizCallBack::OnUserSpeakAction(__int64 nRoomId, __int64 nUserId, int nAction)
{    
	UserSpeakActionInfo info(nRoomId,nUserId,nAction);
	emit UserSpeakAction(info);
}

void CBizCallBack::OnTopVideoChange(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop)
{    
	TopVideoChangeInfo info(nRoomId,nUserId,nTeacherId,bAddToTop);
	emit TopVideoChange(info);
}

void CBizCallBack::OnError(__int64 nRoomId, int nErrorCode)
{    
	ClassErrorInfo info(nRoomId,nErrorCode);
	emit Error(info);	
}

void CBizCallBack::OnRecvCourseInfo(__int64 nCourseId)
{    
	emit RecvCourseInfo(nCourseId);
}

void CBizCallBack::OnTeacherEnterDiscussGroup(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt)
{    
	TeacherEnterDiscussGroupInfo info(nRoomId,nGroupId,nTeacherId,nOpt);
	emit TeacherEnterDiscussGroup(info);	
}

void CBizCallBack::OnGroupCallTeacher(__int64 nRoomId, __int64 nUserId)
{
    GroupCallTeacherInfo info(nRoomId,nUserId);
	emit GroupCallTeacher(info);
}

void CBizCallBack::OnRecvUploadToken(int retCode,__int64 nUserId,int nValidateTime,const char* pwszToken)
{    
	UploadTokenInfo info(retCode,nUserId,nValidateTime,pwszToken);
	emit RecvUploadToken(info);	
}

void CBizCallBack::OnRecvStudentVideoList(__int64 nRoomId,std::vector<__int64>& stuVideoList)
{    
	StudentVideoListInfo info(nRoomId,stuVideoList);
	emit RecvStudentVideoList(info);	
}

void CBizCallBack::OnRecvPrevCoursewareList(__int64 nCourseId,std::vector<biz::SLCoursewareInfo>& cwList)
{    
	PrevCoursewareListInfo info(nCourseId,cwList);
	emit RecvPrevCoursewareList(info);
}

void CBizCallBack::OnRecvSystemMessage(__int64 nUserId,const wchar_t* pwszMsg)
{    
	SystemMessageInfo info(nUserId,pwszMsg);
	emit RecvSystemMessage(info);
}

void CBizCallBack::OnTeacherSwitchVideo(__int64 nRoomId,__int64 nUserId,int nVideo)
{    
	TeacherSwitchVideoInfo info(nRoomId,nUserId,nVideo);
	emit TeacherSwitchVideo(info);
}

void CBizCallBack::OnSetClassMode(__int64 nRoomId,__int64 nUserId,char nMode)
{    
	SetClassModeInfo info(nRoomId,nUserId,nMode);
	emit SetClassMode(info);
}

void CBizCallBack::OnRecvSignMsg(__int64 nRoomId,__int64 nUserId)
{    
	SignMsgInfo info(nRoomId,nUserId);
	emit RecvSignMsg(info);
}

void CBizCallBack::OnRecvSetAssistantMsg(__int64 nRoomId,__int64 nUserId,bool bAs,int ret)
{    
	SetAssistantInfo info(nRoomId,nUserId,bAs,ret);
	emit RecvSetAssistantMsg(info);
}

void CBizCallBack::OnRecvUpdateDevCodeMsg(__int64 nRoomId)
{    
	emit RecvUpdateDevCodeMsg(nRoomId);
}

void CBizCallBack::OnRecvNetDevAccess(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName)
{    
	NetDevAccessInfo info(nRoomId,nUserId,pwszDevName);
	emit RecvNetDevAccess(info);
}

void CBizCallBack::OnRecvChooseMobileResponse(__int64 nTeacherId,__int64 nUserId, int ret, byte nAct)
{    
	ChooseMobileInfo info(nTeacherId,nUserId,ret,nAct);
	emit RecvChooseMobileResponse(info);
}

void CBizCallBack::OnRecvMobileNotify(__int64 nTeacherId,__int64 nUserId)
{    
	MobileNotifyInfo info(nTeacherId,nUserId);
	emit RecvMobileNotify(info);
}

void CBizCallBack::OnRecvMsgList(__int64 nUserId,int nMsgType,std::vector<biz::SLMessageInfo>& msgList)
{    
	MessageListInfo info(nUserId,nMsgType,msgList);
	emit RecvMsgList(info);	
}

void CBizCallBack::OnRecvAutoLoginToken(__int64 nUserId,int ret,LPCWSTR pwszToken)
{    
	AutoLoginTokenInfo info(nUserId,ret,pwszToken);
	emit RecvAutoLoginToken(info);	
}

void CBizCallBack::OnRecvClassMsgReminder(__int64 nCourseId,__int64 nClassId,char nMsgType)
{    
	ClassMsgReminderInfo info(nCourseId,nClassId,nMsgType);
	emit RecvClassMsgReminder(info);
}
